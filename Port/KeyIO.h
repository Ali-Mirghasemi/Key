/**
 * @file KeyIO.h
 * @author Ali Mirghasemi (ali.mirghasemi1376@gmail.com)
 * @brief set mcu hardware
 * @version 0.1
 * @date 2021-06-28
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _KEY_IO_H_
#define _KEY_IO_H_

#include <stdint.h>

#define KEY_HW_CUSTOM       0
#define KEY_HW_AVR          1
#define KEY_HW_STM32F0      2
#define KEY_HW_STM32F1      3
#define KEY_HW_STM32F2      4
#define KEY_HW_STM32F3      5
#define KEY_HW_STM32F4      6

/**
 * @brief set your hardware from above list
 */
#define KEY_HW              KEY_HW_AVR

#if KEY_HW == KEY_HW_AVR
typedef union {
  uint8_t       Value;  
  struct {
      uint8_t   Bit0    : 1;
      uint8_t   Bit1    : 1;
      uint8_t   Bit2    : 1;
      uint8_t   Bit3    : 1;
      uint8_t   Bit4    : 1;
      uint8_t   Bit5    : 1;
      uint8_t   Bit6    : 1;
      uint8_t   Bit7    : 1;
  };
} GPIO_Register;
/**
 * @brief compact 3 io register into one struct
 */
typedef struct {
    // PIN
    volatile GPIO_Register InputData;
    // DDR
    volatile GPIO_Register Direction;
    // PORT
    volatile GPIO_Register OutputData;
} GPIO_TypeDef;

#define GPIOA           ((GPIO_TypeDef*) 0x39)
#define GPIOB           ((GPIO_TypeDef*) 0x36)
#define GPIOC           ((GPIO_TypeDef*) 0x33)
#define GPIOD           ((GPIO_TypeDef*) 0x30)

#define GPIO_PIN_0      0x01
#define GPIO_PIN_1      0x02
#define GPIO_PIN_2      0x04
#define GPIO_PIN_3      0x08
#define GPIO_PIN_4      0x10
#define GPIO_PIN_5      0x20
#define GPIO_PIN_6      0x40
#define GPIO_PIN_7      0x80


#else

typedef void GPIO_TypeDef;

#endif


#endif /* _KEY_IO_H_ */
