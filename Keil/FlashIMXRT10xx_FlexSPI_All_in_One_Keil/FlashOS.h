/**************************************************************************//**
 * @file     FlashOS.h
 * @brief    Data structures and entries Functions
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
#define U8  unsigned char
#define U16 unsigned short
#define U32 unsigned long

#define I8  signed char
#define I16 signed short
#define I32 signed long
	
#define VERS       1           // Interface Version 1.01

#define UNKNOWN    0           // Unknown
#define ONCHIP     1           // On-chip Flash Memory
#define EXT8BIT    2           // External Flash Device on 8-bit  Bus
#define EXT16BIT   3           // External Flash Device on 16-bit Bus
#define EXT32BIT   4           // External Flash Device on 32-bit Bus
#define EXTSPI     5           // External Flash Device on SPI

#define MAX_NUM_SECTORS (512)      // Max. number of sectors, must not be modified.
#define ALGO_VERSION    (0x0101)   // Algo version, must not be modified.

struct SECTOR_INFO  {
  U32 SectorSize;       // Sector Size in bytes
  U32 SectorStartAddr;  // Start address of the sector area (relative to the "BaseAddr" of the flash)
};

#define SECTOR_END 0xFFFFFFFF, 0xFFFFFFFF

struct FlashDevice {
   U16 AlgoVer;       // Algo version number
   U8  Name[128];     // Flash device name
   U16 Type;          // Flash device type
   U32 BaseAddr;      // Flash base address
   U32 TotalSize;     // Total flash device size in Bytes (256 KB)
   U32 PageSize;      // Page Size (number of bytes that will be passed to ProgramPage(). MinAlig is 8 byte
   U32 Reserved;      // Reserved, should be 0
   U8  ErasedVal;     // Flash erased value
   U32 TimeoutProg;   // Program page timeout in ms; 0: use default timeout
   // el xxxx Multi Sector program: 
   U32 TimeoutErase;  // Erase sector timeout in ms 0: Use default timeout
   // el xxxx Multi Sector erase: 
   struct SECTOR_INFO SectorInfo[MAX_NUM_SECTORS]; // Flash sector layout definition
};

//
// Flash module functions
//
extern int Init                (U32 Addr,       U32 Freq,         U32 Func       );
extern int UnInit              (U32 Func                                         );
extern int BlankCheck          (U32 Addr,       U32 NumBytes,     U8  BlankData  );
extern int EraseChip           (void                                             );
extern int EraseSector         (U32 SectorAddr                                   );
extern int ProgramPage         (U32 DestAddr,   U32 NumBytes,     U8  *pSrcBuff  );
extern U32 Verify              (U32 Addr,       U32 NumBytes,     U8  *pSrcBuff  );
