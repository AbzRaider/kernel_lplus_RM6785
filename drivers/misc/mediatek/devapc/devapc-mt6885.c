// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 MediaTek Inc.
 */

#include <linux/bug.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <asm-generic/io.h>

#include "devapc-mt6885.h"
#include "devapc-mtk-multi-ao.h"

static struct mtk_device_info mt6885_devices_infra[] = {
	/* sys_idx, ctrl_idx, vio_idx, device, vio_irq */
	/* 0 */
	{0, 0, 0, "MFG_S_S", true},
	{0, 1, 1, "MFG_S_S-1", true},
	{0, 2, 2, "MFG_S_S-2", true},
	{0, 3, 3, "MFG_S_S-3", true},
	{0, 4, 4, "MFG_S_S-4", true},
	{0, 5, 5, "MFG_S_S-5", true},
	{0, 6, 6, "MFG_S_S-6", true},
	{0, 7, 7, "MFG_S_S-7", true},
	{0, 8, 8, "MFG_S_S-8", true},
	{0, 9, 9, "APU_S_S", true},

	/* 10 */
	{0, 10, 10, "APU_S_S-1", true},
	{0, 11, 11, "APU_S_S-2", true},
	{0, 12, 12, "APU_S_S-3", true},
	{0, 13, 13, "APU_S_S-4", true},
	{0, 14, 14, "APU_S_S-5", true},
	{0, 15, 15, "MCUSYS_CFGREG_APB_S", true},
	{0, 16, 16, "MCUSYS_CFGREG_APB_S-1", true},
	{0, 17, 17, "L3C_S", true},
	{0, 18, 18, "L3C_S-1", true},
	{0, 19, 410, "APU_SRAM_S", true},

	/* 20 */
	{0, 20, 411, "MM_SRAM_S", true},
	{1, 0, 19, "MM_S_S", true},
	{1, 1, 20, "MM_S_S-1", true},
	{1, 2, 21, "MM_S_S-2", true},
	{1, 3, 22, "MM_S_S-3", true},
	{1, 4, 23, "MM_S_S-4", true},
	{1, 5, 24, "MM_S_S-5", true},
	{1, 6, 25, "MM_S_S-6", true},
	{1, 7, 26, "MM_S_S-7", true},
	{1, 8, 27, "MM_S_S-8", true},

	/* 30 */
	{1, 9, 28, "MM_S_S-9", true},
	{1, 10, 29, "MM_S_S-10", true},
	{1, 11, 30, "MM_S_S-11", true},
	{1, 12, 31, "MM_S_S-12", true},
	{1, 13, 32, "MM_S_S-13", true},
	{1, 14, 33, "MM_S_S-14", true},
	{1, 15, 34, "MM_S_S-15", true},
	{1, 16, 35, "MM_S_S-16", true},
	{1, 17, 36, "MM_S_S-17", true},
	{1, 18, 37, "MM_S_S-18", true},

	/* 40 */
	{1, 19, 38, "MM_S_S-19", true},
	{1, 20, 39, "MM_S_S-20", true},
	{1, 21, 40, "MM_S_S-21", true},
	{1, 22, 41, "MM_S_S-22", true},
	{1, 23, 42, "MM_S_S-23", true},
	{1, 24, 43, "MM_S_S-24", true},
	{1, 25, 44, "MM_S_S-25", true},
	{1, 26, 45, "MM_S_S-26", true},
	{1, 27, 46, "MM_S_S-27", true},
	{1, 28, 47, "MM_S_S-28", true},

	/* 50 */
	{1, 29, 48, "MM_S_S-29", true},
	{1, 30, 49, "MM_S_S-30", true},
	{1, 31, 50, "MM_S_S-31", true},
	{1, 32, 51, "MM_S_S-32", true},
	{1, 33, 52, "MM_S_S-33", true},
	{1, 34, 53, "MM_S_S-34", true},
	{1, 35, 54, "MM_S_S-35", true},
	{1, 36, 55, "MM_S_S-36", true},
	{1, 37, 56, "MM_S_S-37", true},
	{1, 38, 57, "MM_S_S-38", true},

	/* 60 */
	{1, 39, 58, "MM_S_S-39", true},
	{1, 40, 59, "MM_S_S-40", true},
	{1, 41, 60, "MM_S_S-41", true},
	{1, 42, 61, "MM_S_S-42", true},
	{1, 43, 62, "MM_S_S-43", true},
	{1, 44, 63, "MM_S_S-44", true},
	{1, 45, 64, "MM_S_S-45", true},
	{1, 46, 65, "MM_S_S-46", true},
	{1, 47, 66, "MM_S_S-47", true},
	{1, 48, 67, "MM_S_S-48", true},

	/* 70 */
	{1, 49, 68, "MM_S_S-49", true},
	{1, 50, 69, "MM_S_S-50", true},
	{1, 51, 70, "MM_S_S-51", true},
	{1, 52, 71, "MDP_S_S", true},
	{1, 53, 72, "MDP_S_S-1", true},
	{1, 54, 73, "MDP_S_S-2", true},
	{1, 55, 74, "MDP_S_S-3", true},
	{1, 56, 75, "MDP_S_S-4", true},
	{1, 57, 76, "MDP_S_S-5", true},
	{1, 58, 77, "MDP_S_S-6", true},

	/* 80 */
	{1, 59, 78, "MDP_S_S-7", true},
	{1, 60, 79, "MDP_S_S-8", true},
	{1, 61, 80, "MDP_S_S-9", true},
	{1, 62, 81, "MDP_S_S-10", true},
	{1, 63, 82, "MDP_S_S-11", true},
	{1, 64, 83, "MDP_S_S-12", true},
	{1, 65, 84, "MDP_S_S-13", true},
	{1, 66, 85, "MDP_S_S-14", true},
	{1, 67, 86, "MDP_S_S-15", true},
	{1, 68, 87, "MDP_S_S-16", true},

	/* 90 */
	{1, 69, 88, "MDP_S_S-17", true},
	{1, 70, 89, "MDP_S_S-18", true},
	{1, 71, 90, "MDP_S_S-19", true},
	{1, 72, 91, "MDP_S_S-20", true},
	{1, 73, 92, "MDP_S_S-21", true},
	{1, 74, 93, "MDP_S_S-22", true},
	{1, 75, 94, "MDP_S_S-23", true},
	{1, 76, 95, "MDP_S_S-24", true},
	{1, 77, 96, "MDP_S_S-25", true},
	{1, 78, 97, "MDP_S_S-26", true},

	/* 100 */
	{1, 79, 98, "MDP_S_S-27", true},
	{1, 80, 99, "MDP_S_S-28", true},
	{1, 81, 100, "MDP_S_S-29", true},
	{1, 82, 101, "MDP_S_S-30", true},
	{1, 83, 102, "MDP_S_S-31", true},
	{1, 84, 103, "MDP_S_S-32", true},
	{1, 85, 104, "MDP_S_S-33", true},
	{1, 86, 105, "MDP_S_S-34", true},
	{1, 87, 106, "MDP_S_S-35", true},
	{1, 88, 107, "MDP_S_S-36", true},

	/* 110 */
	{1, 89, 108, "MDP_S_S-37", true},
	{1, 90, 109, "MDP_S_S-38", true},
	{1, 91, 110, "MDP_S_S-39", true},
	{1, 92, 111, "MDP_S_S-40", true},
	{1, 93, 112, "MDP_S_S-41", true},
	{1, 94, 113, "MDP_S_S-42", true},
	{1, 95, 114, "MDP_S_S-43", true},
	{1, 96, 115, "MDP_S_S-44", true},
	{1, 97, 116, "MDP_S_S-45", true},
	{1, 98, 117, "MDP_S_S-46", true},

	/* 120 */
	{1, 99, 118, "MDP_S_S-47", true},
	{1, 100, 119, "MDP_S_S-48", true},
	{1, 101, 120, "MDP_S_S-49", true},
	{1, 102, 121, "MDP_S_S-50", true},
	{1, 103, 122, "MDP_S_S-51", true},
	{1, 104, 123, "MDP_S_S-52", true},
	{1, 105, 124, "MDP_S_S-53", true},
	{1, 106, 125, "MDP_S_S-54", true},
	{1, 107, 126, "MDP_S_S-55", true},
	{1, 108, 127, "MDP_S_S-56", true},

	/* 130 */
	{1, 109, 128, "MDP_S_S-57", true},
	{1, 110, 129, "MDP_S_S-58", true},
	{1, 111, 130, "MDP_S_S-59", true},
	{1, 112, 131, "MDP_S_S-60", true},
	{1, 113, 132, "MDP_S_S-61", true},
	{1, 114, 133, "MDP_S_S-62", true},
	{1, 115, 134, "MDP_S_S-63", true},
	{1, 116, 135, "MDP_S_S-64", true},
	{1, 117, 136, "MDP_S_S-65", true},
	{1, 118, 137, "MDP_S_S-66", true},

	/* 140 */
	{1, 119, 138, "MDP_S_S-67", true},
	{1, 120, 139, "MDP_S_S-68", true},
	{1, 121, 140, "MDP_S_S-69", true},
	{1, 122, 141, "MDP_S_S-70", true},
	{1, 123, 142, "MDP_S_S-71", true},
	{1, 124, 143, "MDP_S_S-72", true},
	{1, 125, 144, "MDP_S_S-73", true},
	{1, 126, 145, "MDP_S_S-74", true},
	{1, 127, 146, "MDP_S_S-75", true},
	{1, 128, 147, "MDP_S_S-76", true},

	/* 150 */
	{1, 129, 148, "MDP_S_S-77", true},
	{1, 130, 149, "MDP_S_S-78", true},
	{1, 131, 150, "MDP_S_S-79", true},
	{1, 132, 151, "MDP_S_S-80", true},
	{1, 133, 152, "MDP_S_S-81", true},
	{1, 134, 153, "MDP_S_S-82", true},
	{1, 135, 154, "MDP_S_S-83", true},
	{1, 136, 155, "MDP_S_S-84", true},
	{1, 137, 156, "MDP_S_S-85", true},
	{1, 138, 157, "MDP_S_S-86", true},

	/* 160 */
	{1, 139, 158, "MDP_S_S-87", true},
	{1, 140, 159, "MDP_S_S-88", true},
	{1, 141, 160, "MDP_S_S-89", true},
	{1, 142, 161, "MDP_S_S-90", true},
	{1, 143, 162, "MDP_S_S-91", true},
	{1, 144, 163, "MDP_S_S-92", true},
	{1, 145, 164, "MDP_S_S-93", true},
	{1, 146, 165, "MDP_S_S-94", true},
	{1, 147, 166, "MDP_S_S-95", true},
	{1, 148, 167, "MDP_S_S-96", true},

	/* 170 */
	{1, 149, 168, "MDP_S_S-97", true},
	{1, 150, 169, "MDP_S_S-98", true},
	{1, 151, 170, "MDP_S_S-99", true},
	{1, 152, 171, "MDP_S_S-100", true},
	{1, 153, 172, "MDP_S_S-101", true},
	{1, 154, 173, "MDP_S_S-102", true},
	{1, 155, 174, "MDP_S_S-103", true},
	{1, 156, 175, "MDP_S_S-104", true},
	{1, 157, 176, "MDP_S_S-105", true},
	{1, 158, 177, "MDP_S_S-106", true},

	/* 180 */
	{1, 159, 178, "MDP_S_S-107", true},
	{1, 160, 179, "MDP_S_S-108", true},
	{1, 161, 180, "MDP_S_S-109", true},
	{1, 162, 181, "MDP_S_S-110", true},
	{1, 163, 182, "MDP_S_S-111", true},
	{1, 164, 183, "MDP_S_S-112", true},
	{1, 165, 184, "MDP_S_S-113", true},
	{1, 166, 185, "MDP_S_S-114", true},
	{1, 167, 186, "MDP_S_S-115", true},
	{1, 168, 187, "MDP_S_S-116", true},

	/* 190 */
	{1, 169, 188, "MDP_S_S-117", true},
	{1, 170, 189, "MDP_S_S-118", true},
	{1, 171, 190, "MDP_S_S-119", true},
	{1, 172, 191, "MDP_S_S-120", true},
	{1, 173, 192, "MDP_S_S-121", true},
	{1, 174, 193, "MDP_S_S-122", true},
	{1, 175, 194, "MDP_S_S-123", true},
	{1, 176, 195, "MDP_S_S-124", true},
	{1, 177, 196, "MDP_S_S-125", true},
	{1, 178, 197, "MDP_S_S-126", true},

	/* 200 */
	{1, 179, 198, "MDP_S_S-127", true},
	{1, 180, 199, "MDP_S_S-128", true},
	{1, 181, 200, "MDP_S_S-129", true},
	{1, 182, 201, "MDP_S_S-130", true},
	{1, 183, 202, "MDP_S_S-131", true},
	{1, 184, 203, "MDP_S_S-132", true},
	{1, 185, 204, "MDP_S_S-133", true},
	{1, 186, 205, "MDP_S_S-134", true},
	{1, 187, 206, "MDP_S_S-135", true},
	{1, 188, 207, "MDP_S_S-136", true},

	/* 210 */
	{1, 189, 208, "MDP_S_S-137", true},
	{1, 190, 209, "MDP_S_S-138", true},
	{1, 191, 210, "MDP_S_S-139", true},
	{1, 192, 211, "MDP_S_S-140", true},
	{1, 193, 212, "MDP_S_S-141", true},
	{1, 194, 213, "MDP_S_S-142", true},
	{1, 195, 214, "MDP_S_S-143", true},
	{1, 196, 215, "MDP_S_S-144", true},
	{1, 197, 216, "MDP_S_S-145", true},
	{1, 198, 217, "MDP_S_S-146", true},

	/* 220 */
	{1, 199, 218, "MDP_S_S-147", true},
	{1, 200, 219, "MDP_S_S-148", true},
	{1, 201, 220, "MDP_S_S-149", true},
	{1, 202, 221, "MDP_S_S-150", true},
	{1, 203, 222, "MDP_S_S-151", true},
	{1, 204, 223, "MDP_S_S-152", true},
	{1, 205, 224, "MDP_S_S-153", true},
	{1, 206, 225, "MDP_S_S-154", true},
	{1, 207, 226, "MDP_S_S-155", true},
	{1, 208, 227, "MDP_S_S-156", true},

	/* 230 */
	{1, 209, 228, "MDP_S_S-157", true},
	{1, 210, 229, "MDP_S_S-158", true},
	{1, 211, 230, "MDP_S_S-159", true},
	{1, 212, 231, "MDP_S_S-160", true},
	{1, 213, 232, "MDP_S_S-161", true},
	{1, 214, 233, "MDP_S_S-162", true},
	{1, 215, 234, "MDP_S_S-163", true},
	{1, 216, 235, "MDP_S_S-164", true},
	{1, 217, 236, "MDP_S_S-165", true},
	{1, 218, 237, "MDP_S_S-166", true},

	/* 240 */
	{1, 219, 238, "MDP_S_S-167", true},
	{1, 220, 239, "MDP_S_S-168", true},
	{1, 221, 240, "MDP_S_S-169", true},
	{1, 222, 241, "MDP_S_S-170", true},
	{1, 223, 242, "MDP_S_S-171", true},
	{1, 224, 243, "MDP_S_S-172", true},
	{1, 225, 244, "MDP_S_S-173", true},
	{1, 226, 245, "MDP_S_S-174", true},
	{1, 227, 246, "MDP_S_S-175", true},
	{1, 228, 247, "MDP_S_S-176", true},

	/* 250 */
	{1, 229, 248, "MDP_S_S-177", true},
	{1, 230, 249, "MDP_S_S-178", true},
	{1, 231, 250, "MDP_S_S-179", true},
	{1, 232, 251, "MDP_S_S-180", true},
	{1, 233, 252, "MDP_S_S-181", true},
	{1, 234, 253, "MDP_S_S-182", true},
	{1, 235, 254, "MDP_S_S-183", true},
	{1, 236, 255, "MDP_S_S-184", true},
	{1, 237, 256, "MDP_S_S-185", true},
	{1, 238, 257, "MDP_S_S-186", true},

	/* 260 */
	{1, 239, 258, "MDP_S_S-187", true},
	{1, 240, 259, "MDP_S_S-188", true},
	{1, 241, 260, "MDP_S_S-189", true},
	{1, 242, 261, "MDP_S_S-190", true},
	{1, 243, 262, "MDP_S_S-191", true},
	{1, 244, 263, "MDP_S_S-192", true},
	{1, 245, 264, "MDP_S_S-193", true},
	{1, 246, 265, "MDP_S_S-194", true},
	{1, 247, 266, "MDP_S_S-195", true},
	{1, 248, 267, "MDP_S_S-196", true},

	/* 270 */
	{1, 249, 268, "MDP_S_S-197", true},
	{1, 250, 269, "MDP_S_S-198", true},
	{1, 251, 270, "MDP_S_S-199", true},
	{1, 252, 271, "MDP_S_S-200", true},
	{1, 253, 272, "MDP_S_S-201", true},
	{1, 254, 273, "MDP_S_S-202", true},
	{1, 255, 274, "MDP_S_S-203", true},
	{2, 0, 275, "MDP_S_S-204", true},
	{2, 1, 276, "MDP_S_S-205", true},
	{2, 2, 277, "MDP_S_S-206", true},

	/* 280 */
	{2, 3, 278, "MDP_S_S-207", true},
	{2, 4, 279, "MDP_S_S-208", true},
	{2, 5, 280, "MDP_S_S-209", true},
	{2, 6, 281, "MDP_S_S-210", true},
	{2, 7, 282, "MDP_S_S-211", true},
	{2, 8, 283, "MDP_S_S-212", true},
	{2, 9, 284, "MDP_S_S-213", true},
	{2, 10, 285, "MDP_S_S-214", true},
	{2, 11, 286, "MDP_S_S-215", true},
	{2, 12, 287, "MDP_S_S-216", true},

	/* 290 */
	{2, 13, 288, "MDP_S_S-217", true},
	{2, 14, 289, "MDP_S_S-218", true},
	{2, 15, 290, "MDP_S_S-219", true},
	{2, 16, 291, "MDP_S_S-220", true},
	{2, 17, 292, "MDP_S_S-221", true},
	{2, 18, 293, "MDP_S_S-222", true},
	{2, 19, 294, "MDP_S_S-223", true},
	{2, 20, 295, "MDP_S_S-224", true},
	{2, 21, 296, "MDP_S_S-225", true},
	{2, 22, 297, "MDP_S_S-226", true},

	/* 300 */
	{2, 23, 298, "MDP_S_S-227", true},
	{2, 24, 299, "MDP_S_S-228", true},
	{2, 25, 300, "MDP_S_S-229", true},
	{2, 26, 301, "MDP_S_S-230", true},
	{2, 27, 302, "MDP_S_S-231", true},
	{2, 28, 303, "MDP_S_S-232", true},
	{2, 29, 304, "MDP_S_S-233", true},
	{2, 30, 305, "MDP_S_S-234", true},
	{2, 31, 306, "MDP_S_S-235", true},
	{2, 32, 307, "MDP_S_S-236", true},

	/* 310 */
	{2, 33, 308, "MDP_S_S-237", true},
	{2, 34, 309, "MDP_S_S-238", true},
	{2, 35, 310, "MDP_S_S-239", true},
	{2, 36, 311, "MDP_S_S-240", true},
	{2, 37, 312, "MDP_S_S-241", true},
	{2, 38, 313, "MDP_S_S-242", true},
	{2, 39, 314, "MDP_S_S-243", true},
	{2, 40, 315, "MDP_S_S-244", true},
	{2, 41, 316, "MDP_S_S-245", true},
	{2, 42, 317, "MDP_S_S-246", true},

	/* 320 */
	{2, 43, 318, "MDP_S_S-247", true},
	{2, 44, 319, "MDP_S_S-248", true},
	{2, 45, 320, "MDP_S_S-249", true},
	{2, 46, 321, "MDP_S_S-250", true},
	{2, 47, 322, "MDP_S_S-251", true},
	{2, 48, 323, "MDP_S_S-252", true},
	{2, 49, 324, "MDP_S_S-253", true},
	{2, 50, 325, "MDP_S_S-254", true},
	{2, 51, 326, "MDP_S_S-255", true},
	{2, 52, 327, "MDP_S_S-256", true},

	/* 330 */
	{2, 53, 328, "MDP_S_S-257", true},
	{2, 54, 329, "MDP_S_S-258", true},
	{2, 55, 330, "MDP_S_S-259", true},
	{2, 56, 331, "MDP_S_S-260", true},
	{2, 57, 332, "MDP_S_S-261", true},
	{2, 58, 333, "MDP_S_S-262", true},
	{2, 59, 334, "MDP_S_S-263", true},
	{2, 60, 335, "MDP_S_S-264", true},
	{2, 61, 336, "MDP_S_S-265", true},
	{2, 62, 337, "MDP_S_S-266", true},

	/* 340 */
	{2, 63, 338, "MDP_S_S-267", true},
	{2, 64, 339, "MDP_S_S-268", true},
	{2, 65, 340, "MDP_S_S-269", true},
	{2, 66, 341, "MDP_S_S-270", true},
	{2, 67, 342, "MDP_S_S-271", true},
	{2, 68, 343, "MDP_S_S-272", true},
	{2, 69, 344, "MDP_S_S-273", true},
	{2, 70, 345, "MDP_S_S-274", true},
	{2, 71, 346, "MDP_S_S-275", true},
	{2, 72, 347, "MDP_S_S-276", true},

	/* 350 */
	{2, 73, 348, "MDP_S_S-277", true},
	{2, 74, 349, "MDP_S_S-278", true},
	{2, 75, 350, "MDP_S_S-279", true},
	{2, 76, 351, "MDP_S_S-280", true},
	{2, 77, 352, "MDP_S_S-281", true},
	{2, 78, 353, "MDP_S_S-282", true},
	{2, 79, 354, "MDP_S_S-283", true},
	{2, 80, 355, "MDP_S_S-284", true},
	{2, 81, 356, "MDP_S_S-285", true},
	{2, 82, 357, "MDP_S_S-286", true},

	/* 360 */
	{2, 83, 358, "MDP_S_S-287", true},
	{2, 84, 359, "MDP_S_S-288", true},
	{2, 85, 360, "MDP_S_S-289", true},
	{2, 86, 361, "MDP_S_S-290", true},
	{2, 87, 362, "MDP_S_S-291", true},
	{2, 88, 363, "MDP_S_S-292", true},
	{2, 89, 364, "MDP_S_S-293", true},
	{2, 90, 365, "MDP_S_S-294", true},
	{2, 91, 366, "MDP_S_S-295", true},
	{2, 92, 367, "MDP_S_S-296", true},

	/* 370 */
	{2, 93, 368, "MDP_S_S-297", true},
	{2, 94, 369, "MDP_S_S-298", true},
	{2, 95, 370, "DISP2_S_S", true},
	{2, 96, 371, "DISP2_S_S-1", true},
	{2, 97, 372, "DISP2_S_S-2", true},
	{2, 98, 373, "DISP2_S_S-3", true},
	{2, 99, 374, "DISP2_S_S-4", true},
	{2, 100, 375, "DISP2_S_S-5", true},
	{2, 101, 376, "DISP2_S_S-6", true},
	{2, 102, 377, "DISP2_S_S-7", true},

	/* 380 */
	{2, 103, 378, "DISP2_S_S-8", true},
	{2, 104, 379, "DISP2_S_S-9", true},
	{2, 105, 380, "DISP2_S_S-10", true},
	{2, 106, 381, "DISP2_S_S-11", true},
	{2, 107, 382, "DISP2_S_S-12", true},
	{2, 108, 383, "DISP2_S_S-13", true},
	{2, 109, 384, "DISP2_S_S-14", true},
	{2, 110, 385, "DISP2_S_S-15", true},
	{2, 111, 386, "DISP2_S_S-16", true},
	{2, 112, 387, "DISP2_S_S-17", true},

	/* 390 */
	{2, 113, 388, "DISP2_S_S-18", true},
	{2, 114, 389, "DISP2_S_S-19", true},
	{2, 115, 390, "DISP2_S_S-20", true},
	{2, 116, 391, "DISP2_S_S-21", true},
	{2, 117, 392, "DISP2_S_S-22", true},
	{2, 118, 393, "DISP2_S_S-23", true},
	{2, 119, 394, "DISP2_S_S-24", true},
	{2, 120, 395, "DISP2_S_S-25", true},
	{2, 121, 396, "DISP2_S_S-26", true},
	{2, 122, 397, "DISP2_S_S-27", true},

	/* 400 */
	{2, 123, 398, "DISP2_S_S-28", true},
	{2, 124, 399, "DISP2_S_S-29", true},
	{2, 125, 400, "DISP2_S_S-30", true},
	{2, 126, 401, "DISP2_S_S-31", true},
	{2, 127, 402, "DISP2_S_S-32", true},
	{2, 128, 403, "DISP2_S_S-33", true},
	{2, 129, 404, "DISP2_S_S-34", true},
	{2, 130, 405, "DISP2_S_S-35", true},
	{2, 131, 406, "DISP2_S_S-36", true},
	{2, 132, 407, "DISP2_S_S-37", true},

	/* 410 */
	{2, 133, 408, "DISP2_S_S-38", true},
	{2, 134, 409, "DISP2_S_S-39", true},
	{-1, -1, 412, "OOB_way_en", true},
	{-1, -1, 413, "OOB_way_en", true},
	{-1, -1, 414, "OOB_way_en", true},
	{-1, -1, 415, "OOB_way_en", true},
	{-1, -1, 416, "OOB_way_en", true},
	{-1, -1, 417, "OOB_way_en", true},
	{-1, -1, 418, "Decode_error", true},
	{-1, -1, 419, "SRAMROM", true},

	/* 420 */
	{-1, -1, 420, "CQDMA_SECURE", false},
	{-1, -1, 421, "reserve", false},
	{-1, -1, 422, "reserve", false},
	{-1, -1, 423, "DEVICE_APC_INFRA_AO", false},
	{-1, -1, 424, "DEVICE_APC_INFRA_PDN", false},
};

static struct mtk_device_info mt6885_devices_peri[] = {
	/* sys_idx, ctrl_idx, vio_idx, device, vio_irq */
	/* 0 */
	{0, 0, 0, "SPM_APB_S", true},
	{0, 1, 1, "SPM_APB_S-1", true},
	{0, 2, 2, "SPM_APB_S-2", true},
	{0, 3, 3, "SPM_APB_S-3", true},
	{0, 4, 4, "SPM_APB_S-4", true},
	{0, 5, 5, "APMIXEDSYS_APB_S", true},
	{0, 6, 6, "APMIXEDSYS_APB_S-1", true},
	{0, 7, 7, "TOPCKGEN_APB_S", true},
	{0, 8, 8, "INFRACFG_AO_APB_S", true},
	{0, 9, 9, "INFRACFG_AO_MEM_APB_S", true},

	/* 10 */
	{0, 10, 10, "PERICFG_AO_APB_S", true},
	{0, 11, 11, "GPIO_APB_S", true},
	{0, 12, 13, "TOPRGU_APB_S", true},
	{0, 13, 14, "RESERVED_APB_S", true},
	{0, 14, 15, "DEVICE_APC_INFRA_AO_APB_S", true},
	{0, 15, 16, "BCRM_INFRA_AO_APB_S", true},
	{0, 16, 17, "DEBUG_CTRL_INFRA_AO_APB_S", true},
	{0, 17, 18, "DEVICE_APC_PERI_AO_APB_S", true},
	{0, 18, 19, "BCRM_PERI_AO_APB_S", true},
	{0, 19, 20, "DEBUG_CTRL_PERI_AO_APB_S", true},

	/* 20 */
	{0, 20, 21, "AP_CIRQ_EINT_APB_S", true},
	{0, 21, 23, "PMIC_WRAP_APB_S", true},
	{0, 22, 24, "DEVICE_APC_AO_MM_APB_S", true},
	{0, 23, 25, "KP_APB_S", true},
	{0, 24, 26, "TOP_MISC_APB_S", true},
	{0, 25, 27, "DVFSRC_APB_S", true},
	{0, 26, 28, "MBIST_AO_APB_S", true},
	{0, 27, 29, "DPMAIF_AO_APB_S", true},
	{0, 28, 30, "DEVICE_MPU_AO_APB_S", true},
	{0, 29, 31, "SYS_TIMER_APB_S", true},

	/* 30 */
	{0, 30, 32, "MODEM_TEMP_SHARE_APB_S", true},
	{0, 31, 33, "DEVICE_APC_AO_MD_APB_S", true},
	{0, 32, 34, "PMIF1_APB_S", true},
	{0, 33, 35, "PMICSPI_MST_APB_S", true},
	{0, 34, 36, "TIA_APB_S", true},
	{0, 35, 37, "TOPCKGEN_INFRA_CFG_APB_S", true},
	{0, 36, 38, "DRM_DEBUG_TOP_APB_S", true},
	{0, 37, 62, "PWR_MD32_S", true},
	{0, 38, 63, "PWR_MD32_S-1", true},
	{0, 39, 64, "PWR_MD32_S-2", true},

	/* 40 */
	{0, 40, 65, "PWR_MD32_S-3", true},
	{0, 41, 66, "PWR_MD32_S-4", true},
	{0, 42, 67, "PWR_MD32_S-5", true},
	{0, 43, 68, "PWR_MD32_S-6", true},
	{0, 44, 69, "PWR_MD32_S-7", true},
	{0, 45, 70, "PWR_MD32_S-8", true},
	{0, 46, 71, "PWR_MD32_S-9", true},
	{0, 47, 72, "PWR_MD32_S-10", true},
	{0, 48, 73, "AUDIO_S", true},
	{0, 49, 74, "AUDIO_S-1", true},

	/* 50 */
	{0, 50, 75, "UFS_S", true},
	{0, 51, 76, "UFS_S-1", true},
	{0, 52, 77, "UFS_S-2", true},
	{0, 53, 78, "UFS_S-3", true},
	{0, 54, 122, "SSUSB_S", true},
	{0, 55, 123, "SSUSB_S-1", true},
	{0, 56, 124, "SSUSB_S-2", true},
	{0, 57, 125, "DEBUGSYS_APB_S", true},
	{0, 58, 127, "DRAMC_MD32_S0_APB_S", true},
	{0, 59, 128, "DRAMC_MD32_S0_APB_S-1", true},

	/* 60 */
	{0, 60, 129, "DRAMC_MD32_S1_APB_S", true},
	{0, 61, 130, "DRAMC_MD32_S1_APB_S-1", true},
	{0, 62, 136, "MSDC0_S", true},
	{0, 63, 137, "MSDC1_S", true},
	{0, 64, 143, "APDMA_APB_S", true},
	{0, 65, 144, "AUXADC_APB_S", true},
	{0, 66, 145, "UART0_APB_S", true},
	{0, 67, 146, "UART1_APB_S", true},
	{0, 68, 147, "UART2_APB_S", true},
	{0, 69, 148, "PWM_APB_S", true},

	/* 70 */
	{0, 70, 149, "SPI0_APB_S", true},
	{0, 71, 150, "PTP_THERM_CTRL_APB_S", true},
	{0, 72, 151, "BTIF_APB_S", true},
	{0, 73, 152, "DISP_PWM_APB_S", true},
	{0, 74, 153, "SPI1_APB_S", true},
	{0, 75, 154, "SPI2_APB_S", true},
	{0, 76, 155, "SPI3_APB_S", true},
	{0, 77, 156, "IIC_P2P_REMAP_APB0_S", true},
	{0, 78, 157, "IIC_P2P_REMAP_APB1_S", true},
	{0, 79, 158, "SPI4_APB_S", true},

	/* 80 */
	{0, 80, 159, "SPI5_APB_S", true},
	{0, 81, 160, "IIC_P2P_REMAP_APB2_S", true},
	{0, 82, 161, "IIC_P2P_REMAP_APB3_S", true},
	{0, 83, 162, "PERI_BUS_TRACE_APB_S", true},
	{0, 84, 163, "SPI6_APB_S", true},
	{0, 85, 164, "SPI7_APB_S", true},
	{0, 86, 165, "BCRM_PERI_PDN_APB_S", true},
	{0, 87, 166, "BCRM_PERI_PDN2_APB_S", true},
	{0, 88, 167, "DEVICE_APC_PERI_PDN_APB_S", true},
	{0, 89, 168, "DEVICE_APC_PERI_PDN2_APB_S", true},

	/* 90 */
	{0, 90, 169, "PTP_THERM_CTRL2_APB_S", true},
	{0, 91, 170, "IIC_P2P_REMAP_APB4_S", true},
	{0, 92, 171, "IIC_P2P_REMAP_APB5_S", true},
	{0, 93, 172, "DEBUG_TRACKER_APB2_S", true},
	{0, 94, 173, "DRAMC_CH0_TOP0_APB_S", true},
	{0, 95, 174, "DRAMC_CH0_TOP1_APB_S", true},
	{0, 96, 175, "DRAMC_CH0_TOP2_APB_S", true},
	{0, 97, 176, "DRAMC_CH0_TOP3_APB_S", true},
	{0, 98, 177, "DRAMC_CH0_TOP4_APB_S", true},
	{0, 99, 178, "DRAMC_CH0_TOP5_APB_S", true},

	/* 100 */
	{0, 100, 179, "DRAMC_CH0_TOP6_APB_S", true},
	{0, 101, 180, "DRAMC_CH1_TOP0_APB_S", true},
	{0, 102, 181, "DRAMC_CH1_TOP1_APB_S", true},
	{0, 103, 182, "DRAMC_CH1_TOP2_APB_S", true},
	{0, 104, 183, "DRAMC_CH1_TOP3_APB_S", true},
	{0, 105, 184, "DRAMC_CH1_TOP4_APB_S", true},
	{0, 106, 185, "DRAMC_CH1_TOP5_APB_S", true},
	{0, 107, 186, "DRAMC_CH1_TOP6_APB_S", true},
	{0, 108, 187, "DRAMC_CH2_TOP0_APB_S", true},
	{0, 109, 188, "DRAMC_CH2_TOP1_APB_S", true},

	/* 110 */
	{0, 110, 189, "DRAMC_CH2_TOP2_APB_S", true},
	{0, 111, 190, "DRAMC_CH2_TOP3_APB_S", true},
	{0, 112, 191, "DRAMC_CH2_TOP4_APB_S", true},
	{0, 113, 192, "DRAMC_CH2_TOP5_APB_S", true},
	{0, 114, 193, "DRAMC_CH2_TOP6_APB_S", true},
	{0, 115, 194, "DRAMC_CH3_TOP0_APB_S", true},
	{0, 116, 195, "DRAMC_CH3_TOP1_APB_S", true},
	{0, 117, 196, "DRAMC_CH3_TOP2_APB_S", true},
	{0, 118, 197, "DRAMC_CH3_TOP3_APB_S", true},
	{0, 119, 198, "DRAMC_CH3_TOP4_APB_S", true},

	/* 120 */
	{0, 120, 199, "DRAMC_CH3_TOP5_APB_S", true},
	{0, 121, 200, "DRAMC_CH3_TOP6_APB_S", true},
	{0, 122, 203, "CCIF2_AP_APB_S", true},
	{0, 123, 204, "CCIF2_MD_APB_S", true},
	{0, 124, 205, "CCIF3_AP_APB_S", true},
	{0, 125, 206, "CCIF3_MD_APB_S", true},
	{0, 126, 207, "CCIF4_AP_APB_S", true},
	{0, 127, 208, "CCIF4_MD_APB_S", true},
	{0, 128, 209, "INFRA_BUS_TRACE_APB_S", true},
	{0, 129, 210, "CCIF5_AP_APB_S", true},

	/* 130 */
	{0, 130, 211, "CCIF5_MD_APB_S", true},
	{0, 131, 212, "SSC_INFRA_APB0_S", true},
	{0, 132, 213, "SSC_INFRA_APB1_S", true},
	{0, 133, 214, "SSC_INFRA_APB2_S", true},
	{0, 134, 215, "DEVICE_MPU_ACP_APB_S", true},
	{1, 0, 39, "TINSYS_S", true},
	{1, 1, 40, "TINSYS_S-1", true},
	{1, 2, 41, "TINSYS_S-2", true},
	{1, 3, 42, "TINSYS_S-3", true},
	{1, 4, 43, "TINSYS_S-4", true},

	/* 140 */
	{1, 5, 44, "TINSYS_S-5", true},
	{1, 6, 45, "TINSYS_S-6", true},
	{1, 7, 46, "TINSYS_S-7", true},
	{1, 8, 47, "TINSYS_S-8", true},
	{1, 9, 48, "TINSYS_S-9", true},
	{1, 10, 49, "TINSYS_S-10", true},
	{1, 11, 50, "TINSYS_S-11", true},
	{1, 12, 51, "TINSYS_S-12", true},
	{1, 13, 52, "TINSYS_S-13", true},
	{1, 14, 53, "TINSYS_S-14", true},

	/* 150 */
	{1, 15, 54, "TINSYS_S-15", true},
	{1, 16, 55, "TINSYS_S-16", true},
	{1, 17, 56, "TINSYS_S-17", true},
	{1, 18, 57, "TINSYS_S-18", true},
	{1, 19, 58, "TINSYS_S-19", true},
	{1, 20, 59, "TINSYS_S-20", true},
	{1, 21, 60, "TINSYS_S-21", true},
	{1, 22, 61, "TINSYS_S-22", true},
	{1, 23, 79, "MD_AP_S", true},
	{1, 24, 80, "MD_AP_S-1", true},

	/* 160 */
	{1, 25, 81, "MD_AP_S-2", true},
	{1, 26, 82, "MD_AP_S-3", true},
	{1, 27, 83, "MD_AP_S-4", true},
	{1, 28, 84, "MD_AP_S-5", true},
	{1, 29, 85, "MD_AP_S-6", true},
	{1, 30, 86, "MD_AP_S-7", true},
	{1, 31, 87, "MD_AP_S-8", true},
	{1, 32, 88, "MD_AP_S-9", true},
	{1, 33, 89, "MD_AP_S-10", true},
	{1, 34, 90, "MD_AP_S-11", true},

	/* 170 */
	{1, 35, 91, "MD_AP_S-12", true},
	{1, 36, 92, "MD_AP_S-13", true},
	{1, 37, 93, "MD_AP_S-14", true},
	{1, 38, 94, "MD_AP_S-15", true},
	{1, 39, 95, "MD_AP_S-16", true},
	{1, 40, 96, "MD_AP_S-17", true},
	{1, 41, 97, "MD_AP_S-18", true},
	{1, 42, 98, "MD_AP_S-19", true},
	{1, 43, 99, "MD_AP_S-20", true},
	{1, 44, 100, "MD_AP_S-21", true},

	/* 180 */
	{1, 45, 101, "MD_AP_S-22", true},
	{1, 46, 102, "MD_AP_S-23", true},
	{1, 47, 103, "MD_AP_S-24", true},
	{1, 48, 104, "MD_AP_S-25", true},
	{1, 49, 105, "MD_AP_S-26", true},
	{1, 50, 106, "MD_AP_S-27", true},
	{1, 51, 107, "MD_AP_S-28", true},
	{1, 52, 108, "MD_AP_S-29", true},
	{1, 53, 109, "MD_AP_S-30", true},
	{1, 54, 110, "MD_AP_S-31", true},

	/* 190 */
	{1, 55, 111, "MD_AP_S-32", true},
	{1, 56, 112, "MD_AP_S-33", true},
	{1, 57, 113, "MD_AP_S-34", true},
	{1, 58, 114, "MD_AP_S-35", true},
	{1, 59, 115, "MD_AP_S-36", true},
	{1, 60, 116, "MD_AP_S-37", true},
	{1, 61, 117, "MD_AP_S-38", true},
	{1, 62, 118, "MD_AP_S-39", true},
	{1, 63, 119, "MD_AP_S-40", true},
	{1, 64, 120, "MD_AP_S-41", true},

	/* 200 */
	{1, 65, 121, "MD_AP_S-42", true},
	{2, 0, 126, "CONN_S", true},
	{-1, -1, 216, "OOB_way_en", true},
	{-1, -1, 217, "OOB_way_en", true},
	{-1, -1, 218, "OOB_way_en", true},
	{-1, -1, 219, "OOB_way_en", true},
	{-1, -1, 220, "OOB_way_en", true},
	{-1, -1, 221, "OOB_way_en", true},
	{-1, -1, 222, "OOB_way_en", true},
	{-1, -1, 223, "OOB_way_en", true},

	/* 210 */
	{-1, -1, 224, "OOB_way_en", true},
	{-1, -1, 225, "OOB_way_en", true},
	{-1, -1, 226, "OOB_way_en", true},
	{-1, -1, 227, "OOB_way_en", true},
	{-1, -1, 228, "OOB_way_en", true},
	{-1, -1, 229, "OOB_way_en", true},
	{-1, -1, 230, "OOB_way_en", true},
	{-1, -1, 231, "OOB_way_en", true},
	{-1, -1, 232, "OOB_way_en", true},
	{-1, -1, 233, "OOB_way_en", true},

	/* 220 */
	{-1, -1, 234, "OOB_way_en", true},
	{-1, -1, 235, "OOB_way_en", true},
	{-1, -1, 236, "OOB_way_en", true},
	{-1, -1, 237, "OOB_way_en", true},
	{-1, -1, 238, "OOB_way_en", true},
	{-1, -1, 239, "OOB_way_en", true},
	{-1, -1, 240, "OOB_way_en", true},
	{-1, -1, 241, "OOB_way_en", true},
	{-1, -1, 242, "OOB_way_en", true},
	{-1, -1, 243, "OOB_way_en", true},

	/* 230 */
	{-1, -1, 244, "OOB_way_en", true},
	{-1, -1, 245, "OOB_way_en", true},
	{-1, -1, 246, "OOB_way_en", true},
	{-1, -1, 247, "OOB_way_en", true},
	{-1, -1, 248, "OOB_way_en", true},
	{-1, -1, 249, "OOB_way_en", true},
	{-1, -1, 250, "OOB_way_en", true},
	{-1, -1, 251, "OOB_way_en", true},
	{-1, -1, 252, "OOB_way_en", true},
	{-1, -1, 253, "OOB_way_en", true},

	/* 240 */
	{-1, -1, 254, "OOB_way_en", true},
	{-1, -1, 255, "OOB_way_en", true},
	{-1, -1, 256, "OOB_way_en", true},
	{-1, -1, 257, "OOB_way_en", true},
	{-1, -1, 258, "OOB_way_en", true},
	{-1, -1, 259, "OOB_way_en", true},
	{-1, -1, 260, "OOB_way_en", true},
	{-1, -1, 261, "OOB_way_en", true},
	{-1, -1, 262, "OOB_way_en", true},
	{-1, -1, 263, "OOB_way_en", true},

	/* 250 */
	{-1, -1, 264, "OOB_way_en", true},
	{-1, -1, 265, "OOB_way_en", true},
	{-1, -1, 266, "OOB_way_en", true},
	{-1, -1, 267, "OOB_way_en", true},
	{-1, -1, 268, "OOB_way_en", true},
	{-1, -1, 269, "OOB_way_en", true},
	{-1, -1, 270, "OOB_way_en", true},
	{-1, -1, 271, "OOB_way_en", true},
	{-1, -1, 272, "OOB_way_en", true},
	{-1, -1, 273, "OOB_way_en", true},

	/* 260 */
	{-1, -1, 274, "OOB_way_en", true},
	{-1, -1, 275, "OOB_way_en", true},
	{-1, -1, 276, "OOB_way_en", true},
	{-1, -1, 277, "OOB_way_en", true},
	{-1, -1, 278, "OOB_way_en", true},
	{-1, -1, 279, "OOB_way_en", true},
	{-1, -1, 280, "OOB_way_en", true},
	{-1, -1, 281, "OOB_way_en", true},
	{-1, -1, 282, "OOB_way_en", true},
	{-1, -1, 283, "OOB_way_en", true},

	/* 270 */
	{-1, -1, 284, "OOB_way_en", true},
	{-1, -1, 285, "OOB_way_en", true},
	{-1, -1, 286, "OOB_way_en", true},
	{-1, -1, 287, "OOB_way_en", true},
	{-1, -1, 288, "OOB_way_en", true},
	{-1, -1, 289, "OOB_way_en", true},
	{-1, -1, 290, "OOB_way_en", true},
	{-1, -1, 291, "OOB_way_en", true},
	{-1, -1, 292, "OOB_way_en", true},
	{-1, -1, 293, "OOB_way_en", true},

	/* 280 */
	{-1, -1, 294, "OOB_way_en", true},
	{-1, -1, 295, "OOB_way_en", true},
	{-1, -1, 296, "OOB_way_en", true},
	{-1, -1, 297, "OOB_way_en", true},
	{-1, -1, 298, "OOB_way_en", true},
	{-1, -1, 299, "OOB_way_en", true},
	{-1, -1, 300, "OOB_way_en", true},
	{-1, -1, 301, "OOB_way_en", true},
	{-1, -1, 302, "OOB_way_en", true},
	{-1, -1, 303, "OOB_way_en", true},

	/* 290 */
	{-1, -1, 304, "OOB_way_en", true},
	{-1, -1, 305, "OOB_way_en", true},
	{-1, -1, 306, "OOB_way_en", true},
	{-1, -1, 307, "OOB_way_en", true},
	{-1, -1, 308, "OOB_way_en", true},
	{-1, -1, 309, "OOB_way_en", true},
	{-1, -1, 310, "OOB_way_en", true},
	{-1, -1, 311, "OOB_way_en", true},
	{-1, -1, 312, "OOB_way_en", true},
	{-1, -1, 313, "OOB_way_en", true},

	/* 300 */
	{-1, -1, 314, "OOB_way_en", true},
	{-1, -1, 315, "OOB_way_en", true},
	{-1, -1, 316, "OOB_way_en", true},
	{-1, -1, 317, "OOB_way_en", true},
	{-1, -1, 318, "OOB_way_en", true},
	{-1, -1, 319, "OOB_way_en", true},
	{-1, -1, 320, "OOB_way_en", true},
	{-1, -1, 321, "OOB_way_en", true},
	{-1, -1, 322, "OOB_way_en", true},
	{-1, -1, 323, "OOB_way_en", true},

	/* 310 */
	{-1, -1, 324, "OOB_way_en", true},
	{-1, -1, 325, "OOB_way_en", true},
	{-1, -1, 326, "OOB_way_en", true},
	{-1, -1, 327, "OOB_way_en", true},
	{-1, -1, 328, "OOB_way_en", true},
	{-1, -1, 329, "OOB_way_en", true},
	{-1, -1, 330, "OOB_way_en", true},
	{-1, -1, 331, "OOB_way_en", true},
	{-1, -1, 332, "OOB_way_en", true},
	{-1, -1, 333, "OOB_way_en", true},

	/* 320 */
	{-1, -1, 334, "OOB_way_en", true},
	{-1, -1, 335, "OOB_way_en", true},
	{-1, -1, 336, "OOB_way_en", true},
	{-1, -1, 337, "OOB_way_en", true},
	{-1, -1, 338, "OOB_way_en", true},
	{-1, -1, 339, "OOB_way_en", true},
	{-1, -1, 340, "OOB_way_en", true},
	{-1, -1, 341, "OOB_way_en", true},
	{-1, -1, 342, "OOB_way_en", true},
	{-1, -1, 343, "OOB_way_en", true},

	/* 330 */
	{-1, -1, 344, "OOB_way_en", true},
	{-1, -1, 345, "OOB_way_en", true},
	{-1, -1, 346, "Decode_error", true},
	{-1, -1, 347, "Decode_error", true},
	{-1, -1, 348, "Decode_error", true},
	{-1, -1, 349, "Decode_error", true},
	{-1, -1, 350, "Decode_error", true},
	{-1, -1, 351, "Decode_error", true},
	{-1, -1, 352, "MDP_MALI", true},
	{-1, -1, 353, "DISP2_MALI", true},

	/* 340 */
	{-1, -1, 354, "MMSYS_MALI", true},
	{-1, -1, 355, "PMIC_WRAP", false},
	{-1, -1, 356, "PMIF1", false},
	{-1, -1, 357, "AP_DMA", false},
	{-1, -1, 358, "DISP_PWM", false},
	{-1, -1, 359, "IMP_IIC_WRAP", false},
	{-1, -1, 360, "DEVICE_APC_PERI_AO", false},
	{-1, -1, 361, "DEVICE_APC_PERI_PDN", false},
	{-1, -1, 12, "RESERVED", false},
	{-1, -1, 22, "RESERVED", false},

	/* 350 */
	{-1, -1, 131, "RESERVED", false},
	{-1, -1, 132, "RESERVED", false},
	{-1, -1, 133, "RESERVED", false},
	{-1, -1, 134, "RESERVED", false},
	{-1, -1, 135, "RESERVED", false},
	{-1, -1, 138, "RESERVED", false},
	{-1, -1, 139, "RESERVED", false},
	{-1, -1, 140, "RESERVED", false},
	{-1, -1, 141, "RESERVED", false},
	{-1, -1, 142, "RESERVED", false},

	/* 360 */
	{-1, -1, 201, "RESERVED", false},
	{-1, -1, 202, "RESERVED", false},
};

static struct mtk_device_info mt6885_devices_peri2[] = {
	/* sys_idx, ctrl_idx, vio_idx, device, vio_irq */
	/* 0 */
	{0, 0, 0, "EFUSE_DEBUG_AO_APB_S", true},
	{0, 1, 1, "APXGPT_APB_S", true},
	{0, 2, 2, "SEJ_APB_S", true},
	{0, 3, 3, "AES_TOP0_APB_S", true},
	{0, 4, 4, "SECURITY_AO_APB_S", true},
	{0, 5, 5, "DEVICE_APC_PERI_AO2_APB_S", true},
	{0, 6, 6, "BCRM_PERI_AO2_APB_S", true},
	{0, 7, 7, "DEBUG_CTRL_PERI_AO2_APB_S", true},
	{0, 8, 8, "SPMI_MST_APB_S", true},
	{0, 9, 9, "MCUPM_APB_S", true},

	/* 10 */
	{0, 10, 10, "MCUPM_APB_S-1", true},
	{0, 11, 11, "MCUPM_APB_S-2", true},
	{0, 12, 12, "MCUPM_APB_S-3", true},
	{0, 13, 13, "GCE_APB_S", true},
	{0, 14, 14, "GCE_APB_S-1", true},
	{0, 15, 15, "GCE_APB_S-2", true},
	{0, 16, 16, "GCE_APB_S-3", true},
	{0, 17, 17, "DPMAIF_PDN_APB_S", true},
	{0, 18, 18, "DPMAIF_PDN_APB_S-1", true},
	{0, 19, 19, "DPMAIF_PDN_APB_S-2", true},

	/* 20 */
	{0, 20, 20, "DPMAIF_PDN_APB_S-3", true},
	{0, 21, 21, "GCE_M2_APB_S", true},
	{0, 22, 22, "GCE_M2_APB_S-1", true},
	{0, 23, 23, "GCE_M2_APB_S-2", true},
	{0, 24, 24, "GCE_M2_APB_S-3", true},
	{0, 25, 25, "BND_EAST_APB0_S", true},
	{0, 26, 26, "BND_EAST_APB1_S", true},
	{0, 27, 27, "BND_EAST_APB2_S", true},
	{0, 28, 28, "BND_EAST_APB3_S", true},
	{0, 29, 29, "BND_EAST_APB4_S", true},

	/* 30 */
	{0, 30, 30, "BND_EAST_APB5_S", true},
	{0, 31, 31, "BND_EAST_APB6_S", true},
	{0, 32, 32, "BND_EAST_APB7_S", true},
	{0, 33, 33, "BND_EAST_APB8_S", true},
	{0, 34, 34, "BND_EAST_APB9_S", true},
	{0, 35, 35, "BND_EAST_APB10_S", true},
	{0, 36, 36, "BND_EAST_APB11_S", true},
	{0, 37, 37, "BND_EAST_APB12_S", true},
	{0, 38, 38, "BND_EAST_APB13_S", true},
	{0, 39, 39, "BND_EAST_APB14_S", true},

	/* 40 */
	{0, 40, 40, "BND_EAST_APB15_S", true},
	{0, 41, 41, "BND_WEST_APB0_S", true},
	{0, 42, 42, "BND_WEST_APB1_S", true},
	{0, 43, 43, "BND_WEST_APB2_S", true},
	{0, 44, 44, "BND_WEST_APB3_S", true},
	{0, 45, 45, "BND_WEST_APB4_S", true},
	{0, 46, 46, "BND_WEST_APB5_S", true},
	{0, 47, 47, "BND_WEST_APB6_S", true},
	{0, 48, 48, "BND_WEST_APB7_S", true},
	{0, 49, 49, "BND_NORTH_APB0_S", true},

	/* 50 */
	{0, 50, 50, "BND_NORTH_APB1_S", true},
	{0, 51, 51, "BND_NORTH_APB2_S", true},
	{0, 52, 52, "BND_NORTH_APB3_S", true},
	{0, 53, 53, "BND_NORTH_APB4_S", true},
	{0, 54, 54, "BND_NORTH_APB5_S", true},
	{0, 55, 55, "BND_NORTH_APB6_S", true},
	{0, 56, 56, "BND_NORTH_APB7_S", true},
	{0, 57, 57, "BND_NORTH_APB8_S", true},
	{0, 58, 58, "BND_NORTH_APB9_S", true},
	{0, 59, 59, "BND_NORTH_APB10_S", true},

	/* 60 */
	{0, 60, 60, "BND_NORTH_APB11_S", true},
	{0, 61, 61, "BND_NORTH_APB12_S", true},
	{0, 62, 62, "BND_NORTH_APB13_S", true},
	{0, 63, 63, "BND_NORTH_APB14_S", true},
	{0, 64, 64, "BND_NORTH_APB15_S", true},
	{0, 65, 65, "BND_SOUTH_APB0_S", true},
	{0, 66, 66, "BND_SOUTH_APB1_S", true},
	{0, 67, 67, "BND_SOUTH_APB2_S", true},
	{0, 68, 68, "BND_SOUTH_APB3_S", true},
	{0, 69, 69, "BND_SOUTH_APB4_S", true},

	/* 70 */
	{0, 70, 70, "BND_SOUTH_APB5_S", true},
	{0, 71, 71, "BND_SOUTH_APB6_S", true},
	{0, 72, 72, "BND_SOUTH_APB7_S", true},
	{0, 73, 73, "BND_SOUTH_APB8_S", true},
	{0, 74, 74, "BND_SOUTH_APB9_S", true},
	{0, 75, 75, "BND_SOUTH_APB10_S", true},
	{0, 76, 76, "BND_SOUTH_APB11_S", true},
	{0, 77, 77, "BND_SOUTH_APB12_S", true},
	{0, 78, 78, "BND_SOUTH_APB13_S", true},
	{0, 79, 79, "BND_SOUTH_APB14_S", true},

	/* 80 */
	{0, 80, 80, "BND_SOUTH_APB15_S", true},
	{0, 81, 81, "BND_EAST_NORTH_APB0_S", true},
	{0, 82, 82, "BND_EAST_NORTH_APB1_S", true},
	{0, 83, 83, "BND_EAST_NORTH_APB2_S", true},
	{0, 84, 84, "BND_EAST_NORTH_APB3_S", true},
	{0, 85, 85, "BND_EAST_NORTH_APB4_S", true},
	{0, 86, 86, "BND_EAST_NORTH_APB5_S", true},
	{0, 87, 87, "BND_EAST_NORTH_APB6_S", true},
	{0, 88, 88, "BND_EAST_NORTH_APB7_S", true},
	{0, 89, 89, "SYS_CIRQ_APB_S", true},

	/* 90 */
	{0, 90, 90, "EFUSE_DEBUG_PDN_APB_S", true},
	{0, 91, 91, "DEVICE_APC_INFRA_PDN_APB_S", true},
	{0, 92, 92, "DEBUG_TRACKER_APB_S", true},
	{0, 93, 93, "CCIF0_AP_APB_S", true},
	{0, 94, 94, "CCIF0_MD_APB_S", true},
	{0, 95, 95, "CCIF1_AP_APB_S", true},
	{0, 96, 96, "CCIF1_MD_APB_S", true},
	{0, 97, 97, "MBIST_PDN_APB_S", true},
	{0, 98, 98, "INFRACFG_PDN_APB_S", true},
	{0, 99, 99, "TRNG_APB_S", true},

	/* 100 */
	{0, 100, 100, "DX_CC_APB_S", true},
	{0, 101, 101, "CQ_DMA_APB_S", true},
	{0, 102, 102, "SRAMROM_APB_S", true},
	{0, 103, 104, "INFRACFG_MEM_APB_S", true},
	{0, 104, 105, "RESERVED_DVFS_PROC_APB_S", true},
	{0, 105, 108, "SYS_CIRQ1_APB_S", true},
	{0, 106, 109, "SYS_CIRQ2_APB_S", true},
	{0, 107, 110, "DEBUG_TRACKER_APB1_S", true},
	{0, 108, 112, "EMI_APB_S", true},
	{0, 109, 113, "EMI_MPU_APB_S", true},

	/* 110 */
	{0, 110, 114, "DEVICE_MPU_PDN_APB_S", true},
	{0, 111, 115, "EMI_SUB_INFRA_APB_S", true},
	{0, 112, 116, "EMI_MPU_SUB_INFRA_APB_S", true},
	{0, 113, 117, "DEVICE_MPU_PDN_SUB_INFRA_APB_S", true},
	{0, 114, 118, "MBIST_PDN_SUB_INFRA_APB_S", true},
	{0, 115, 119, "INFRACFG_MEM_SUB_INFRA_APB_S", true},
	{0, 116, 120, "PERI_FAST_M_APB_S", true},
	{0, 117, 121, "PERI_SLOW_M_APB_S", true},
	{0, 118, 122, "BCRM_SUB_INFRA_AO_APB_S", true},
	{0, 119, 123, "DEBUG_CTRL_SUB_INFRA_AO_APB_S", true},

	/* 120 */
	{0, 120, 124, "BCRM_INFRA_PDN_APB_S", true},
	{0, 121, 125, "BCRM_SUB_INFRA_PDN_APB_S", true},
	{0, 122, 126, "SSC_SUB_INFRA_APB0_S", true},
	{0, 123, 127, "SSC_SUB_INFRA_APB1_S", true},
	{0, 124, 128, "SSC_SUB_INFRA_APB2_S", true},
	{0, 125, 129, "INFRACFG_AO_MEM_SUB_INFRA_APB_S", true},
	{-1, -1, 130, "OOB_way_en", true},
	{-1, -1, 131, "OOB_way_en", true},
	{-1, -1, 132, "OOB_way_en", true},
	{-1, -1, 133, "OOB_way_en", true},

	/* 130 */
	{-1, -1, 134, "OOB_way_en", true},
	{-1, -1, 135, "OOB_way_en", true},
	{-1, -1, 136, "OOB_way_en", true},
	{-1, -1, 137, "OOB_way_en", true},
	{-1, -1, 138, "OOB_way_en", true},
	{-1, -1, 139, "OOB_way_en", true},
	{-1, -1, 140, "OOB_way_en", true},
	{-1, -1, 141, "OOB_way_en", true},
	{-1, -1, 142, "OOB_way_en", true},
	{-1, -1, 143, "OOB_way_en", true},

	/* 140 */
	{-1, -1, 144, "OOB_way_en", true},
	{-1, -1, 145, "OOB_way_en", true},
	{-1, -1, 146, "OOB_way_en", true},
	{-1, -1, 147, "OOB_way_en", true},
	{-1, -1, 148, "OOB_way_en", true},
	{-1, -1, 149, "OOB_way_en", true},
	{-1, -1, 150, "OOB_way_en", true},
	{-1, -1, 151, "OOB_way_en", true},
	{-1, -1, 152, "OOB_way_en", true},
	{-1, -1, 153, "OOB_way_en", true},

	/* 150 */
	{-1, -1, 154, "OOB_way_en", true},
	{-1, -1, 155, "OOB_way_en", true},
	{-1, -1, 156, "OOB_way_en", true},
	{-1, -1, 157, "OOB_way_en", true},
	{-1, -1, 158, "OOB_way_en", true},
	{-1, -1, 159, "OOB_way_en", true},
	{-1, -1, 160, "OOB_way_en", true},
	{-1, -1, 161, "OOB_way_en", true},
	{-1, -1, 162, "OOB_way_en", true},
	{-1, -1, 163, "OOB_way_en", true},

	/* 160 */
	{-1, -1, 164, "OOB_way_en", true},
	{-1, -1, 165, "OOB_way_en", true},
	{-1, -1, 166, "OOB_way_en", true},
	{-1, -1, 167, "OOB_way_en", true},
	{-1, -1, 168, "OOB_way_en", true},
	{-1, -1, 169, "OOB_way_en", true},
	{-1, -1, 170, "OOB_way_en", true},
	{-1, -1, 171, "OOB_way_en", true},
	{-1, -1, 172, "OOB_way_en", true},
	{-1, -1, 173, "OOB_way_en", true},

	/* 170 */
	{-1, -1, 174, "OOB_way_en", true},
	{-1, -1, 175, "OOB_way_en", true},
	{-1, -1, 176, "OOB_way_en", true},
	{-1, -1, 177, "OOB_way_en", true},
	{-1, -1, 178, "OOB_way_en", true},
	{-1, -1, 179, "OOB_way_en", true},
	{-1, -1, 180, "OOB_way_en", true},
	{-1, -1, 181, "OOB_way_en", true},
	{-1, -1, 182, "OOB_way_en", true},
	{-1, -1, 183, "OOB_way_en", true},

	/* 180 */
	{-1, -1, 184, "OOB_way_en", true},
	{-1, -1, 185, "OOB_way_en", true},
	{-1, -1, 186, "OOB_way_en", true},
	{-1, -1, 187, "OOB_way_en", true},
	{-1, -1, 188, "OOB_way_en", true},
	{-1, -1, 189, "OOB_way_en", true},
	{-1, -1, 190, "OOB_way_en", true},
	{-1, -1, 191, "OOB_way_en", true},
	{-1, -1, 192, "OOB_way_en", true},
	{-1, -1, 193, "OOB_way_en", true},

	/* 190 */
	{-1, -1, 194, "OOB_way_en", true},
	{-1, -1, 195, "OOB_way_en", true},
	{-1, -1, 196, "OOB_way_en", true},
	{-1, -1, 197, "OOB_way_en", true},
	{-1, -1, 198, "OOB_way_en", true},
	{-1, -1, 199, "OOB_way_en", true},
	{-1, -1, 200, "OOB_way_en", true},
	{-1, -1, 201, "OOB_way_en", true},
	{-1, -1, 202, "OOB_way_en", true},
	{-1, -1, 203, "OOB_way_en", true},

	/* 200 */
	{-1, -1, 204, "OOB_way_en", true},
	{-1, -1, 205, "OOB_way_en", true},
	{-1, -1, 206, "OOB_way_en", true},
	{-1, -1, 207, "OOB_way_en", true},
	{-1, -1, 208, "OOB_way_en", true},
	{-1, -1, 209, "OOB_way_en", true},
	{-1, -1, 210, "OOB_way_en", true},
	{-1, -1, 211, "OOB_way_en", true},
	{-1, -1, 212, "OOB_way_en", true},
	{-1, -1, 213, "OOB_way_en", true},

	/* 210 */
	{-1, -1, 214, "OOB_way_en", true},
	{-1, -1, 215, "OOB_way_en", true},
	{-1, -1, 216, "OOB_way_en", true},
	{-1, -1, 217, "OOB_way_en", true},
	{-1, -1, 218, "OOB_way_en", true},
	{-1, -1, 219, "OOB_way_en", true},
	{-1, -1, 220, "OOB_way_en", true},
	{-1, -1, 221, "OOB_way_en", true},
	{-1, -1, 222, "OOB_way_en", true},
	{-1, -1, 223, "OOB_way_en", true},

	/* 220 */
	{-1, -1, 224, "OOB_way_en", true},
	{-1, -1, 225, "OOB_way_en", true},
	{-1, -1, 226, "OOB_way_en", true},
	{-1, -1, 227, "OOB_way_en", true},
	{-1, -1, 228, "OOB_way_en", true},
	{-1, -1, 229, "OOB_way_en", true},
	{-1, -1, 230, "OOB_way_en", true},
	{-1, -1, 231, "OOB_way_en", true},
	{-1, -1, 232, "OOB_way_en", true},
	{-1, -1, 233, "OOB_way_en", true},

	/* 230 */
	{-1, -1, 234, "OOB_way_en", true},
	{-1, -1, 235, "OOB_way_en", true},
	{-1, -1, 236, "OOB_way_en", true},
	{-1, -1, 237, "OOB_way_en", true},
	{-1, -1, 238, "OOB_way_en", true},
	{-1, -1, 239, "OOB_way_en", true},
	{-1, -1, 240, "OOB_way_en", true},
	{-1, -1, 241, "OOB_way_en", true},
	{-1, -1, 242, "OOB_way_en", true},
	{-1, -1, 243, "OOB_way_en", true},

	/* 240 */
	{-1, -1, 244, "OOB_way_en", true},
	{-1, -1, 245, "OOB_way_en", true},
	{-1, -1, 246, "OOB_way_en", true},
	{-1, -1, 247, "OOB_way_en", true},
	{-1, -1, 248, "OOB_way_en", true},
	{-1, -1, 249, "OOB_way_en", true},
	{-1, -1, 250, "OOB_way_en", true},
	{-1, -1, 251, "OOB_way_en", true},
	{-1, -1, 252, "Decode_error", true},
	{-1, -1, 253, "Decode_error", true},

	/* 250 */
	{-1, -1, 254, "Decode_error", true},
	{-1, -1, 255, "Decode_error", true},
	{-1, -1, 256, "Decode_error", true},
	{-1, -1, 257, "Decode_error", true},
	{-1, -1, 258, "Decode_error", true},
	{-1, -1, 259, "Decode_error", true},
	{-1, -1, 260, "CQ_DMA", false},
	{-1, -1, 261, "EMI", false},
	{-1, -1, 262, "SUB_EMI", false},
	{-1, -1, 263, "SUB_EMI_MPU", false},

	/* 260 */
	{-1, -1, 264, "EMI_MPU", false},
	{-1, -1, 265, "GCE_M", false},
	{-1, -1, 266, "GCE_M2", false},
	{-1, -1, 267, "DEVICE_APC_PERI_AO2", false},
	{-1, -1, 268, "DEVICE_APC_PERI_PDN2", false},
	{-1, -1, 103, "RESERVED", false},
	{-1, -1, 106, "RESERVED", false},
	{-1, -1, 107, "RESERVED", false},
	{-1, -1, 111, "RESERVED", false},
};

static struct mtk_device_num mtk6885_devices_num[] = {
	{SLAVE_TYPE_INFRA, VIO_SLAVE_NUM_INFRA},
	{SLAVE_TYPE_PERI, VIO_SLAVE_NUM_PERI},
	{SLAVE_TYPE_PERI2, VIO_SLAVE_NUM_PERI2},
};

static struct PERIAXI_ID_INFO peri_mi_id_to_master[] = {
	{"THERM2",       { 0, 0, 0 } },
	{"SPM",          { 0, 1, 0 } },
	{"CCU",          { 0, 0, 1 } },
	{"THERM",        { 0, 1, 1 } },
	{"SPM_DRAMC",    { 1, 1, 0 } },
};

static struct INFRAAXI_ID_INFO infra_mi_id_to_master[] = {
	{"CONNSYS_WFDMA",     { 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2 } },
	{"CONNSYS_ICAP",      { 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2 } },
	{"CONNSYS_WF_MCU",    { 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2 } },
	{"CONNSYS_BT_MCU",    { 0, 0, 0, 0, 1, 0, 0, 1, 2, 2, 2, 2, 2 } },
	{"CONNSYS_GPS",       { 0, 0, 0, 0, 1, 0, 0, 0, 2, 2, 2, 2, 2 } },
	{"Tinysys",           { 0, 1, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0 } },
	{"GCE_M2",            { 0, 0, 1, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0 } },
	{"CQ_DMA",            { 0, 0, 1, 0, 1, 0, 0, 2, 2, 2, 0, 0, 0 } },
	{"DebugTop",          { 0, 0, 1, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0 } },
	{"SSUSB",             { 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 2, 2 } },
	{"PWM",               { 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0 } },
	{"MSDC1",             { 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0 } },
	{"SPI6",              { 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1 } },
	{"SPI0",              { 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1 } },
	{"APU",               { 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 2, 2, 0 } },
	{"MSDC0",             { 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 2, 2, 0 } },
	{"SPI2",              { 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0 } },
	{"SPI3",              { 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0 } },
	{"SPI4",              { 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0 } },
	{"SPI5",              { 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0 } },
	{"SPI7",              { 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0 } },
	{"Audio",             { 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0 } },
	{"SPI1",              { 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0 } },
	{"AP_DMA_EXT",        { 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 2, 2, 2 } },
	{"THERM2",            { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0 } },
	{"SPM",               { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0 } },
	{"CCU",               { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0 } },
	{"THERM",             { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0 } },
	{"DX_CC",             { 0, 0, 1, 0, 0, 0, 1, 2, 2, 2, 2, 0, 0 } },
	{"GCE_M",             { 0, 0, 1, 0, 1, 0, 1, 2, 2, 0, 0, 0, 0 } },
	{"DPMAIF",            { 0, 1, 1, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0 } },
	{"MCUPM",             { 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{"SSPM",              { 0, 1, 0, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0 } },
	{"UFS",               { 0, 0, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0 } },
	{"APMCU_write",       { 1, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{"APMCU_write",       { 1, 2, 2, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0 } },
	{"APMCU_write",       { 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0 } },
	{"APMCU_read",        { 1, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{"APMCU_read",        { 1, 2, 2, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0 } },
	{"APMCU_read",        { 1, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 } },
	{"APMCU_read",        { 1, 2, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 } },
	{"APMCU_read",        { 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0 } },
	{"APMCU_read",        { 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0 } },
};

static const char *infra_mi_trans(uint32_t bus_id)
{
	int master_count = ARRAY_SIZE(infra_mi_id_to_master);
	const char *master = "UNKNOWN_MASTER_FROM_INFRA";
	int i, j;

	for (i = 0; i < master_count; i++) {
		for (j = 0; j < INFRAAXI_MI_BIT_LENGTH; j++) {
			if (infra_mi_id_to_master[i].bit[j] == 2)
				continue;
			if (((bus_id >> j) & 0x1) ==
					infra_mi_id_to_master[i].bit[j])
				continue;
			break;
		}
		if (j == INFRAAXI_MI_BIT_LENGTH) {
			pr_debug(PFX "%s %s %s\n",
				"catch it from INFRAAXI_MI",
				"Master is:",
				infra_mi_id_to_master[i].master);
			master = infra_mi_id_to_master[i].master;
		}
	}

	return master;
}

static const char *peri_mi_trans(uint32_t bus_id)
{
	int master_count = ARRAY_SIZE(peri_mi_id_to_master);
	const char *master = "UNKNOWN_MASTER_FROM_PERI";
	int i, j;

	if ((bus_id & 0x3) == 0x0)
		return infra_mi_trans(bus_id >> 2);
	else if ((bus_id & 0x3) == 0x2)
		return "MD_AP_M";
	else if ((bus_id & 0x3) == 0x3)
		return "AP_DMA_M";

	for (i = 0 ; i < master_count; i++) {
		for (j = 0 ; j < PERIAXI_MI_BIT_LENGTH ; j++) {
			if (peri_mi_id_to_master[i].bit[j] == 2)
				continue;
			if (((bus_id >> j) & 0x1) ==
					peri_mi_id_to_master[i].bit[j])
				continue;
			break;
		}
		if (j == PERIAXI_MI_BIT_LENGTH) {
			pr_debug(PFX "%s %s %s\n",
				"catch it from PERIAXI_MI.",
				"Master is:",
				peri_mi_id_to_master[i].master);
			master = peri_mi_id_to_master[i].master;
		}
	}

	return master;
}

static const char *mt6885_bus_id_to_master(uint32_t bus_id, uint32_t vio_addr,
		int slave_type, int shift_sta_bit, int domain)
{
	uint16_t h_2byte;
	uint8_t h_1byte;

	UNUSED(slave_type);
	UNUSED(shift_sta_bit);

	pr_debug(PFX "[DEVAPC] bus_id:0x%x, vio_addr:0x%x\n",
		bus_id, vio_addr);

	if (bus_id == 0x0 && vio_addr == 0x0)
		return NULL;

	/* bus only reference bit 0~29 */
	vio_addr = vio_addr & 0x3FFFFFFF;

	h_1byte = (vio_addr >> 24) & 0xFF;
	h_2byte = (vio_addr >> 16) & 0xFFFF;

	if ((vio_addr >= TINYSYS_START_ADDR && vio_addr <= TINYSYS_END_ADDR) ||
	    (vio_addr >= MD_START_ADDR && vio_addr <= MD_END_ADDR)) {
		pr_debug(PFX "[DEVAPC] bus_id might be wrong\n");

		if (domain == 0x1)
			return "SSPM";
		else if (domain == 0x2)
			return "CONNSYS";

	} else if (vio_addr >= CONN_START_ADDR && vio_addr <= CONN_END_ADDR) {
		pr_debug(PFX "[DEVAPC] bus_id might be wrong\n");

		if (domain == 0x1)
			return "MD";
		else if (domain == 0x2)
			return "ADSP";
	}

	if (h_2byte < 0x0C51 || (h_2byte >= 0x0C80 && h_2byte < 0x0CC0)) {
		pr_debug(PFX "vio_addr is from L3C or on-chip SRAMROM\n");
		if ((bus_id & 0x1) == 1)
			return "EMI_L2C_M";

		return infra_mi_trans(bus_id >> 1);

	} else if (h_1byte == 0x15 || h_1byte == 0x1A || h_1byte == 0x1B ||
			h_1byte == 0x1F) {
		pr_debug(PFX "vio_addr is from MDP System\n");
		if ((bus_id & 0x1) == 0)
			return "GCE_M2";

		return infra_mi_trans(bus_id >> 1);

	} else if (h_2byte >= 0x1410 && h_2byte < 0x1420) {
		pr_debug(PFX "vio_addr is from DISP-2 MM System\n");
		if ((bus_id & 0x1) == 1)
			return "GCE_M";
		else if ((bus_id >> 3) == 0)
			pr_debug(PFX "Master might be %s\n", "GCE_M2");

		return infra_mi_trans(bus_id >> 1);

	} else if (h_1byte == 0x14 || h_1byte == 0x16 || h_1byte == 0x17) {
		pr_debug(PFX "vio_addr is from DISP MM System\n");
		if ((bus_id & 0x1) == 1)
			return "GCE_M";

		return infra_mi_trans(bus_id >> 1);

	} else if (h_1byte == 0x13 || h_1byte == 0x19) {
		pr_debug(PFX "vio_addr is from APU or GPU\n");
		return infra_mi_trans(bus_id);

	} else if (h_2byte >= 0x1050 && h_2byte < 0x1090) {
		pr_debug(PFX "vio_addr is from Tinysys\n");
		if ((bus_id & 0x1) == 0)
			return "MD_AP_M";

		return peri_mi_trans(bus_id >> 1);

	} else if (h_2byte >= 0x1040 && h_2byte < 0x1050) {
		pr_debug(PFX "vio_addr is from PWR_MD32\n");
		if ((bus_id & 0x1) == 0)
			return "MD_AP_M";

		return peri_mi_trans(bus_id >> 1);

	} else if (h_2byte >= 0x1120 && h_2byte < 0x1121) {
		pr_debug(PFX "vio_addr is from SSUSB\n");
		if ((bus_id & 0x1) == 0)
			return "MD_AP_M";

		return peri_mi_trans(bus_id >> 1);

	} else {
		return peri_mi_trans(bus_id);
	}
}

/* violation index corresponds to subsys */
const char *index_to_subsys(int slave_type, uint32_t vio_index,
		uint32_t vio_addr)
{
	int i;

	if (slave_type == SLAVE_TYPE_INFRA &&
			vio_index < VIO_SLAVE_NUM_INFRA) {

		/* check violation address */
		if (vio_addr >= MFG_START_ADDR && vio_addr <= MFG_END_ADDR)
			return "MFGSYS";

		/* check violation index */
		if (vio_index == SMI_LARB0_VIO_INDEX ||
				vio_index == SMI_LARB1_VIO_INDEX ||
				vio_index == SMI_LARB2_VIO_INDEX ||
				vio_index == SMI_LARB3_VIO_INDEX ||
				vio_index == SMI_LARB4_VIO_INDEX ||
				vio_index == SMI_LARB5_VIO_INDEX ||
				vio_index == SMI_LARB6_VIO_INDEX ||
				vio_index == SMI_LARB7_VIO_INDEX ||
				vio_index == SMI_LARB8_VIO_INDEX ||
				vio_index == SMI_LARB9_VIO_INDEX ||
				vio_index == SMI_LARB10_VIO_INDEX ||
				vio_index == SMI_LARB11_VIO_INDEX ||
				vio_index == SMI_LARB12_VIO_INDEX ||
				vio_index == SMI_LARB13_VIO_INDEX ||
				vio_index == SMI_LARB14_VIO_INDEX ||
				vio_index == SMI_LARB15_VIO_INDEX ||
				vio_index == SMI_LARB16_VIO_INDEX ||
				vio_index == SMI_LARB17_VIO_INDEX ||
				vio_index == SMI_LARB18_VIO_INDEX ||
				vio_index == SMI_LARB19_VIO_INDEX ||
				vio_index == SMI_LARB20_VIO_INDEX)
			return "SMI";

		if (vio_index == IOMMU0_VIO_INDEX ||
				vio_index == IOMMU1_VIO_INDEX ||
				vio_index == IOMMU0_SEC_VIO_INDEX ||
				vio_index == IOMMU1_SEC_VIO_INDEX)
			return "IOMMU";

		else if (vio_index >= CAM_SENINF_START &&
				vio_index <= CAM_SENINF_END)
			return "CAMSYS_SENINF";

		switch (vio_index) {
		case MFG_START ... MFG_END:
			return "MFGSYS";
		case MM_DISP_START ... MM_DISP_END:
		case MM_DISP2_START ... MM_DISP2_END:
			return "MMSYS_DISP";
		case MM_SSRAM_VIO_INDEX:
		case MM_MDP_START ... MM_MDP_END:
			return "MMSYS_MDP";
		case IMG_START ... IMG_END:
			return "IMGSYS";
		case VDEC_START ... VDEC_END:
			return "VDECSYS";
		case VENC_START ... VENC_END:
			return "VENCSYS";
		case APU_SSRAM_VIO_INDEX:
		case APU_START ... APU_END:
			return "APUSYS";
		case CAM_CCU_START ... CAM_CCU_END:
			return "CAMSYS_CCU";
		case CAM_START ... CAM_END:
			return "CAMSYS";
		case IPE_START ... IPE_END:
			return "IPESYS";
		case MDP_START ... MDP_END:
			return "MMSYS_MDP";
		default:
			break;
		}

		for (i = 0; i < VIO_SLAVE_NUM_INFRA; i++) {
			if (vio_index == mt6885_devices_infra[i].vio_index)
				return mt6885_devices_infra[i].device;
		}

	} else if (slave_type == SLAVE_TYPE_PERI &&
			vio_index < VIO_SLAVE_NUM_PERI) {

		/* check violation index */
		switch (vio_index) {
		case TINY_START ... TINY_END:
			return "TINYSYS";
		case MD_START ... MD_END:
			return "MDSYS";
		case CONN_VIO_INDEX:
			return "CONNSYS";
		default:
			break;
		}

		for (i = 0; i < VIO_SLAVE_NUM_PERI; i++) {
			if (vio_index == mt6885_devices_peri[i].vio_index)
				return mt6885_devices_peri[i].device;
		}

	} else if (slave_type == SLAVE_TYPE_PERI2 &&
			vio_index < VIO_SLAVE_NUM_PERI2) {

		/* check violation address */
		if ((vio_addr >= GCE_START_ADDR && vio_addr <= GCE_END_ADDR) ||
				(vio_addr >= GCE_M2_START_ADDR &&
				 vio_addr <= GCE_M2_END_ADDR))
			return "GCE";

		for (i = 0; i < VIO_SLAVE_NUM_PERI2; i++) {
			if (vio_index == mt6885_devices_peri2[i].vio_index)
				return mt6885_devices_peri2[i].device;
		}
	}

	return "OUT_OF_BOUND";
}

static void mm2nd_vio_handler(void __iomem *infracfg,
			      struct mtk_devapc_vio_info *vio_info,
			      bool mdp_vio, bool disp2_vio, bool mmsys_vio)
{
	uint32_t vio_sta, vio_dbg, rw;
	uint32_t vio_sta_num;
	uint32_t vio0_offset;
	char mm_str[64] = {0};
	void __iomem *reg;
	int i;

	if (!infracfg) {
		pr_err(PFX "%s, param check failed, infracfg ptr is NULL\n",
				__func__);
		return;
	}

	if (mdp_vio) {
		vio_sta_num = INFRACFG_MDP_VIO_STA_NUM;
		vio0_offset = INFRACFG_MDP_SEC_VIO0_OFFSET;

		strncpy(mm_str, "INFRACFG_MDP_SEC_VIO", sizeof(mm_str));

	} else if (disp2_vio) {
		vio_sta_num = INFRACFG_DISP2_VIO_STA_NUM;
		vio0_offset = INFRACFG_DISP2_SEC_VIO0_OFFSET;

		strncpy(mm_str, "INFRACFG_DISP2_SEC_VIO", sizeof(mm_str));

	} else if (mmsys_vio) {
		vio_sta_num = INFRACFG_MM_VIO_STA_NUM;
		vio0_offset = INFRACFG_MM_SEC_VIO0_OFFSET;

		strncpy(mm_str, "INFRACFG_MM_SEC_VIO", sizeof(mm_str));

	} else {
		pr_err(PFX "%s: param check failed, %s:%s, %s:%s, %s:%s\n",
				__func__,
				"mdp_vio", mdp_vio ? "true" : "false",
				"disp2_vio", disp2_vio ? "true" : "false",
				"mmsys_vio", mmsys_vio ? "true" : "false");
		return;
	}

	/* Get mm2nd violation status */
	for (i = 0; i < vio_sta_num; i++) {
		reg = infracfg + vio0_offset + i * 4;
		vio_sta = readl(reg);
		if (vio_sta)
			pr_debug(PFX "MM 2nd violation: %s%d:0x%x\n",
					mm_str, i, vio_sta);
	}

	/* Get mm2nd violation address */
	reg = infracfg + vio0_offset + i * 4;
	vio_info->vio_addr = readl(reg);

	/* Get mm2nd violation information */
	reg = infracfg + vio0_offset + (i + 1) * 4;
	vio_dbg = readl(reg);

	vio_info->domain_id = (vio_dbg & INFRACFG_MM2ND_VIO_DOMAIN_MASK) >>
		INFRACFG_MM2ND_VIO_DOMAIN_SHIFT;

	vio_info->master_id = (vio_dbg & INFRACFG_MM2ND_VIO_ID_MASK) >>
		INFRACFG_MM2ND_VIO_ID_SHIFT;

	rw = (vio_dbg & INFRACFG_MM2ND_VIO_RW_MASK) >>
		INFRACFG_MM2ND_VIO_RW_SHIFT;
	vio_info->read = (rw == 0);
	vio_info->write = (rw == 1);
}

static uint32_t mt6885_shift_group_get(int slave_type, uint32_t vio_idx)
{
	if (slave_type == SLAVE_TYPE_INFRA) {
		if ((vio_idx >= 0 && vio_idx <= 8) || vio_idx == 412)
			return 0;
		else if ((vio_idx >= 9 && vio_idx <= 14) || vio_idx == 413)
			return 1;
		else if ((vio_idx >= 15 && vio_idx <= 16) || vio_idx == 414)
			return 2;
		else if ((vio_idx >= 17 && vio_idx <= 18) || vio_idx == 415)
			return 3;
		else if (vio_idx >= 19 && vio_idx <= 70)
			return 4;
		else if (vio_idx >= 71 && vio_idx <= 369)
			return 5;
		else if (vio_idx >= 370 && vio_idx <= 409)
			return 6;
		else if (vio_idx == 410 || vio_idx == 416)
			return 7;
		else if (vio_idx == 411 || vio_idx == 417)
			return 8;

		pr_err(PFX "%s:%d Wrong vio_idx:0x%x\n",
				__func__, __LINE__, vio_idx);

	} else if (slave_type == SLAVE_TYPE_PERI) {
		if (vio_idx >= 0 && vio_idx <= 4)
			return 0;
		else if (vio_idx >= 5 && vio_idx <= 6)
			return 1;
		else if ((vio_idx >= 7 && vio_idx <= 38) || vio_idx == 216 ||
				(vio_idx >= 217 && vio_idx <= 248) ||
				vio_idx == 346)
			return 2;
		else if ((vio_idx >= 39 && vio_idx <= 61) || vio_idx == 249)
			return 3;
		else if ((vio_idx >= 62 && vio_idx <= 72) || vio_idx == 250)
			return 4;
		else if ((vio_idx >= 73 && vio_idx <= 74) || vio_idx == 251)
			return 5;
		else if ((vio_idx >= 75 && vio_idx <= 78) || vio_idx == 252)
			return 6;
		else if ((vio_idx >= 79 && vio_idx <= 121) || vio_idx == 253)
			return 7;
		else if ((vio_idx >= 122 && vio_idx <= 124) || vio_idx == 254)
			return 8;
		else if (vio_idx == 125 || vio_idx == 255 ||
				vio_idx == 256 || vio_idx == 347)
			return 9;
		else if (vio_idx == 126 || vio_idx == 257)
			return 10;
		else if (vio_idx == 127 || vio_idx == 128)
			return 11;
		else if (vio_idx == 129 || vio_idx == 130)
			return 12;
		else if ((vio_idx >= 131 && vio_idx <= 142) ||
				(vio_idx >= 258 && vio_idx <= 269) ||
				vio_idx == 348)
			return 13;
		else if ((vio_idx >= 143 && vio_idx <= 172) || vio_idx == 270 ||
				(vio_idx >= 271 && vio_idx <= 300) ||
				vio_idx == 349)
			return 14;
		else if ((vio_idx >= 173 && vio_idx <= 202) || vio_idx == 301 ||
				(vio_idx >= 302 && vio_idx <= 331) ||
				vio_idx == 350)
			return 15;
		else if ((vio_idx >= 203 && vio_idx <= 215) ||
				(vio_idx >= 332 && vio_idx <= 345) ||
				vio_idx == 351)
			return 16;

		pr_err(PFX "%s:%d Wrong vio_idx:0x%x\n",
				__func__, __LINE__, vio_idx);

	} else if (slave_type == SLAVE_TYPE_PERI2) {
		if ((vio_idx >= 0 && vio_idx <= 8) ||
				(vio_idx >= 130 && vio_idx <= 139) ||
				vio_idx == 252)
			return 0;
		else if (vio_idx >= 9 && vio_idx <= 12)
			return 1;
		else if (vio_idx >= 13 && vio_idx <= 16)
			return 2;
		else if (vio_idx >= 17 && vio_idx <= 20)
			return 3;
		else if (vio_idx >= 21 && vio_idx <= 24)
			return 4;
		else if ((vio_idx >= 25 && vio_idx <= 40) ||
				(vio_idx >= 140 && vio_idx <= 156) ||
				vio_idx == 253)
			return 5;
		else if ((vio_idx >= 41 && vio_idx <= 48) ||
				(vio_idx >= 157 && vio_idx <= 165) ||
				vio_idx == 254)
			return 6;
		else if ((vio_idx >= 49 && vio_idx <= 64) ||
				(vio_idx >= 166 && vio_idx <= 182) ||
				vio_idx == 255)
			return 7;
		else if ((vio_idx >= 65 && vio_idx <= 80) ||
				(vio_idx >= 183 && vio_idx <= 199) ||
				vio_idx == 256)
			return 8;
		else if ((vio_idx >= 81 && vio_idx <= 88) ||
				(vio_idx >= 200 && vio_idx <= 208) ||
				vio_idx == 257)
			return 9;
		else if ((vio_idx >= 89 && vio_idx <= 111) ||
				(vio_idx >= 209 && vio_idx <= 232) ||
				vio_idx == 258)
			return 10;
		else if ((vio_idx >= 112 && vio_idx <= 40) ||
				(vio_idx >= 233 && vio_idx <= 251) ||
				vio_idx == 259)
			return 11;

		pr_err(PFX "%s:%d Wrong vio_idx:0x%x\n",
				__func__, __LINE__, vio_idx);

	} else {
		pr_err(PFX "%s:%d Wrong slave_type:0x%x\n",
				__func__, __LINE__, slave_type);
	}

	return 31;
}

void devapc_catch_illegal_range(phys_addr_t phys_addr, size_t size)
{
	phys_addr_t test_pa = 0x17a54c50;

	/*
	 * Catch BROM addr mapped
	 */
	if (phys_addr >= 0x0 && phys_addr < SRAM_START_ADDR) {
		pr_err(PFX "%s %s:(%pa), %s:(0x%lx)\n",
				"catch BROM address mapped!",
				"phys_addr", &phys_addr,
				"size", size);
		BUG_ON(1);
	}

	if ((phys_addr <= test_pa) && (phys_addr + size > test_pa)) {
		pr_err(PFX "%s %s:(%pa), %s:(0x%lx), %s:(%pa)\n",
				"catch VENCSYS address mapped!",
				"phys_addr", &phys_addr,
				"size", size, "test_pa", &test_pa);
		BUG_ON(1);
	}
}
EXPORT_SYMBOL(devapc_catch_illegal_range);

static struct mtk_devapc_dbg_status mt6885_devapc_dbg_stat = {
	.enable_ut = PLAT_DBG_UT_DEFAULT,
	.enable_KE = PLAT_DBG_KE_DEFAULT,
	.enable_AEE = PLAT_DBG_AEE_DEFAULT,
	.enable_WARN = PLAT_DBG_WARN_DEFAULT,
	.enable_dapc = PLAT_DBG_DAPC_DEFAULT,
};

static const char * const slave_type_to_str[] = {
	"SLAVE_TYPE_INFRA",
	"SLAVE_TYPE_PERI",
	"SLAVE_TYPE_PERI2",
	"WRONG_SLAVE_TYPE",
};

static int mtk_vio_mask_sta_num[] = {
	VIO_MASK_STA_NUM_INFRA,
	VIO_MASK_STA_NUM_PERI,
	VIO_MASK_STA_NUM_PERI2,
};

static struct mtk_devapc_vio_info mt6885_devapc_vio_info = {
	.vio_mask_sta_num = mtk_vio_mask_sta_num,
	.sramrom_vio_idx = SRAMROM_VIO_INDEX,
	.mdp_vio_idx = MDP_VIO_INDEX,
	.disp2_vio_idx = DISP2_VIO_INDEX,
	.mmsys_vio_idx = MMSYS_VIO_INDEX,
	.sramrom_slv_type = SRAMROM_SLAVE_TYPE,
	.mm2nd_slv_type = MM2ND_SLAVE_TYPE,
};

static const struct mtk_infra_vio_dbg_desc mt6885_vio_dbgs = {
	.vio_dbg_mstid = INFRA_VIO_DBG_MSTID,
	.vio_dbg_mstid_start_bit = INFRA_VIO_DBG_MSTID_START_BIT,
	.vio_dbg_dmnid = INFRA_VIO_DBG_DMNID,
	.vio_dbg_dmnid_start_bit = INFRA_VIO_DBG_DMNID_START_BIT,
	.vio_dbg_w_vio = INFRA_VIO_DBG_W_VIO,
	.vio_dbg_w_vio_start_bit = INFRA_VIO_DBG_W_VIO_START_BIT,
	.vio_dbg_r_vio = INFRA_VIO_DBG_R_VIO,
	.vio_dbg_r_vio_start_bit = INFRA_VIO_DBG_R_VIO_START_BIT,
	.vio_addr_high = INFRA_VIO_ADDR_HIGH,
	.vio_addr_high_start_bit = INFRA_VIO_ADDR_HIGH_START_BIT,
};

static const struct mtk_sramrom_sec_vio_desc mt6885_sramrom_sec_vios = {
	.vio_id_mask = SRAMROM_SEC_VIO_ID_MASK,
	.vio_id_shift = SRAMROM_SEC_VIO_ID_SHIFT,
	.vio_domain_mask = SRAMROM_SEC_VIO_DOMAIN_MASK,
	.vio_domain_shift = SRAMROM_SEC_VIO_DOMAIN_SHIFT,
	.vio_rw_mask = SRAMROM_SEC_VIO_RW_MASK,
	.vio_rw_shift = SRAMROM_SEC_VIO_RW_SHIFT,
};

static const uint32_t mt6885_devapc_pds[] = {
	PD_VIO_MASK_OFFSET,
	PD_VIO_STA_OFFSET,
	PD_VIO_DBG0_OFFSET,
	PD_VIO_DBG1_OFFSET,
	PD_VIO_DBG2_OFFSET,
	PD_APC_CON_OFFSET,
	PD_SHIFT_STA_OFFSET,
	PD_SHIFT_SEL_OFFSET,
	PD_SHIFT_CON_OFFSET,
};

static struct mtk_devapc_soc mt6885_data = {
	.dbg_stat = &mt6885_devapc_dbg_stat,
	.slave_type_arr = slave_type_to_str,
	.slave_type_num = SLAVE_TYPE_NUM,
	.device_info[SLAVE_TYPE_INFRA] = mt6885_devices_infra,
	.device_info[SLAVE_TYPE_PERI] = mt6885_devices_peri,
	.device_info[SLAVE_TYPE_PERI2] = mt6885_devices_peri2,
	.ndevices = mtk6885_devices_num,
	.vio_info = &mt6885_devapc_vio_info,
	.vio_dbgs = &mt6885_vio_dbgs,
	.sramrom_sec_vios = &mt6885_sramrom_sec_vios,
	.devapc_pds = mt6885_devapc_pds,
	.subsys_get = &index_to_subsys,
	.master_get = &mt6885_bus_id_to_master,
	.mm2nd_vio_handler = &mm2nd_vio_handler,
	.shift_group_get = mt6885_shift_group_get,
};

static const struct of_device_id mt6885_devapc_dt_match[] = {
	{ .compatible = "mediatek,mt6885-devapc" },
	{},
};

static int mt6885_devapc_probe(struct platform_device *pdev)
{
	return mtk_devapc_probe(pdev, &mt6885_data);
}

static int mt6885_devapc_remove(struct platform_device *dev)
{
	return mtk_devapc_remove(dev);
}

static struct platform_driver mt6885_devapc_driver = {
	.probe = mt6885_devapc_probe,
	.remove = mt6885_devapc_remove,
	.driver = {
		.name = KBUILD_MODNAME,
		.of_match_table = mt6885_devapc_dt_match,
	},
};

module_platform_driver(mt6885_devapc_driver);

MODULE_DESCRIPTION("Mediatek MT6885 Device APC Driver");
MODULE_AUTHOR("Neal Liu <neal.liu@mediatek.com>");
MODULE_LICENSE("GPL");
