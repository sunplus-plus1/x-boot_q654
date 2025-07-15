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


struct otp_data_info {
	unsigned char sd_vsel;
	unsigned char sdio_vsel;
	unsigned char ca55_vsel;
	unsigned char mipitx_vset;
	unsigned int product_id;
	unsigned char chip_id[3];
	unsigned char therm_calib[3];
	unsigned char mac_addr[6];
	unsigned int cpu_freq;
};
#define OTP_INFO_SRAM_ADDR 0xfa29f000

#define OTP_INFO_DATA ((volatile struct otp_data_info *)OTP_INFO_SRAM_ADDR)

#endif /* __INC_REGMAP_H */
