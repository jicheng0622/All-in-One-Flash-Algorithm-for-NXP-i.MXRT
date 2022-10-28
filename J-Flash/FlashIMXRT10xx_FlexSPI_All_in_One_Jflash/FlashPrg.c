/**************************************************************************//**
 * @file     FlashPrg.c
 * @brief    Flash Programming Functions adapted for New Device Flash
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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "FlashOS.h"
#include "flexspi_nor_flash.h"
#include "fsl_ocotp.h"

/* Enable program eFuse inside RT family */
//#define BT_FUSE                       1

#define PAGE_SIZE_SHIFT              (8)   // Smallest amount of data that can be programmed. <PageSize> = 2 ^ Shift. Shift = 2 => <PageSize> = 2^2 = 4 bytes
#define SECTOR_SIZE_SHIFT            (12)   // Flashes with uniform sectors only. <SectorSize> = 2 ^ Shift. Shift = 12 => <SectorSize> = 2 ^ 12 = 4096 bytes

#ifndef DISABLE_WDOG
  #define DISABLE_WDOG                1
#endif

/** private data **/
__attribute__((aligned(4))) const flexspi_nor_config_t config_block ;/*= {
	0x46, 0x43, 0x46, 0x42, 0x00, 0x04, 0x01, 0x56, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x04, 0x07, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEB, 0x04, 0x18, 0x0A,
	0x06, 0x32, 0x04, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x04, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xD8, 0x04, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x18, 0x08,
	0x04, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x60, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};*/
static uint32_t fspi_instance;
extern void clock_setup(void);
extern uint32_t SystemCoreClock;
extern uint32_t get_ocotp_clock(void);

void Disable_Watchdog(void)
{
    /* Disable Watchdog Power Down Counter */
  WDOG1->WMCR &= ~WDOG_WMCR_PDE_MASK;
  WDOG2->WMCR &= ~WDOG_WMCR_PDE_MASK;
  /* Watchdog disable */
  if ((WDOG1->WCR & WDOG_WCR_WDE_MASK) != 0U)
  {
     WDOG1->WCR &= ~WDOG_WCR_WDE_MASK;
  }
  if ((WDOG2->WCR & WDOG_WCR_WDE_MASK) != 0U)
  {
     WDOG2->WCR &= ~WDOG_WCR_WDE_MASK;
  }
  if ((RTWDOG->CS & RTWDOG_CS_CMD32EN_MASK) != 0U)
  {
     RTWDOG->CNT = 0xD928C520U; /* 0xD928C520U is the update key */
  }
  else
  {
    RTWDOG->CNT = 0xC520U;
    RTWDOG->CNT = 0xD928U;
  }
  RTWDOG->TOVAL = 0xFFFF;
  RTWDOG->CS = (uint32_t) ((RTWDOG->CS) & ~RTWDOG_CS_EN_MASK) | RTWDOG_CS_UPDATE_MASK;
}

/*********************************************************************
*
*       Init
*
*  Function description
*    Handles the initialization of the flash module.
*
*  Parameters
*    Addr: Flash base address
*    Freq: Clock frequency in Hz
*    Func: Specifies the action followed by Init() (e.g.: 1 - Erase, 2 - Program, 3 - Verify / Read)
*
*  Return value 
*    0 O.K.
*    1 Error
*/
int Init(U32 Addr, U32 Freq, U32 Func) {
  static status_t status;  
  serial_nor_config_option_t option;
	
	(void)Addr;
  (void)Freq;
  (void)Func;
	
#if DISABLE_WDOG  
  Disable_Watchdog();
#endif
	
  /* Default option configuration for ISSI QSPI Flash */
  option.option0.U = 0xC0000107;
	
	/* Consider HyperFlash is default memory in EVK_RT1050 */
#if defined(MIMXRT1052_SERIES) 
  if(2 == ((SRC->SBMR1>>8) & 0x7))
  {
    option.option0.U = 0xc0233009;//HperflashV1.8v
  }
#endif
	
	option.option1.U = 0x00000000;
	
	uint8_t * Opt0 = (uint8_t *)0x20201000;
	uint8_t * Opt1 = (uint8_t *)0x20201008;
	
	if(Opt0[3]=='O'&&Opt0[2]=='p'&&Opt0[1]=='t'&&Opt0[0]=='0')
	{
		option.option0.U = *((uint32_t *)0x20201004);
	}
  if(Opt1[3]=='O'&&Opt1[2]=='p'&&Opt1[1]=='t'&&Opt1[0]=='1')
	{
		option.option1.U = *((uint32_t *)0x2020100C);
	}
	
	/* Disable Systick which might be enabled by bootrom to avoid interrupt as no systick ISR in this code */
  if ((SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) != 0U)
  {
      SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  }
	
  clock_setup();
	
	if(0x60000000 == (Addr&0xF0000000UL))//For RT1060/RT1050/RT1020/RT1010
  {
    fspi_instance = 0;
  } 
  else if(0x70000000 == (Addr&0xF0000000UL))//For RT1064
  {
    fspi_instance = 2;
  }
  else
    fspi_instance = 0;
	
	status = flexspi_nor_get_config(fspi_instance, (flexspi_nor_config_t *) &config_block, &option);
	
  if(status	==  kStatus_Success)
    status = flexspi_nor_flash_init(fspi_instance, (flexspi_nor_config_t *) &config_block);
	
#ifdef BT_FUSE
  OCOTP_Init(OCOTP, get_ocotp_clock());
  uint32_t fuseData;
  /* OCOTP address = 0x06 means fusemap address = 0x460,
     refer to Chapter Fusemap in RT's RM document. */
  status = OCOTP_ReadFuseShadowRegisterExt(OCOTP, 0x06, &fuseData, 1);
  /* Set BT_FUSE_SEL = 1, boot options are controlled by 
     the eFuses instead of GPIO CFG pins */
  if((fuseData & (1<<4)) == 0){
    fuseData |= 1<<4;
    status = OCOTP_WriteFuseShadowRegister(OCOTP, 0x06, fuseData);
  }
#endif
	
  return (status);
}
                           
/*********************************************************************
*
*       UnInit
*
*  Function description
*    Handles the de-initialization of the flash module.
*    It is called once per flash programming step (Erase, Program, Verify)
*
*  Parameters
*    Func  Caller type (e.g.: 1 - Erase, 2 - Program, 3 - Verify)
*
*  Return value
*    == 0  O.K.
*    == 1  Error
*
*  Notes
*    (1) This function is mandatory.
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
int UnInit(U32 Func){
	
	  flexspi_clear_cache(fspi_instance);

    flexspi_clear_sequence_pointer(fspi_instance);
	
    return (0);
}

/*********************************************************************
*
*       EraseSector
*
*  Function description
*    Erases one flash sector.
*
*  Parameters
*    SectorAddr  Absolute address of the sector to be erased
*
*  Return value
*    == 0  O.K.
*    == 1  Error
*
*  Notes
*    (1) This function is mandatory.
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
int EraseSector(U32 SectorAddr) {
  static status_t status;
  SectorAddr &= 0x0FFFFFFF;
    
  status = flexspi_nor_flash_erase_sector(fspi_instance, (flexspi_nor_config_t *) &config_block, SectorAddr);
    
  return status;

}
/*********************************************************************
*
*       EraseChip
*
*  Function description
*    Erases the entire flash.
*
*  Return value
*    == 0  O.K.
*    == 1  Error
*
*  Notes
*    (1) This function is optional. If not present, J-Link will always use EraseSector() for erasing.
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
int EraseChip(void) {
  static status_t status;

  status = flexspi_nor_flash_erase_all(fspi_instance, (flexspi_nor_config_t *) &config_block);
    
  return (status);
  
}
/*********************************************************************
*
*       ProgramPage
*
*  Function description
*    Programs one flash page.
*
*  Parameters
*    DestAddr  Address to start programming on
*    NumBytes  Number of bytes to program. Guaranteed to be == <FlashDevice.PageSize>
*    pSrcBuff  Pointer to data to be programmed
*
*  Return value
*    == 0  O.K.
*    == 1  Error
*
*  Notes
*    (1) This function is mandatory.
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
int ProgramPage(U32 DestAddr, U32 NumBytes, U8 *pSrcBuff) {
  static status_t status;
  DestAddr &= 0x0FFFFFFF;
    
  status = flexspi_nor_flash_page_program(fspi_instance, (flexspi_nor_config_t *) &config_block, DestAddr, (const uint32_t *)pSrcBuff);

  return (status);
}

/*********************************************************************
*
*       BlankCheck
*
*  Function description
*    Checks if a memory region is blank
*
*  Parameters
*    Addr       Address to start checking
*    NumBytes   Number of bytes to be checked
*    BlankData  Blank (erased) value of flash (Most flashes have 0xFF, some have 0x00, some do not have a defined erased value)
*
*  Return value
*    == 0  O.K., blank
*    == 1  O.K., *not* blank
*     < 0  Error
*
*  Notes
*    (1) This function is optional. If not present, the J-Link software will assume that erased state of a sector can be determined via normal memory-mapped readback of sector.
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
int BlankCheck(U32 Addr, U32 NumBytes, U8 BlankData) {
  volatile U8* pData;
  //
  // Simply read data from flash and compare against <BlankData>
  //
  
  pData = (volatile U8*)Addr;
  do {
    if (*pData++ != BlankData) {
      return 1;
    }
  } while (--NumBytes);
  return 0;
}
