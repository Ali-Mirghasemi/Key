#include "Key.h"

/* private macros */
#define KEY_NULL            ((Key*) 0)

#if KEY_MAX_NUM == -1
    #define Key_ptr(KEY)    KEY
#else
    #define Key_ptr(KEY)    (*KEY)
#endif // KEY_MAX_NUM == -1

/* private variables */
static const Key_Driver* keyDriver;
#if KEY_MAX_NUM == -1
    static Key* lastKey = KEY_NULL;
#else
    static Key* keys[KEY_MAX_NUM] = {0};
#endif // KEY_MAX_NUM == -1

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
void Key_irq(void) {
#if KEY_MAX_NUM == -1
    Key* pKey = lastKey;
#else
    Key** pKey = keys;
#endif
    while (KEY_NULL != Key_ptr(pKey)) {
        // update current state
    #if KEY_ACTIVE_STATE
        Key_ptr(pKey)->State = (Key_ptr(pKey)->State << 1) | (keyDriver->readPin(Key_ptr(pKey)->Config) ^ Key_ptr(pKey)->ActiveState);
    #else
        Key_ptr(pKey)->State = (Key_ptr(pKey)->State << 1) | keyDriver->readPin(Key_ptr(pKey)->Config);
    #endif // KEY_ACTIVE_STATE
        
        // call callback on new state
	#if KEY_NONE_CALLBACK
		if (Key_ptr(pKey)->NotActive == Key_NotHandled) {
    #if KEY_MULTI_CALLBACK
            Key_ptr(pKey)->NotActive = Key_ptr(pKey)->Callbacks.callbacks[Key_ptr(pKey)->State](Key_ptr(pKey), Key_ptr(pKey)->State);
    #else
            Key_ptr(pKey)->NotActive = Key_ptr(pKey)->Callbacks.onChange(Key_ptr(pKey), pKey->State);
    #endif /* KEY_MULTI_CALLBACK_ENABLE */
        }
        else if (Key_State_None == Key_ptr(pKey)->State && Key_ptr(pKey)->NotActive != Key_NotHandled) {
                Key_ptr(pKey)->NotActive = Key_NotHandled;
        }
    #else
		if (Key_State_None != Key_ptr(pKey)->State && Key_ptr(pKey)->NotActive == Key_NotHandled) {
        #if KEY_MULTI_CALLBACK
            Key_ptr(pKey)->NotActive = Key_ptr(pKey)->Callbacks.callbacks[Key_ptr(pKey)->State](Key_ptr(pKey), Key_ptr(pKey)->State);
        #else
            Key_ptr(pKey)->NotActive = Key_ptr(pKey)->Callbacks.onChange(Key_ptr(pKey), Key_ptr(pKey)->State);
        #endif /* KEY_MULTI_CALLBACK_ENABLE */
        }
        else if (Key_State_None == Key_ptr(pKey)->State && Key_ptr(pKey)->NotActive != Key_NotHandled) {
            Key_ptr(pKey)->NotActive = Key_NotHandled;
        }
    #endif
    #if KEY_MAX_NUM == -1
        // switch to previous key
        pKey = pKey->Previous;
    #else
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
 * @brief add key into list for process
 * 
 * @param key address of key
 * @param config key pin configuration
 */
uint8_t Key_add(Key* key, const Key_PinConfig* config) {
    // add new key to list
    key->State = Key_State_None;
    key->NotActive = Key_NotHandled;
    Key_setConfig(key, config);
    // init IOs
    keyDriver->initPin(config);
#if KEY_MAX_NUM == -1
    // add key to linked list
    key->Previous = lastKey;
    lastKey = key;
    return 1;
#else
    // find empty space for new key
    {
        uint8_t len = KEY_MAX_NUM;
        Key** pKey = keys;
        while (len--) {
            if (KEY_NULL == *pKey) {
                *pKey = key;
                return 1;
            }
            pKey++;
        } 
    }
    return 0;
#endif // KEY_MAX_NUM == -1
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
#else
    Key** pKey = keys;
#endif // KEY_MAX_NUM == -1
    // find key
    while (KEY_NULL != Key_ptr(pKey)) {
    #if KEY_MAX_NUM == -1
        // check key with remove key
        if (remove == pKey->Previous) {
            // deinit IO
					#if KEY_USE_DEINIT
            if (keyDriver->deinitPin) {
                keyDriver->deinitPin(remove->Config);
            }
					#endif
            // remove key dropped from link list
            pKey->Previous = remove->Previous;
            remove->Previous = NULL;
            break;
        }
        pKey = pKey->Previous;
    #else
        if (remove == *pKey) {
            *pKey = KEY_NULL;
            break;
        }
        pKey++;
    #endif // KEY_MAX_NUM == -1
    }
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
#endif /* KEY_ACTIVE_STATE_ENABLE */
