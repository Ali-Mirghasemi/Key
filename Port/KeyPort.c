#include "KeyPort.h"
#include "KeyIO.h"

const Key_Driver KEY_DRIVER = {
    Key_initPin,
    Key_readPin,
#if KEY_USE_DEINIT
    Key_deInitPin,
#endif
};

#if KEY_HW == KEY_HW_AVR

void Key_initPin(const Key_PinConfig* config) {
    config->IO->Direction.Value &= ~config->Pin;
    config->IO->OutputData.Value |= config->Pin;
}
uint8_t Key_readPin(const Key_PinConfig* config) {
    return (config->IO->InputData.Value & config->Pin) != 0;
}
#if KEY_USE_DEINIT
void Key_deInitPin(const Key_PinConfig* config) {
    config->IO->OutputData.Value &= ~config->Pin;
}
#endif

#elif KEY_HW == KEY_HW_STM32F0 

void Key_initPin(const Key_PinConfig* config) {
    config->IO->Direction.Value &= ~config->Pin;
    config->IO->OutputData.Value |= config->Pin;
}
uint8_t Key_readPin(const Key_PinConfig* config) {
    return (config->IO->InputData.Value & config->Pin) != 0;
}
#if KEY_USE_DEINIT
void Key_deInitPin(const Key_PinConfig* config) {
    config->IO->OutputData.Value &= ~config->Pin;
}
#endif

#elif KEY_HW == KEY_HW_STM32F1 
    
#elif KEY_HW == KEY_HW_STM32F2
    
#elif KEY_HW == KEY_HW_STM32F3
    
#elif KEY_HW == KEY_HW_STM32F4

void Key_initPin(const Key_PinConfig* config) {
    config->IO->MODER &= ~(0x03 << (config->Pin + config->Pin));
#if !KEY_ACTIVE_STATE
    config->IO->PUPDR &= ~(0x03 << (config->Pin + config->Pin));
    config->IO->PUPDR |= 0x01 << (config->Pin + config->Pin);
#endif
}
uint8_t Key_readPin(const Key_PinConfig* config) {
    return (config->IO->IDR & config->Pin) != 0;
}
#if KEY_USE_DEINIT
void Key_deInitPin(const Key_PinConfig* config) {
    config->IO->MODER &= ~(0x03 << (config->Pin + config->Pin));
#if !KEY_ACTIVE_STATE
    config->IO->PUPDR &= ~(0x03 << (config->Pin + config->Pin));
#endif
}
#endif

#else

    #error "Please select your hardware"

#endif // KEY_HW



