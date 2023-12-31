/*
 * Copyright (c) 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/backlight.h>
#include <drm/drmP.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>

#include <linux/gpio/consumer.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>
#include <video/of_videomode.h>
#include <video/videomode.h>

#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#define CONFIG_MTK_PANEL_EXT
#if defined(CONFIG_MTK_PANEL_EXT)
#include "../mediatek/mtk_panel_ext.h"
#include "../mediatek/mtk_log.h"
#endif

#ifdef CONFIG_MTK_ROUND_CORNER_SUPPORT
#include "../mediatek/mtk_corner_pattern/mtk_data_hw_roundedpattern.h"
#endif

/* enable this to check panel self -bist pattern */
/* #define PANEL_BIST_PATTERN */

/* option function to read data from some panel address */
/* #define PANEL_SUPPORT_READBACK */

struct lg_panel {
	struct device *dev;
	struct drm_panel panel;
	struct backlight_device *backlight;
	struct gpio_desc *reset_gpio;

	bool prepared;
	bool enabled;

	int error;
};

#define lg_dcs_write_seq(ctx, seq...)                                          \
	({                                                                     \
		const u8 d[] = {seq};                                          \
		BUILD_BUG_ON_MSG(ARRAY_SIZE(d) > 64,                           \
				 "DCS sequence too big for stack");            \
		lg_dcs_write(ctx, d, ARRAY_SIZE(d));                           \
	})

#define lg_dcs_write_seq_static(ctx, seq...)                                   \
	({                                                                     \
		static const u8 d[] = {seq};                                   \
		lg_dcs_write(ctx, d, ARRAY_SIZE(d));                           \
	})

static inline struct lg_panel *panel_to_lg(struct drm_panel *panel)
{
	return container_of(panel, struct lg_panel, panel);
}

#ifdef PANEL_SUPPORT_READBACK
static int lg_dcs_read(struct lg_panel *ctx, u8 cmd, void *data, size_t len)
{
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	ssize_t ret;

	if (ctx->error < 0)
		return 0;

	ret = mipi_dsi_dcs_read(dsi, cmd, data, len);
	if (ret < 0) {
		dev_err(ctx->dev, "error %d reading dcs seq:(%#x)\n", ret, cmd);
		ctx->error = ret;
	}

	return ret;
}

static void lg_panel_get_data(struct lg_panel *ctx)
{
	u8 buffer[3] = {0};
	static int ret;

	if (ret == 0) {
		ret = lg_dcs_read(ctx, 0x0A, buffer, 1);
		dev_info(ctx->dev, "return %d data(0x%08x) to dsi engine\n",
			 ret, buffer[0] | (buffer[1] << 8));
	}
}
#endif

#if defined(CONFIG_RT5081_PMU_DSV) || defined(CONFIG_MT6370_PMU_DSV)
static struct regulator *disp_bias_pos;
static struct regulator *disp_bias_neg;

static int lg_panel_bias_regulator_init(void)
{
	static int regulator_inited;
	int ret = 0;

	if (regulator_inited)
		return ret;

	/* please only get regulator once in a driver */
	disp_bias_pos = regulator_get(NULL, "dsv_pos");
	if (IS_ERR(disp_bias_pos)) { /* handle return value */
		ret = PTR_ERR(disp_bias_pos);
		pr_err("get dsv_pos fail, error: %d\n", ret);
		return ret;
	}

	disp_bias_neg = regulator_get(NULL, "dsv_neg");
	if (IS_ERR(disp_bias_neg)) { /* handle return value */
		ret = PTR_ERR(disp_bias_neg);
		pr_err("get dsv_neg fail, error: %d\n", ret);
		return ret;
	}

	regulator_inited = 1;
	return ret; /* must be 0 */
}

static int lg_panel_bias_enable(void)
{
	int ret = 0;
	int retval = 0;

	lg_panel_bias_regulator_init();

	/* set voltage with min & max*/
	ret = regulator_set_voltage(disp_bias_pos, 5400000, 5400000);
	if (ret < 0)
		pr_err("set voltage disp_bias_pos fail, ret = %d\n", ret);
	retval |= ret;

	ret = regulator_set_voltage(disp_bias_neg, 5400000, 5400000);
	if (ret < 0)
		pr_err("set voltage disp_bias_neg fail, ret = %d\n", ret);
	retval |= ret;

	/* enable regulator */
	ret = regulator_enable(disp_bias_pos);
	if (ret < 0)
		pr_err("enable regulator disp_bias_pos fail, ret = %d\n", ret);
	retval |= ret;

	ret = regulator_enable(disp_bias_neg);
	if (ret < 0)
		pr_err("enable regulator disp_bias_neg fail, ret = %d\n", ret);
	retval |= ret;

	return retval;
}

static int lg_panel_bias_disable(void)
{
	int ret = 0;
	int retval = 0;

	lg_panel_bias_regulator_init();

	ret = regulator_disable(disp_bias_neg);
	if (ret < 0)
		pr_err("disable regulator disp_bias_neg fail, ret = %d\n", ret);
	retval |= ret;

	ret = regulator_disable(disp_bias_pos);
	if (ret < 0)
		pr_err("disable regulator disp_bias_pos fail, ret = %d\n", ret);
	retval |= ret;

	return retval;
}
#endif

static void lg_dcs_write(struct lg_panel *ctx, const void *data, size_t len)
{
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	ssize_t ret;
	char *addr;

	if (ctx->error < 0)
		return;

	addr = (char *)data;
	if ((int)*addr < 0xB0)
		ret = mipi_dsi_dcs_write_buffer(dsi, data, len);
	else
		ret = mipi_dsi_generic_write(dsi, data, len);
	if (ret < 0) {
		dev_err(ctx->dev, "error %zd writing seq: %ph\n", ret, data);
		ctx->error = ret;
	}
}

static void lg_panel_init(struct lg_panel *ctx)
{
	ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
	gpiod_set_value(ctx->reset_gpio, 0);
	udelay(15 * 1000);
	gpiod_set_value(ctx->reset_gpio, 1);
	udelay(1 * 1000);
	gpiod_set_value(ctx->reset_gpio, 0);
	udelay(10 * 1000);
	gpiod_set_value(ctx->reset_gpio, 1);
	udelay(10 * 1000);
	devm_gpiod_put(ctx->dev, ctx->reset_gpio);

	lg_dcs_write_seq_static(ctx, 0xB9, 0x83, 0x11, 0x2B);
	lg_dcs_write_seq_static(ctx, 0xB1, 0xF8, 0x29, 0x29, 0x00, 0x00, 0x0F,
				0x14, 0x0F, 0x14, 0x33);
	lg_dcs_write_seq_static(ctx, 0xD2, 0x2C, 0x2C);
	lg_dcs_write_seq_static(ctx, 0xB2, 0x80, 0x02, 0x00, 0x80, 0x70, 0x00,
				0x08, 0x1C, 0x09, 0x01, 0x04);
	lg_dcs_write_seq_static(ctx, 0xE9, 0xD1);
	lg_dcs_write_seq_static(ctx, 0xB2, 0x00, 0x08);
	lg_dcs_write_seq_static(ctx, 0xE9, 0x00);
	lg_dcs_write_seq_static(ctx, 0xE9, 0xCE);
	lg_dcs_write_seq_static(ctx, 0xB2, 0xA3);
	lg_dcs_write_seq_static(ctx, 0xE9, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x02);
	lg_dcs_write_seq_static(ctx, 0xB2, 0xB5, 0x0A);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x00);
	lg_dcs_write_seq_static(ctx, 0xDD, 0x00, 0x00, 0x08, 0x1C, 0x09, 0x34,
				0x34, 0x8B);
	lg_dcs_write_seq_static(ctx, 0xB4, 0x01, 0xD3, 0x00, 0x00, 0x00, 0x00,
				0x03, 0xD0, 0x00, 0x00, 0x0F, 0xCB, 0x01, 0x00,
				0x00, 0x13, 0x00, 0x2E, 0x08, 0x01, 0x12, 0x00,
				0x00, 0x2E);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x02);
	lg_dcs_write_seq_static(ctx, 0xB4, 0x92);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x00);
	lg_dcs_write_seq_static(ctx, 0xB6, 0x81, 0x81, 0xE3);
	lg_dcs_write_seq_static(ctx, 0xC0, 0x44);
	lg_dcs_write_seq_static(ctx, 0xCC, 0x08);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x03);
	lg_dcs_write_seq_static(
		ctx, 0xC1, 0xFF, 0xFA, 0xF6, 0xF3, 0xEF, 0xEB, 0xE7, 0xE0, 0xDC,
		0xD9, 0xD6, 0xD2, 0xCF, 0xCB, 0xC7, 0xC3, 0xBF, 0xBB, 0xB7,
		0xB0, 0xA8, 0xA1, 0x9A, 0x92, 0x89, 0x81, 0x7A, 0x73, 0x6B,
		0x63, 0x5A, 0x51, 0x48, 0x40, 0x38, 0x31, 0x29, 0x20, 0x16,
		0x0D, 0x09, 0x07, 0x05, 0x02, 0x00, 0x08, 0x2E, 0xF6, 0x20,
		0x18, 0x94, 0xF8, 0x6F, 0x59, 0x18, 0xFC, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x02);
	lg_dcs_write_seq_static(
		ctx, 0xC1, 0xFF, 0xFA, 0xF6, 0xF3, 0xEF, 0xEB, 0xE7, 0xE0, 0xDC,
		0xD9, 0xD6, 0xD2, 0xCF, 0xCB, 0xC7, 0xC3, 0xBF, 0xBB, 0xB7,
		0xB0, 0xA8, 0xA1, 0x9A, 0x92, 0x89, 0x81, 0x7A, 0x73, 0x6B,
		0x63, 0x5A, 0x51, 0x48, 0x40, 0x38, 0x31, 0x29, 0x20, 0x16,
		0x0D, 0x09, 0x07, 0x05, 0x02, 0x00, 0x08, 0x2E, 0xF6, 0x20,
		0x18, 0x94, 0xF8, 0x6F, 0x59, 0x18, 0xFC, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x01);
	lg_dcs_write_seq_static(
		ctx, 0xC1, 0xFF, 0xFA, 0xF6, 0xF3, 0xEF, 0xEB, 0xE7, 0xE0, 0xDC,
		0xD9, 0xD6, 0xD2, 0xCF, 0xCB, 0xC7, 0xC3, 0xBF, 0xBB, 0xB7,
		0xB0, 0xA8, 0xA1, 0x9A, 0x92, 0x89, 0x81, 0x7A, 0x73, 0x6B,
		0x63, 0x5A, 0x51, 0x48, 0x40, 0x38, 0x31, 0x29, 0x20, 0x16,
		0x0D, 0x09, 0x07, 0x05, 0x02, 0x00, 0x08, 0x2E, 0xF6, 0x20,
		0x18, 0x94, 0xF8, 0x6F, 0x59, 0x18, 0xFC, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x00);
	lg_dcs_write_seq_static(ctx, 0xC1, 0x01);
	lg_dcs_write_seq_static(ctx, 0xD3, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
				0x01, 0x0A, 0x0A, 0x07, 0x00, 0x00, 0x08, 0x08,
				0x08, 0x08, 0x08, 0x32, 0x10, 0x08, 0x00, 0x08);
	lg_dcs_write_seq_static(ctx, 0xE9, 0xE3);
	lg_dcs_write_seq_static(ctx, 0xD3, 0x05, 0x08, 0x86);
	lg_dcs_write_seq_static(ctx, 0xE9, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x01);
	lg_dcs_write_seq_static(ctx, 0xE9, 0xC8);
	lg_dcs_write_seq_static(ctx, 0xD3, 0x81);
	lg_dcs_write_seq_static(ctx, 0xE9, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x00);
	lg_dcs_write_seq_static(
		ctx, 0xD5, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x31,
		0x31, 0x30, 0x30, 0x2F, 0x2F, 0x31, 0x31, 0x30, 0x30, 0x2F,
		0x2F, 0xC0, 0x18, 0x40, 0x40, 0x01, 0x00, 0x07, 0x06, 0x05,
		0x04, 0x03, 0x02, 0x21, 0x20, 0x18, 0x18, 0x19, 0x19, 0x18,
		0x18, 0x03, 0x03, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18);
	lg_dcs_write_seq_static(
		ctx, 0xD6, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x31,
		0x31, 0x30, 0x30, 0x2F, 0x2F, 0x31, 0x31, 0x30, 0x30, 0x2F,
		0x2F, 0xC0, 0x18, 0x40, 0x40, 0x02, 0x03, 0x04, 0x05, 0x06,
		0x07, 0x00, 0x01, 0x20, 0x21, 0x18, 0x18, 0x18, 0x18, 0x19,
		0x19, 0x20, 0x20, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18);
	lg_dcs_write_seq_static(ctx, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00);

	lg_dcs_write_seq_static(ctx, 0xBD, 0x01);
	lg_dcs_write_seq_static(ctx, 0xD8, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
				0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
				0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
				0xAA, 0xAA);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x02);
	lg_dcs_write_seq_static(ctx, 0xD8, 0xAF, 0xFF, 0xFA, 0xAA, 0xBA, 0xAA,
				0xAA, 0xFF, 0xFA, 0xAA, 0xBA, 0xAA);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x03);
	lg_dcs_write_seq_static(ctx, 0xD8, 0xAA, 0xAA, 0xAB, 0xAA, 0xAE, 0xAA,
				0xAA, 0xAA, 0xAB, 0xAA, 0xAE, 0xAA, 0xAA, 0xFF,
				0xFA, 0xAA, 0xBA, 0xAA, 0xAA, 0xFF, 0xFA, 0xAA,
				0xBA, 0xAA);

	lg_dcs_write_seq_static(ctx, 0xBD, 0x00);
	lg_dcs_write_seq_static(ctx, 0xE7, 0x09, 0x09, 0x00, 0x07, 0xE6, 0x00,
				0x27, 0x00, 0x07, 0x00, 0x00, 0xE6, 0x2A, 0x00,
				0xE6, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x01,
				0x00, 0x12, 0x04);
	lg_dcs_write_seq_static(ctx, 0xE9, 0xE4);
	lg_dcs_write_seq_static(ctx, 0xE7, 0x17, 0x69);
	lg_dcs_write_seq_static(ctx, 0xE9, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x01);
	lg_dcs_write_seq_static(ctx, 0xE7, 0x02, 0x00, 0x01, 0x20, 0x01, 0x0E,
				0x08, 0xEE, 0x09);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x02);
	lg_dcs_write_seq_static(ctx, 0xE7, 0x20, 0x20, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x03);
	lg_dcs_write_seq_static(ctx, 0xE7, 0x00, 0x08, 0x01, 0x00, 0x00, 0x20);
	lg_dcs_write_seq_static(ctx, 0xE9, 0xC9);
	lg_dcs_write_seq_static(ctx, 0xE7, 0x2E, 0xCB);
	lg_dcs_write_seq_static(ctx, 0xE9, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x00);
	lg_dcs_write_seq_static(ctx, 0xD1, 0x27);
	lg_dcs_write_seq_static(ctx, 0xBC, 0x07);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x01);
	lg_dcs_write_seq_static(ctx, 0xE9, 0xC2);
	lg_dcs_write_seq_static(ctx, 0xCB, 0x27);
	lg_dcs_write_seq_static(ctx, 0xE9, 0x00);
	lg_dcs_write_seq_static(ctx, 0xBD, 0x00);
	lg_dcs_write_seq_static(ctx, 0x51, 0x0F, 0xFF);
	lg_dcs_write_seq_static(ctx, 0x53, 0x24);
	lg_dcs_write_seq_static(ctx, 0x55, 0x00);
	lg_dcs_write_seq_static(ctx, 0x35, 0x00);
	lg_dcs_write_seq_static(ctx, 0x44, 0x08, 0x66);

	lg_dcs_write_seq_static(ctx, 0x11);
	msleep(150);

	lg_dcs_write_seq_static(ctx, 0xE9, 0xC2);
	lg_dcs_write_seq_static(ctx, 0xB0, 0x01);
	lg_dcs_write_seq_static(ctx, 0xE9, 0x00);

	lg_dcs_write_seq_static(ctx, 0x29);
	msleep(50);
}

static int lg_disable(struct drm_panel *panel)
{
	struct lg_panel *ctx = panel_to_lg(panel);

	if (!ctx->enabled)
		return 0;

	if (ctx->backlight) {
		ctx->backlight->props.power = FB_BLANK_POWERDOWN;
		backlight_update_status(ctx->backlight);
	}

	ctx->enabled = false;

	return 0;
}

static int lg_unprepare(struct drm_panel *panel)
{
	struct lg_panel *ctx = panel_to_lg(panel);

	if (!ctx->prepared)
		return 0;

	lg_dcs_write_seq_static(ctx, MIPI_DCS_SET_DISPLAY_OFF);
	msleep(50);
	lg_dcs_write_seq_static(ctx, MIPI_DCS_ENTER_SLEEP_MODE);
	msleep(150);

	ctx->error = 0;
	ctx->prepared = false;
#if defined(CONFIG_RT5081_PMU_DSV) || defined(CONFIG_MT6370_PMU_DSV)
	lg_panel_bias_disable();
#endif

	return 0;
}

static int lg_prepare(struct drm_panel *panel)
{
	struct lg_panel *ctx = panel_to_lg(panel);
	int ret;

	pr_debug("%s\n", __func__);
	if (ctx->prepared)
		return 0;

#if defined(CONFIG_RT5081_PMU_DSV) || defined(CONFIG_MT6370_PMU_DSV)
	lg_panel_bias_enable();
#endif

	lg_panel_init(ctx);

	ret = ctx->error;
	if (ret < 0)
		lg_unprepare(panel);

	ctx->prepared = true;

#ifdef PANEL_SUPPORT_READBACK
	lg_panel_get_data(ctx);
#endif

	return ret;
}

static int lg_enable(struct drm_panel *panel)
{
	struct lg_panel *ctx = panel_to_lg(panel);

	if (ctx->enabled)
		return 0;

	if (ctx->backlight) {
		ctx->backlight->props.power = FB_BLANK_UNBLANK;
		backlight_update_status(ctx->backlight);
	}

	ctx->enabled = true;

	return 0;
}

static const struct drm_display_mode default_mode = {
	.clock = 151110,
	.hdisplay = 1080,
	.hsync_start = 1080 + 40,
	.hsync_end = 1080 + 40 + 10,
	.htotal = 1080 + 40 + 10 + 20,
	.vdisplay = 2160,
	.vsync_start = 2160 + 20,
	.vsync_end = 2160 + 20 + 2,
	.vtotal = 2160 + 20 + 2 + 8,
	.vrefresh = 60,
};

#if defined(CONFIG_MTK_PANEL_EXT)
static int panel_ext_reset(struct drm_panel *panel, int on)
{
	struct lg_panel *ctx = panel_to_lg(panel);

	ctx->reset_gpio = devm_gpiod_get(ctx->dev, "reset", GPIOD_OUT_HIGH);
	gpiod_set_value(ctx->reset_gpio, on);
	devm_gpiod_put(ctx->dev, ctx->reset_gpio);

	return 0;
}

static int panel_ata_check(struct drm_panel *panel)
{
	struct lg_panel *ctx = panel_to_lg(panel);
	struct mipi_dsi_device *dsi = to_mipi_dsi_device(ctx->dev);
	unsigned char data[3];
	unsigned char id[3] = {0x83, 0x11, 0x2b};
	ssize_t ret;

	ret = mipi_dsi_dcs_read(dsi, 0x4, data, 3);
	if (ret < 0)
		pr_err("%s error\n", __func__);

	DDPINFO("ATA read data %x %x %x\n", data[0], data[1], data[2]);

	if (data[0] == id[0] &&
			data[1] == id[1] &&
			data[2] == id[2])
		return 1;

	DDPINFO("ATA expect read data is %x %x %x\n",
		id[0], id[1], id[2]);

	return 0;
}

static struct mtk_panel_params ext_params = {
	.esd_check_enable = 1,
	.physical_width_um = 64152,
	.physical_height_um = 128304,
#ifdef CONFIG_MTK_ROUND_CORNER_SUPPORT
	.round_corner_en = 1,
	.corner_pattern_height = ROUND_CORNER_H_TOP,
	.corner_pattern_height_bot = ROUND_CORNER_H_BOT,
	.corner_pattern_tp_size = sizeof(top_rc_pattern),
	.corner_pattern_lt_addr = (void *)top_rc_pattern,
#endif
};

static struct mtk_panel_funcs ext_funcs = {
	.reset = panel_ext_reset,
	.ata_check = panel_ata_check,
};
#endif

struct panel_desc {
	const struct drm_display_mode *modes;
	unsigned int num_modes;

	unsigned int bpc;

	struct {
		unsigned int width;
		unsigned int height;
	} size;

	/**
	 * @prepare: the time (in milliseconds) that it takes for the panel to
	 *           become ready and start receiving video data
	 * @enable: the time (in milliseconds) that it takes for the panel to
	 *          display the first valid frame after starting to receive
	 *          video data
	 * @disable: the time (in milliseconds) that it takes for the panel to
	 *           turn the display off (no content is visible)
	 * @unprepare: the time (in milliseconds) that it takes for the panel
	 *             to power itself down completely
	 */
	struct {
		unsigned int prepare;
		unsigned int enable;
		unsigned int disable;
		unsigned int unprepare;
	} delay;
};

static int lg_get_modes(struct drm_panel *panel)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(panel->drm, &default_mode);
	if (!mode) {
		dev_err(panel->drm->dev, "failed to add mode %ux%ux@%u\n",
			default_mode.hdisplay, default_mode.vdisplay,
			default_mode.vrefresh);
		return -ENOMEM;
	}

	drm_mode_set_name(mode);
	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(panel->connector, mode);

	panel->connector->display_info.width_mm = 64;
	panel->connector->display_info.height_mm = 128;

	return 1;
}

static const struct drm_panel_funcs lg_drm_funcs = {
	.disable = lg_disable,
	.unprepare = lg_unprepare,
	.prepare = lg_prepare,
	.enable = lg_enable,
	.get_modes = lg_get_modes,
};

static int lg_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct lg_panel *ctx;
	struct device_node *backlight;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(struct lg_panel), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	mipi_dsi_set_drvdata(dsi, ctx);

	ctx->dev = dev;
	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_LPM | MIPI_DSI_MODE_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	backlight = of_parse_phandle(dev->of_node, "backlight", 0);
	if (backlight) {
		ctx->backlight = of_find_backlight_by_node(backlight);
		of_node_put(backlight);

		if (!ctx->backlight)
			return -EPROBE_DEFER;
	}

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio)) {
		dev_err(dev, "cannot get reset-gpios %ld\n",
			PTR_ERR(ctx->reset_gpio));
		return PTR_ERR(ctx->reset_gpio);
	}
	devm_gpiod_put(dev, ctx->reset_gpio);

	ctx->prepared = true;
	ctx->enabled = true;

	drm_panel_init(&ctx->panel);
	ctx->panel.dev = dev;
	ctx->panel.funcs = &lg_drm_funcs;

	ret = drm_panel_add(&ctx->panel);
	if (ret < 0)
		return ret;

	ret = mipi_dsi_attach(dsi);
	if (ret < 0)
		drm_panel_remove(&ctx->panel);

#if defined(CONFIG_MTK_PANEL_EXT)
	ret = mtk_panel_ext_create(dev, &ext_params, &ext_funcs, &ctx->panel);
	if (ret < 0)
		return ret;
#endif

	pr_err("%s-\n", __func__);

	return ret;
}

static int lg_remove(struct mipi_dsi_device *dsi)
{
	struct lg_panel *ctx = mipi_dsi_get_drvdata(dsi);

	mipi_dsi_detach(dsi);
	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id lg_of_match[] = {
	{
		.compatible = "lg,0565g40108,cmd",
	},
	{} };

MODULE_DEVICE_TABLE(of, lg_of_match);

static struct mipi_dsi_driver lg_driver = {
	.probe = lg_probe,
	.remove = lg_remove,
	.driver = {

			.name = "panel-lg-0565g40108-cmd",
			.owner = THIS_MODULE,
			.of_match_table = lg_of_match,
		},
};

module_mipi_dsi_driver(lg_driver);

MODULE_AUTHOR("Robin Chen <robin.chen@mediatek.com>");
MODULE_DESCRIPTION("lg 0565g40108 CMD LCD Panel Driver");
MODULE_LICENSE("GPL v2");
