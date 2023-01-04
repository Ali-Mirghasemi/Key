#include "Key.h"

/* private variables */
static const Key_Driver* keyDriver;
#if KEY_MAX_NUM == -1
    static Key* lastKey = KEY_NULL;
#else
    static Key keys[KEY_MAX_NUM] = {0};
#endif // KEY_MAX_NUM == -1

#if KEY_USE_INIT
    #define __initPin(CONF)          if (keyDriver->initPin) { keyDriver->initPin(CONF); }
#else
    #define __initPin(CONF)
#endif

#if KEY_USE_DEINIT
    #define __deinitPin(CONF)        if (keyDriver->deinitPin) { keyDriver->deinitPin(CONF); }
#else
    #define __deinitPin(CONF)
#endif

/**
 * @brief use for initialize
 * 
 * @param driver 
 */
void Key_init(const Key_Driver* driver) {
    keyDriver = driver;
}
/**
 * @brief user must place it in timer with 20ms ~ 50ms 
 * all of callbacks handle and fire in this function
 */
void Key_handle(void) {
    Key_State state;
#if KEY_MAX_NUM == -1
    Key* pKey = lastKey;
    while (KEY_NULL != pKey) {
#else
    Key* pKey = keys;
    uint8_t len = KEY_MAX_NUM;
    while (len-- > 0) {
        if (pKey->Configured) {
#endif
    #if KEY_ENABLE_FLAG
        if (pKey->Enabled) {
    #endif // KEY_ENABLE_FLAG
        // update current state
        state = (Key_State) pKey->State;
    #if KEY_ACTIVE_STATE
        state = (Key_State)(((state << 1) | (keyDriver->readPin(pKey->Config) ^ pKey->ActiveState)) & 0x03);
    #else
        state = (Key_State) (((state << 1) | keyDriver->readPin(pKey->Config)) & 0x03);
    #endif // KEY_ACTIVE_STATE
        pKey->State = state;
        // call callback on new state
		if (pKey->NotActive == Key_NotHandled
        #if !KEY_NONE_CALLBACK
            && Key_State_None != state
        #endif  
            ) {
        #if KEY_MULTI_CALLBACK
            if (pKey->Callbacks.callbacks[state]) {
                pKey->NotActive = pKey->Callbacks.callbacks[state](pKey, state);
            }
        #else
            if (pKey->Callbacks.onChange) {
                pKey->NotActive = pKey->Callbacks.onChange(pKey, state);
            }
        #endif /* KEY_MULTI_CALLBACK_ENABLE */
        }
        else if (Key_State_None == state && pKey->NotActive != Key_NotHandled) {
            pKey->NotActive = Key_NotHandled;
        }
    #if KEY_ENABLE_FLAG
        }
    #endif // KEY_ENABLE_FLAG
    #if KEY_MAX_NUM == -1
        // switch to previous key
        pKey = pKey->Previous;
    #else
        }
        pKey++;
    #endif // KEY_MAX_NUM == -1
    }
}

/**
 * @brief set new pin configuration for key
 * 
 * @param key address of key instance
 * @param config new pin configuration
 * @return uint8_t return 1 if key added, 0 if there is no space
 */
void Key_setConfig(Key* key, const Key_PinConfig* config) {
    key->Config = config;
}
/**
 * @brief get key pin config
 * 
 * @param key 
 * @return const Key_PinConfig* 
 */
const Key_PinConfig* Key_getConfig(Key* key) {
    return key->Config;
}
#if KEY_MAX_NUM > 0
/**
 * @brief finding a empty space for new key in array
 * 
 * @return Key* return null if not found empty space
 */
Key* Key_new(void) {
    uint8_t len = KEY_MAX_NUM;
    Key* pKey = keys;
    while (len-- > 0) {
        if (!pKey->Configured) {
            return pKey;
        }
        pKey++;
    }
    return KEY_NULL;
}
#endif // KEY_MAX_NUM
/**
 * @brief add key into list for process
 * 
 * @param key address of key
 * @param config key pin configuration
 */
uint8_t Key_add(Key* key, const Key_PinConfig* config) {
    // check for null
    if (KEY_NULL == key) {
        return 0;
    }
    // add new key to list
    key->State = Key_State_None;
    key->NotActive = Key_NotHandled;
    Key_setConfig(key, config);
    // init IOs
    __initPin(config);
#if KEY_MAX_NUM == -1
    // add key to linked list
    key->Previous = lastKey;
    lastKey = key;
#endif // KEY_MAX_NUM == -1
    key->Configured = 1;
    key->Enabled = 1;
    return 1;
}
/**
 * @brief remove key from list
 * 
 * @param remove address of key
 * @return uint8_t return 1 if key found, 0 if not found
 */
uint8_t Key_remove(Key* remove) {
#if KEY_MAX_NUM == -1
    Key* pKey = lastKey;
    // check last key first
    if (remove == pKey) {
        // deinit IO
        __deinitPin(remove->Config);
        // remove key dropped from link list
        pKey->Previous = remove->Previous;
        remove->Previous = KEY_NULL;
        remove->Configured = 0;
        remove->Enabled = 0;
        return 1;
    }
    while (KEY_NULL != pKey) {
        if (remove == pKey->Previous) {
            // deinit IO
            __deinitPin(remove->Config);
            // remove key dropped from link list
            pKey->Previous = remove->Previous;
            remove->Previous = KEY_NULL;
            remove->Configured = 0;
            remove->Enabled = 0;
            return 1;
        }
        pKey = pKey->Previous;
    }
#else
    uint8_t len = KEY_MAX_NUM;
    Key* pKey = keys;
    while (len--) {
        if (remove == pKey && pKey->Configured) {
            pKey->Configured = 0;
            pKey->Enabled = 0;
            return 1;
        }
        pKey++;
    }
#endif // KEY_MAX_NUM == -1
    return 0;
}
/**
 * @brief finding key based on PinConfig in list
 * 
 * @param config 
 * @return Key* 
 */
Key* Key_find(const Key_PinConfig* config) {
#if KEY_MAX_NUM == -1
    Key* pKey = lastKey;
    while (KEY_NULL != pKey) {
        if (config == pKey->Config) {
            return pKey;
        }
        pKey = pKey->Previous;
    }
#else
    uint8_t len = KEY_MAX_NUM;
    Key* pKey = keys;
    while (len--) {
        if (config == pKey->Config) {
            return pKey;
        }
        pKey++;
    }
#endif // KEY_MAX_NUM == -1
    return KEY_NULL;
}

#if KEY_MULTI_CALLBACK
void Key_onHold(Key* key, Key_Callback cb) {
    key->Callbacks.onHold = cb;
}
void Key_onReleased(Key* key, Key_Callback cb) {
    key->Callbacks.onReleased = cb;
}
void Key_onPressed(Key* key, Key_Callback cb) {
    key->Callbacks.onPressed = cb;
}
#if KEY_NONE_CALLBACK
void Key_onNone(Key* key, Key_Callback cb) {
    key->Callbacks.onNone = cb;
}
#endif // KEY_NONE_CALLBACK
#else
void Key_onChange(Key* key, Key_Callback cb) {
    key->Callbacks.onChange = cb;
}
#endif // KEY_MULTI_CALLBACK

#if KEY_ACTIVE_STATE
void Key_setActiveState(Key* key, Key_ActiveState state) {
    key->ActiveState = (uint8_t) state;
}
Key_ActiveState Key_getActiveState(Key* key) {
    return (Key_ActiveState) key->ActiveState;
}
#endif /* KEY_ACTIVE_STATE_ENABLE */

#if KEY_ENABLE_FLAG
void Key_setEnabled(Key* key, uint8_t enabled) {
    key->Enabled = enabled;
}
uint8_t Key_isEnabled(Key* key) {
    return key->Enabled;
}
#endif

#if KEY_ARGS
void Key_setArgs(Key*, void* args) {
    key->Args = args;
}
void* Key_getArgs(Key* key) {
    return key->Args;
}
#endif

