/*********************************************************************
*            (c) 1995 - 2020 SEGGER Microcontroller GmbH             *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
**********************************************************************
----------------------------------------------------------------------
File    : FlashPrg.c
Purpose : Implementation of RAMCode template
--------  END-OF-HEADER  ---------------------------------------------
*/

#include <stddef.h>
#include "FlashOS.h"
#include "flexspi_nor_flash.h"


/*********************************************************************
*
*       Defines (configurable)
*
**********************************************************************
*/

//
// Only compile in functions that make sense to keep RAMCode as small as possible
//
#define SUPPORT_NATIVE_VERIFY         (0)   // Non-memory mapped flashes only. Flash cannot be read memory-mapped
#define SUPPORT_NATIVE_READ_FUNCTION  (0)   // Non-memory mapped flashes only. Flash cannot be read memory-mapped
#define SUPPORT_ERASE_CHIP            (1)   // To potentially speed up production programming: Erases whole flash bank / chip with special command
#define SUPPORT_TURBO_MODE            (1)   // Currently available for Cortex-M only
#define SUPPORT_SEGGER_OPEN_ERASE     (1)   // Flashes with uniform sectors only. Speed up erase because 1 OFL call may erase multiple sectors

/*********************************************************************
*
*       Defines (fixed)
*
**********************************************************************
*/

#define PAGE_SIZE_SHIFT              (8)   // Smallest amount of data that can be programmed. <PageSize> = 2 ^ Shift. Shift = 2 => <PageSize> = 2^2 = 4 bytes
#define SECTOR_SIZE_SHIFT            (12)   // Flashes with uniform sectors only. <SectorSize> = 2 ^ Shift. Shift = 12 => <SectorSize> = 2 ^ 12 = 4096 bytes


//
// Default definitions for optional functions if not compiled in
// Makes Api table code further down less ugly
//
#if (SUPPORT_ERASE_CHIP == 0)
  #define EraseChip NULL
#endif
#if (SUPPORT_NATIVE_VERIFY == 0)
  #define Verify NULL
#endif
#if (SUPPORT_NATIVE_READ_FUNCTION == 0)
  #define SEGGER_OPEN_Read NULL
#endif
#if (SUPPORT_SEGGER_OPEN_ERASE == 0)
  #define SEGGER_OPEN_Erase NULL
#endif
#if (SUPPORT_TURBO_MODE == 0)
  #define SEGGER_OPEN_Start NULL
#endif

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

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

//
// Independent watchdog registers
//
typedef struct {
  volatile U32 KR;
  volatile U32 PR;
  volatile U32 RLR;
  volatile U32 SR;
} IWDG_REGS;

//
// Flash registers
//
typedef struct {
  volatile U32 ACR;
  volatile U32 KEYR;
  volatile U32 OPTKEYR;
  volatile U32 SR;
  volatile U32 CR;
  volatile U32 OPTCR;
} FLASH_REGS;

typedef struct {
  U32 acr;
} RESTORE_INFO;

static void _FeedWatchdog(void);

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static RESTORE_INFO _RestoreInfo;

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/

volatile int PRGDATA_StartMarker __attribute__ ((section ("PrgData")));         // Mark start of <PrgData> segment. Non-static to make sure linker can keep this symbol. Dummy needed to make sure that <PrgData> section in resulting ELF file is present. Needed by open flash loader logic on PC side

const SEGGER_OFL_API SEGGER_OFL_Api __attribute__ ((section ("PrgCode"))) = {   // Mark start of <PrgCode> segment. Non-static to make sure linker can keep this symbol. 
  _FeedWatchdog,
  Init,
  UnInit,
  EraseSector,
  ProgramPage,
  BlankCheck,
  EraseChip,
  Verify,
  SEGGER_OPEN_CalcCRC,
  SEGGER_OPEN_Read,
  SEGGER_OPEN_Program,
  SEGGER_OPEN_Erase,
  SEGGER_OPEN_Start
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static void _FeedWatchdog(void)
{

}

static void Disable_Watchdog(void)
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
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       Init
*
*  Function description
*    Handles the initialization of the flash module.
*    It is called once per flash programming step (Erase, Program, Verify)
*
*  Parameters
*    Addr: Flash base address
*    Freq: Clock frequency in Hz
*    Func: Specifies the action followed by Init() (e.g.: 1 - Erase, 2 - Program, 3 - Verify / Read)
*
*  Return value
*    == 0  O.K.
*    == 1  Error
*
*  Notes
*    (1) This function is mandatory.
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
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
	
  if(Opt0[3]=='O'&&Opt0[2]=='p'&&Opt0[1]=='t'&&Opt0[0]=='0'){
    option.option0.U = *((uint32_t *)0x20201004);
  }
  if(Opt1[3]=='O'&&Opt1[2]=='p'&&Opt1[1]=='t'&&Opt1[0]=='1'){
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
	
  if(status == kStatus_Success)
    status = flexspi_nor_flash_init(fspi_instance, (flexspi_nor_config_t *) &config_block);
	
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
int UnInit(U32 Func) {
  volatile RESTORE_INFO * pInfo;
  
  flexspi_clear_cache(fspi_instance);

  flexspi_clear_sequence_pointer(fspi_instance);

  return 0;
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

/*********************************************************************
*
*       SEGGER_OPEN_CalcCRC
*
*  Function description
*    Calculates the CRC over a specified number of bytes
*    Even more optimized version of Verify() as this avoids downloading the compare data into the RAMCode for comparison.
*    Heavily reduces traffic between J-Link software and target and therefore speeds up verification process significantly.
*
*  Parameters
*    CRC       CRC start value
*    Addr      Address where to start calculating CRC from
*    NumBytes  Number of bytes to calculate CRC on
*    Polynom   Polynom to be used for CRC calculation
*
*  Return value
*    CRC
*
*  Notes
*    (1) This function is optional
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
U32 SEGGER_OPEN_CalcCRC(U32 CRC, U32 Addr, U32 NumBytes, U32 Polynom) {
  CRC = SEGGER_OFL_Lib_CalcCRC(&SEGGER_OFL_Api, CRC, Addr, NumBytes, Polynom);   // Use lib function from SEGGER by default. Pass API pointer to it because it may need to call the read function (non-memory mapped flashes)
  return CRC;
}

/*********************************************************************
*
*       SEGGER_OPEN_Program
*
*  Function description
*    Optimized variant of ProgramPage() which allows multiple pages to be programmed in 1 RAMCode call.
*
*  Parameters
*    DestAddr  Address to start flash programming at.
*    NumBytes  Number of bytes to be program. Guaranteed to be multiple of <FlashDevice.PageSize>
*    pSrcBuff  Pointer to data to be programmed
*
*  Return value
*    == 0  O.K.
*    == 1  Error
*
*  Notes
*    (1) This function is optional. If not present, the J-Link software will use ProgramPage()
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
int SEGGER_OPEN_Program(U32 DestAddr, U32 NumBytes, U8 *pSrcBuff) {
  U32 NumPages;
  int r;

  NumPages = (NumBytes >> PAGE_SIZE_SHIFT);
  r = 0;
  do {
    r = ProgramPage(DestAddr, (1uL << PAGE_SIZE_SHIFT), pSrcBuff);
    if (r < 0) {
      return r;
    }
    DestAddr += (1uL << PAGE_SIZE_SHIFT);
    pSrcBuff += (1uL << PAGE_SIZE_SHIFT);
  } while (--NumPages);
  return r;
}

/*********************************************************************
*
*       Verify
*
*  Function description
*    Verifies flash contents.
*    Usually not compiled in. Only needed for non-memory mapped flashes.
*
*  Parameters
*    Addr      Address to start verify on
*    NumBytes  Number of bytes to verify
*    pBuff     Pointer data to compare flash contents to
*
*  Return value
*    == (Addr + NumBytes): O.K.
*    != (Addr + NumBytes): *not* O.K. (ideally the fail address is returned)
*
*  Notes
*    (1) This function is optional. If not present, the J-Link software will assume that flash memory can be verified via memory-mapped readback of flash contents.
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
#if SUPPORT_NATIVE_VERIFY
U32 Verify(U32 Addr, U32 NumBytes, U8 *pBuff) {
  unsigned char *pFlash;
  unsigned long r;

  pFlash = (unsigned char *)Addr;
  r = Addr + NumBytes;
  do {
      if (*pFlash != *pBuff) {
        r = (unsigned long)pFlash;
        break;
      }
      pFlash++;
      pBuff++;
  } while (--NumBytes);
  return r;
}
#endif

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
#if SUPPORT_ERASE_CHIP
int EraseChip(void) {
  static status_t status;

  //_FeedWatchdog();

  status = flexspi_nor_flash_erase_all(fspi_instance, (flexspi_nor_config_t *) &config_block);
    
  return (status);
  
}
#endif

/*********************************************************************
*
*       SEGGER_OPEN_Read
*
*  Function description
*    Reads a specified number of bytes from flash into the provided buffer.
*    Usually not compiled in. Only needed for non-memory mapped flashes.
*
*  Parameters
*    Addr      Address to start reading from
*    NumBytes  Number of bytes to read
*    pDestBuff Pointer to buffer to store read data
*
*  Return value
*    >= 0: O.K., NumBytes read
*    <  0: Error
*
*  Notes
*    (1) This function is optional. If not present, the J-Link software will assume that a normal memory-mapped read can be performed to read from flash.
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
#if SUPPORT_NATIVE_READ_FUNCTION
int SEGGER_OPEN_Read(U32 Addr, U32 NumBytes, U8 *pDestBuff) {
  //
  // Read function
  // Add your code here...
  //
  //_FeedWatchdog();
  return NumBytes;
}
#endif

/*********************************************************************
*
*       SEGGER_OPEN_Erase
*
*  Function description
*    Erases one or more flash sectors.
*    The implementation from this template only works on flashes that have uniform sectors.
*
*  Notes
*    (1) This function can rely on that at least one sector will be passed
*    (2) This function must be able to handle multiple sectors at once
*    (3) This function can rely on that only multiple sectors of the same sector
*        size will be passed. (e.g. if the device has two sectors with different
*        sizes, the DLL will call this function two times with NumSectors = 1)
*
*  Parameters
*    SectorAddr:  Address of the start sector to be erased
*    SectorIndex: Index of the start sector to be erased (1st sector handled by this flash bank: SectorIndex == 0)
*    NumSectors:  Number of sectors to be erased. Min. 1
*
*  Return value
*    == 0  O.K.
*    == 1  Error
*
*  Notes
*    (1) This function is optional. If not present, the J-Link software will use EraseSector()
*    (2) Use "noinline" attribute to make sure that function is never inlined and label not accidentally removed by linker from ELF file.
*/
#if SUPPORT_SEGGER_OPEN_ERASE
int SEGGER_OPEN_Erase(U32 SectorAddr, U32 SectorIndex, U32 NumSectors) {
  int r;

  (void)SectorIndex;
  //_FeedWatchdog();
  r = 0;
  do {
    r = EraseSector(SectorAddr);
    if (r) {
      break;
    }
    SectorAddr += (1 << SECTOR_SIZE_SHIFT);
  } while (--NumSectors);
  return r;
}
#endif

/*********************************************************************
*
*       SEGGER_OPEN_Start
*
*  Function description
*    Starts the turbo mode of flash algo.
*    Currently only available for Cortex-M based targets.
*/
#if SUPPORT_TURBO_MODE
void SEGGER_OPEN_Start(volatile struct SEGGER_OPEN_CMD_INFO* pInfo) {
  SEGGER_OFL_Lib_StartTurbo(&SEGGER_OFL_Api, pInfo);
}
#endif

/**************************** End of file ***************************/
