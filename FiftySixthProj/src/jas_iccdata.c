/*
 * Copyright (c) 2002-2003 Michael David Adams.
 * All rights reserved.
 */

/* __START_OF_JASPER_LICENSE__
 * 
 * JasPer License Version 2.0
 * 
 * Copyright (c) 1999-2000 Image Power, Inc.
 * Copyright (c) 1999-2000 The University of British Columbia
 * Copyright (c) 2001-2003 Michael David Adams
 * 
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person (the
 * "User") obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 * 
 * 1.  The above copyright notices and this permission notice (which
 * includes the disclaimer below) shall be included in all copies or
 * substantial portions of the Software.
 * 
 * 2.  The name of a copyright holder shall not be used to endorse or
 * promote products derived from the Software without specific prior
 * written permission.
 * 
 * THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS
 * LICENSE.  NO USE OF THE SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
 * THIS DISCLAIMER.  THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  NO ASSURANCES ARE
 * PROVIDED BY THE COPYRIGHT HOLDERS THAT THE SOFTWARE DOES NOT INFRINGE
 * THE PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF ANY OTHER ENTITY.
 * EACH COPYRIGHT HOLDER DISCLAIMS ANY LIABILITY TO THE USER FOR CLAIMS
 * BROUGHT BY ANY OTHER ENTITY BASED ON INFRINGEMENT OF INTELLECTUAL
 * PROPERTY RIGHTS OR OTHERWISE.  AS A CONDITION TO EXERCISING THE RIGHTS
 * GRANTED HEREUNDER, EACH USER HEREBY ASSUMES SOLE RESPONSIBILITY TO SECURE
 * ANY OTHER INTELLECTUAL PROPERTY RIGHTS NEEDED, IF ANY.  THE SOFTWARE
 * IS NOT FAULT-TOLERANT AND IS NOT INTENDED FOR USE IN MISSION-CRITICAL
 * SYSTEMS, SUCH AS THOSE USED IN THE OPERATION OF NUCLEAR FACILITIES,
 * AIRCRAFT NAVIGATION OR COMMUNICATION SYSTEMS, AIR TRAFFIC CONTROL
 * SYSTEMS, DIRECT LIFE SUPPORT MACHINES, OR WEAPONS SYSTEMS, IN WHICH
 * THE FAILURE OF THE SOFTWARE OR SYSTEM COULD LEAD DIRECTLY TO DEATH,
 * PERSONAL INJURY, OR SEVERE PHYSICAL OR ENVIRONMENTAL DAMAGE ("HIGH
 * RISK ACTIVITIES").  THE COPYRIGHT HOLDERS SPECIFICALLY DISCLAIM ANY
 * EXPRESS OR IMPLIED WARRANTY OF FITNESS FOR HIGH RISK ACTIVITIES.
 * 
 * __END_OF_JASPER_LICENSE__
 */

#include <jas_config.h>
#include <jas_types.h>

uchar jas_iccprofdata_srgb[] =
{
	0x00, 0x00, 0x0c, 0x48, 0x4c, 0x69, 0x6e, 0x6f,
	0x02, 0x10, 0x00, 0x00, 0x6d, 0x6e, 0x74, 0x72,
	0x52, 0x47, 0x42, 0x20, 0x58, 0x59, 0x5a, 0x20,
	0x07, 0xce, 0x00, 0x02, 0x00, 0x09, 0x00, 0x06,
	0x00, 0x31, 0x00, 0x00, 0x61, 0x63, 0x73, 0x70,
	0x4d, 0x53, 0x46, 0x54, 0x00, 0x00, 0x00, 0x00,
	0x49, 0x45, 0x43, 0x20, 0x73, 0x52, 0x47, 0x42,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf6, 0xd6,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xd3, 0x2d,
	0x48, 0x50, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x11, 0x63, 0x70, 0x72, 0x74,
	0x00, 0x00, 0x01, 0x50, 0x00, 0x00, 0x00, 0x33,
	0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x01, 0x84,
	0x00, 0x00, 0x00, 0x6c, 0x77, 0x74, 0x70, 0x74,
	0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x14,
	0x62, 0x6b, 0x70, 0x74, 0x00, 0x00, 0x02, 0x04,
	0x00, 0x00, 0x00, 0x14, 0x72, 0x58, 0x59, 0x5a,
	0x00, 0x00, 0x02, 0x18, 0x00, 0x00, 0x00, 0x14,
	0x67, 0x58, 0x59, 0x5a, 0x00, 0x00, 0x02, 0x2c,
	0x00, 0x00, 0x00, 0x14, 0x62, 0x58, 0x59, 0x5a,
	0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x14,
	0x64, 0x6d, 0x6e, 0x64, 0x00, 0x00, 0x02, 0x54,
	0x00, 0x00, 0x00, 0x70, 0x64, 0x6d, 0x64, 0x64,
	0x00, 0x00, 0x02, 0xc4, 0x00, 0x00, 0x00, 0x88,
	0x76, 0x75, 0x65, 0x64, 0x00, 0x00, 0x03, 0x4c,
	0x00, 0x00, 0x00, 0x86, 0x76, 0x69, 0x65, 0x77,
	0x00, 0x00, 0x03, 0xd4, 0x00, 0x00, 0x00, 0x24,
	0x6c, 0x75, 0x6d, 0x69, 0x00, 0x00, 0x03, 0xf8,
	0x00, 0x00, 0x00, 0x14, 0x6d, 0x65, 0x61, 0x73,
	0x00, 0x00, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x24,
	0x74, 0x65, 0x63, 0x68, 0x00, 0x00, 0x04, 0x30,
	0x00, 0x00, 0x00, 0x0c, 0x72, 0x54, 0x52, 0x43,
	0x00, 0x00, 0x04, 0x3c, 0x00, 0x00, 0x08, 0x0c,
	0x67, 0x54, 0x52, 0x43, 0x00, 0x00, 0x04, 0x3c,
	0x00, 0x00, 0x08, 0x0c, 0x62, 0x54, 0x52, 0x43,
	0x00, 0x00, 0x04, 0x3c, 0x00, 0x00, 0x08, 0x0c,
	0x74, 0x65, 0x78, 0x74, 0x00, 0x00, 0x00, 0x00,
	0x43, 0x6f, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68,
	0x74, 0x20, 0x28, 0x63, 0x29, 0x20, 0x31, 0x39,
	0x39, 0x38, 0x20, 0x48, 0x65, 0x77, 0x6c, 0x65,
	0x74, 0x74, 0x2d, 0x50, 0x61, 0x63, 0x6b, 0x61,
	0x72, 0x64, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x61,
	0x6e, 0x79, 0x00, 0x00, 0x64, 0x65, 0x73, 0x63,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12,
	0x73, 0x52, 0x47, 0x42, 0x20, 0x49, 0x45, 0x43,
	0x36, 0x31, 0x39, 0x36, 0x36, 0x2d, 0x32, 0x2e,
	0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x12, 0x73, 0x52, 0x47,
	0x42, 0x20, 0x49, 0x45, 0x43, 0x36, 0x31, 0x39,
	0x36, 0x36, 0x2d, 0x32, 0x2e, 0x31, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xf3, 0x51, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x16, 0xcc, 0x58, 0x59, 0x5a, 0x20,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x6f, 0xa2, 0x00, 0x00, 0x38, 0xf5,
	0x00, 0x00, 0x03, 0x90, 0x58, 0x59, 0x5a, 0x20,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x99,
	0x00, 0x00, 0xb7, 0x85, 0x00, 0x00, 0x18, 0xda,
	0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x24, 0xa0, 0x00, 0x00, 0x0f, 0x84,
	0x00, 0x00, 0xb6, 0xcf, 0x64, 0x65, 0x73, 0x63,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16,
	0x49, 0x45, 0x43, 0x20, 0x68, 0x74, 0x74, 0x70,
	0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x69,
	0x65, 0x63, 0x2e, 0x63, 0x68, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x16, 0x49, 0x45, 0x43, 0x20, 0x68, 0x74, 0x74,
	0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e,
	0x69, 0x65, 0x63, 0x2e, 0x63, 0x68, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x64, 0x65, 0x73, 0x63,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2e,
	0x49, 0x45, 0x43, 0x20, 0x36, 0x31, 0x39, 0x36,
	0x36, 0x2d, 0x32, 0x2e, 0x31, 0x20, 0x44, 0x65,
	0x66, 0x61, 0x75, 0x6c, 0x74, 0x20, 0x52, 0x47,
	0x42, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x75, 0x72,
	0x20, 0x73, 0x70, 0x61, 0x63, 0x65, 0x20, 0x2d,
	0x20, 0x73, 0x52, 0x47, 0x42, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x2e, 0x49, 0x45, 0x43, 0x20, 0x36, 0x31, 0x39,
	0x36, 0x36, 0x2d, 0x32, 0x2e, 0x31, 0x20, 0x44,
	0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x20, 0x52,
	0x47, 0x42, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x75,
	0x72, 0x20, 0x73, 0x70, 0x61, 0x63, 0x65, 0x20,
	0x2d, 0x20, 0x73, 0x52, 0x47, 0x42, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x64, 0x65, 0x73, 0x63,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c,
	0x52, 0x65, 0x66, 0x65, 0x72, 0x65, 0x6e, 0x63,
	0x65, 0x20, 0x56, 0x69, 0x65, 0x77, 0x69, 0x6e,
	0x67, 0x20, 0x43, 0x6f, 0x6e, 0x64, 0x69, 0x74,
	0x69, 0x6f, 0x6e, 0x20, 0x69, 0x6e, 0x20, 0x49,
	0x45, 0x43, 0x36, 0x31, 0x39, 0x36, 0x36, 0x2d,
	0x32, 0x2e, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x52,
	0x65, 0x66, 0x65, 0x72, 0x65, 0x6e, 0x63, 0x65,
	0x20, 0x56, 0x69, 0x65, 0x77, 0x69, 0x6e, 0x67,
	0x20, 0x43, 0x6f, 0x6e, 0x64, 0x69, 0x74, 0x69,
	0x6f, 0x6e, 0x20, 0x69, 0x6e, 0x20, 0x49, 0x45,
	0x43, 0x36, 0x31, 0x39, 0x36, 0x36, 0x2d, 0x32,
	0x2e, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x76, 0x69, 0x65, 0x77,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xa4, 0xfe,
	0x00, 0x14, 0x5f, 0x2e, 0x00, 0x10, 0xcf, 0x14,
	0x00, 0x03, 0xed, 0xcc, 0x00, 0x04, 0x13, 0x0b,
	0x00, 0x03, 0x5c, 0x9e, 0x00, 0x00, 0x00, 0x01,
	0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x4c, 0x09, 0x56, 0x00, 0x50, 0x00, 0x00,
	0x00, 0x57, 0x1f, 0xe7, 0x6d, 0x65, 0x61, 0x73,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x02, 0x8f, 0x00, 0x00, 0x00, 0x02,
	0x73, 0x69, 0x67, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x43, 0x52, 0x54, 0x20, 0x63, 0x75, 0x72, 0x76,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x0a, 0x00, 0x0f,
	0x00, 0x14, 0x00, 0x19, 0x00, 0x1e, 0x00, 0x23,
	0x00, 0x28, 0x00, 0x2d, 0x00, 0x32, 0x00, 0x37,
	0x00, 0x3b, 0x00, 0x40, 0x00, 0x45, 0x00, 0x4a,
	0x00, 0x4f, 0x00, 0x54, 0x00, 0x59, 0x00, 0x5e,
	0x00, 0x63, 0x00, 0x68, 0x00, 0x6d, 0x00, 0x72,
	0x00, 0x77, 0x00, 0x7c, 0x00, 0x81, 0x00, 0x86,
	0x00, 0x8b, 0x00, 0x90, 0x00, 0x95, 0x00, 0x9a,
	0x00, 0x9f, 0x00, 0xa4, 0x00, 0xa9, 0x00, 0xae,
	0x00, 0xb2, 0x00, 0xb7, 0x00, 0xbc, 0x00, 0xc1,
	0x00, 0xc6, 0x00, 0xcb, 0x00, 0xd0, 0x00, 0xd5,
	0x00, 0xdb, 0x00, 0xe0, 0x00, 0xe5, 0x00, 0xeb,
	0x00, 0xf0, 0x00, 0xf6, 0x00, 0xfb, 0x01, 0x01,
	0x01, 0x07, 0x01, 0x0d, 0x01, 0x13, 0x01, 0x19,
	0x01, 0x1f, 0x01, 0x25, 0x01, 0x2b, 0x01, 0x32,
	0x01, 0x38, 0x01, 0x3e, 0x01, 0x45, 0x01, 0x4c,
	0x01, 0x52, 0x01, 0x59, 0x01, 0x60, 0x01, 0x67,
	0x01, 0x6e, 0x01, 0x75, 0x01, 0x7c, 0x01, 0x83,
	0x01, 0x8b, 0x01, 0x92, 0x01, 0x9a, 0x01, 0xa1,
	0x01, 0xa9, 0x01, 0xb1, 0x01, 0xb9, 0x01, 0xc1,
	0x01, 0xc9, 0x01, 0xd1, 0x01, 0xd9, 0x01, 0xe1,
	0x01, 0xe9, 0x01, 0xf2, 0x01, 0xfa, 0x02, 0x03,
	0x02, 0x0c, 0x02, 0x14, 0x02, 0x1d, 0x02, 0x26,
	0x02, 0x2f, 0x02, 0x38, 0x02, 0x41, 0x02, 0x4b,
	0x02, 0x54, 0x02, 0x5d, 0x02, 0x67, 0x02, 0x71,
	0x02, 0x7a, 0x02, 0x84, 0x02, 0x8e, 0x02, 0x98,
	0x02, 0xa2, 0x02, 0xac, 0x02, 0xb6, 0x02, 0xc1,
	0x02, 0xcb, 0x02, 0xd5, 0x02, 0xe0, 0x02, 0xeb,
	0x02, 0xf5, 0x03, 0x00, 0x03, 0x0b, 0x03, 0x16,
	0x03, 0x21, 0x03, 0x2d, 0x03, 0x38, 0x03, 0x43,
	0x03, 0x4f, 0x03, 0x5a, 0x03, 0x66, 0x03, 0x72,
	0x03, 0x7e, 0x03, 0x8a, 0x03, 0x96, 0x03, 0xa2,
	0x03, 0xae, 0x03, 0xba, 0x03, 0xc7, 0x03, 0xd3,
	0x03, 0xe0, 0x03, 0xec, 0x03, 0xf9, 0x04, 0x06,
	0x04, 0x13, 0x04, 0x20, 0x04, 0x2d, 0x04, 0x3b,
	0x04, 0x48, 0x04, 0x55, 0x04, 0x63, 0x04, 0x71,
	0x04, 0x7e, 0x04, 0x8c, 0x04, 0x9a, 0x04, 0xa8,
	0x04, 0xb6, 0x04, 0xc4, 0x04, 0xd3, 0x04, 0xe1,
	0x04, 0xf0, 0x04, 0xfe, 0x05, 0x0d, 0x05, 0x1c,
	0x05, 0x2b, 0x05, 0x3a, 0x05, 0x49, 0x05, 0x58,
	0x05, 0x67, 0x05, 0x77, 0x05, 0x86, 0x05, 0x96,
	0x05, 0xa6, 0x05, 0xb5, 0x05, 0xc5, 0x05, 0xd5,
	0x05, 0xe5, 0x05, 0xf6, 0x06, 0x06, 0x06, 0x16,
	0x06, 0x27, 0x06, 0x37, 0x06, 0x48, 0x06, 0x59,
	0x06, 0x6a, 0x06, 0x7b, 0x06, 0x8c, 0x06, 0x9d,
	0x06, 0xaf, 0x06, 0xc0, 0x06, 0xd1, 0x06, 0xe3,
	0x06, 0xf5, 0x07, 0x07, 0x07, 0x19, 0x07, 0x2b,
	0x07, 0x3d, 0x07, 0x4f, 0x07, 0x61, 0x07, 0x74,
	0x07, 0x86, 0x07, 0x99, 0x07, 0xac, 0x07, 0xbf,
	0x07, 0xd2, 0x07, 0xe5, 0x07, 0xf8, 0x08, 0x0b,
	0x08, 0x1f, 0x08, 0x32, 0x08, 0x46, 0x08, 0x5a,
	0x08, 0x6e, 0x08, 0x82, 0x08, 0x96, 0x08, 0xaa,
	0x08, 0xbe, 0x08, 0xd2, 0x08, 0xe7, 0x08, 0xfb,
	0x09, 0x10, 0x09, 0x25, 0x09, 0x3a, 0x09, 0x4f,
	0x09, 0x64, 0x09, 0x79, 0x09, 0x8f, 0x09, 0xa4,
	0x09, 0xba, 0x09, 0xcf, 0x09, 0xe5, 0x09, 0xfb,
	0x0a, 0x11, 0x0a, 0x27, 0x0a, 0x3d, 0x0a, 0x54,
	0x0a, 0x6a, 0x0a, 0x81, 0x0a, 0x98, 0x0a, 0xae,
	0x0a, 0xc5, 0x0a, 0xdc, 0x0a, 0xf3, 0x0b, 0x0b,
	0x0b, 0x22, 0x0b, 0x39, 0x0b, 0x51, 0x0b, 0x69,
	0x0b, 0x80, 0x0b, 0x98, 0x0b, 0xb0, 0x0b, 0xc8,
	0x0b, 0xe1, 0x0b, 0xf9, 0x0c, 0x12, 0x0c, 0x2a,
	0x0c, 0x43, 0x0c, 0x5c, 0x0c, 0x75, 0x0c, 0x8e,
	0x0c, 0xa7, 0x0c, 0xc0, 0x0c, 0xd9, 0x0c, 0xf3,
	0x0d, 0x0d, 0x0d, 0x26, 0x0d, 0x40, 0x0d, 0x5a,
	0x0d, 0x74, 0x0d, 0x8e, 0x0d, 0xa9, 0x0d, 0xc3,
	0x0d, 0xde, 0x0d, 0xf8, 0x0e, 0x13, 0x0e, 0x2e,
	0x0e, 0x49, 0x0e, 0x64, 0x0e, 0x7f, 0x0e, 0x9b,
	0x0e, 0xb6, 0x0e, 0xd2, 0x0e, 0xee, 0x0f, 0x09,
	0x0f, 0x25, 0x0f, 0x41, 0x0f, 0x5e, 0x0f, 0x7a,
	0x0f, 0x96, 0x0f, 0xb3, 0x0f, 0xcf, 0x0f, 0xec,
	0x10, 0x09, 0x10, 0x26, 0x10, 0x43, 0x10, 0x61,
	0x10, 0x7e, 0x10, 0x9b, 0x10, 0xb9, 0x10, 0xd7,
	0x10, 0xf5, 0x11, 0x13, 0x11, 0x31, 0x11, 0x4f,
	0x11, 0x6d, 0x11, 0x8c, 0x11, 0xaa, 0x11, 0xc9,
	0x11, 0xe8, 0x12, 0x07, 0x12, 0x26, 0x12, 0x45,
	0x12, 0x64, 0x12, 0x84, 0x12, 0xa3, 0x12, 0xc3,
	0x12, 0xe3, 0x13, 0x03, 0x13, 0x23, 0x13, 0x43,
	0x13, 0x63, 0x13, 0x83, 0x13, 0xa4, 0x13, 0xc5,
	0x13, 0xe5, 0x14, 0x06, 0x14, 0x27, 0x14, 0x49,
	0x14, 0x6a, 0x14, 0x8b, 0x14, 0xad, 0x14, 0xce,
	0x14, 0xf0, 0x15, 0x12, 0x15, 0x34, 0x15, 0x56,
	0x15, 0x78, 0x15, 0x9b, 0x15, 0xbd, 0x15, 0xe0,
	0x16, 0x03, 0x16, 0x26, 0x16, 0x49, 0x16, 0x6c,
	0x16, 0x8f, 0x16, 0xb2, 0x16, 0xd6, 0x16, 0xfa,
	0x17, 0x1d, 0x17, 0x41, 0x17, 0x65, 0x17, 0x89,
	0x17, 0xae, 0x17, 0xd2, 0x17, 0xf7, 0x18, 0x1b,
	0x18, 0x40, 0x18, 0x65, 0x18, 0x8a, 0x18, 0xaf,
	0x18, 0xd5, 0x18, 0xfa, 0x19, 0x20, 0x19, 0x45,
	0x19, 0x6b, 0x19, 0x91, 0x19, 0xb7, 0x19, 0xdd,
	0x1a, 0x04, 0x1a, 0x2a, 0x1a, 0x51, 0x1a, 0x77,
	0x1a, 0x9e, 0x1a, 0xc5, 0x1a, 0xec, 0x1b, 0x14,
	0x1b, 0x3b, 0x1b, 0x63, 0x1b, 0x8a, 0x1b, 0xb2,
	0x1b, 0xda, 0x1c, 0x02, 0x1c, 0x2a, 0x1c, 0x52,
	0x1c, 0x7b, 0x1c, 0xa3, 0x1c, 0xcc, 0x1c, 0xf5,
	0x1d, 0x1e, 0x1d, 0x47, 0x1d, 0x70, 0x1d, 0x99,
	0x1d, 0xc3, 0x1d, 0xec, 0x1e, 0x16, 0x1e, 0x40,
	0x1e, 0x6a, 0x1e, 0x94, 0x1e, 0xbe, 0x1e, 0xe9,
	0x1f, 0x13, 0x1f, 0x3e, 0x1f, 0x69, 0x1f, 0x94,
	0x1f, 0xbf, 0x1f, 0xea, 0x20, 0x15, 0x20, 0x41,
	0x20, 0x6c, 0x20, 0x98, 0x20, 0xc4, 0x20, 0xf0,
	0x21, 0x1c, 0x21, 0x48, 0x21, 0x75, 0x21, 0xa1,
	0x21, 0xce, 0x21, 0xfb, 0x22, 0x27, 0x22, 0x55,
	0x22, 0x82, 0x22, 0xaf, 0x22, 0xdd, 0x23, 0x0a,
	0x23, 0x38, 0x23, 0x66, 0x23, 0x94, 0x23, 0xc2,
	0x23, 0xf0, 0x24, 0x1f, 0x24, 0x4d, 0x24, 0x7c,
	0x24, 0xab, 0x24, 0xda, 0x25, 0x09, 0x25, 0x38,
	0x25, 0x68, 0x25, 0x97, 0x25, 0xc7, 0x25, 0xf7,
	0x26, 0x27, 0x26, 0x57, 0x26, 0x87, 0x26, 0xb7,
	0x26, 0xe8, 0x27, 0x18, 0x27, 0x49, 0x27, 0x7a,
	0x27, 0xab, 0x27, 0xdc, 0x28, 0x0d, 0x28, 0x3f,
	0x28, 0x71, 0x28, 0xa2, 0x28, 0xd4, 0x29, 0x06,
	0x29, 0x38, 0x29, 0x6b, 0x29, 0x9d, 0x29, 0xd0,
	0x2a, 0x02, 0x2a, 0x35, 0x2a, 0x68, 0x2a, 0x9b,
	0x2a, 0xcf, 0x2b, 0x02, 0x2b, 0x36, 0x2b, 0x69,
	0x2b, 0x9d, 0x2b, 0xd1, 0x2c, 0x05, 0x2c, 0x39,
	0x2c, 0x6e, 0x2c, 0xa2, 0x2c, 0xd7, 0x2d, 0x0c,
	0x2d, 0x41, 0x2d, 0x76, 0x2d, 0xab, 0x2d, 0xe1,
	0x2e, 0x16, 0x2e, 0x4c, 0x2e, 0x82, 0x2e, 0xb7,
	0x2e, 0xee, 0x2f, 0x24, 0x2f, 0x5a, 0x2f, 0x91,
	0x2f, 0xc7, 0x2f, 0xfe, 0x30, 0x35, 0x30, 0x6c,
	0x30, 0xa4, 0x30, 0xdb, 0x31, 0x12, 0x31, 0x4a,
	0x31, 0x82, 0x31, 0xba, 0x31, 0xf2, 0x32, 0x2a,
	0x32, 0x63, 0x32, 0x9b, 0x32, 0xd4, 0x33, 0x0d,
	0x33, 0x46, 0x33, 0x7f, 0x33, 0xb8, 0x33, 0xf1,
	0x34, 0x2b, 0x34, 0x65, 0x34, 0x9e, 0x34, 0xd8,
	0x35, 0x13, 0x35, 0x4d, 0x35, 0x87, 0x35, 0xc2,
	0x35, 0xfd, 0x36, 0x37, 0x36, 0x72, 0x36, 0xae,
	0x36, 0xe9, 0x37, 0x24, 0x37, 0x60, 0x37, 0x9c,
	0x37, 0xd7, 0x38, 0x14, 0x38, 0x50, 0x38, 0x8c,
	0x38, 0xc8, 0x39, 0x05, 0x39, 0x42, 0x39, 0x7f,
	0x39, 0xbc, 0x39, 0xf9, 0x3a, 0x36, 0x3a, 0x74,
	0x3a, 0xb2, 0x3a, 0xef, 0x3b, 0x2d, 0x3b, 0x6b,
	0x3b, 0xaa, 0x3b, 0xe8, 0x3c, 0x27, 0x3c, 0x65,
	0x3c, 0xa4, 0x3c, 0xe3, 0x3d, 0x22, 0x3d, 0x61,
	0x3d, 0xa1, 0x3d, 0xe0, 0x3e, 0x20, 0x3e, 0x60,
	0x3e, 0xa0, 0x3e, 0xe0, 0x3f, 0x21, 0x3f, 0x61,
	0x3f, 0xa2, 0x3f, 0xe2, 0x40, 0x23, 0x40, 0x64,
	0x40, 0xa6, 0x40, 0xe7, 0x41, 0x29, 0x41, 0x6a,
	0x41, 0xac, 0x41, 0xee, 0x42, 0x30, 0x42, 0x72,
	0x42, 0xb5, 0x42, 0xf7, 0x43, 0x3a, 0x43, 0x7d,
	0x43, 0xc0, 0x44, 0x03, 0x44, 0x47, 0x44, 0x8a,
	0x44, 0xce, 0x45, 0x12, 0x45, 0x55, 0x45, 0x9a,
	0x45, 0xde, 0x46, 0x22, 0x46, 0x67, 0x46, 0xab,
	0x46, 0xf0, 0x47, 0x35, 0x47, 0x7b, 0x47, 0xc0,
	0x48, 0x05, 0x48, 0x4b, 0x48, 0x91, 0x48, 0xd7,
	0x49, 0x1d, 0x49, 0x63, 0x49, 0xa9, 0x49, 0xf0,
	0x4a, 0x37, 0x4a, 0x7d, 0x4a, 0xc4, 0x4b, 0x0c,
	0x4b, 0x53, 0x4b, 0x9a, 0x4b, 0xe2, 0x4c, 0x2a,
	0x4c, 0x72, 0x4c, 0xba, 0x4d, 0x02, 0x4d, 0x4a,
	0x4d, 0x93, 0x4d, 0xdc, 0x4e, 0x25, 0x4e, 0x6e,
	0x4e, 0xb7, 0x4f, 0x00, 0x4f, 0x49, 0x4f, 0x93,
	0x4f, 0xdd, 0x50, 0x27, 0x50, 0x71, 0x50, 0xbb,
	0x51, 0x06, 0x51, 0x50, 0x51, 0x9b, 0x51, 0xe6,
	0x52, 0x31, 0x52, 0x7c, 0x52, 0xc7, 0x53, 0x13,
	0x53, 0x5f, 0x53, 0xaa, 0x53, 0xf6, 0x54, 0x42,
	0x54, 0x8f, 0x54, 0xdb, 0x55, 0x28, 0x55, 0x75,
	0x55, 0xc2, 0x56, 0x0f, 0x56, 0x5c, 0x56, 0xa9,
	0x56, 0xf7, 0x57, 0x44, 0x57, 0x92, 0x57, 0xe0,
	0x58, 0x2f, 0x58, 0x7d, 0x58, 0xcb, 0x59, 0x1a,
	0x59, 0x69, 0x59, 0xb8, 0x5a, 0x07, 0x5a, 0x56,
	0x5a, 0xa6, 0x5a, 0xf5, 0x5b, 0x45, 0x5b, 0x95,
	0x5b, 0xe5, 0x5c, 0x35, 0x5c, 0x86, 0x5c, 0xd6,
	0x5d, 0x27, 0x5d, 0x78, 0x5d, 0xc9, 0x5e, 0x1a,
	0x5e, 0x6c, 0x5e, 0xbd, 0x5f, 0x0f, 0x5f, 0x61,
	0x5f, 0xb3, 0x60, 0x05, 0x60, 0x57, 0x60, 0xaa,
	0x60, 0xfc, 0x61, 0x4f, 0x61, 0xa2, 0x61, 0xf5,
	0x62, 0x49, 0x62, 0x9c, 0x62, 0xf0, 0x63, 0x43,
	0x63, 0x97, 0x63, 0xeb, 0x64, 0x40, 0x64, 0x94,
	0x64, 0xe9, 0x65, 0x3d, 0x65, 0x92, 0x65, 0xe7,
	0x66, 0x3d, 0x66, 0x92, 0x66, 0xe8, 0x67, 0x3d,
	0x67, 0x93, 0x67, 0xe9, 0x68, 0x3f, 0x68, 0x96,
	0x68, 0xec, 0x69, 0x43, 0x69, 0x9a, 0x69, 0xf1,
	0x6a, 0x48, 0x6a, 0x9f, 0x6a, 0xf7, 0x6b, 0x4f,
	0x6b, 0xa7, 0x6b, 0xff, 0x6c, 0x57, 0x6c, 0xaf,
	0x6d, 0x08, 0x6d, 0x60, 0x6d, 0xb9, 0x6e, 0x12,
	0x6e, 0x6b, 0x6e, 0xc4, 0x6f, 0x1e, 0x6f, 0x78,
	0x6f, 0xd1, 0x70, 0x2b, 0x70, 0x86, 0x70, 0xe0,
	0x71, 0x3a, 0x71, 0x95, 0x71, 0xf0, 0x72, 0x4b,
	0x72, 0xa6, 0x73, 0x01, 0x73, 0x5d, 0x73, 0xb8,
	0x74, 0x14, 0x74, 0x70, 0x74, 0xcc, 0x75, 0x28,
	0x75, 0x85, 0x75, 0xe1, 0x76, 0x3e, 0x76, 0x9b,
	0x76, 0xf8, 0x77, 0x56, 0x77, 0xb3, 0x78, 0x11,
	0x78, 0x6e, 0x78, 0xcc, 0x79, 0x2a, 0x79, 0x89,
	0x79, 0xe7, 0x7a, 0x46, 0x7a, 0xa5, 0x7b, 0x04,
	0x7b, 0x63, 0x7b, 0xc2, 0x7c, 0x21, 0x7c, 0x81,
	0x7c, 0xe1, 0x7d, 0x41, 0x7d, 0xa1, 0x7e, 0x01,
	0x7e, 0x62, 0x7e, 0xc2, 0x7f, 0x23, 0x7f, 0x84,
	0x7f, 0xe5, 0x80, 0x47, 0x80, 0xa8, 0x81, 0x0a,
	0x81, 0x6b, 0x81, 0xcd, 0x82, 0x30, 0x82, 0x92,
	0x82, 0xf4, 0x83, 0x57, 0x83, 0xba, 0x84, 0x1d,
	0x84, 0x80, 0x84, 0xe3, 0x85, 0x47, 0x85, 0xab,
	0x86, 0x0e, 0x86, 0x72, 0x86, 0xd7, 0x87, 0x3b,
	0x87, 0x9f, 0x88, 0x04, 0x88, 0x69, 0x88, 0xce,
	0x89, 0x33, 0x89, 0x99, 0x89, 0xfe, 0x8a, 0x64,
	0x8a, 0xca, 0x8b, 0x30, 0x8b, 0x96, 0x8b, 0xfc,
	0x8c, 0x63, 0x8c, 0xca, 0x8d, 0x31, 0x8d, 0x98,
	0x8d, 0xff, 0x8e, 0x66, 0x8e, 0xce, 0x8f, 0x36,
	0x8f, 0x9e, 0x90, 0x06, 0x90, 0x6e, 0x90, 0xd6,
	0x91, 0x3f, 0x91, 0xa8, 0x92, 0x11, 0x92, 0x7a,
	0x92, 0xe3, 0x93, 0x4d, 0x93, 0xb6, 0x94, 0x20,
	0x94, 0x8a, 0x94, 0xf4, 0x95, 0x5f, 0x95, 0xc9,
	0x96, 0x34, 0x96, 0x9f, 0x97, 0x0a, 0x97, 0x75,
	0x97, 0xe0, 0x98, 0x4c, 0x98, 0xb8, 0x99, 0x24,
	0x99, 0x90, 0x99, 0xfc, 0x9a, 0x68, 0x9a, 0xd5,
	0x9b, 0x42, 0x9b, 0xaf, 0x9c, 0x1c, 0x9c, 0x89,
	0x9c, 0xf7, 0x9d, 0x64, 0x9d, 0xd2, 0x9e, 0x40,
	0x9e, 0xae, 0x9f, 0x1d, 0x9f, 0x8b, 0x9f, 0xfa,
	0xa0, 0x69, 0xa0, 0xd8, 0xa1, 0x47, 0xa1, 0xb6,
	0xa2, 0x26, 0xa2, 0x96, 0xa3, 0x06, 0xa3, 0x76,
	0xa3, 0xe6, 0xa4, 0x56, 0xa4, 0xc7, 0xa5, 0x38,
	0xa5, 0xa9, 0xa6, 0x1a, 0xa6, 0x8b, 0xa6, 0xfd,
	0xa7, 0x6e, 0xa7, 0xe0, 0xa8, 0x52, 0xa8, 0xc4,
	0xa9, 0x37, 0xa9, 0xa9, 0xaa, 0x1c, 0xaa, 0x8f,
	0xab, 0x02, 0xab, 0x75, 0xab, 0xe9, 0xac, 0x5c,
	0xac, 0xd0, 0xad, 0x44, 0xad, 0xb8, 0xae, 0x2d,
	0xae, 0xa1, 0xaf, 0x16, 0xaf, 0x8b, 0xb0, 0x00,
	0xb0, 0x75, 0xb0, 0xea, 0xb1, 0x60, 0xb1, 0xd6,
	0xb2, 0x4b, 0xb2, 0xc2, 0xb3, 0x38, 0xb3, 0xae,
	0xb4, 0x25, 0xb4, 0x9c, 0xb5, 0x13, 0xb5, 0x8a,
	0xb6, 0x01, 0xb6, 0x79, 0xb6, 0xf0, 0xb7, 0x68,
	0xb7, 0xe0, 0xb8, 0x59, 0xb8, 0xd1, 0xb9, 0x4a,
	0xb9, 0xc2, 0xba, 0x3b, 0xba, 0xb5, 0xbb, 0x2e,
	0xbb, 0xa7, 0xbc, 0x21, 0xbc, 0x9b, 0xbd, 0x15,
	0xbd, 0x8f, 0xbe, 0x0a, 0xbe, 0x84, 0xbe, 0xff,
	0xbf, 0x7a, 0xbf, 0xf5, 0xc0, 0x70, 0xc0, 0xec,
	0xc1, 0x67, 0xc1, 0xe3, 0xc2, 0x5f, 0xc2, 0xdb,
	0xc3, 0x58, 0xc3, 0xd4, 0xc4, 0x51, 0xc4, 0xce,
	0xc5, 0x4b, 0xc5, 0xc8, 0xc6, 0x46, 0xc6, 0xc3,
	0xc7, 0x41, 0xc7, 0xbf, 0xc8, 0x3d, 0xc8, 0xbc,
	0xc9, 0x3a, 0xc9, 0xb9, 0xca, 0x38, 0xca, 0xb7,
	0xcb, 0x36, 0xcb, 0xb6, 0xcc, 0x35, 0xcc, 0xb5,
	0xcd, 0x35, 0xcd, 0xb5, 0xce, 0x36, 0xce, 0xb6,
	0xcf, 0x37, 0xcf, 0xb8, 0xd0, 0x39, 0xd0, 0xba,
	0xd1, 0x3c, 0xd1, 0xbe, 0xd2, 0x3f, 0xd2, 0xc1,
	0xd3, 0x44, 0xd3, 0xc6, 0xd4, 0x49, 0xd4, 0xcb,
	0xd5, 0x4e, 0xd5, 0xd1, 0xd6, 0x55, 0xd6, 0xd8,
	0xd7, 0x5c, 0xd7, 0xe0, 0xd8, 0x64, 0xd8, 0xe8,
	0xd9, 0x6c, 0xd9, 0xf1, 0xda, 0x76, 0xda, 0xfb,
	0xdb, 0x80, 0xdc, 0x05, 0xdc, 0x8a, 0xdd, 0x10,
	0xdd, 0x96, 0xde, 0x1c, 0xde, 0xa2, 0xdf, 0x29,
	0xdf, 0xaf, 0xe0, 0x36, 0xe0, 0xbd, 0xe1, 0x44,
	0xe1, 0xcc, 0xe2, 0x53, 0xe2, 0xdb, 0xe3, 0x63,
	0xe3, 0xeb, 0xe4, 0x73, 0xe4, 0xfc, 0xe5, 0x84,
	0xe6, 0x0d, 0xe6, 0x96, 0xe7, 0x1f, 0xe7, 0xa9,
	0xe8, 0x32, 0xe8, 0xbc, 0xe9, 0x46, 0xe9, 0xd0,
	0xea, 0x5b, 0xea, 0xe5, 0xeb, 0x70, 0xeb, 0xfb,
	0xec, 0x86, 0xed, 0x11, 0xed, 0x9c, 0xee, 0x28,
	0xee, 0xb4, 0xef, 0x40, 0xef, 0xcc, 0xf0, 0x58,
	0xf0, 0xe5, 0xf1, 0x72, 0xf1, 0xff, 0xf2, 0x8c,
	0xf3, 0x19, 0xf3, 0xa7, 0xf4, 0x34, 0xf4, 0xc2,
	0xf5, 0x50, 0xf5, 0xde, 0xf6, 0x6d, 0xf6, 0xfb,
	0xf7, 0x8a, 0xf8, 0x19, 0xf8, 0xa8, 0xf9, 0x38,
	0xf9, 0xc7, 0xfa, 0x57, 0xfa, 0xe7, 0xfb, 0x77,
	0xfc, 0x07, 0xfc, 0x98, 0xfd, 0x29, 0xfd, 0xba,
	0xfe, 0x4b, 0xfe, 0xdc, 0xff, 0x6d, 0xff, 0xff
};

int jas_iccprofdata_srgblen = sizeof(jas_iccprofdata_srgb);

uchar jas_iccprofdata_sgray[] = {
	0x00, 0x00, 0x01, 0x8a, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x20, 0x00, 0x00, 0x73, 0x63, 0x6e, 0x72,
	0x47, 0x52, 0x41, 0x59, 0x58, 0x59, 0x5a, 0x20,
	0x07, 0xd3, 0x00, 0x01, 0x00, 0x1f, 0x00, 0x0d,
	0x00, 0x35, 0x00, 0x21, 0x61, 0x63, 0x73, 0x70,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x4b, 0x4f, 0x44, 0x41, 0x73, 0x47, 0x72, 0x79,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf6, 0xd6,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xd3, 0x2d,
	0x4a, 0x50, 0x45, 0x47, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x04, 0x64, 0x65, 0x73, 0x63,
	0x00, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00, 0x86,
	0x63, 0x70, 0x72, 0x74, 0x00, 0x00, 0x01, 0x3c,
	0x00, 0x00, 0x00, 0x2b, 0x77, 0x74, 0x70, 0x74,
	0x00, 0x00, 0x01, 0x68, 0x00, 0x00, 0x00, 0x14,
	0x6b, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x7c,
	0x00, 0x00, 0x00, 0x0e, 0x64, 0x65, 0x73, 0x63,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c,
	0x52, 0x65, 0x73, 0x74, 0x72, 0x69, 0x63, 0x74,
	0x65, 0x64, 0x20, 0x49, 0x43, 0x43, 0x20, 0x70,
	0x72, 0x6f, 0x66, 0x69, 0x6c, 0x65, 0x20, 0x64,
	0x65, 0x73, 0x63, 0x72, 0x69, 0x62, 0x69, 0x6e,
	0x67, 0x20, 0x73, 0x52, 0x47, 0x42, 0x2d, 0x67,
	0x72, 0x65, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x74, 0x65, 0x78, 0x74,
	0x00, 0x00, 0x00, 0x00, 0x43, 0x6f, 0x70, 0x79,
	0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0x32, 0x30,
	0x30, 0x33, 0x20, 0x73, 0x52, 0x47, 0x42, 0x2d,
	0x67, 0x72, 0x65, 0x79, 0x20, 0x52, 0x65, 0x66,
	0x65, 0x72, 0x65, 0x6e, 0x63, 0x65, 0x00, 0x00,
	0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xf3, 0x54, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x16, 0xcf, 0x63, 0x75, 0x72, 0x76,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0xcd
};

int jas_iccprofdata_sgraylen = sizeof(jas_iccprofdata_sgray);
