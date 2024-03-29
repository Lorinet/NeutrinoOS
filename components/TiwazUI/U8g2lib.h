#ifndef U8G2LIB_H
#define U8G2LIB_H

#include "u8g2.h"
#include "gpio.h"
#ifdef __cplusplus
extern "C"
{
#endif
	uint8_t u8x8_arm_linux_gpio_and_delay(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
	uint8_t u8x8_byte_arm_linux_hw_i2c(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
	uint8_t u8x8_byte_arm_linux_hw_spi(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
#ifdef __cplusplus
}
#endif
#endif
