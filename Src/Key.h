/**
 * @file Key.h
 * @author Ali Mirghasemi (ali.mirghasemi1376@gmail.com)
 * @brief this library use for drive key, button and input signals
 * @version 0.1.0
 * @date 2021-06-28
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _KEY_H_
#define _KEY_H_

#ifdef __cplusplus
extern "C" {
#endif 

/******************************************************************************/
/*                                Configuration                               */
/******************************************************************************/
/**
 * Include your libs
 * IO libs maybe needed
 */
#include <stdint.h>
#include "..\Port\KeyIO.h"

/**
 * @brief define KEY_MULTI_CALLBACK if u want have sperate callback functions
 * for each state such as Key_onPressed, Key_onHold, Key_onReleased
 */
#define KEY_MULTI_CALLBACK              1

/**
 * @brief user can have different Key Active State for each key (pin)
 */
#define KEY_ACTIVE_STATE                0

/**
 * @brief give user Key_State_None callback
 * None callback fire periodically
 */
#define KEY_NONE_CALLBACK	            0

/**
 * @brief user must define deinitPin function in Key_Driver
 */
#define KEY_USE_DEINIT	                0
/**
 * @brief if Key is based on pair of GPIO and Pin num must enable it
 * for arduino must disable it
 */
#define KEY_CONFIG_IO	                1

/**
 * @brief hold key io
 * user can change it to GPIO_TypeDef or anything else that system want
 */
typedef GPIO_TypeDef* Key_IO;
/**
 * @brief hold key pin num or pin bit
 * user can change it to uint8_t for 8-bit systems like AVR
 */
typedef uint8_t Key_Pin;

/**
 * @brief maximum number of keys
 * -1 for unlimited, lib use linked list 
 * x for limited keys, lib use pointer array
 */
#define KEY_MAX_NUM                     -1

/**
 * @brief user can store some args in key struct and retrive them in callbacks
 */
#define KEY_ARGS                        0

/******************************************************************************/


/**
 * @brief hold pin configuration that use for handle key
 */
typedef struct {
#if KEY_CONFIG_IO
    Key_IO     	IO;
#endif
    Key_Pin     Pin;
} Key_PinConfig;

/**
 * @brief Key state
 *        _____                __(None)__
 * (Presed)  <-|____(Hold)____|-> (Released)
 */
typedef enum {
    Key_State_Hold          = 0x00,
    Key_State_Released      = 0x01,
    Key_State_Pressed       = 0x02,
    Key_State_None          = 0x03
} Key_State;

/**
 * @brief logic of key
 * Active Low for Pull-Up keys
 * Active High for Pull-Down keys
 * remember if you have keys with different logic must enable KEY_ACTIVE_STATE
 */
typedef enum {
    Key_ActiveState_Low     = 0,
    Key_ActiveState_High    = 1
} Key_ActiveState;

/**
 * @brief show next callbacks can fire or ignore incoming callbacks
 */
typedef enum {
    Key_NotHandled          = 0,
    Key_Handled	            = 1
} Key_HandleStatus;

struct _Key;
typedef struct _Key Key;

/**
 * @brief initialize pin in input mode, remember if your pin is pull-up, or pull-down 
 * must configured in init function
 * this function call when new key add to queue
 */
typedef void (*Key_InitPinFn)(const Key_PinConfig* config);
/**
 * @brief de-initialize pin and change pin to reset mode
 * this function call on remove key
 */
typedef void (*Key_DeInitPinFn)(const Key_PinConfig* config);
/**
 * @brief this function must return value of a pin
 * 0 -> LOW, 1 -> HIGH
 */
typedef uint8_t (*Key_ReadPinFn)(const Key_PinConfig* config);
/**
 * @brief this callback call when key state change
 * 
 * @param key show which key changed
 * @param state show current state of key
 * @return user can return Key_NotHandled (0) if wanna get callback on other events 
 *                  otherwise can return Key_Handled (1) that mean key handled nad next event is onPressed
 */
typedef Key_HandleStatus (*Key_Callback)(Key* key, Key_State state);
/**
 * @brief hold minimum function for Key lib to work
 * user must pass atleast init and read functions to key library
 */
typedef struct {
    Key_InitPinFn     initPin;
    Key_ReadPinFn     readPin;
    #if KEY_USE_DEINIT
        Key_DeInitPinFn   deinitPin;
    #endif
} Key_Driver;


// determine how many callbacks need
#if KEY_MULTI_CALLBACK && KEY_NONE_CALLBACK
    #define KEY_CALLBACKS_NUM 4
#elif KEY_MULTI_CALLBACK
    #define KEY_CALLBACKS_NUM 3
#else
    #define KEY_CALLBACKS_NUM 1
#endif

typedef union {
    Key_Callback            callbacks[KEY_CALLBACKS_NUM];
    struct {
    #if KEY_MULTI_CALLBACK
        Key_Callback        onHold;
        Key_Callback        onReleased;
        Key_Callback        onPressed;
    #if KEY_NONE_CALLBACK
        Key_Callback        onNone;
    #endif // KEY_NONE_CALLBACK
    #else
        Key_Callback        onChange;
    #endif // KEY_MULTI_CALLBACK

    };
} Key_Callbacks;

/**
 * @brief this struct hold Key parameters
 * do not change the parameters or something else with it
 */
struct _Key {
#if KEY_MAX_NUM == -1
    struct _Key*            Previous;               			/**< point to previous key, if it's null show they key is end of linked list */
#endif // KEY_MAX_NUM == -1
#if KEY_ARGS
    void*                   Args;
#endif
    const Key_PinConfig*    Config;                 			/**< hold pointer to pin configuration */
    Key_Callbacks           Callbacks;                          /**< hold user separate callbacks for each key state */
    uint8_t                 State           : 2;    			/**< show current state of key*/
    uint8_t                 NotActive       : 1;    			/**< show other states will be ignore or not */
    uint8_t                 ActiveState     : 1;    			/**< this parameters use only when Activestate Enabled */
    uint8_t                 Reserved        : 4;    
};

void Key_init(const Key_Driver* driver);
void Key_irq(void);

void Key_setConfig(Key* key, const Key_PinConfig* config);
const Key_PinConfig* Key_getConfig(Key* key);

uint8_t Key_add(Key* key, const Key_PinConfig* config);
uint8_t Key_remove(Key* remove);

#if KEY_MULTI_CALLBACK
    void Key_onHold(Key* key, Key_Callback cb);
    void Key_onReleased(Key* key, Key_Callback cb);
    void Key_onPressed(Key* key, Key_Callback cb);
#if KEY_NONE_CALLBACK
    void Key_onNone(Key* key, Key_Callback cb);
#endif // KEY_NONE_CALLBACK
#else
    void Key_onChange(Key* key, Key_Callback cb);
#endif // KEY_MULTI_CALLBACK


#if KEY_ACTIVE_STATE
    void Key_setActiveState(Key* key, Key_ActiveState state);
    Key_ActiveState Key_getActiveState(Key* key);
#endif

#if KEY_ARGS
    void Key_setArgs(Key*, void* args);
    void* Key_getArgs(Key* key);
#endif

#ifdef __cplusplus
};
#endif

#endif /* _KEY_H_ */
