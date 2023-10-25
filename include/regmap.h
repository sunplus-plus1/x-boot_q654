#ifndef __INC_REGMAP_H
#define __INC_REGMAP_H

#include <config.h>

#include <regmap_sp7350.h>

// set PIN _p to GPIO
// IN(_iso=0) or out(_iso=1)
// _out is output value for OUT pin
void gpio_set_IO( uint8_t _p, uint8_t _iso, uint8_t _out);

// get gpio value for pin _p
uint8_t gpio_getV( uint8_t _p);

#endif /* __INC_REGMAP_H */
