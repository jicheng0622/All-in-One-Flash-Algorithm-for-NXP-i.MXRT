/* -----------------------------------------------------------------------------
 * Copyright (c) 2018 Adesto Technologies
 *
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software. Permission is granted to anyone to use this 
 * software for any purpose, including commercial applications, and to alter 
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be 
 *    appreciated but is not required. 
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be 
 *    misrepresented as being the original software. 
 * 
 * 3. This notice may not be removed or altered from any source distribution.
 * --------------------------------------------------------------------------- */

// i.MX RT1050 external flash (FlexSPI) based address
// (originally included this parameter from NXP SDK header file MIMXRT1052.h.
// Unfortunately this header file brings in some junk with it, so we're defining it here instead)
#define FLASH_BASE_ADDR (0x60000000)

// EcoXiP size definitions
#define FLASH_PAGE_SIZE  (0x100) //256
#define FLASH_SECTORE_SIZE (0x1000) //4096
#define FLASH_SIZE (0x00400000U) //4MB
