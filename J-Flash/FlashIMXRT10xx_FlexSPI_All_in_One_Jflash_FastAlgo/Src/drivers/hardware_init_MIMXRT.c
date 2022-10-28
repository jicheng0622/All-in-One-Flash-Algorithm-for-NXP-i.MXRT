/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "fsl_device_registers.h"
#include <assert.h>

#include "fsl_flexspi.h"
#include "flexspi_nor_flash.h"
#include "fusemap.h"
#include "peripherals_pinmux.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FREQ_396MHz (396000000U)
#define FREQ_480MHz (480000000U)
#define FREQ_528MHz (528000000U)
#define FREQ_24MHz (24000000U)

typedef struct
{
    uint8_t RESERVED0[0x65C];
    __IO uint32_t SPI_B0_SW_MUX_CTL_PAD[14];
    __IO uint32_t SPI_B1_SW_MUX_CTL_PAD[8];
    __IO uint32_t SPI_B0_SW_PAD_CTL_PAD[14];
    __IO uint32_t SPI_B1_SW_PAD_CTL_PAD[8];
    uint8_t RESERVED1[0x072c - 0x070C];
    __IO uint32_t FLEXSPI2_SELECT_INPUT[11];

} IOMUXC2_Type;

enum __flexspi2_select_input_idx
{
    kFlexSpi2_DQS_Idx,
    kFlexSpi2_FA_DATA0_Idx,
    kFlexSpi2_FA_DATA1_Idx,
    kFlexSpi2_FA_DATA2_Idx,
    kFlexSpi2_FA_DATA3_Idx,
    kFlexSpi2_FB_DATA0_Idx,
    kFlexSpi2_FB_DATA1_Idx,
    kFlexSpi2_FB_DATA2_Idx,
    kFlexSpi2_FB_DATA3_Idx,
    kFlexSpi2_FA_SCLK_Idx,
    kFlexSpi2_FB_SCLK_Idx,
};

#define IOMUXC2 ((volatile IOMUXC2_Type *)(IOMUXC_BASE))
/*******************************************************************************
 * Prototype
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Codes
 ******************************************************************************/
//!@brief Configure IOMUX for FlexSPI Peripheral
void flexspi_iomux_config(uint32_t instance, flexspi_mem_config_t *config)
{
    uint32_t csPadCtlValue = FLEXSPI_SW_PAD_CTL_VAL;
    uint32_t dqsPadCtlValue = FLEXSPI_DQS_SW_PAD_CTL_VAL;
    uint32_t sclkPadCtlValue = FLEXSPI_SW_PAD_CTL_VAL;
    uint32_t dataPadCtlValue = FLEXSPI_SW_PAD_CTL_VAL;

    if (flexspi_is_padsetting_override_enable(config))
    {
        csPadCtlValue = config->csPadSettingOverride;
        dqsPadCtlValue = config->dqsPadSettingOverride;
        sclkPadCtlValue = config->sclkPadSettingOverride;
        dataPadCtlValue = config->dataPadSettingOverride;
    }

    if (instance == 0)
    { 
      if (config->controllerMiscOption & FLEXSPI_BITMASK(kFlexSpiMiscOffset_SecondPinMux))
      {
          // The secondary FlexSPI Pinmux, supports only 1 Flash
          if (config->sflashA1Size > 0)
          {
              // FLEXSPIA_SS0_B
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SEC_SS0_B_IDX] = FLEXSPIA_SEC_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SEC_SS0_B_IDX] = csPadCtlValue;
              // FLEXSPIA_SCLK
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SEC_SCLK_IDX] =
                  FLEXSPIA_SEC_MUX_VAL | IOMUXC_SW_MUX_CTL_PAD_SION(1);
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SEC_SCLK_IDX] = sclkPadCtlValue;
              IOMUXC->SELECT_INPUT[SELECT_INPUT_FLEXSPIA_SEC_SCLK_IDX] = 0x01;

              // FLEXSPIA_DATA0
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SEC_DATA0_IDX] = FLEXSPIA_SEC_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SEC_DATA0_IDX] = dataPadCtlValue;
              IOMUXC->SELECT_INPUT[SELECT_INPUT_FLEXSPIA_SEC_DATA0_IDX] = 0x01;

              // FLEXSPIA_DATA1
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SEC_DATA1_IDX] = FLEXSPIA_SEC_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SEC_DATA1_IDX] = dataPadCtlValue;
              IOMUXC->SELECT_INPUT[SELECT_INPUT_FLEXSPIA_SEC_DATA1_IDX] = 0x01;

              // FLEXSPIA_DATA2
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SEC_DATA2_IDX] = FLEXSPIA_SEC_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SEC_DATA2_IDX] = dataPadCtlValue;
              IOMUXC->SELECT_INPUT[SELECT_INPUT_FLEXSPIA_SEC_DATA2_IDX] = 0x01;

              // FLEXSPIA_DATA3
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SEC_DATA3_IDX] = FLEXSPIA_SEC_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SEC_DATA3_IDX] = dataPadCtlValue;
              IOMUXC->SELECT_INPUT[SELECT_INPUT_FLEXSPIA_SEC_DATA3_IDX] = 0x01;
          }
      }
      else // The primary FlexSPI pinmux, support octal Flash and up to 4 QuadSPI NOR Flash
      {
          // Pinmux configuration for FLEXSPI PortA
          if (config->sflashA1Size || config->sflashA2Size)
          {
              if (config->sflashA2Size)
              {
                  // FLEXSPIA_SS1_B
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SS1_B_IDX] = FLEXSPIA_SS1_MUX_VAL;
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SS1_B_IDX] = csPadCtlValue;
              }

              // Basic pinmux configuration for FLEXSPI
              if (config->sflashA1Size)
              {
                  // FLEXSPIA_SS0_B
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SS0_B_IDX] = FLEXSPIA_MUX_VAL;
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SS0_B_IDX] = csPadCtlValue;
              }

              // FLEXSPIA_SCLK
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SCLK_IDX] = FLEXSPIA_MUX_VAL | IOMUXC_SW_MUX_CTL_PAD_SION(1);
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SCLK_IDX] = sclkPadCtlValue;

              // FLEXSPIA_DATA0
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_DATA0_IDX] = FLEXSPIA_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_DATA0_IDX] = dataPadCtlValue;

              // FLEXSPIA_DATA1
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_DATA1_IDX] = FLEXSPIA_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_DATA1_IDX] = dataPadCtlValue;

              // FLEXSPIA_DATA2
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_DATA2_IDX] = FLEXSPIA_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_DATA2_IDX] = dataPadCtlValue;

              // FLEXSPIA_DATA3
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_DATA3_IDX] = FLEXSPIA_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_DATA3_IDX] = dataPadCtlValue;

              if (config->sflashPadType == kSerialFlash_8Pads)
              {
                  // FLEXSPIA_DATA4 / FLEXSPIB_DATA0
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_DATA0_IDX] = FLEXSPIA_MUX_VAL;
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_DATA0_IDX] = dataPadCtlValue;

                  // FLEXSPIA_DATA5 / FLEXSPIB_DATA1
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_DATA1_IDX] = FLEXSPIA_MUX_VAL;
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_DATA1_IDX] = dataPadCtlValue;

                  // FLEXSPIA_DATA6 / FLEXSPIB_DATA2
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_DATA2_IDX] = FLEXSPIA_MUX_VAL;
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_DATA2_IDX] = dataPadCtlValue;

                  // FLEXSPIA_DATA7 / FLEXSPIB_DATA3
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_DATA3_IDX] = FLEXSPIA_MUX_VAL;
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_DATA3_IDX] = dataPadCtlValue;
              }

              // Configure DQS pad
              if ((config->readSampleClkSrc == kFlexSPIReadSampleClk_ExternalInputFromDqsPad) ||
                  (config->readSampleClkSrc == kFlexSPIReadSampleClk_LoopbackFromDqsPad))
              {
                  // FLEXSPIA_DQS
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_DQS_IDX] =
                      FLEXSPIA_MUX_VAL | IOMUXC_SW_MUX_CTL_PAD_SION(1);
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_DQS_IDX] = dqsPadCtlValue;
              }

              // Configure Differential Clock pin
              if (flexspi_is_differential_clock_enable(config))
              {
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIA_SCLK_B_IDX] = FLEXSPIA_MUX_VAL;
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIA_SCLK_B_IDX] = sclkPadCtlValue;
              }
          }

          // Pinmux configuration for FLEXSPI PortB
          if (config->sflashB1Size || config->sflashB2Size)
          {
              if (config->sflashB2Size)
              {
                  // FLEXSPIB_SS1_B
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_SS1_B_IDX] = FLEXSPIB_SS1_MUX_VAL;
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_SS1_B_IDX] = csPadCtlValue;
              }

              // Basic pinmux configuration for FLEXSPI
              if (config->sflashB1Size)
              {
                  // FLEXSPIB_SS0_B
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_SS0_B_IDX] = FLEXSPIB_SS0_MUX_VAL;
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_SS0_B_IDX] = csPadCtlValue;
              }

              // FLEXSPIB_SCLK
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_SCLK_IDX] = FLEXSPIB_MUX_VAL | IOMUXC_SW_MUX_CTL_PAD_SION(1);
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_SCLK_IDX] = sclkPadCtlValue;

              // FLEXSPIB_DATA0
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_DATA0_IDX] = FLEXSPIB_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_DATA0_IDX] = dataPadCtlValue;

              // FLEXSPIB_DATA1
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_DATA1_IDX] = FLEXSPIB_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_DATA1_IDX] = dataPadCtlValue;

              // FLEXSPIB_DATA2
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_DATA2_IDX] = FLEXSPIB_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_DATA2_IDX] = dataPadCtlValue;

              // FLEXSPIB_DATA3
              IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_DATA3_IDX] = FLEXSPIB_MUX_VAL;
              IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_DATA3_IDX] = dataPadCtlValue;

              // Configure DQS pad
              if ((config->readSampleClkSrc == kFlexSPIReadSampleClk_ExternalInputFromDqsPad) ||
                  (config->readSampleClkSrc == kFlexSPIReadSampleClk_LoopbackFromDqsPad))
              {
                  // FLEXSPIB_DQS
                  IOMUXC->SW_MUX_CTL_PAD[SW_MUX_CTL_PAD_FLEXSPIB_DQS_IDX] =
                      FLEXSPIB_DQS_MUX_VAL | IOMUXC_SW_MUX_CTL_PAD_SION(1);
                  IOMUXC->SW_PAD_CTL_PAD[SW_PAD_CTL_PAD_FLEXSPIB_DQS_IDX] = dqsPadCtlValue;
              }
          }
      }
    }
    else if(instance == 2)
    {
      if(config->sflashA1Size)
      {
          // SS0
          IOMUXC2->SPI_B1_SW_MUX_CTL_PAD[6] = 0;
          IOMUXC2->SPI_B1_SW_PAD_CTL_PAD[6] = csPadCtlValue;

          // SCLK
          IOMUXC2->SPI_B0_SW_MUX_CTL_PAD[8] = 0 | IOMUXC_SW_MUX_CTL_PAD_SION(1);
          IOMUXC2->SPI_B0_SW_PAD_CTL_PAD[8] = sclkPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FA_SCLK_Idx] = 1;

          // DATA0
          IOMUXC2->SPI_B0_SW_MUX_CTL_PAD[2] = 0;
          IOMUXC2->SPI_B0_SW_PAD_CTL_PAD[2] = dataPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FA_DATA0_Idx] = 2;

          // DATA1
          IOMUXC2->SPI_B1_SW_MUX_CTL_PAD[3] = 0;
          IOMUXC2->SPI_B1_SW_PAD_CTL_PAD[3] = dataPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FA_DATA1_Idx] = 0;

          // DATA2
          IOMUXC2->SPI_B1_SW_MUX_CTL_PAD[2] = 0;
          IOMUXC2->SPI_B1_SW_PAD_CTL_PAD[2] = dataPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FA_DATA2_Idx] = 0;

          // DATA3
          IOMUXC2->SPI_B0_SW_MUX_CTL_PAD[10] = 0;
          IOMUXC2->SPI_B0_SW_PAD_CTL_PAD[10] = dataPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FA_DATA3_Idx] = 2;

          // Configure DQS pad
          if ((config->readSampleClkSrc == kFlexSPIReadSampleClk_ExternalInputFromDqsPad) ||
              (config->readSampleClkSrc == kFlexSPIReadSampleClk_LoopbackFromDqsPad))
          {
              IOMUXC2->SPI_B0_SW_MUX_CTL_PAD[9] = 0 | IOMUXC_SW_MUX_CTL_PAD_SION(1);
              IOMUXC2->SPI_B0_SW_PAD_CTL_PAD[9] = dqsPadCtlValue;
              IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_DQS_Idx] = 2;
          }
      }

      if(config->sflashB1Size || (config->sflashA1Size && (config->sflashPadType == kSerialFlash_8Pads)))
      {
          // DATA4/0
          IOMUXC2->SPI_B0_SW_MUX_CTL_PAD[11] = 0;
          IOMUXC2->SPI_B0_SW_PAD_CTL_PAD[11] = dataPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FB_DATA0_Idx] = 1;
          // DATA5/1
          IOMUXC2->SPI_B0_SW_MUX_CTL_PAD[7] = 0;
          IOMUXC2->SPI_B0_SW_PAD_CTL_PAD[7] = dataPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FB_DATA1_Idx] = 1;
          // DATA6/2
          IOMUXC2->SPI_B0_SW_MUX_CTL_PAD[3] = 0;
          IOMUXC2->SPI_B0_SW_PAD_CTL_PAD[3] = dataPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FB_DATA2_Idx] = 1;
          // DATA7/3
          IOMUXC2->SPI_B0_SW_MUX_CTL_PAD[4] = 0;
          IOMUXC2->SPI_B0_SW_PAD_CTL_PAD[4] = dataPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FB_DATA3_Idx] = 1;
      }

      // Configure Differential Clock pin or PORTB is enabled
      if(flexspi_is_differential_clock_enable(config) || config->sflashB1Size)
      {
          IOMUXC2->SPI_B0_SW_MUX_CTL_PAD[1] = 0 | IOMUXC_SW_MUX_CTL_PAD_SION(1);
          IOMUXC2->SPI_B0_SW_PAD_CTL_PAD[1] = sclkPadCtlValue;
          IOMUXC2->FLEXSPI2_SELECT_INPUT[kFlexSpi2_FB_SCLK_Idx] = 0;
      }
    }
}


status_t flexspi_set_failsafe_setting(flexspi_mem_config_t *config)
{
    status_t status = kStatus_InvalidArgument;
    do
    {
        if (config == NULL)
        {
            break;
        }
// This is an example that shows how to override the default pad setting in ROM, for now, the pad setting in ROM is
// idential to below values
// So, below codes are not required.
#if 0
        // See IOMUXC pad setting definitions for more details.
        config->controllerMiscOption |= (1<<kFlexSpiMiscOffset_PadSettingOverrideEnable);
        config->dqsPadSettingOverride = 0x130f1;
        config->sclkPadSettingOverride = 0x10f1;
        config->csPadSettingOverride = 0x10f1;
        config->dataPadSettingOverride = 0x10f1;
#endif
        if (config->readSampleClkSrc == kFlexSPIReadSampleClk_ExternalInputFromDqsPad)
        {
            if (config->controllerMiscOption & (1 << kFlexSpiMiscOffset_DdrModeEnable))
            {
                config->dataValidTime[0].time_100ps = 19; // 1.9 ns // 1/4 * cycle of 133MHz DDR
            }
            else
            {
                if (config->dataValidTime[0].delay_cells < 1)
                {
                    config->dataValidTime[0].time_100ps = 38; // 3.8 ns // 1/2 * cycle of 133MHz DDR
                }
            }
        }
        status = kStatus_Success;

    } while (0);

    return status;
}

void dummy_byte_callback(uint8_t byte)
{
    (void)byte;
}


//!@brief Write FlexSPI persistent content
status_t flexspi_nor_write_persistent(const uint32_t data)
{
    SRC->GPR[2] = data;

    return kStatus_Success;
}
//!@brief Read FlexSPI persistent content
status_t flexspi_nor_read_persistent(uint32_t *data)
{
    *data = SRC->GPR[2];

    return kStatus_Success;
}


void flexspi_update_padsetting(flexspi_mem_config_t *config, uint32_t driveStrength)
{
#define IOMUXC_PAD_SETTING_DSE_SHIFT (3)
#define IOMUXC_PAD_SETTING_DSE_MASK (0x07 << IOMUXC_PAD_SETTING_DSE_SHIFT)
#define IOMUXC_PAD_SETTING_DSE(x) (((x) << IOMUXC_PAD_SETTING_DSE_SHIFT) & IOMUXC_PAD_SETTING_DSE_MASK)
    if (driveStrength)
    {
        config->controllerMiscOption |= FLEXSPI_BITMASK(kFlexSpiMiscOffset_PadSettingOverrideEnable);
        config->dqsPadSettingOverride =
            (FLEXSPI_DQS_SW_PAD_CTL_VAL & ~IOMUXC_PAD_SETTING_DSE_MASK) | IOMUXC_PAD_SETTING_DSE(driveStrength);
        config->sclkPadSettingOverride =
            (FLEXSPI_SW_PAD_CTL_VAL & ~IOMUXC_PAD_SETTING_DSE_MASK) | IOMUXC_PAD_SETTING_DSE(driveStrength);
        config->dataPadSettingOverride =
            (FLEXSPI_SW_PAD_CTL_VAL & ~IOMUXC_PAD_SETTING_DSE_MASK) | IOMUXC_PAD_SETTING_DSE(driveStrength);

        config->csPadSettingOverride =
            (FLEXSPI_DQS_SW_PAD_CTL_VAL & ~IOMUXC_PAD_SETTING_DSE_MASK) | IOMUXC_PAD_SETTING_DSE(driveStrength);
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
