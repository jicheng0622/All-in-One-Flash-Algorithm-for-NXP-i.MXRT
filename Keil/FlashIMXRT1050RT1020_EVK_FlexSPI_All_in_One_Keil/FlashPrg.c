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
#include "flexspi_nor_flash.h"

#ifndef DISABLE_WDOG
  #define DISABLE_WDOG                 1
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

int Init (unsigned long adr, unsigned long clk, unsigned long fnc)
{
  static status_t status;  
  serial_nor_config_option_t option;
	
#if DISABLE_WDOG  
  Disable_Watchdog();
#endif
  
  /* Default option0 configuration for ISSI QSPI Flash */
  option.option0.U = 0xC0000107;
	
	/* Consider HyperFlash is default memory in EVK_RT1050 */
#if defined(MIMXRT1052_SERIES) 
  if(2 == ((SRC->SBMR1>>8) & 0x7))
  {
    option.option0.U = 0xc0233009;//HperflashV1.8v
  }
#endif
	
	/* Default option1 configuration */
	option.option1.U = 0x00000000;
	
	uint8_t * Opt0 = (uint8_t *)0x20201000;
	uint8_t * Opt1 = (uint8_t *)0x20201008;
	
	if(Opt0[3]=='O'
		 &&Opt0[2]=='p'
	   &&Opt0[1]=='t'
	   &&Opt0[0]=='0')
	{
		option.option0.U = *((uint32_t *)0x20201004);
	}
  if(Opt1[3]=='O'
		 &&Opt1[2]=='p'
	   &&Opt1[1]=='t'
	   &&Opt1[0]=='1')
	{
		option.option1.U = *((uint32_t *)0x2020100C);
	}
	
	/* Disable Systick which might be enabled by bootrom to avoid interrupt as no systick ISR in this code */
  if ((SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) != 0U)
  {
      SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  }
	
  clock_setup();
	
	if(0x60000000 == (adr&0xF0000000UL))//For RT1060/RT1050/RT1020/RT1010
  {
    fspi_instance = 0;
  } 
  else if(0x70000000 == (adr&0xF0000000UL))//For RT1064
  {
    fspi_instance = 2;
  }
  else
    fspi_instance = 0;
	
	status = flexspi_nor_get_config(fspi_instance, (flexspi_nor_config_t *) &config_block, &option);
	
  if(status	==  kStatus_Success)
    status = flexspi_nor_flash_init(fspi_instance, (flexspi_nor_config_t *) &config_block);
	
  return (status);
}

                                  
/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc)
{
    return (0);
}


unsigned long Verify (unsigned long adr, unsigned long sz, unsigned char *buf)
{
    if(!memcmp(buf, (void*)adr, sz))
    {
        return (adr+sz);                                  // Finished without Errors
    }
    else
    {
        return 0;
    }
}

int EraseChip (void)
{
    static status_t status;

    status = flexspi_nor_flash_erase_all(fspi_instance, (flexspi_nor_config_t *) &config_block);
    
    return (status);
}



int EraseSector (unsigned long adr)
{
	  static status_t status;
    adr &= 0x0FFFFFFF;
    
    status = flexspi_nor_flash_erase_sector(fspi_instance, (flexspi_nor_config_t *) &config_block, adr);
    
    return status;
}


int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf)
{
	  static status_t status;
    adr &= 0x0FFFFFFF;
    
    status = flexspi_nor_flash_page_program(fspi_instance, (flexspi_nor_config_t *) &config_block, adr, (const uint32_t *)buf);

    return (status);
}
