#include "KeyPort.h"



#if KEY_HW == KEY_HW_AVR

void Key_initPin(const Key_PinConfig* config) {
    config->IO->Direction &= ~config->Pin;
    config->IO->OutputData |= config->Pin;
}
void Key_deInitPin(const Key_PinConfig* config) {
    config->IO->OutputData &= ~config->Pin;
}
uint8_t Key_readPin(const Key_PinConfig* config) {
    return config->IO->InputData & config->Pin != 0;
}

#endif // KEY_HW



