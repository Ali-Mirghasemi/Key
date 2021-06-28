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

void Key_initPin(const Key_PinConfig* config);
uint8_t Key_readPin(const Key_PinConfig* config);

#if KEY_USE_DEINIT
void Key_deInitPin(const Key_PinConfig* config);
#endif

static const Key_Driver keyDriver = {
    Key_initPin,
    Key_readPin,
#if KEY_USE_DEINIT
    Key_deInitPin,
#endif
};

#endif /* _KEY_PORT_H_ */
