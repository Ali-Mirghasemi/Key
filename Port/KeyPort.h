/**
 * @file KeyPort.h
 * @author Ali Mirghasemi (ali.mirghasemi1376@gmail.com)
 * @brief this file show how to port lib into new hardware
 *          and have multiple of pre-implemented ports
 * @version 0.1.0
 * @date 2021-06-28
 * 
 * @copyright Copyright (c) 2021 Ali_Mirghasemi
 * 
 */

#ifndef _KEY_PORT_H_
#define _KEY_PORT_H_

#include "Key.h"

#define KEY_HW_AVR          0
#define KEY_HW_STM32F0      1
#define KEY_HW_STM32F1      2
#define KEY_HW_STM32F2      3
#define KEY_HW_STM32F3      4
#define KEY_HW_STM32F4      5

/**
 * @brief set your hardware from above list
 */
#define KEY_HW              KEY_HW_AVR

void Key_initPin(const Key_PinConfig* config);
void Key_deInitPin(const Key_PinConfig* config);
uint8_t Key_readPin(const Key_PinConfig* config);

extern const Key_Driver keyDriver = {
    Key_initPin,
    Key_readPin,
#if KEY_USE_DEINIT
    Key_deInitPin,
#endif
};

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

#endif

#endif /* _KEY_PORT_H_ */
