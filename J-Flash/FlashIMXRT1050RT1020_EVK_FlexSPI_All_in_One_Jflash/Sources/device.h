/*************************************************************************
 *
 * device definitons
 *
 **************************************************************************/
#include <stdint.h>
#include "flash_config.h"

#include "fsl_device_registers.h"
/*device struct*/
typedef struct{
#if USE_ARGC_ARGV
  uint32_t (*init)(void *base_of_flash,int argc, char const *argv[]);
#else
  uint32_t (*init)(void *base_of_flash);
#endif /* USE_ARGC_ARGV */
  uint32_t (*write)(uint32_t addr,uint32_t count,char const *buffer);
  uint32_t (*erase)(void *block_start, uint32_t block_size);
  uint32_t (*erase_chip)(void);
  uint32_t (*signoff)(void);
} device_t;

extern const device_t HyperFlash;
extern const device_t QSPIFlash;

