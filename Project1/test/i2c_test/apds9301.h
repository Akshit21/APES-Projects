#ifndef _APDS9301_H__
#define _APDS9301_H__

#include <stdint.h>
#include "user_i2c.h"

#define APDS9301_ADDR (0x39)

#define CONTROL_REG (0x00)
#define TIMING_REG  (0x01)
#define THRESH_REG (0x02)
#define THRESHLOWLOW_REG  (0x02)
#define THRESHLOWHIGH_REG (0x03)
#define THRESHHIGHLOW_REG (0x04)
#define THRESHHIGHHIGH_REG  (0x05)
#define INTERRUPT_REG (0x06)
#define ID_REG  (0x0A)
#define DATA0_REG   (0x0C)
#define DATA0LOW_REG  (0x0C)
#define DATA0HIGH_REG (0x0D)
#define DATA1_REG (0x0E)
#define DATA1LOW_REG (0x0E)
#define DATA1HIGH_REG (0x0F)

#define POWER_ON  (0x03)
#define POWER_OFF (0x00)


#endif
