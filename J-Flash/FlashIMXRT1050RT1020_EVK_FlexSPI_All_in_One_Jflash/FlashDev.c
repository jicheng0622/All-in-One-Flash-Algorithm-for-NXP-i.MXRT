/**************************************************************************//**
 * @file     FlashDev.c
 * @brief    Flash Device Description for New Device Flash
 * @version  V1.0.0
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2010-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/***********************************************************************/

#include "FlashOS.h"        // FlashOS Structures
////RT1050_EVK HyperFlash IS26KS512S
//struct FlashDevice const FlashDevice  =  {
//   FLASH_DRV_VERS,               // Driver Version, do not modify!
//   "MIMXRT IS26KS512S (256KB Sec)",// Device Name 
//   EXT8BIT,                      // Device Type
//   0x60000000,                   // Device Start Address
//   0x04000000,                   // Device Size (64MB)
//   512,                          // Programming Page Size
//   0,                            // Reserved, must be 0
//   0xFF,                         // Initial Content of Erased Memory
//   1000,                         // Program Page Timeout 1000 mSec
//   8000,                         // Erase Sector Timeout 8000 mSec

//// Specify Size and Address of Sectors
//   0x040000, 0x000000,           // Sector Size  1kB (128 Sectors)
//   SECTOR_END
//};

struct FlashDevice const FlashDevice  =  {
   ALGO_VERSION,                // Driver Version, do not modify!
#ifdef CPU_MIMXRT1052CVL5B
	"MIMXRT1050_SPINor_AllinOne", // Device Name
#elif defined(CPU_MIMXRT1021DAG5A)
   "MIMXRT1021_SPINor_AllinOne", // Device Name
#else
	 "Unkown part flash algo",
#endif
   EXTSPI,                       // Device Type
   0x60000000,                   // Device Start Address
   0x00800000U,                  // Device Size (8MB)
   256,                          // Programming Page Size
   0,                            // Reserved, must be 0
   0xFF,                         // Initial Content of Erased Memory
   2000,                         // Program Page Timeout 1000 mSec
   6000,                         // Erase Sector Timeout 3000 mSec

// Specify Size and Address of Sectors
   0x1000, 0x000000, // Sector size and start offset of this sector group
   SECTOR_END
};



