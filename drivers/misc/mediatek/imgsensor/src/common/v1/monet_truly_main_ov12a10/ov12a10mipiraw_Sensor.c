/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/*****************************************************************************
 *
 * Filename:
 * ---------
 *     OV12a10mipi_Sensor.c
 *
 * Project:
 * --------
 *	 ALPS
 *
 * Description:
 * ------------
 *	 Source code of Sensor driver
 *
 * Setting version:
 * ------------
 *   update full pd setting for OV12a10EB_03B
 *---------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#define PFX "ov12a10_camera_sensor"
#define pr_fmt(fmt) PFX "[%s] " fmt, __func__

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/atomic.h>
#include <linux/types.h>

#include "ov12a10mipiraw_Sensor.h"
#include <soc/oppo/oppo_project.h>

/* Zhen.Quan@Camera.Driver, 2019/10/12, add for [otp bringup] */
#include "cam_cal_define.h"
/*xiaojun.Pu@Camera.Driver, 2019/10/15, add for [add hardware_info for factory]*/
#include <linux/hardware_info.h>
/* Zhen.Quan@Camera.Driver, 2019/10/17, add for [otp bringup] */
#include "imgsensor_read_eeprom.h"
#define ENABLE_OV12A10_OTP 1

#define LOG_INF(format, args...)    \
	pr_debug(PFX "[%s] " format, __func__, ##args)

#define MULTI_WRITE 1
static DEFINE_SPINLOCK(imgsensor_drv_lock);

static struct imgsensor_info_struct imgsensor_info = {
	.sensor_id = MONET_TRULY_MAIN_OV12A10_SENSOR_ID,

	.checksum_value = 0x8b86a64,

	.pre = {
	        .pclk =   108000000,    //jack yan check
		.linelength =  1064,
		.framelength = 3346,
		.startx = 0,
		.starty = 0,
		.grabwindow_width =  2048,
		.grabwindow_height = 1536,
	        .mipi_data_lp2hs_settle_dc = 21,
		.mipi_pixel_rate = 467200000,//Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview
		.max_framerate = 300,
	},
	.cap = {
		.pclk =  108000000,
		.linelength =  1064,
		.framelength = 3346,
		.startx = 0,
		.starty = 0,
		.grabwindow_width =  4096,
		.grabwindow_height = 3072,
		.mipi_data_lp2hs_settle_dc = 21,  //80   30-120
		.mipi_pixel_rate = 467200000,//Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview
		.max_framerate = 300,
	},
	/*size@15fps, same as capture*/
	.cap1 = {
		.pclk = 108000000,
		.linelength = 1064,
		.framelength = 6692,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 4096,
		.grabwindow_height = 3072,
		.mipi_data_lp2hs_settle_dc = 21, //80
		.mipi_pixel_rate = 467200000,//Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview
		.max_framerate = 150,
	},
	.cap2 = {
		.pclk =  108000000,
		.linelength =  1064,
		.framelength = 4182,
		.startx = 0,
		.starty = 0,
		.grabwindow_width =  4096,
		.grabwindow_height = 3072,
		.mipi_data_lp2hs_settle_dc = 21,
		.mipi_pixel_rate = 467200000,//Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview
		.max_framerate = 240,
	},
	.normal_video = {
		.pclk =  108000000,
		.linelength =  1064,
		.framelength = 3346,
		.startx = 0,
		.starty = 0,
		.grabwindow_width =  4096,
		.grabwindow_height = 3072,
		.mipi_data_lp2hs_settle_dc = 21,
		.mipi_pixel_rate = 467200000,//Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview
		.max_framerate = 300,
	},
	.hs_video = {/*yang.guo@ODM_WT.Camera.hal, 2019/12/21, mod customer setting  for hs video cam  */
		.pclk = 108000000,
		.linelength  = 1064,
		.framelength =  844,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 1280,
		.grabwindow_height = 720,
		.mipi_data_lp2hs_settle_dc = 90,
		.mipi_pixel_rate = 467200000,
		.max_framerate = 1200,
        },
	.slim_video = {
		.pclk = 108000000,
		.linelength =  1064,
		.framelength = 3346,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 2048,
		.grabwindow_height = 1536,
		.mipi_data_lp2hs_settle_dc = 21,
		.mipi_pixel_rate = 467200000,//Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview
		.max_framerate = 300,
	},
	.custom1 = {
		.pclk = 108000000,
		.linelength =  1064,
		.framelength = 3346,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 2048,
		.grabwindow_height = 1536,
		.mipi_data_lp2hs_settle_dc = 21,
		.mipi_pixel_rate = 467200000,//Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview
		.max_framerate = 300,
	},
	#if 0
	.custom2 = {
		.pclk = 108000000,
		.linelength =  1064,
		.framelength = 3346,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 4096,
		.grabwindow_height = 3072,
		.mipi_data_lp2hs_settle_dc = 21,
		.max_framerate = 300,
	},
	#else
    //*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, add customer setting  for dual cam start */
	.custom2 = {
		.pclk = 108000000,
		.linelength =  1064,
		.framelength = 4229,//4182->4229
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 3264,
		.grabwindow_height = 2448,
		.mipi_data_lp2hs_settle_dc = 21,
		.mipi_pixel_rate = 467200000,//Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview
		.max_framerate = 240,//24.001838481
	},
    //*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, add customer setting  for dual cam end */
	#endif
	.margin = 8,
	.min_shutter = 0x4,        //min shutter check
	.max_frame_length = 0x7fff,
	.ae_shut_delay_frame = 0,  //check
	.ae_sensor_gain_delay_frame = 0,  //check
	.ae_ispGain_delay_frame = 2,
	.frame_time_delay_frame = 2,//*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, add customer setting  for dual cam */
	.ihdr_support = 0,
	.ihdr_le_firstline = 0,
	.sensor_mode_num = 7,

	.cap_delay_frame = 3,
	.pre_delay_frame = 2,
	.video_delay_frame = 2,
	.hs_video_delay_frame = 2,
	.slim_video_delay_frame = 2,
	.custom1_delay_frame = 2,
	.custom2_delay_frame = 2,
	/*Duilin.Qin@ODM_WT.Camera.Driver.493476, 2019/10/21, modify mclk driving current */
	.isp_driving_current = ISP_DRIVING_2MA, //mclk driving current
	.sensor_interface_type = SENSOR_INTERFACE_TYPE_MIPI,
	.mipi_sensor_type = MIPI_OPHY_NCSI2,
	.mipi_settle_delay_mode = 1,
	/*Tian.Tian@ODM_WT.CAMERA.Driver.2019/10/15,modify for camera rotation 180*/
	.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_B,
	.mclk = 24,//mclk value, suggest 24 or 26 for 24Mhz or 26Mhz
	.mipi_lane_num = SENSOR_MIPI_4_LANE,//mipi lane num
	.i2c_addr_table = { 0x6C, 0xff},
	.i2c_speed = 400,
};
//how to make sure of this shutter and gain jack ???
static struct imgsensor_struct imgsensor = {
	.mirror = IMAGE_NORMAL,
	.sensor_mode = IMGSENSOR_MODE_INIT,
	.shutter = 0x3D0,
	.gain = 0x100,
	.dummy_pixel = 0,
	.dummy_line = 0,
	.current_fps = 30,
	.autoflicker_en = KAL_FALSE,
	.test_pattern = KAL_FALSE,
	.current_scenario_id = MSDK_SCENARIO_ID_CAMERA_PREVIEW,
	.ihdr_en = 0,
	.i2c_write_id = 0x6c,
        /*Yuan.Yuan@ODM_WT.CAMERA.Driver  20191226 modify for exiting long shutter not correctly when shutter is in 0.5s~1.5s*/
        .current_ae_effective_frame = 1,//N+1
};

static struct SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[10] = {
//Preview OK
	{ 4112, 3088,    0,    0, 4112, 3088, 2056, 1544,
	4,    4, 2048, 1536,    0,    0, 2048, 1536},
//capture
	{ 4112, 3088,    0,    0, 4112, 3088, 4112, 3088,
	8,    8, 4096, 3072,    0,    0, 4096, 3072},
//normal-video
	{ 4112, 3088,    0,    0, 4112, 3088, 4112, 3088,
	8,    8, 4096, 3072,    0,    0, 4096, 3072},
//hs-video
	{ 4112, 3088,   0,   800,  4096, 1472,   2048,
        736,  384,    8,   1280,  720,  0,   0,   1280,  720},/*yang.guo@ODM_WT.Camera.hal, 2019/12/21, mod customer setting  for hs video cam  */
//slim-video
	{ 4112, 3088,  0, 0, 4112,   3088,   2056,  1544,
	4, 4,  2048,  1536,  0,   0,   2048,  1536},
//custom1
	{ 4112, 3088,    0,    0, 4112, 3088, 2056, 1544,
	4,    4, 2048, 1536,    0,    0, 2048, 1536},
	#if 0
//custom2
	{ 4112, 3088,    0,    0, 4112, 3088, 4112, 3088,
	8,    8, 4096, 3072,    0,    0, 4096, 3072},
	#else
//custom2
	{ 4112, 3088,    0,    264, 4112, 2560, 4112, 2560,424,   56, 3264, 2448,    0,    0, 3264, 2448}, //*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, add customer setting  for dual cam  */
	#endif
};
/*Tian.Tian@ODM_WT.CAMERA.Driver.2019/10/18,Add for camera PDAF*/
static struct SET_PD_BLOCK_INFO_T imgsensor_pd_info = {
	 .i4OffsetX = 0,
	 .i4OffsetY = 0,
	 .i4PitchX = 32,
	 .i4PitchY = 32,
	 .i4PairNum = 8,
	 .i4SubBlkW = 16,
	 .i4SubBlkH = 8,
	 .i4BlockNumX = 128,
	 .i4BlockNumY = 96,
	 .i4PosL = {{10, 2}, {26, 2}, {2, 14}, {18, 14},
			{10, 18}, {26, 18}, {2, 30}, {18, 30} },
	 .i4PosR = {{10, 6}, {26, 6}, {2, 10}, {18, 10},
			{10, 22}, {26, 22}, {2, 26}, {18, 26} },
	 .iMirrorFlip = 0,
	 .i4BlockNumX = 128,
	 .i4BlockNumY = 96,
};

#if MULTI_WRITE
#define I2C_BUFFER_LEN 225
#else
#define I2C_BUFFER_LEN 3
#endif

static kal_uint16 ov12a10_table_write_cmos_sensor(
					kal_uint16 *para, kal_uint32 len)
{
	char puSendCmd[I2C_BUFFER_LEN];
	kal_uint32 tosend, IDX;
	kal_uint16 addr = 0, addr_last = 0, data;

	tosend = 0;
	IDX = 0;
	while (len > IDX) {
		addr = para[IDX];

		{
			puSendCmd[tosend++] = (char)(addr >> 8);
			puSendCmd[tosend++] = (char)(addr & 0xFF);
			data = para[IDX + 1];
			puSendCmd[tosend++] = (char)(data & 0xFF);
			IDX += 2;
			addr_last = addr;

		}
#if MULTI_WRITE
		if ((I2C_BUFFER_LEN - tosend) < 3 ||
			len == IDX ||
			addr != addr_last) {
			iBurstWriteReg_multi(puSendCmd, tosend,
				imgsensor.i2c_write_id,
				3, imgsensor_info.i2c_speed);

			tosend = 0;
		}
#else
		iWriteRegI2C(puSendCmd, 3, imgsensor.i2c_write_id);
		tosend = 0;

#endif
	}
	return 0;
}

static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte = 0;
	char pusendcmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };

	iReadRegI2C(pusendcmd, 2, (u8 *)&get_byte, 1, imgsensor.i2c_write_id);

	return get_byte;
}

static void write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
	char pusendcmd[4] = {(char)(addr >> 8),
		(char)(addr & 0xFF), (char)(para & 0xFF)};

	iWriteRegI2C(pusendcmd, 3, imgsensor.i2c_write_id);
}

static void set_dummy(void)
{
	//check

	write_cmos_sensor(0x380c, imgsensor.line_length >> 8);
	write_cmos_sensor(0x380d, imgsensor.line_length & 0xFF);
	write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
	write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
}

static void set_max_framerate(UINT16 framerate, kal_bool min_framelength_en)
{
	//kal_int16 dummy_line;
	kal_uint32 frame_length = imgsensor.frame_length;

	frame_length = imgsensor.pclk / framerate * 10 /
		imgsensor.line_length;
	LOG_INF("frame_length =%d,imgsensor.frame_length=%d\n", frame_length,imgsensor.frame_length);
	spin_lock(&imgsensor_drv_lock);
	imgsensor.frame_length =
		(frame_length > imgsensor.min_frame_length) ?
		frame_length : imgsensor.min_frame_length;
	imgsensor.dummy_line = imgsensor.frame_length -
		imgsensor.min_frame_length;

	LOG_INF("frame_length =%d,imgsensor.frame_length=%d\n", frame_length,imgsensor.frame_length);
	if (imgsensor.frame_length > imgsensor_info.max_frame_length) {
		imgsensor.frame_length = imgsensor_info.max_frame_length;
		imgsensor.dummy_line = imgsensor.frame_length -
			imgsensor.min_frame_length;
	}
	if (min_framelength_en)
		imgsensor.min_frame_length = imgsensor.frame_length;
	spin_unlock(&imgsensor_drv_lock);
	LOG_INF("frame_length =%d,imgsensor.frame_length=%d\n", frame_length,imgsensor.frame_length);

	set_dummy();  //adjust the fps and write it to the sensor
}

/*Yuan.Yuan@ODM_WT.CAMERA.Driver  20191226 modify for exiting long shutter not correctly when shutter is in 0.5s~1.5s*/
static int frame_skip = -1;
static void write_shutter(kal_uint32 shutter)
{
    kal_uint16 realtime_fps = 0;
    unsigned int reg_line_long_exp = 0;
    if (frame_skip > 0) {
        frame_skip--;
        //LOG_INF("frame_skip= %d",frame_skip);
    }
    spin_lock(&imgsensor_drv_lock);
    if (shutter > imgsensor.min_frame_length - imgsensor_info.margin)
        imgsensor.frame_length = shutter + imgsensor_info.margin;
    else
        imgsensor.frame_length = imgsensor.min_frame_length;

    if (imgsensor.frame_length > imgsensor_info.max_frame_length)
        imgsensor.frame_length = imgsensor_info.max_frame_length;
    spin_unlock(&imgsensor_drv_lock);

    shutter = (shutter < imgsensor_info.min_shutter) ? imgsensor_info.min_shutter : shutter;
    shutter = shutter > imgsensor_info.max_frame_length ? shutter : (shutter > (imgsensor_info.max_frame_length - imgsensor_info.margin)) ? (imgsensor_info.max_frame_length - imgsensor_info.margin) : shutter;

    //frame_length and shutter should be an even number.
    shutter = (shutter >> 1) << 1;
    imgsensor.frame_length = (imgsensor.frame_length >> 1) << 1;

    //LOG_INF("check shutter before long exposure is shutter=%ld,",shutter);

    if(shutter > imgsensor_info.max_frame_length){//max_frame_length 7FFF
        //Frame exposure mode customization for LE
        imgsensor.ae_frm_mode.frame_mode_1 = IMGSENSOR_AE_MODE_SE;
        imgsensor.ae_frm_mode.frame_mode_2 = IMGSENSOR_AE_MODE_SE;
        imgsensor.current_ae_effective_frame = 1;

        frame_skip = 1;//need to skip N+1 and then shutdown long exposure

        reg_line_long_exp = (shutter - (imgsensor.frame_length - imgsensor_info.margin))*imgsensor.line_length/256;//calculate expo time
        LOG_INF("longexposure gogo mtk enable shutter is %d, 0x3C92->0x%x, 0x3C93->0x%x, 0x3C94->0x%x, 0x3C95->0x%x",shutter, (reg_line_long_exp>>24) & 0xFF, (reg_line_long_exp>>16) & 0xFF, (reg_line_long_exp>>8) & 0xFF, (reg_line_long_exp>>0) & 0xFF);
        write_cmos_sensor(0x3C80,0x0C);
        write_cmos_sensor(0x3C90,0x01);

        write_cmos_sensor(0x3C92,(reg_line_long_exp>>24) & 0xFF);
        write_cmos_sensor(0x3C93,(reg_line_long_exp>>16) & 0xFF);
        write_cmos_sensor(0x3C94,(reg_line_long_exp>>8) & 0xFF);
        write_cmos_sensor(0x3C95,(reg_line_long_exp>>0) & 0xFF);

        write_cmos_sensor(0x3500, ((imgsensor.frame_length - imgsensor_info.margin) >> 12) & 0x0F);
        write_cmos_sensor(0x3501, ((imgsensor.frame_length - imgsensor_info.margin) >> 4) & 0xFF);
        write_cmos_sensor(0x3502, ((imgsensor.frame_length - imgsensor_info.margin) << 4) & 0xF0);

        write_cmos_sensor(0x380E, (imgsensor.frame_length) >> 8);
        write_cmos_sensor(0x380F, (imgsensor.frame_length) & 0xFF);

        write_cmos_sensor(0x3508,0x00);//gain
        write_cmos_sensor(0x3509,0x80);//gain
        write_cmos_sensor(0x350A,0x04);//DIGIgain

        write_cmos_sensor(0x3826,0x0D);// R RST FSIN
        write_cmos_sensor(0x3827,0x04);// R RST FSIN
        write_cmos_sensor(0x401A,0x40);//
        write_cmos_sensor(0x3902,0x01);//remove register when use group write
        write_cmos_sensor(0x5203,0x0C);
        write_cmos_sensor(0x5204,0x11);
        write_cmos_sensor(0x520D,0xF5);
        write_cmos_sensor(0x520E,0xF5);
        write_cmos_sensor(0x5208,0xFF);
        write_cmos_sensor(0x5201,0xA8);
        write_cmos_sensor(0x5200,0x01);
        //streaming_control(1);
        LOG_INF("read out register 0x380E->0x%x, 0x380F->0x%x",read_cmos_sensor(0x380E),read_cmos_sensor(0x380F));
        LOG_INF("read out register 0x3500->0x%x, 0x3501->0x%x, 0x3502->0x%x",read_cmos_sensor(0x3500),read_cmos_sensor(0x3501),read_cmos_sensor(0x3502));
        LOG_INF("read out register 0x3C92->0x%x, 0x3C93->0x%x, 0x3C94->0x%x, 0x3C95->0x%x",read_cmos_sensor(0x3C92), read_cmos_sensor(0x3C93),read_cmos_sensor(0x3C94),read_cmos_sensor(0x3C95));
    }else{
        if(0 == frame_skip){
            LOG_INF("longexpose exit shutter is %d,",shutter);
            //streaming_control(0);
            write_cmos_sensor(0x3C80,0x00);
            write_cmos_sensor(0x3C90,0x00);

            write_cmos_sensor(0x3826,0x00);
            write_cmos_sensor(0x3827,0x00);
            write_cmos_sensor(0x401A,0x58);
            write_cmos_sensor(0x4092,0x00);
            write_cmos_sensor(0x3714,0x24);
            write_cmos_sensor(0x5203,0x24);
            write_cmos_sensor(0x5204,0x12);
            write_cmos_sensor(0x520D,0x0F);
            write_cmos_sensor(0x520E,0xFD);
            write_cmos_sensor(0x5208,0x40);
            write_cmos_sensor(0x5201,0x94);
            write_cmos_sensor(0x5200,0x03);
            //streaming_control(1);
            frame_skip = -1;
        }
        imgsensor.current_ae_effective_frame = 1;

        //auroflicker:need to avoid 15fps and 30 fps
        if (imgsensor.autoflicker_en == KAL_TRUE) {
            realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
            /*if (realtime_fps >= 297 && realtime_fps <= 305) {
                            realtime_fps = 296;
                            set_max_framerate(realtime_fps, 0);
                    } else if (realtime_fps >= 147 && realtime_fps <= 150) {
                            realtime_fps = 146;
                            set_max_framerate(realtime_fps, 0);
                    } else {*/
                imgsensor.frame_length = (imgsensor.frame_length  >> 1) << 1;
                write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
                write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
             //}
        } else {
            imgsensor.frame_length = (imgsensor.frame_length  >> 1) << 1;

            write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
            write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
        }

        //Warning : shutter must be even. Odd might happen Unexpected Results
        write_cmos_sensor(0x3500, (shutter >> 12) & 0x0F);   //need to verify
        write_cmos_sensor(0x3501, (shutter >> 4) & 0xFF);
        write_cmos_sensor(0x3502, (shutter <<4) & 0xF0);
    }

    LOG_INF("shutter =%d, framelength =%d, realtime_fps =%d\n",
        shutter, imgsensor.frame_length, realtime_fps);
}

/*Weiwei.Cui@Camera.Driver, 2019/11/8, modify for [long shutter settings] */
static void set_shutter(kal_uint32 shutter)//check out
{
	unsigned long flags;

	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);
	write_shutter(shutter);
}

static kal_uint32 get_sensor_temperature(void)
{
	UINT32 temperature = 0;
	INT32 temperature_convert = 0;

	/*TEMP_SEN_CTL */
	write_cmos_sensor(0x4d12, 0x01);
	temperature = (read_cmos_sensor(0x4d13) << 8) |
		read_cmos_sensor(0x4d13);

	temperature_convert = 192 - temperature / 256;

	if (temperature_convert > 192)
		temperature_convert = 192;
	else if (temperature_convert < -64)
		temperature_convert = -64;

	return 20;
	//return temperature_convert;
}

static kal_uint32 streaming_control(kal_bool enable)
{
	LOG_INF("%s enable =%d\n", __func__, enable);
	if (enable)
		write_cmos_sensor(0x0100, 0x01);
	else
		write_cmos_sensor(0x0100, 0x00);

	mdelay(10);

	return ERROR_NONE;
}

static void set_shutter_frame_length(kal_uint16 shutter,
			kal_uint16 frame_length)
{
	kal_uint16 realtime_fps = 0;

	unsigned long flags;

	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);

	if (frame_length > 1)
		imgsensor.frame_length = frame_length;

	spin_lock(&imgsensor_drv_lock);
	if (shutter > imgsensor.frame_length - imgsensor_info.margin)
		imgsensor.frame_length = shutter + imgsensor_info.margin;

	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		imgsensor.frame_length = imgsensor_info.max_frame_length;

	spin_unlock(&imgsensor_drv_lock);

	shutter = (shutter < imgsensor_info.min_shutter) ?
		imgsensor_info.min_shutter : shutter;
	shutter =
		(shutter > (imgsensor_info.max_frame_length -
		imgsensor_info.margin)) ? (imgsensor_info.max_frame_length -
		imgsensor_info.margin) : shutter;

	//frame_length and shutter should be an even number.
	shutter = (shutter >> 1) << 1;
	imgsensor.frame_length = (imgsensor.frame_length >> 1) << 1;
//auroflicker:need to avoid 15fps and 30 fps
	LOG_INF("shutter =%d, framelength =%d, realtime_fps =%d,imgsensor.pclk=%d,imgsensor.line_length=%d\n",
		shutter, imgsensor.frame_length, realtime_fps,imgsensor.pclk,imgsensor.line_length);
	if (imgsensor.autoflicker_en == KAL_TRUE) {
		realtime_fps = imgsensor.pclk /
			imgsensor.line_length * 10 / imgsensor.frame_length;
		LOG_INF("shutter =%d, framelength =%d, realtime_fps =%d,imgsensor.pclk=%d,imgsensor.line_length=%d\n",
			shutter, imgsensor.frame_length, realtime_fps,imgsensor.pclk,imgsensor.line_length);
		if (realtime_fps >= 297 && realtime_fps <= 305) {
			realtime_fps = 296;
			set_max_framerate(realtime_fps, 0);
		} else if (realtime_fps >= 147 && realtime_fps <= 150) {
			realtime_fps = 146;
			set_max_framerate(realtime_fps, 0);
		} else {
			imgsensor.frame_length =
				(imgsensor.frame_length  >> 1) << 1;
			write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
			write_cmos_sensor(0x380f,
					  imgsensor.frame_length & 0xFF);
		}
	} else {
		imgsensor.frame_length = (imgsensor.frame_length  >> 1) << 1;

		write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
	}

	write_cmos_sensor(0x3500, (shutter >> 12) & 0x0F);   //need to verify
	write_cmos_sensor(0x3501, (shutter >> 4) & 0xFF);
	write_cmos_sensor(0x3502, (shutter<<4)  & 0xF0);

	LOG_INF("shutter =%d, framelength =%d, realtime_fps =%d,imgsensor.pclk=%d,imgsensor.line_length=%d\n",
		shutter, imgsensor.frame_length, realtime_fps,imgsensor.pclk,imgsensor.line_length);
}				/* set_shutter_frame_length */


static kal_uint16 gain2reg(const kal_uint16 gain)//check out
{
	kal_uint16 iReg = 0x0000;

	//platform 1xgain = 64, sensor driver 1*gain = 0x80
	iReg = gain*128/BASEGAIN;

	if (iReg < 0x80)     // sensor 1xGain
		iReg = 0X80;
	if (iReg > 0x7c0)    // sensor 15.5xGain
		iReg = 0X7C0;

	return iReg;
}

static kal_uint16 set_gain(kal_uint16 gain)//check out
{
	kal_uint16 reg_gain;
	unsigned long flags;

	reg_gain = gain2reg(gain);
	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.gain = reg_gain;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);

	LOG_INF("gain = %d , reg_gain = 0x%x\n ", gain, reg_gain);
	if(reg_gain<(0x7c0/2))
	    reg_gain=reg_gain*1;//*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, add for dual aesync  debug */
	LOG_INF("gain = %d , reg_gain = 0x%x\n ", gain, reg_gain);
	write_cmos_sensor(0x03508, (reg_gain >> 8));
	write_cmos_sensor(0x03509, (reg_gain&0xff));

	return gain;
}

static void ihdr_write_shutter_gain(kal_uint16 le,
			kal_uint16 se, kal_uint16 gain)
{
	//LOG_INF("le:0x%x, se:0x%x, gain:0x%x\n", le, se, gain);
}

static void night_mode(kal_bool enable)
{
	/* LOG_INF("night_mode do nothing"); */
}

#if MULTI_WRITE
kal_uint16 addr_data_pair_init_ov12a10[] = {
	0x0103, 0x01,
	0x0302, 0x49,
	0x0303, 0x00,
	0x0304, 0x00,
	0x0305, 0x01,
	0x030b, 0x06,
	0x030c, 0x02,
	0x030d, 0x88,
	0x030e, 0x02,
	0x0312, 0x11,
	0x0313, 0x05,
	0x3002, 0x21,
	0x3012, 0x40,
	0x3013, 0x72,
	0x3016, 0x72,
	0x301b, 0xd0,
	0x301d, 0xf0,
	0x301f, 0xd0,
	0x3021, 0x23,
	0x3022, 0x01,
	0x3106, 0x15,
	0x3107, 0x23,
	0x3500, 0x00,
	0x3501, 0xd0,
	0x3502, 0x00,
	0x3505, 0x83,
	0x3508, 0x02,
	0x3509, 0x00,
	0x3600, 0x43,
	0x3611, 0x8a,
	0x3613, 0x97,
	0x3620, 0x80,
	0x3624, 0x2c,
	0x3625, 0xa0,
	0x3626, 0x00,
	0x3631, 0x00,
	0x3632, 0x01,
	0x3641, 0x80,
	0x3642, 0x12,
	0x3644, 0x78,
	0x3645, 0xa7,
	0x364e, 0x44,
	0x364f, 0x44,
	0x3650, 0x11,
	0x3654, 0x00,
	0x3657, 0x31,
	0x3659, 0x0c,
	0x365f, 0x07,
	0x3661, 0x17,
	0x3662, 0x17,
	0x3663, 0x17,
	0x3664, 0x17,
	0x3666, 0x08,
	0x366b, 0x20,
	0x366c, 0xa4,
	0x366d, 0x20,
	0x366e, 0xa4,
	0x3680, 0x00,
	0x3714, 0x24,
	0x371a, 0x3e,
	0x3737, 0x04,
	0x3739, 0x12,
	0x3765, 0x20,
	0x3767, 0x00,
	0x37a1, 0x3e,
	0x37a8, 0x4d,
	0x37ab, 0x2c,
	0x37c2, 0x04,
	0x37d8, 0x03,
	0x37d9, 0x0c,
	0x37e0, 0x00,
	0x37e1, 0x0a,
	0x37e2, 0x14,
	0x37e3, 0x04,
	0x37e4, 0x2a,
	0x37e5, 0x03,
	0x37e6, 0x04,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x00,
	0x3803, 0x00,
	0x3804, 0x10,
	0x3805, 0x0f,
	0x3806, 0x0c,
	0x3807, 0x0f,
	0x3808, 0x10,
	0x3809, 0x00,
	0x380a, 0x0c,
	0x380b, 0x00,
	0x380c, 0x04,
	0x380d, 0x28,
	0x380e, 0x0d,
	0x380f, 0x12,
	0x3811, 0x0a,
	0x3813, 0x08,
	0x3814, 0x01,
	0x3815, 0x01,
	0x3816, 0x01,
	0x3817, 0x01,
	0x3820, 0xb0, /*Tian.Tian@ODM_WT.CAMERA.Driver.2019/10/15,modify for camera rotation 180*/
	0x3821, 0x00,
	0x3822, 0x91,
	0x3823, 0x18,
	0x3826, 0x00,
	0x3827, 0x00,
	0x3829, 0x03,
	0x3832, 0x08,
	0x3833, 0x30,
	0x3c80, 0x00,
	0x3c87, 0x01,
	0x3c8c, 0x1a,
	0x3c8d, 0x68,
	0x3c97, 0x02,
	0x3cc0, 0x40,
	0x3cc1, 0x54,
	0x3cc2, 0x34,
	0x3cc3, 0x04,
	0x3cc4, 0x00,
	0x3cc5, 0x00,
	0x3cc6, 0x00,
	0x3cc7, 0x00,
	0x3cc8, 0x00,
	0x3cc9, 0x00,
	0x3d8c, 0x73,
	0x3d8d, 0xc0,
	0x4001, 0x2b,
	0x4008, 0x02,
	0x4009, 0x0f,
	0x4011, 0xff,
	0x4013, 0x08,
	0x4014, 0x08,
	0x4015, 0x08,
	0x4017, 0x08,
	0x401a, 0x58,
	0x4050, 0x04,
	0x4051, 0x0b,
	0x405e, 0x20,
	0x4501, 0x00,
	0x4503, 0x00,
	0x450a, 0x04,
	0x4601, 0x30,
	0x4800, 0x00,
	0x481f, 0x30,
	0x4837, 0x0d,
	0x483c, 0x0f,
	0x484b, 0x01,
	0x4d00, 0x05,
	0x4d01, 0x19,
	0x4d02, 0xfd,
	0x4d03, 0xd1,
	0x4d04, 0xff,
	0x4d05, 0xff,
	0x5000, 0x09,
	0x5001, 0x42,
	0x5002, 0x45,
	0x5005, 0x00,
	0x5081, 0x04,
	0x5180, 0x00,
	0x5181, 0x10,
	0x5182, 0x02,
	0x5183, 0x0f,
	0x5185, 0x6c,
	0x5200, 0x03,
	0x520b, 0x07,
	0x520c, 0x0f,
	0x0100, 0x00
};
#endif

static void sensor_init(void) //check out
{
#if MULTI_WRITE
	LOG_INF("%s init_setting MULTI_WRITE\n", __func__);
	ov12a10_table_write_cmos_sensor(
		addr_data_pair_init_ov12a10,
		sizeof(addr_data_pair_init_ov12a10) /
		sizeof(kal_uint16));
#else
	LOG_INF("%s init_setting\n", __func__);

	write_cmos_sensor(0x0103, 0x01);
	write_cmos_sensor(0x0302, 0x49);
	write_cmos_sensor(0x0303, 0x00);
	write_cmos_sensor(0x0304, 0x00);
	write_cmos_sensor(0x0305, 0x01);
	write_cmos_sensor(0x030b, 0x06);
	write_cmos_sensor(0x030c, 0x02);
	write_cmos_sensor(0x030d, 0x88);
	write_cmos_sensor(0x030e, 0x02);
	write_cmos_sensor(0x0312, 0x11);
	write_cmos_sensor(0x0313, 0x05);
	write_cmos_sensor(0x3002, 0x21);
	write_cmos_sensor(0x3012, 0x40);
	write_cmos_sensor(0x3013, 0x72);
	write_cmos_sensor(0x3016, 0x72);
	write_cmos_sensor(0x301b, 0xd0);
	write_cmos_sensor(0x301d, 0xf0);
	write_cmos_sensor(0x301f, 0xd0);
	write_cmos_sensor(0x3021, 0x23);
	write_cmos_sensor(0x3022, 0x01);
	write_cmos_sensor(0x3106, 0x15);
	write_cmos_sensor(0x3107, 0x23);
	write_cmos_sensor(0x3500, 0x00);
	write_cmos_sensor(0x3501, 0xd0);
	write_cmos_sensor(0x3502, 0x00);
	write_cmos_sensor(0x3505, 0x83);
	write_cmos_sensor(0x3508, 0x02);
	write_cmos_sensor(0x3509, 0x00);
	write_cmos_sensor(0x3600, 0x43);
	write_cmos_sensor(0x3611, 0x8a);
	write_cmos_sensor(0x3613, 0x97);
	write_cmos_sensor(0x3620, 0x80);
	write_cmos_sensor(0x3624, 0x2c);
	write_cmos_sensor(0x3625, 0xa0);
	write_cmos_sensor(0x3626, 0x00);
	write_cmos_sensor(0x3631, 0x00);
	write_cmos_sensor(0x3632, 0x01);
	write_cmos_sensor(0x3641, 0x80);
	write_cmos_sensor(0x3642, 0x12);
	write_cmos_sensor(0x3644, 0x78);
	write_cmos_sensor(0x3645, 0xa7);
	write_cmos_sensor(0x364e, 0x44);
	write_cmos_sensor(0x364f, 0x44);
	write_cmos_sensor(0x3650, 0x11);
	write_cmos_sensor(0x3654, 0x00);
	write_cmos_sensor(0x3657, 0x31);
	write_cmos_sensor(0x3659, 0x0c);
	write_cmos_sensor(0x365f, 0x07);
	write_cmos_sensor(0x3661, 0x17);
	write_cmos_sensor(0x3662, 0x17);
	write_cmos_sensor(0x3663, 0x17);
	write_cmos_sensor(0x3664, 0x17);
	write_cmos_sensor(0x3666, 0x08);
	write_cmos_sensor(0x366b, 0x20);
	write_cmos_sensor(0x366c, 0xa4);
	write_cmos_sensor(0x366d, 0x20);
	write_cmos_sensor(0x366e, 0xa4);
	write_cmos_sensor(0x3680, 0x00);
	write_cmos_sensor(0x3714, 0x24);
	write_cmos_sensor(0x371a, 0x3e);
	write_cmos_sensor(0x3737, 0x04);
	write_cmos_sensor(0x3739, 0x12);
	write_cmos_sensor(0x3765, 0x20);
	write_cmos_sensor(0x3767, 0x00);
	write_cmos_sensor(0x37a1, 0x3e);
	write_cmos_sensor(0x37a8, 0x4d);
	write_cmos_sensor(0x37ab, 0x2c);
	write_cmos_sensor(0x37c2, 0x04);
	write_cmos_sensor(0x37d8, 0x03);
	write_cmos_sensor(0x37d9, 0x0c);
	write_cmos_sensor(0x37e0, 0x00);
	write_cmos_sensor(0x37e1, 0x0a);
	write_cmos_sensor(0x37e2, 0x14);
	write_cmos_sensor(0x37e3, 0x04);
	write_cmos_sensor(0x37e4, 0x2a);
	write_cmos_sensor(0x37e5, 0x03);
	write_cmos_sensor(0x37e6, 0x04);
	write_cmos_sensor(0x3800, 0x00);
	write_cmos_sensor(0x3801, 0x00);
	write_cmos_sensor(0x3802, 0x00);
	write_cmos_sensor(0x3803, 0x00);
	write_cmos_sensor(0x3804, 0x10);
	write_cmos_sensor(0x3805, 0x0f);
	write_cmos_sensor(0x3806, 0x0c);
	write_cmos_sensor(0x3807, 0x0f);
	write_cmos_sensor(0x3808, 0x10);
	write_cmos_sensor(0x3809, 0x00);
	write_cmos_sensor(0x380a, 0x0c);
	write_cmos_sensor(0x380b, 0x00);
	write_cmos_sensor(0x380c, 0x04);//428 = 1064
	write_cmos_sensor(0x380d, 0x28);
	write_cmos_sensor(0x380e, 0x0d);
	write_cmos_sensor(0x380f, 0x12);
	write_cmos_sensor(0x3811, 0x0a);
	write_cmos_sensor(0x3813, 0x08);
	write_cmos_sensor(0x3814, 0x01);
	write_cmos_sensor(0x3815, 0x01);
	write_cmos_sensor(0x3816, 0x01);
	write_cmos_sensor(0x3817, 0x01);
	write_cmos_sensor(0x3820, 0xa8);
	write_cmos_sensor(0x3821, 0x00);
	write_cmos_sensor(0x3822, 0x91);
	write_cmos_sensor(0x3823, 0x18);
	write_cmos_sensor(0x3826, 0x00);
	write_cmos_sensor(0x3827, 0x00);
	write_cmos_sensor(0x3829, 0x03);
	write_cmos_sensor(0x3832, 0x08);
	write_cmos_sensor(0x3833, 0x30);
	write_cmos_sensor(0x3c80, 0x00);
	write_cmos_sensor(0x3c87, 0x01);
	write_cmos_sensor(0x3c8c, 0x1a);
	write_cmos_sensor(0x3c8d, 0x68);
	write_cmos_sensor(0x3c97, 0x02);
	write_cmos_sensor(0x3cc0, 0x40);
	write_cmos_sensor(0x3cc1, 0x54);
	write_cmos_sensor(0x3cc2, 0x34);
	write_cmos_sensor(0x3cc3, 0x04);
	write_cmos_sensor(0x3cc4, 0x00);
	write_cmos_sensor(0x3cc5, 0x00);
	write_cmos_sensor(0x3cc6, 0x00);
	write_cmos_sensor(0x3cc7, 0x00);
	write_cmos_sensor(0x3cc8, 0x00);
	write_cmos_sensor(0x3cc9, 0x00);
	write_cmos_sensor(0x3d8c, 0x73);
	write_cmos_sensor(0x3d8d, 0xc0);
	write_cmos_sensor(0x4001, 0x2b);
	write_cmos_sensor(0x4008, 0x02);
	write_cmos_sensor(0x4009, 0x0f);
	write_cmos_sensor(0x4011, 0xff);
	write_cmos_sensor(0x4013, 0x08);
	write_cmos_sensor(0x4014, 0x08);
	write_cmos_sensor(0x4015, 0x08);
	write_cmos_sensor(0x4017, 0x08);
	write_cmos_sensor(0x401a, 0x58);
	write_cmos_sensor(0x4050, 0x04);
	write_cmos_sensor(0x4051, 0x0b);
	write_cmos_sensor(0x405e, 0x20);
	write_cmos_sensor(0x4501, 0x00);
	write_cmos_sensor(0x4503, 0x00);
	write_cmos_sensor(0x450a, 0x04);
	write_cmos_sensor(0x4601, 0x30);
	write_cmos_sensor(0x4800, 0x00);
	write_cmos_sensor(0x481f, 0x30);
	write_cmos_sensor(0x4837, 0x0d);
	write_cmos_sensor(0x483c, 0x0f);
	write_cmos_sensor(0x484b, 0x01);
	write_cmos_sensor(0x4d00, 0x05);
	write_cmos_sensor(0x4d01, 0x19);
	write_cmos_sensor(0x4d02, 0xfd);
	write_cmos_sensor(0x4d03, 0xd1);
	write_cmos_sensor(0x4d04, 0xff);
	write_cmos_sensor(0x4d05, 0xff);
	write_cmos_sensor(0x5000, 0x09);
	write_cmos_sensor(0x5001, 0x42);
	write_cmos_sensor(0x5002, 0x45);
	write_cmos_sensor(0x5005, 0x00);
	write_cmos_sensor(0x5081, 0x04);
	write_cmos_sensor(0x5180, 0x00);
	write_cmos_sensor(0x5181, 0x10);
	write_cmos_sensor(0x5182, 0x02);
	write_cmos_sensor(0x5183, 0x0f);
	write_cmos_sensor(0x5185, 0x6c);
	write_cmos_sensor(0x5200, 0x03);
	write_cmos_sensor(0x520b, 0x07);
	write_cmos_sensor(0x520c, 0x0f);
	write_cmos_sensor(0x0100, 0x00);
	mdelay(5);
#endif
}

#if MULTI_WRITE
kal_uint16 addr_data_pair_preview_ov12a10[] = {
	0x3631, 0x00,
	0x3642, 0x10,
	0x3666, 0x08,
	0x366b, 0x20,
	0x366c, 0xa4,
	0x366d, 0x20,
	0x366e, 0xa4,
	0x3714, 0x28,
	0x371a, 0x3e,
	0x3737, 0x08,
	0x3739, 0x20,
	0x37c2, 0x14,
	0x37d9, 0x0c,
	0x37e3, 0x08,
	0x37e4, 0x36,
	0x37e6, 0x08,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x00,
	0x3803, 0x00,
	0x3804, 0x10,
	0x3805, 0x1f,
	0x3806, 0x0c,
	0x3807, 0x0f,
	0x3808, 0x08,
	0x3809, 0x00,
	0x380a, 0x06,
	0x380b, 0x00,
	0x380c, 0x04,
	0x380d, 0x28,
	0x380e, 0x0d,
	0x380f, 0x12,
	0x3810, 0x00,
	0x3811, 0x04,
	0x3813, 0x04,
	0x3814, 0x03,
	0x3816, 0x03,
	0x3820, 0xb3, /*Tian.Tian@ODM_WT.CAMERA.Driver.2019/10/15,modify for camera rotation 180*/
	0x4009, 0x0d,
	0x4050, 0x04,
	0x4051, 0x0b,
	0x4501, 0x00,
	0x5002, 0x45,
	0x3501, 0x69,
	0x3502, 0x20,
	0x3820, 0xb3, /*Tian.Tian@ODM_WT.CAMERA.Driver.2019/10/15,modify for camera rotation 180*/
	0x3811, 0x04, /*Tian.Tian@ODM_WT.CAMERA.Driver.2019/11/29,modify for camera color*/
	0x3813, 0x04  /*Tian.Tian@ODM_WT.CAMERA.Driver.2019/11/29,modify for camera color*/
};
#endif

static void preview_setting(void)//jack yan check
{
	LOG_INF("%s RES_2048*1536_30fps\n", __func__);
#if MULTI_WRITE
	ov12a10_table_write_cmos_sensor(
		addr_data_pair_preview_ov12a10,
		sizeof(addr_data_pair_preview_ov12a10) /
		sizeof(kal_uint16));
#else
	write_cmos_sensor(0x3631, 0x00);
	write_cmos_sensor(0x3642, 0x10);
	write_cmos_sensor(0x3666, 0x08);
	write_cmos_sensor(0x366b, 0x20);
	write_cmos_sensor(0x366c, 0xa4);
	write_cmos_sensor(0x366d, 0x20);
	write_cmos_sensor(0x366e, 0xa4);
	write_cmos_sensor(0x3714, 0x28);
	write_cmos_sensor(0x371a, 0x3e);
	write_cmos_sensor(0x3737, 0x08);
	write_cmos_sensor(0x3739, 0x20);
	write_cmos_sensor(0x37c2, 0x14);
	write_cmos_sensor(0x37d9, 0x0c);
	write_cmos_sensor(0x37e3, 0x08);
	write_cmos_sensor(0x37e4, 0x36);
	write_cmos_sensor(0x37e6, 0x08);
	write_cmos_sensor(0x3800, 0x00);
	write_cmos_sensor(0x3801, 0x00);
	write_cmos_sensor(0x3802, 0x00);
	write_cmos_sensor(0x3803, 0x00);
	write_cmos_sensor(0x3804, 0x10);
	write_cmos_sensor(0x3805, 0x1f);//0x1f
	write_cmos_sensor(0x3806, 0x0c);
	write_cmos_sensor(0x3807, 0x0f);
	write_cmos_sensor(0x3808, 0x08);
	write_cmos_sensor(0x3809, 0x00);
	write_cmos_sensor(0x380a, 0x06);
	write_cmos_sensor(0x380b, 0x00);
	write_cmos_sensor(0x380c, 0x04);
	write_cmos_sensor(0x380d, 0x28);
	write_cmos_sensor(0x380e, 0x0d);//d12 = 3346
	write_cmos_sensor(0x380f, 0x12);
	write_cmos_sensor(0x3810, 0x00);
	write_cmos_sensor(0x3811, 0x04);
	write_cmos_sensor(0x3813, 0x04);
	write_cmos_sensor(0x3814, 0x03);
	write_cmos_sensor(0x3816, 0x03);
	write_cmos_sensor(0x3820, 0xab);
	write_cmos_sensor(0x4009, 0x0d);
	write_cmos_sensor(0x4050, 0x04);
	write_cmos_sensor(0x4051, 0x0b);
	write_cmos_sensor(0x4501, 0x00);
	write_cmos_sensor(0x5002, 0x45);
	write_cmos_sensor(0x3501, 0x69);
	write_cmos_sensor(0x3502, 0x20);
	write_cmos_sensor(0x3820, 0xab);
	write_cmos_sensor(0x3811, 0x04);
	write_cmos_sensor(0x3813, 0x04);
#endif
#if 0
//	#ifdef OV12A10_SYNC_OPEN
	write_cmos_senor(0x3002, 0x21);
	write_cmos_senor(0x3832, 0x08);
	write_cmos_senor(0x3833, 0x30);
//	#endif
#endif

}

#if MULTI_WRITE
kal_uint16 addr_data_pair_capture_15fps_ov12a10[] = {
	0x3631, 0x00,
	0x3642, 0x12,
	0x3666, 0x08,
	0x366b, 0x20,
	0x366c, 0xa4,
	0x366d, 0x20,
	0x366e, 0xa4,
	0x3714, 0x24,
	0x371a, 0x3e,
	0x3737, 0x04,
	0x3739, 0x12,
	0x37c2, 0x04,
	0x37d9, 0x0c,
	0x37e3, 0x04,
	0x37e4, 0x2a,
	0x37e6, 0x04,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x00,
	0x3803, 0x00,
	0x3804, 0x10,
	0x3805, 0x0f,
	0x3806, 0x0c,
	0x3807, 0x0f,
	0x3808, 0x10,
	0x3809, 0x00,
	0x380a, 0x0c,
	0x380b, 0x00,
	0x380e, 0x1a,
	0x380f, 0x24,
	0x3810, 0x00,
	0x3811, 0x0a,
	0x3813, 0x08,
	0x3814, 0x01,
	0x3816, 0x01,
	0x3820, 0xa8,
	0x4009, 0x0f,
	0x4050, 0x04,
	0x4051, 0x0b,
	0x4501, 0x00,
	0x5002, 0x45,
	0x3501, 0xd0,
	0x3502, 0xa0,
	0x3820, 0xa8,
	0x3811, 0x0a,
	0x3813, 0x08
};

kal_uint16 addr_data_pair_capture_24fps_ov12a10[] = {
	0x3631, 0x00,
	0x3642, 0x12,
	0x3666, 0x08,
	0x366b, 0x20,
	0x366c, 0xa4,
	0x366d, 0x20,
	0x366e, 0xa4,
	0x3714, 0x24,
	0x371a, 0x3e,
	0x3737, 0x04,
	0x3739, 0x12,
	0x37c2, 0x04,
	0x37d9, 0x0c,
	0x37e3, 0x04,
	0x37e4, 0x2a,
	0x37e6, 0x04,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x00,
	0x3803, 0x00,
	0x3804, 0x10,
	0x3805, 0x0f,
	0x3806, 0x0c,
	0x3807, 0x0f,
	0x3808, 0x10,
	0x3809, 0x00,
	0x380a, 0x0c,
	0x380b, 0x00,
	0x380e, 0x10,
	0x380f, 0x56,
	0x3810, 0x00,
	0x3811, 0x0a,
	0x3813, 0x08,
	0x3814, 0x01,
	0x3816, 0x01,
	0x3820, 0xa8,
	0x4009, 0x0f,
	0x4050, 0x04,
	0x4051, 0x0b,
	0x4501, 0x00,
	0x5002, 0x45,
	0x3501, 0xd0,
	0x3502, 0xa0,
	0x3820, 0xa8,
	0x3811, 0x0a,
	0x3813, 0x08
};

kal_uint16 addr_data_pair_capture_30fps_ov12a10[] = {
	0x3631, 0x30,
	0x3642, 0x12,
	0x3666, 0x08,
	0x366b, 0x20,
	0x366c, 0xa4,
	0x366d, 0x20,
	0x366e, 0xa4,
	0x3714, 0x24,
	0x371a, 0x3e,
	0x3737, 0x04,
	0x3739, 0x12,
	0x37c2, 0x04,
	0x37d9, 0x0c,
	0x37e3, 0x04,
	0x37e4, 0x2a,
	0x37e6, 0x04,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x00,
	0x3803, 0x00,
	0x3804, 0x10,
	0x3805, 0x0f,
	0x3806, 0x0c,
	0x3807, 0x0f,
	0x3808, 0x10,
	0x3809, 0x00,
	0x380a, 0x0c,
	0x380b, 0x00,
	0x380c, 0x04,
	0x380d, 0x28,
	0x380e, 0x0d,
	0x380f, 0x12,
	0x3810, 0x00,
	0x3811, 0x08,
	0x3813, 0x0a,
	0x3814, 0x01,
	0x3816, 0x01,
	0x3820, 0xb0, /*Tian.Tian@ODM_WT.CAMERA.Driver.2019/10/15,modify for camera rotation 180*/
	0x4009, 0x0f,
	0x4050, 0x04,
	0x4051, 0x0b,
	0x4501, 0x00,
	0x5002, 0x45,
	0x3501, 0xd0,
	0x3502, 0xa0,
	//0x0100, 0x01,
};
#endif

static void capture_setting(kal_uint16 currefps)//jack yan check
{
	LOG_INF("capture 4096*3072 currefps = %d\n",
		currefps);
#if MULTI_WRITE
	if (currefps == 150) {
		ov12a10_table_write_cmos_sensor(
			addr_data_pair_capture_15fps_ov12a10,
			sizeof(addr_data_pair_capture_15fps_ov12a10) /
			sizeof(kal_uint16));
	} else if (currefps == 240) {
		ov12a10_table_write_cmos_sensor(
			addr_data_pair_capture_24fps_ov12a10,
			sizeof(addr_data_pair_capture_24fps_ov12a10) /
			sizeof(kal_uint16));
	} else {
		ov12a10_table_write_cmos_sensor(
			addr_data_pair_capture_30fps_ov12a10,
			sizeof(addr_data_pair_capture_30fps_ov12a10) /
			sizeof(kal_uint16));
	}
#else
	if (currefps == 150) {
		//15fps for PIP
		write_cmos_sensor(0x3631, 0x00);
		write_cmos_sensor(0x3642, 0x12);
		write_cmos_sensor(0x3666, 0x08);
		write_cmos_sensor(0x366b, 0x20);
		write_cmos_sensor(0x366c, 0xa4);
		write_cmos_sensor(0x366d, 0x20);
		write_cmos_sensor(0x366e, 0xa4);
		write_cmos_sensor(0x3714, 0x24);
		write_cmos_sensor(0x371a, 0x3e);
		write_cmos_sensor(0x3737, 0x04);
		write_cmos_sensor(0x3739, 0x12);
		write_cmos_sensor(0x37c2, 0x04);
		write_cmos_sensor(0x37d9, 0x0c);
		write_cmos_sensor(0x37e3, 0x04);
		write_cmos_sensor(0x37e4, 0x2a);
		write_cmos_sensor(0x37e6, 0x04);
		write_cmos_sensor(0x3800, 0x00);
		write_cmos_sensor(0x3801, 0x00);
		write_cmos_sensor(0x3802, 0x00);
		write_cmos_sensor(0x3803, 0x00);
		write_cmos_sensor(0x3804, 0x10);
		write_cmos_sensor(0x3805, 0x0f);
		write_cmos_sensor(0x3806, 0x0c);
		write_cmos_sensor(0x3807, 0x0f);
		write_cmos_sensor(0x3808, 0x10);
		write_cmos_sensor(0x3809, 0x00);
		write_cmos_sensor(0x380a, 0x0c);
		write_cmos_sensor(0x380b, 0x00);
		write_cmos_sensor(0x380e, 0x1a);//d12 = 3346 -> 6692
		write_cmos_sensor(0x380f, 0x24);
		write_cmos_sensor(0x3810, 0x00);
		write_cmos_sensor(0x3811, 0x0a);
		write_cmos_sensor(0x3813, 0x08);
		write_cmos_sensor(0x3814, 0x01);
		write_cmos_sensor(0x3816, 0x01);
		write_cmos_sensor(0x3820, 0xa8);
		write_cmos_sensor(0x4009, 0x0f);
		write_cmos_sensor(0x4050, 0x04);
		write_cmos_sensor(0x4051, 0x0b);
		write_cmos_sensor(0x4501, 0x00);
		write_cmos_sensor(0x5002, 0x45);
		write_cmos_sensor(0x3501, 0xd0);
		write_cmos_sensor(0x3502, 0xa0);
		write_cmos_sensor(0x3820, 0xa8);
		write_cmos_sensor(0x3811, 0x0a);
		write_cmos_sensor(0x3813, 0x08);

#if 0
//	#ifdef OV12A10_SYNC_OPEN
	write_cmos_senor(0x3002, 0x21);//close the func
	write_cmos_senor(0x3832, 0x08);
	write_cmos_senor(0x3833, 0x30);
//	#endif
#endif
	} else if (currefps == 240) {
		//24fps for PIP
		write_cmos_sensor(0x3631, 0x00);
		write_cmos_sensor(0x3642, 0x12);
		write_cmos_sensor(0x3666, 0x08);
		write_cmos_sensor(0x366b, 0x20);
		write_cmos_sensor(0x366c, 0xa4);
		write_cmos_sensor(0x366d, 0x20);
		write_cmos_sensor(0x366e, 0xa4);
		write_cmos_sensor(0x3714, 0x24);
		write_cmos_sensor(0x371a, 0x3e);
		write_cmos_sensor(0x3737, 0x04);
		write_cmos_sensor(0x3739, 0x12);
		write_cmos_sensor(0x37c2, 0x04);
		write_cmos_sensor(0x37d9, 0x0c);
		write_cmos_sensor(0x37e3, 0x04);
		write_cmos_sensor(0x37e4, 0x2a);
		write_cmos_sensor(0x37e6, 0x04);
		write_cmos_sensor(0x3800, 0x00);
		write_cmos_sensor(0x3801, 0x00);
		write_cmos_sensor(0x3802, 0x00);
		write_cmos_sensor(0x3803, 0x00);
		write_cmos_sensor(0x3804, 0x10);
		write_cmos_sensor(0x3805, 0x0f);
		write_cmos_sensor(0x3806, 0x0c);
		write_cmos_sensor(0x3807, 0x0f);
		write_cmos_sensor(0x3808, 0x10);
		write_cmos_sensor(0x3809, 0x00);
		write_cmos_sensor(0x380a, 0x0c);
		write_cmos_sensor(0x380b, 0x00);
		write_cmos_sensor(0x380e, 0x10);
		write_cmos_sensor(0x380f, 0x56);
		write_cmos_sensor(0x3810, 0x00);
		write_cmos_sensor(0x3811, 0x0a);
		write_cmos_sensor(0x3813, 0x08);
		write_cmos_sensor(0x3814, 0x01);
		write_cmos_sensor(0x3816, 0x01);
		write_cmos_sensor(0x3820, 0xa8);
		write_cmos_sensor(0x4009, 0x0f);
		write_cmos_sensor(0x4050, 0x04);
		write_cmos_sensor(0x4051, 0x0b);
		write_cmos_sensor(0x4501, 0x00);
		write_cmos_sensor(0x5002, 0x45);
		write_cmos_sensor(0x3501, 0xd0);
		write_cmos_sensor(0x3502, 0xa0);
		write_cmos_sensor(0x3820, 0xa8);
		write_cmos_sensor(0x3811, 0x0a);
		write_cmos_sensor(0x3813, 0x08);
#if 0
//	#ifdef OV12A10_SYNC_OPEN
	write_cmos_senor(0x3002, 0x21);//close the func
	write_cmos_senor(0x3832, 0x08);
	write_cmos_senor(0x3833, 0x30);
//	#endif
#endif

	} else {
		//30fps
		write_cmos_sensor(0x3631, 0x30);
		write_cmos_sensor(0x3642, 0x12);
		write_cmos_sensor(0x3666, 0x08);
		write_cmos_sensor(0x366b, 0x20);
		write_cmos_sensor(0x366c, 0xa4);
		write_cmos_sensor(0x366d, 0x20);
		write_cmos_sensor(0x366e, 0xa4);
		write_cmos_sensor(0x3714, 0x24);
		write_cmos_sensor(0x371a, 0x3e);
		write_cmos_sensor(0x3737, 0x04);
		write_cmos_sensor(0x3739, 0x12);
		write_cmos_sensor(0x37c2, 0x04);
		write_cmos_sensor(0x37d9, 0x0c);
		write_cmos_sensor(0x37e3, 0x04);
		write_cmos_sensor(0x37e4, 0x2a);
		write_cmos_sensor(0x37e6, 0x04);
		write_cmos_sensor(0x3800, 0x00);
		write_cmos_sensor(0x3801, 0x00);
		write_cmos_sensor(0x3802, 0x00);
		write_cmos_sensor(0x3803, 0x00);
		write_cmos_sensor(0x3804, 0x10);
		write_cmos_sensor(0x3805, 0x0f);
		write_cmos_sensor(0x3806, 0x0c);
		write_cmos_sensor(0x3807, 0x0f);
		write_cmos_sensor(0x3808, 0x10);
		write_cmos_sensor(0x3809, 0x00);
		write_cmos_sensor(0x380a, 0x0c);
		write_cmos_sensor(0x380b, 0x00);
		write_cmos_sensor(0x380e, 0x0d);//3346
		write_cmos_sensor(0x380f, 0x12);
		write_cmos_sensor(0x3810, 0x00);
		write_cmos_sensor(0x3811, 0x0a);
		write_cmos_sensor(0x3813, 0x08);
		write_cmos_sensor(0x3814, 0x01);
		write_cmos_sensor(0x3816, 0x01);
		write_cmos_sensor(0x3820, 0xa8);
		write_cmos_sensor(0x4009, 0x0f);
		write_cmos_sensor(0x4050, 0x04);
		write_cmos_sensor(0x4051, 0x0b);
		write_cmos_sensor(0x4501, 0x00);
		write_cmos_sensor(0x5002, 0x45);
		write_cmos_sensor(0x3501, 0xd0);
		write_cmos_sensor(0x3502, 0xa0);
		write_cmos_sensor(0x3820, 0xa8);
		write_cmos_sensor(0x3811, 0x0a);
		write_cmos_sensor(0x3813, 0x08);

#if 0
//	#ifdef OV12A10_SYNC_OPEN

	write_cmos_sensor(0x3002, 0x61);
	write_cmos_sensor(0x3832, 0x18);
	write_cmos_sensor(0x3833, 0x10);
	write_cmos_sensor(0x3818, 0x02);
	write_cmos_sensor(0x3819, 0x44);
	write_cmos_sensor(0x381a, 0x1c);
	write_cmos_sensor(0x381b, 0xe0);

//	#endif
#endif
	}
#endif
}

static void normal_video_setting(kal_uint16 currefps)
{
	LOG_INF("%s RES_4096*3072_30fps\n", __func__);
#if MULTI_WRITE
	ov12a10_table_write_cmos_sensor(
		addr_data_pair_capture_30fps_ov12a10,
		sizeof(addr_data_pair_capture_30fps_ov12a10) /
		sizeof(kal_uint16));
#else
	write_cmos_sensor(0x3631, 0x30);
	write_cmos_sensor(0x3642, 0x12);
	write_cmos_sensor(0x3666, 0x08);
	write_cmos_sensor(0x366b, 0x20);
	write_cmos_sensor(0x366c, 0xa4);
	write_cmos_sensor(0x366d, 0x20);
	write_cmos_sensor(0x366e, 0xa4);
	write_cmos_sensor(0x3714, 0x24);
	write_cmos_sensor(0x371a, 0x3e);
	write_cmos_sensor(0x3737, 0x04);
	write_cmos_sensor(0x3739, 0x12);
	write_cmos_sensor(0x37c2, 0x04);
	write_cmos_sensor(0x37d9, 0x0c);
	write_cmos_sensor(0x37e3, 0x04);
	write_cmos_sensor(0x37e4, 0x2a);
	write_cmos_sensor(0x37e6, 0x04);
	write_cmos_sensor(0x3800, 0x00);
	write_cmos_sensor(0x3801, 0x00);
	write_cmos_sensor(0x3802, 0x00);
	write_cmos_sensor(0x3803, 0x00);
	write_cmos_sensor(0x3804, 0x10);
	write_cmos_sensor(0x3805, 0x0f);
	write_cmos_sensor(0x3806, 0x0c);
	write_cmos_sensor(0x3807, 0x0f);
	write_cmos_sensor(0x3808, 0x10);
	write_cmos_sensor(0x3809, 0x00);
	write_cmos_sensor(0x380a, 0x0c);
	write_cmos_sensor(0x380b, 0x00);
	write_cmos_sensor(0x380e, 0x0d);//3346
	write_cmos_sensor(0x380f, 0x12);
	write_cmos_sensor(0x3810, 0x00);
	write_cmos_sensor(0x3811, 0x0a);
	write_cmos_sensor(0x3813, 0x08);
	write_cmos_sensor(0x3814, 0x01);
	write_cmos_sensor(0x3816, 0x01);
	write_cmos_sensor(0x3820, 0xa8);
	write_cmos_sensor(0x4009, 0x0f);
	write_cmos_sensor(0x4050, 0x04);
	write_cmos_sensor(0x4051, 0x0b);
	write_cmos_sensor(0x4501, 0x00);
	write_cmos_sensor(0x5002, 0x45);
	write_cmos_sensor(0x3501, 0xd0);
	write_cmos_sensor(0x3502, 0xa0);
	write_cmos_sensor(0x3820, 0xa8);
	write_cmos_sensor(0x3811, 0x0a);
	write_cmos_sensor(0x3813, 0x08);
#endif
#if 0
//	#ifdef OV12A10_SYNC_OPEN
	write_cmos_senor(0x3002, 0x21);
	write_cmos_senor(0x3832, 0x08);
	write_cmos_senor(0x3833, 0x30);
//	#endif
#endif
}

#if MULTI_WRITE
/*yang.guo@ODM_WT.Camera.hal, 2019/12/21, mod customer setting  for hs video cam  */
kal_uint16 addr_data_pair_hs_video_ov12a10[] = {
        0x3642,0x10,
        0x3666,0x00,
        0x366b,0xa4,
        0x366c,0x20,
        0x366d,0xa4,
        0x366e,0x20,
        0x3714,0x28,
        0x371a,0x3e,
        0x3737,0x08,
        0x3739,0x20,
        0x37c2,0x14,
        0x37d9,0x0c,
        0x37e3,0x08,
        0x37e4,0x36,
        0x37e6,0x08,
        0x3800,0x00,
        0x3801,0x00,
        0x3802,0x03,
        0x3803,0x68,
        0x3804,0x0f,
        0x3805,0xff,
        0x3806,0x09,
        0x3807,0x27,
        0x3808,0x05,
        0x3809,0x00,
        0x380a,0x02,
        0x380b,0xd0,
        0x380e,0x03,
        0x380f,0x4c,
        0x3810,0x01,
        0x3811,0x84,
        0x3813,0x08,
        0x3814,0x03,
        0x3816,0x03,
        0x3820,0xb3,
        0x4009,0x0d,
        0x4050,0x04,
        0x4051,0x0b,
        0x4501,0x00,
        0x5002,0x45,
        0x3501,0x34,
        0x3502,0x40,
        0x0100,0x01
};
#endif

static void hs_video_setting(void)
{
	LOG_INF("%s RES_1280x720_120fps\n", __func__);
#if MULTI_WRITE
	ov12a10_table_write_cmos_sensor(
		addr_data_pair_hs_video_ov12a10,
		sizeof(addr_data_pair_hs_video_ov12a10) /
		sizeof(kal_uint16));
#else
	write_cmos_sensor(0x3631, 0x00);
	write_cmos_sensor(0x3642, 0x08);
	write_cmos_sensor(0x3666, 0x08);
	write_cmos_sensor(0x366b, 0x20);
	write_cmos_sensor(0x366c, 0xa4);
	write_cmos_sensor(0x366d, 0x20);
	write_cmos_sensor(0x366e, 0xa4);
	write_cmos_sensor(0x3714, 0x30);
	write_cmos_sensor(0x371a, 0x3f);
	write_cmos_sensor(0x3737, 0x08);
	write_cmos_sensor(0x3739, 0x20);
	write_cmos_sensor(0x37c2, 0x2c);
	write_cmos_sensor(0x37d9, 0x06);
	write_cmos_sensor(0x37e3, 0x08);
	write_cmos_sensor(0x37e4, 0x36);
	write_cmos_sensor(0x37e6, 0x08);
	write_cmos_sensor(0x3800, 0x00);
	write_cmos_sensor(0x3801, 0x00);
	write_cmos_sensor(0x3802, 0x00);
	write_cmos_sensor(0x3803, 0x00);
	write_cmos_sensor(0x3804, 0x10);
	write_cmos_sensor(0x3805, 0x3f);
	write_cmos_sensor(0x3806, 0x0c);
	write_cmos_sensor(0x3807, 0x0f);
	write_cmos_sensor(0x3808, 0x04);
	write_cmos_sensor(0x3809, 0x00);
	write_cmos_sensor(0x380a, 0x03);
	write_cmos_sensor(0x380b, 0x00);
	write_cmos_sensor(0x380e, 0x03);//34c = 844
	write_cmos_sensor(0x380f, 0x4c);
	write_cmos_sensor(0x3810, 0x00);
	write_cmos_sensor(0x3811, 0x06);
	write_cmos_sensor(0x3813, 0x02);
	write_cmos_sensor(0x3814, 0x07);
	write_cmos_sensor(0x3816, 0x07);
	write_cmos_sensor(0x3820, 0xac);
	write_cmos_sensor(0x4009, 0x05);
	write_cmos_sensor(0x4050, 0x02);
	write_cmos_sensor(0x4051, 0x05);
	write_cmos_sensor(0x4501, 0x30);
	write_cmos_sensor(0x5002, 0x05);
	write_cmos_sensor(0x3501, 0x34);
	write_cmos_sensor(0x3502, 0x40);
	//write_cmos_sensor(0x3820, 0xab);
	//write_cmos_sensor(0x3811, 0x84);
	//write_cmos_sensor(0x3813, 0x08);
#endif
#if 0
//    #ifdef OV12A10_SYNC_OPEN
	write_cmos_senor(0x3002, 0x21);
	write_cmos_senor(0x3832, 0x08);
	write_cmos_senor(0x3833, 0x30);
//	#endif
#endif
}

static void slim_video_setting(void)//jack yan check
{
	LOG_INF("%s RES_2048*1536_30fps\n", __func__);
	preview_setting();
}

static void custom1_setting(void)
{
	LOG_INF("CUSTOM1_setting 2048*1536_30fps\n");
#if MULTI_WRITE
	ov12a10_table_write_cmos_sensor(
		addr_data_pair_preview_ov12a10,
		sizeof(addr_data_pair_preview_ov12a10) /
		sizeof(kal_uint16));
#else
	write_cmos_sensor(0x3631, 0x00);
	write_cmos_sensor(0x3642, 0x10);
	write_cmos_sensor(0x3666, 0x08);
	write_cmos_sensor(0x366b, 0x20);
	write_cmos_sensor(0x366c, 0xa4);
	write_cmos_sensor(0x366d, 0x20);
	write_cmos_sensor(0x366e, 0xa4);
	write_cmos_sensor(0x3714, 0x28);
	write_cmos_sensor(0x371a, 0x3e);
	write_cmos_sensor(0x3737, 0x08);
	write_cmos_sensor(0x3739, 0x20);
	write_cmos_sensor(0x37c2, 0x14);
	write_cmos_sensor(0x37d9, 0x0c);
	write_cmos_sensor(0x37e3, 0x08);
	write_cmos_sensor(0x37e4, 0x36);
	write_cmos_sensor(0x37e6, 0x08);
	write_cmos_sensor(0x3800, 0x00);
	write_cmos_sensor(0x3801, 0x00);
	write_cmos_sensor(0x3802, 0x00);
	write_cmos_sensor(0x3803, 0x00);
	write_cmos_sensor(0x3804, 0x10);
	write_cmos_sensor(0x3805, 0x1f);
	write_cmos_sensor(0x3806, 0x0c);
	write_cmos_sensor(0x3807, 0x0f);
	write_cmos_sensor(0x3808, 0x08);
	write_cmos_sensor(0x3809, 0x00);
	write_cmos_sensor(0x380a, 0x06);
	write_cmos_sensor(0x380b, 0x00);
	write_cmos_sensor(0x380e, 0x0d);
	write_cmos_sensor(0x380f, 0x12);
	write_cmos_sensor(0x3810, 0x00);
	write_cmos_sensor(0x3811, 0x04);
	write_cmos_sensor(0x3813, 0x04);
	write_cmos_sensor(0x3814, 0x03);
	write_cmos_sensor(0x3816, 0x03);
	write_cmos_sensor(0x3820, 0xab);
	write_cmos_sensor(0x4009, 0x0d);
	write_cmos_sensor(0x4050, 0x04);
	write_cmos_sensor(0x4051, 0x0b);
	write_cmos_sensor(0x4501, 0x00);
	write_cmos_sensor(0x5002, 0x45);
	write_cmos_sensor(0x3501, 0x69);
	write_cmos_sensor(0x3502, 0x20);
	write_cmos_sensor(0x3820, 0xab);
	write_cmos_sensor(0x3811, 0x04);
	write_cmos_sensor(0x3813, 0x04);
#endif
}

//*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, add customer setting  for dual cam start */
#if MULTI_WRITE
kal_uint16 addr_data_pair_custom2_24fps_ov12a10[] = {
	0x3631, 0x00,
	0x3642, 0x12,
	0x3666, 0x08,
	0x366b, 0x20,
	0x366c, 0xa4,
	0x366d, 0x20,
	0x366e, 0xa4,
	0x3714, 0x24,
	0x371a, 0x3e,
	0x3737, 0x04,
	0x3739, 0x12,
	0x37c2, 0x04,
	0x37d9, 0x0c,
	0x37e3, 0x04,
	0x37e4, 0x2a,
	0x37e6, 0x04,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x01,
	0x3803, 0x08,
	0x3804, 0x10,
	0x3805, 0x0f,
	0x3806, 0x0b,
	0x3807, 0x07,
	0x3808, 0x0c,
	0x3809, 0xc0,
	0x380a, 0x09,
	0x380b, 0x90,
	0x380e, 0x10,
	0x380f, 0x85,
	0x3810, 0x01,
	0x3811, 0xaa,/*qinduilin@ODM_WT.Camera.driver, 2019/12/03, modify for dual camera opposit color*/
	0x3813, 0x38,
	0x3814, 0x01,
	0x3816, 0x01,
	0x3820, 0xb0,
	0x4009, 0x0f,
	0x4050, 0x04,
	0x4051, 0x0b,
	0x4501, 0x00,
	0x5002, 0x45,
	0x3501, 0xd0,
	0x3502, 0x00
};
#endif
//*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, add customer setting  for dual cam end */
static void custom2_setting(void)//jack yan check
{
	LOG_INF("CUSTOM2_setting 3264*2448_24fps\n");
#if MULTI_WRITE
    #if 0
	ov12a10_table_write_cmos_sensor(
		addr_data_pair_capture_30fps_ov12a10,
		sizeof(addr_data_pair_capture_30fps_ov12a10) /
		sizeof(kal_uint16));
    #else
    //*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, add customer setting  for dual cam start */
    ov12a10_table_write_cmos_sensor(
		addr_data_pair_custom2_24fps_ov12a10,
		sizeof(addr_data_pair_custom2_24fps_ov12a10) /
		sizeof(kal_uint16));
    //*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, add customer setting  for dual cam end */
    #endif
#else
	write_cmos_sensor(0x3631, 0x00);
	write_cmos_sensor(0x3642, 0x12);
	write_cmos_sensor(0x3666, 0x08);
	write_cmos_sensor(0x366b, 0x20);
	write_cmos_sensor(0x366c, 0xa4);
	write_cmos_sensor(0x366d, 0x20);
	write_cmos_sensor(0x366e, 0xa4);
	write_cmos_sensor(0x3714, 0x24);
	write_cmos_sensor(0x371a, 0x3e);
	write_cmos_sensor(0x3737, 0x04);
	write_cmos_sensor(0x3739, 0x12);
	write_cmos_sensor(0x37c2, 0x04);
	write_cmos_sensor(0x37d9, 0x0c);
	write_cmos_sensor(0x37e3, 0x04);
	write_cmos_sensor(0x37e4, 0x2a);
	write_cmos_sensor(0x37e6, 0x04);
	write_cmos_sensor(0x3800, 0x00);
	write_cmos_sensor(0x3801, 0x00);
	write_cmos_sensor(0x3802, 0x00);
	write_cmos_sensor(0x3803, 0x00);
	write_cmos_sensor(0x3804, 0x10);
	write_cmos_sensor(0x3805, 0x0f);
	write_cmos_sensor(0x3806, 0x0c);
	write_cmos_sensor(0x3807, 0x0f);
	write_cmos_sensor(0x3808, 0x10);
	write_cmos_sensor(0x3809, 0x00);
	write_cmos_sensor(0x380a, 0x0c);
	write_cmos_sensor(0x380b, 0x00);
	write_cmos_sensor(0x380e, 0x0d);
	write_cmos_sensor(0x380f, 0x12);
	write_cmos_sensor(0x3810, 0x00);
	write_cmos_sensor(0x3811, 0x0a);
	write_cmos_sensor(0x3813, 0x08);
	write_cmos_sensor(0x3814, 0x01);
	write_cmos_sensor(0x3816, 0x01);
	write_cmos_sensor(0x3820, 0xa8);
	write_cmos_sensor(0x4009, 0x0f);
	write_cmos_sensor(0x4050, 0x04);
	write_cmos_sensor(0x4051, 0x0b);
	write_cmos_sensor(0x4501, 0x00);
	write_cmos_sensor(0x5002, 0x45);
	write_cmos_sensor(0x3501, 0xd0);
	write_cmos_sensor(0x3502, 0xa0);
	write_cmos_sensor(0x3820, 0xa8);
	write_cmos_sensor(0x3811, 0x0a);
	write_cmos_sensor(0x3813, 0x08);
#endif
}

static kal_uint32 return_sensor_id(void)
{
	return ((read_cmos_sensor(0x300b) << 8) |
		read_cmos_sensor(0x300c));
}

static kal_uint64 read_head_id(void)
{
	kal_uint64 head_id = 0;
	kal_uint16 module_byte_l = 0;
	kal_uint16 module_byte_h = 0;
	kal_uint16 sensor_byte_l = 0;
	kal_uint16 sensor_byte_h = 0;
	kal_uint16 module_byte = 0;
	kal_uint16 sensor_byte = 0;

	char pusendcmd_module_l[2] = {(char)(0x00 >> 8), (char)(0x00 & 0xFF)};
	char pusendcmd_sensor_l[2] = {(char)(0x06 >> 8), (char)(0x06 & 0xFF)};
	char pusendcmd_module_h[2] = {(char)(0x01 >> 8), (char)(0x01 & 0xFF)};
	char pusendcmd_sensor_h[2] = {(char)(0x07 >> 8), (char)(0x07 & 0xFF)};

	iReadRegI2C(pusendcmd_module_l, 2, (u8 *)&module_byte_l, 1, 0xA0/*EEPROM_READ_ID*/);
	iReadRegI2C(pusendcmd_sensor_l, 2, (u8 *)&sensor_byte_l, 1, 0xA0/*EEPROM_READ_ID*/);
	iReadRegI2C(pusendcmd_module_h, 2, (u8 *)&module_byte_h, 1, 0xA0/*EEPROM_READ_ID*/);
	iReadRegI2C(pusendcmd_sensor_h, 2, (u8 *)&sensor_byte_h, 1, 0xA0/*EEPROM_READ_ID*/);

	sensor_byte = ((sensor_byte_h << 8) | (sensor_byte_l));
	module_byte = ((module_byte_h << 8) | (module_byte_l));

	head_id = (kal_uint64)((sensor_byte << 24) | module_byte);
	LOG_INF("the head id is %llx\n", head_id);

	return head_id;
}

static kal_uint32 monet_project(void) //monet:1, monetZ:2
{
	kal_uint32 version_value;

	version_value = get_Operator_Version();
	if (version_value == 111 || version_value == 112
		|| version_value == 113 || version_value == 115
		|| version_value == 116) {
		return 1;
	} else if (version_value == 101 || version_value == 114) {
		return 2;
	}

	return 0;
}

static kal_uint32 get_imgsensor_id(UINT32 *sensor_id)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
	kal_uint64 head_id = 0;

	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);

		do {
			*sensor_id = return_sensor_id();
			if (*sensor_id == imgsensor_info.sensor_id) {
				head_id = read_head_id();
				if(!check_otp_data(&monet_truly_main_ov12a10_eeprom_data, monet_truly_main_ov12a10_checksum, sensor_id)){
					break;
				} else {
					/*xiaojun.Pu@Camera.Driver, 2019/10/15, add for [add hardware_info for factory]*/
					//hardwareinfo_set_prop(HARDWARE_BACK_CAM_MOUDULE_ID, "Truly");
				}
				if ((head_id == 0x2f000002) && (monet_project() == 1)) {
//					hardwareinfo_set_prop(HARDWARE_BACK_SUB_CAM_MOUDULE_ID, "");
					LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id, *sensor_id);
					return ERROR_NONE;
				} else {
					*sensor_id = 0xFFFFFFFF;
					LOG_INF("match fail");
					return ERROR_SENSOR_CONNECT_FAIL;
				}
			}
			retry--;
		} while (retry > 0);
		i++;
		retry = 1;
	}
	if (*sensor_id != imgsensor_info.sensor_id) {
		LOG_INF("%s failed: 0x%x\n", __func__, *sensor_id);
		*sensor_id = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}

	return ERROR_NONE;
}

static kal_uint32 open(void) //check out
{
	kal_uint8 i = 0;
	kal_uint8 retry = 1;
	kal_uint32 sensor_id = 0;

	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
			sensor_id = return_sensor_id();
			if (sensor_id == imgsensor_info.sensor_id) {
				LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n",
					imgsensor.i2c_write_id, sensor_id);
				break;
			}

			retry--;
		} while (retry > 0);
		i++;
		if (sensor_id == imgsensor_info.sensor_id)
			break;
		retry = 2;
	}
	if (imgsensor_info.sensor_id != sensor_id) {
		LOG_INF("Open sensor id fail: 0x%x\n", sensor_id);
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	sensor_init();//jack_yan

	spin_lock(&imgsensor_drv_lock);
	imgsensor.autoflicker_en = KAL_FALSE;
	imgsensor.sensor_mode = IMGSENSOR_MODE_INIT;
	imgsensor.shutter = 0x3D0;   //jack
	imgsensor.gain    = 0x100;   //jack
	imgsensor.pclk = imgsensor_info.pre.pclk;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.dummy_pixel = 0;
	imgsensor.dummy_line = 0;
	imgsensor.ihdr_en = 0;
	imgsensor.test_pattern = KAL_FALSE;
	imgsensor.current_fps = imgsensor_info.pre.max_framerate;
	imgsensor.pdaf_mode = 0;
	spin_unlock(&imgsensor_drv_lock);

	return ERROR_NONE;
}

static kal_uint32 close(void)
{
        /*HuangMiao@ODM_WT.Camera.Driver.494898, 2019/10/28, ADD streaming_control */
        LOG_INF("E\n");
	streaming_control(KAL_FALSE);
	return ERROR_NONE;
}   /*  close  */

static kal_uint32 preview(
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("%s setting E\n", __func__);

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_PREVIEW;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	//imgsensor.video_mode = KAL_FALSE;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.current_fps = imgsensor.current_fps;
	//imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	preview_setting();
	return ERROR_NONE;
}

static kal_uint32 capture(
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CAPTURE;
	//PIP capture: 24fps for less than 13M, 20fps for 16M,15fps for 20M
	if (imgsensor.current_fps == imgsensor_info.cap2.max_framerate) {
		imgsensor.pclk = imgsensor_info.cap2.pclk;
		imgsensor.line_length = imgsensor_info.cap2.linelength;
		imgsensor.frame_length = imgsensor_info.cap2.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap2.framelength;
		//imgsensor.autoflicker_en = KAL_FALSE;
	//PIP capture: 24fps for less than 13M, 20fps for 16M,15fps for 20M
	} else if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {
		imgsensor.pclk = imgsensor_info.cap1.pclk;
		imgsensor.line_length = imgsensor_info.cap1.linelength;
		imgsensor.frame_length = imgsensor_info.cap1.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap1.framelength;
		//imgsensor.autoflicker_en = KAL_FALSE;
	} else {
		imgsensor.pclk = imgsensor_info.cap.pclk;
		imgsensor.line_length = imgsensor_info.cap.linelength;
		imgsensor.frame_length = imgsensor_info.cap.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap.framelength;
		//imgsensor.autoflicker_en = KAL_FALSE;
	}

	spin_unlock(&imgsensor_drv_lock);

	capture_setting(imgsensor.current_fps);

	return ERROR_NONE;
}   /* capture() */

static kal_uint32 normal_video(
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_VIDEO;
	imgsensor.pclk = imgsensor_info.normal_video.pclk;
	imgsensor.line_length =
		imgsensor_info.normal_video.linelength;
	imgsensor.frame_length =
		imgsensor_info.normal_video.framelength;
	imgsensor.min_frame_length =
		imgsensor_info.normal_video.framelength;
	spin_unlock(&imgsensor_drv_lock);
	normal_video_setting(imgsensor.current_fps);
	return ERROR_NONE;
}

static kal_uint32 hs_video(
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_HIGH_SPEED_VIDEO;
	imgsensor.pclk = imgsensor_info.hs_video.pclk;
	//imgsensor.video_mode = KAL_TRUE;
	imgsensor.line_length = imgsensor_info.hs_video.linelength;
	imgsensor.frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	hs_video_setting();
	return ERROR_NONE;
}

static kal_uint32 slim_video(
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_SLIM_VIDEO;
	imgsensor.pclk = imgsensor_info.slim_video.pclk;
	//imgsensor.video_mode = KAL_TRUE;
	imgsensor.line_length = imgsensor_info.slim_video.linelength;
	imgsensor.frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.min_frame_length =
		imgsensor_info.slim_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	slim_video_setting();
	return ERROR_NONE;
}

static kal_uint32 custom1(
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM1;
	imgsensor.pclk = imgsensor_info.custom1.pclk;
	imgsensor.line_length = imgsensor_info.custom1.linelength;
	imgsensor.frame_length = imgsensor_info.custom1.framelength;
	imgsensor.min_frame_length = imgsensor_info.custom1.framelength;
	imgsensor.current_fps = imgsensor.current_fps;
	spin_unlock(&imgsensor_drv_lock);
	custom1_setting();
	return ERROR_NONE;
}

static kal_uint32 custom2(
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM2;
	imgsensor.pclk = imgsensor_info.custom2.pclk;
	imgsensor.line_length = imgsensor_info.custom2.linelength;
	imgsensor.frame_length = imgsensor_info.custom2.framelength;
	imgsensor.min_frame_length = imgsensor_info.custom2.framelength;
	imgsensor.current_fps = imgsensor.current_fps;
	spin_unlock(&imgsensor_drv_lock);
	custom2_setting();
	return ERROR_NONE;
}

static kal_uint32 get_resolution(
		MSDK_SENSOR_RESOLUTION_INFO_STRUCT * sensor_resolution)
{
	sensor_resolution->SensorFullWidth =
		imgsensor_info.cap.grabwindow_width;
	sensor_resolution->SensorFullHeight =
		imgsensor_info.cap.grabwindow_height;

	sensor_resolution->SensorPreviewWidth =
		imgsensor_info.pre.grabwindow_width;
	sensor_resolution->SensorPreviewHeight =
		imgsensor_info.pre.grabwindow_height;

	sensor_resolution->SensorVideoWidth =
		imgsensor_info.normal_video.grabwindow_width;
	sensor_resolution->SensorVideoHeight =
		imgsensor_info.normal_video.grabwindow_height;

	sensor_resolution->SensorHighSpeedVideoWidth =
		imgsensor_info.hs_video.grabwindow_width;
	sensor_resolution->SensorHighSpeedVideoHeight =
		imgsensor_info.hs_video.grabwindow_height;

	sensor_resolution->SensorSlimVideoWidth =
		imgsensor_info.slim_video.grabwindow_width;
	sensor_resolution->SensorSlimVideoHeight =
		imgsensor_info.slim_video.grabwindow_height;

	sensor_resolution->SensorCustom2Width =
		imgsensor_info.custom2.grabwindow_width;
	sensor_resolution->SensorCustom2Height =
		imgsensor_info.custom2.grabwindow_height;

	sensor_resolution->SensorCustom1Width =
		imgsensor_info.custom1.grabwindow_width;
	sensor_resolution->SensorCustom1Height =
		imgsensor_info.custom1.grabwindow_height;
	return ERROR_NONE;
}   /*  get_resolution  */

static kal_uint32 get_info(
			enum MSDK_SCENARIO_ID_ENUM scenario_id,
			MSDK_SENSOR_INFO_STRUCT *sensor_info,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	if (scenario_id == 0)
		LOG_INF("scenario_id = %d\n", scenario_id);

	sensor_info->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
	 /* not use */
	sensor_info->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW;
	// inverse with datasheet
	sensor_info->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorInterruptDelayLines = 4; /* not use */
	sensor_info->SensorResetActiveHigh = FALSE; /* not use */
	sensor_info->SensorResetDelayCount = 5; /* not use */

	sensor_info->SensroInterfaceType =
		imgsensor_info.sensor_interface_type;
	sensor_info->MIPIsensorType = imgsensor_info.mipi_sensor_type;
	sensor_info->SettleDelayMode =
		imgsensor_info.mipi_settle_delay_mode;
	sensor_info->SensorOutputDataFormat =
		imgsensor_info.sensor_output_dataformat;

	sensor_info->CaptureDelayFrame = imgsensor_info.cap_delay_frame;
	sensor_info->PreviewDelayFrame = imgsensor_info.pre_delay_frame;
	sensor_info->VideoDelayFrame = imgsensor_info.video_delay_frame;
	sensor_info->HighSpeedVideoDelayFrame =
		imgsensor_info.hs_video_delay_frame;
	sensor_info->SlimVideoDelayFrame =
		imgsensor_info.slim_video_delay_frame;
	sensor_info->FrameTimeDelayFrame =
		imgsensor_info.frame_time_delay_frame;

	sensor_info->Custom1DelayFrame =
		imgsensor_info.custom1_delay_frame;
	sensor_info->Custom2DelayFrame =
		imgsensor_info.custom2_delay_frame;

	sensor_info->SensorMasterClockSwitch = 0; /* not use */
	sensor_info->SensorDrivingCurrent =
		imgsensor_info.isp_driving_current;
/* The frame of setting shutter default 0 for TG int */
	sensor_info->AEShutDelayFrame =
		imgsensor_info.ae_shut_delay_frame;
	/* The frame of setting sensor gain */
	sensor_info->AESensorGainDelayFrame =
		imgsensor_info.ae_sensor_gain_delay_frame;
	sensor_info->AEISPGainDelayFrame =
		imgsensor_info.ae_ispGain_delay_frame;
	sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
	sensor_info->IHDR_LE_FirstLine = imgsensor_info.ihdr_le_firstline;
	sensor_info->SensorModeNum = imgsensor_info.sensor_mode_num;
/*0: NO PDAF, 1: PDAF Raw Data mode, 2:PDAF VC mode*/
	sensor_info->PDAF_Support = 1;

//sensor_info->HDR_Support = 0; /*0: NO HDR, 1: iHDR, 2:mvHDR, 3:zHDR*/
	sensor_info->SensorMIPILaneNumber = imgsensor_info.mipi_lane_num;
	sensor_info->SensorClockFreq = imgsensor_info.mclk;
	sensor_info->SensorClockDividCount = 3; /* not use */
	sensor_info->SensorClockRisingCount = 0;
	sensor_info->SensorClockFallingCount = 2; /* not use */
	sensor_info->SensorPixelClockCount = 3; /* not use */
	sensor_info->SensorDataLatchCount = 2; /* not use */

	sensor_info->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->SensorWidthSampling = 0;  // 0 is default 1x
	sensor_info->SensorHightSampling = 0;   // 0 is default 1x
	sensor_info->SensorPacketECCOrder = 1;

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
	    sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
				imgsensor_info.pre.mipi_data_lp2hs_settle_dc;

	break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	    sensor_info->SensorGrabStartX = imgsensor_info.cap.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.cap.starty;

	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
				imgsensor_info.cap.mipi_data_lp2hs_settle_dc;
	break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
	    sensor_info->SensorGrabStartX =
			imgsensor_info.normal_video.startx;
	    sensor_info->SensorGrabStartY =
			imgsensor_info.normal_video.starty;

	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.normal_video.mipi_data_lp2hs_settle_dc;

	break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
	    sensor_info->SensorGrabStartX = imgsensor_info.hs_video.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.hs_video.starty;

	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.hs_video.mipi_data_lp2hs_settle_dc;
	break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
	    sensor_info->SensorGrabStartX =
			imgsensor_info.slim_video.startx;
	    sensor_info->SensorGrabStartY =
			imgsensor_info.slim_video.starty;

	    sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.slim_video.mipi_data_lp2hs_settle_dc;
	break;
	case MSDK_SCENARIO_ID_CUSTOM1:
		sensor_info->SensorGrabStartX = imgsensor_info.custom1.startx;
		sensor_info->SensorGrabStartY = imgsensor_info.custom1.starty;
		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.custom1.mipi_data_lp2hs_settle_dc;
		break;
	case MSDK_SCENARIO_ID_CUSTOM2:
		sensor_info->SensorGrabStartX = imgsensor_info.custom2.startx;
		sensor_info->SensorGrabStartY = imgsensor_info.custom2.starty;
		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.custom2.mipi_data_lp2hs_settle_dc;
		break;
	default:
	    sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
	    sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
	break;
	}

	return ERROR_NONE;
}   /*  get_info  */


static kal_uint32 control(
			enum MSDK_SCENARIO_ID_ENUM scenario_id,
			MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("scenario_id = %d\n", scenario_id);

	spin_lock(&imgsensor_drv_lock);
	imgsensor.current_scenario_id = scenario_id;
	spin_unlock(&imgsensor_drv_lock);

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
	    preview(image_window, sensor_config_data);
	break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	    capture(image_window, sensor_config_data);
	break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
	    normal_video(image_window, sensor_config_data);
	break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
	    hs_video(image_window, sensor_config_data);
	break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
	    slim_video(image_window, sensor_config_data);
	break;
	case MSDK_SCENARIO_ID_CUSTOM1:
		custom1(image_window, sensor_config_data);
	break;
	case MSDK_SCENARIO_ID_CUSTOM2:
		custom2(image_window, sensor_config_data);
	break;
	default:
		LOG_INF("Error ScenarioId setting");
		preview(image_window, sensor_config_data);
	return ERROR_INVALID_SCENARIO_ID;
	}

	return ERROR_NONE;
}   /* control() */

static kal_uint32 set_video_mode(UINT16 framerate)
{
	LOG_INF("%s framerate = %d\n ", __func__, framerate);
	// SetVideoMode Function should fix framerate
	if (framerate == 0)
	// Dynamic frame rate
		return ERROR_NONE;

	spin_lock(&imgsensor_drv_lock);
	if ((framerate == 300) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 296;
	else if ((framerate == 150) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 146;
	else
		imgsensor.current_fps = framerate;
	spin_unlock(&imgsensor_drv_lock);

	set_max_framerate(imgsensor.current_fps, 1);

	return ERROR_NONE;
}

static kal_uint32 set_auto_flicker_mode(kal_bool enable, UINT16 framerate)
{
	LOG_INF("%s enable = %d, framerate = %d\n",
		__func__, enable, framerate);

	spin_lock(&imgsensor_drv_lock);
	if (enable) //enable auto flicker
		imgsensor.autoflicker_en = KAL_TRUE;
	else //Cancel Auto flick
		imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);

	return ERROR_NONE;
}

static kal_uint32 set_max_framerate_by_scenario(
			enum MSDK_SCENARIO_ID_ENUM scenario_id,
			MUINT32 framerate)
{
	kal_uint32 frameHeight;

	LOG_INF("scenario_id = %d, framerate = %d\n", scenario_id, framerate);

	if (framerate == 0)
		return ERROR_NONE;

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
	    frameHeight = imgsensor_info.pre.pclk / framerate * 10 /
			imgsensor_info.pre.linelength;
	    spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
			(frameHeight > imgsensor_info.pre.framelength) ?
			(frameHeight - imgsensor_info.pre.framelength):0;
	    imgsensor.frame_length = imgsensor_info.pre.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		if (framerate == 0)
			return ERROR_NONE;
	    frameHeight = imgsensor_info.normal_video.pclk /
			framerate * 10 / imgsensor_info.normal_video.linelength;
	    spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line = (frameHeight >
			imgsensor_info.normal_video.framelength) ?
		(frameHeight - imgsensor_info.normal_video.framelength):0;
	    imgsensor.frame_length = imgsensor_info.normal_video.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
/*Liangtao.Hu@Camera.Driver, 2019/11/18, mod for when linelengths of register setting are same, setting is error*/
#if 1
		if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {//*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, fix bug for preview cannot come  to 30 fps */
			frameHeight = imgsensor_info.cap1.pclk /
				framerate * 10 / imgsensor_info.cap1.linelength;
			spin_lock(&imgsensor_drv_lock);

			imgsensor.dummy_line = (frameHeight >
				imgsensor_info.cap1.framelength) ?
			(frameHeight - imgsensor_info.cap1.framelength) : 0;
			imgsensor.frame_length =
				imgsensor_info.cap1.framelength +
				imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
		}	else if	(imgsensor.current_fps == imgsensor_info.cap2.max_framerate) {//*yaoguizhen@ODM_WT.Camera.hal, 2019/11/21, fix bug for preview cannot come  to 30 fps */
			frameHeight = imgsensor_info.cap2.pclk /
				framerate * 10 / imgsensor_info.cap2.linelength;
			spin_lock(&imgsensor_drv_lock);

			imgsensor.dummy_line = (frameHeight >
				imgsensor_info.cap2.framelength) ?
			(frameHeight - imgsensor_info.cap2.framelength):0;
			imgsensor.frame_length =
				imgsensor_info.cap2.framelength +
				imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
		}	else	{
			frameHeight = imgsensor_info.cap.pclk /
				framerate * 10 / imgsensor_info.cap.linelength;
			spin_lock(&imgsensor_drv_lock);

			imgsensor.dummy_line = (frameHeight >
				imgsensor_info.cap.framelength) ? (frameHeight -
				imgsensor_info.cap.framelength):0;
			imgsensor.frame_length =
				imgsensor_info.cap.framelength +
				imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
		}
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
#else
		frameHeight = imgsensor.pclk/
			framerate * 10 / imgsensor.line_length;
		spin_lock(&imgsensor_drv_lock);

		imgsensor.dummy_line = (frameHeight >
			imgsensor.frame_length) ? (frameHeight -
			imgsensor.frame_length):0;
		imgsensor.frame_length =
			imgsensor.frame_length +
			imgsensor.dummy_line;
		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
#endif
	break;

	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
	    frameHeight = imgsensor_info.hs_video.pclk /
			framerate * 10 / imgsensor_info.hs_video.linelength;
	    spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
			(frameHeight > imgsensor_info.hs_video.framelength) ?
			(frameHeight - imgsensor_info.hs_video.framelength):0;
		imgsensor.frame_length = imgsensor_info.hs_video.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
	    frameHeight = imgsensor_info.slim_video.pclk /
			framerate * 10 / imgsensor_info.slim_video.linelength;
	    spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
			(frameHeight > imgsensor_info.slim_video.framelength) ?
			(frameHeight - imgsensor_info.slim_video.framelength):0;
	    imgsensor.frame_length =
			imgsensor_info.slim_video.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_CUSTOM1:
	    frameHeight = imgsensor_info.custom1.pclk /
			framerate * 10 / imgsensor_info.custom1.linelength;
	    spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
			(frameHeight > imgsensor_info.custom1.framelength) ?
			(frameHeight - imgsensor_info.custom1.framelength):0;
	    imgsensor.frame_length =
			imgsensor_info.custom1.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	case MSDK_SCENARIO_ID_CUSTOM2:
	    frameHeight = imgsensor_info.custom2.pclk /
			framerate * 10 / imgsensor_info.custom1.linelength;
	    spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
			(frameHeight > imgsensor_info.custom1.framelength) ?
			(frameHeight - imgsensor_info.custom1.framelength):0;
	    imgsensor.frame_length =
			imgsensor_info.custom1.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	break;
	default:
		//coding with  preview scenario by default
	    frameHeight = imgsensor_info.pre.pclk /
			framerate * 10 / imgsensor_info.pre.linelength;
	    spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
			(frameHeight > imgsensor_info.pre.framelength) ?
			(frameHeight - imgsensor_info.pre.framelength):0;
	    imgsensor.frame_length = imgsensor_info.pre.framelength +
			imgsensor.dummy_line;
	    imgsensor.min_frame_length = imgsensor.frame_length;
	    spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
	    LOG_INF("error scenario_id = %d,use preview scenario\n",
			scenario_id);
	break;
	}
	return ERROR_NONE;
}

static kal_uint32 get_default_framerate_by_scenario(
			enum MSDK_SCENARIO_ID_ENUM scenario_id,
			MUINT32 *framerate)
{
	if (scenario_id == 0)
		LOG_INF("[3058]scenario_id = %d\n", scenario_id);

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		*framerate = imgsensor_info.pre.max_framerate;
		break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		*framerate = imgsensor_info.normal_video.max_framerate;
		break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		*framerate = imgsensor_info.cap.max_framerate;
		break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
		*framerate = imgsensor_info.hs_video.max_framerate;
		break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
		*framerate = imgsensor_info.slim_video.max_framerate;
		break;
	case MSDK_SCENARIO_ID_CUSTOM1:
		*framerate = imgsensor_info.custom1.max_framerate;
		break;
	case MSDK_SCENARIO_ID_CUSTOM2:
		*framerate = imgsensor_info.custom2.max_framerate;
		break;
	default:
		break;
	}

	return ERROR_NONE;
}

static kal_uint32 set_test_pattern_mode(kal_bool enable)
{
	LOG_INF("%s enable: %d\n", __func__, enable);

	if (enable)
		write_cmos_sensor(0x4503, 0x80);//has been tested
	else
		write_cmos_sensor(0x4503, 0x00);

	spin_lock(&imgsensor_drv_lock);
	imgsensor.test_pattern = enable;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}

static kal_uint32 feature_control(
			MSDK_SENSOR_FEATURE_ENUM feature_id,
			UINT8 *feature_para, UINT32 *feature_para_len)
{
	UINT16 *feature_return_para_16 = (UINT16 *) feature_para;
	UINT16 *feature_data_16 = (UINT16 *) feature_para;
	UINT32 *feature_return_para_32 = (UINT32 *) feature_para;
	UINT32 *feature_data_32 = (UINT32 *) feature_para;
	INT32 *feature_return_para_i32 = (INT32 *) feature_para;
	unsigned long long *feature_data =
		(unsigned long long *) feature_para;

	struct SENSOR_WINSIZE_INFO_STRUCT *wininfo;
	MSDK_SENSOR_REG_INFO_STRUCT *sensor_reg_data =
		(MSDK_SENSOR_REG_INFO_STRUCT *) feature_para;
	struct SET_PD_BLOCK_INFO_T *PDAFinfo;

	if (!((feature_id == 3040) || (feature_id == 3058)))
		LOG_INF("feature_id = %d\n", feature_id);

	switch (feature_id) {
	case SENSOR_FEATURE_GET_PERIOD:
	    *feature_return_para_16++ = imgsensor.line_length;
	    *feature_return_para_16 = imgsensor.frame_length;
	    *feature_para_len = 4;
	break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
	    *feature_return_para_32 = imgsensor.pclk;
	    *feature_para_len = 4;
	break;
	case SENSOR_FEATURE_SET_ESHUTTER:
	    set_shutter(*feature_data);//jack yan
	break;
	case SENSOR_FEATURE_SET_NIGHTMODE:
	    night_mode((BOOL) (*feature_data));
	break;
	case SENSOR_FEATURE_SET_GAIN:
	    set_gain((UINT16) *feature_data);//jack yan
	break;
	case SENSOR_FEATURE_SET_FLASHLIGHT:
	break;
	case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
	break;
	case SENSOR_FEATURE_SET_REGISTER:
	    write_cmos_sensor(sensor_reg_data->RegAddr,
			sensor_reg_data->RegData);
	break;
	case SENSOR_FEATURE_GET_REGISTER:
	    sensor_reg_data->RegData =
			read_cmos_sensor(sensor_reg_data->RegAddr);
	break;
	case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
	    *feature_return_para_32 = LENS_DRIVER_ID_DO_NOT_CARE;
	    *feature_para_len = 4;
	break;
	case SENSOR_FEATURE_SET_VIDEO_MODE:
	    set_video_mode(*feature_data);
	break;
	case SENSOR_FEATURE_CHECK_SENSOR_ID:
	    get_imgsensor_id(feature_return_para_32);
	break;
	case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
	    set_auto_flicker_mode((BOOL)*feature_data_16,
			*(feature_data_16+1));
	break;
	case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
		set_max_framerate_by_scenario(
			(enum MSDK_SCENARIO_ID_ENUM)*feature_data,
				*(feature_data+1));
	break;
	case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
	    get_default_framerate_by_scenario(
			(enum MSDK_SCENARIO_ID_ENUM)*(feature_data),
			(MUINT32 *)(uintptr_t)(*(feature_data+1)));
		break;
	case SENSOR_FEATURE_GET_PDAF_DATA:
	break;
	case SENSOR_FEATURE_SET_TEST_PATTERN:
	    set_test_pattern_mode((BOOL)*feature_data);
	break;
	case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:
	    *feature_return_para_32 = imgsensor_info.checksum_value;
	    *feature_para_len = 4;
	break;
	case SENSOR_FEATURE_SET_FRAMERATE:
	    spin_lock(&imgsensor_drv_lock);
	    imgsensor.current_fps = *feature_data_32;
		spin_unlock(&imgsensor_drv_lock);
	break;
	case SENSOR_FEATURE_GET_CROP_INFO:
	    LOG_INF("GET_CROP_INFO scenarioId:%d\n", *feature_data_32);

	    wininfo = (struct SENSOR_WINSIZE_INFO_STRUCT *)
			(uintptr_t)(*(feature_data+1));
		switch (*feature_data_32) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[1],
				sizeof(struct  SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[2],
				sizeof(struct  SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[3],
				sizeof(struct  SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[4],
				sizeof(struct  SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_CUSTOM1:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[5],
				sizeof(struct  SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_CUSTOM2:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[6],
				sizeof(struct  SENSOR_WINSIZE_INFO_STRUCT));
		break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[0],
				sizeof(struct  SENSOR_WINSIZE_INFO_STRUCT));
		break;
		}
	break;
	case SENSOR_FEATURE_SET_IHDR_SHUTTER_GAIN:
	    LOG_INF("SENSOR_SET_SENSOR_IHDR LE=%d, SE=%d, Gain=%d\n",
			(UINT16)*feature_data, (UINT16)*(feature_data+1),
			(UINT16)*(feature_data+2));
	    ihdr_write_shutter_gain((UINT16)*feature_data,
			(UINT16)*(feature_data+1), (UINT16)*(feature_data+2));
	break;

	case SENSOR_FEATURE_GET_PDAF_INFO:
		PDAFinfo = (struct SET_PD_BLOCK_INFO_T *)
			(uintptr_t)(*(feature_data+1));

		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			memcpy((void *)PDAFinfo, (void *)&imgsensor_pd_info,
				sizeof(struct SET_PD_BLOCK_INFO_T));
			break;

		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_CUSTOM1:
		case MSDK_SCENARIO_ID_CUSTOM2:
		default:
			break;
		}
	break;
	case SENSOR_FEATURE_GET_SENSOR_PDAF_CAPACITY:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 1;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *) (uintptr_t) (*(feature_data + 1)) = 1;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
			break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
			break;
		case MSDK_SCENARIO_ID_CUSTOM1:
			*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
			break;
		case MSDK_SCENARIO_ID_CUSTOM2:
			*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
			break;
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
			break;
		}
	break;
	case SENSOR_FEATURE_SET_PDAF:
		imgsensor.pdaf_mode = *feature_data_16;
	break;
	case SENSOR_FEATURE_SET_SHUTTER_FRAME_TIME:
		set_shutter_frame_length((UINT16) *feature_data,
			(UINT16) *(feature_data + 1));
		break;
	case SENSOR_FEATURE_GET_TEMPERATURE_VALUE:
		*feature_return_para_i32 = get_sensor_temperature();
		*feature_para_len = 4;
	break;
	case SENSOR_FEATURE_SET_STREAMING_SUSPEND:
		streaming_control(KAL_FALSE);
	break;
	case SENSOR_FEATURE_SET_STREAMING_RESUME:
		if (*feature_data != 0)
			set_shutter(*feature_data);
		streaming_control(KAL_TRUE);
	break;
        ///*Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview*/
	case SENSOR_FEATURE_GET_MIPI_PIXEL_RATE:
        {
                kal_uint32 rate;
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
                        rate = imgsensor_info.cap.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
                        rate = imgsensor_info.pre.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
                        rate = imgsensor_info.hs_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
                        rate = imgsensor_info.slim_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_CUSTOM1:
                        rate = imgsensor_info.custom1.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_CUSTOM2:
                        rate = imgsensor_info.custom2.mipi_pixel_rate;
			break;
		default:
			rate = imgsensor_info.pre.mipi_pixel_rate;
			break;
		}
                *(MUINT32*)(uintptr_t)(*(feature_data + 1)) = rate;
        }
	break;
        ///*Yang.Guo@ODM_WT.CAMERA.Driver 2557613,2019/12/12, Add PIXEL_RATE test for cts testDualCameraPreview*/
        /*Yuan.Yuan@ODM_WT.CAMERA.Driver  20191226 modify for exiting long shutter not correctly when shutter is in 0.5s~1.5s*/
        case SENSOR_FEATURE_GET_AE_EFFECTIVE_FRAME_FOR_LE:
            *feature_return_para_32 = imgsensor.current_ae_effective_frame;
        break;
        case SENSOR_FEATURE_GET_AE_FRAME_MODE_FOR_LE:
            memcpy(feature_return_para_32, &imgsensor.ae_frm_mode,
                sizeof(struct IMGSENSOR_AE_FRM_MODE));
        break;
	default:
	break;
	}

	return ERROR_NONE;
}   /*  feature_control()  */

static struct SENSOR_FUNCTION_STRUCT sensor_func = {
	open,
	get_info,
	get_resolution,
	feature_control,
	control,
	close
};

UINT32 OV12A10_MIPI_RAW_SensorInit(struct SENSOR_FUNCTION_STRUCT **pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc != NULL)
		*pfFunc =  &sensor_func;
	return ERROR_NONE;
}
