# Key Manager Library

## Abstract

This library used for handle input keys and buttons

it's based on HAL layer
user can port library to every hardware 

## Port

for port library user must implement following functions

initPin - configure pin based on input PinConfig
```C
void Key_initPin(const Key_PinConfig* config);
```
deinitPin - reset pin to register reset value
```C
void Key_deInitPin(const Key_PinConfig* config);
```
readPin - read pin and return 1 or 0
```C
uint8_t Key_readPin(const Key_PinConfig* config);
```

and in the final step give Key_Driver to Key_init function
remember deinitPin function it's optional and can be disable in Configuration 
```C
const Key_Driver KEY_DRIVER = {
    Key_initPin,
    Key_readPin,
#if KEY_USE_DEINIT
    Key_deInitPin,
#endif
};
//...

Key_init(&keyDriver);

```

## Dependencies
- stdint.h

## Configuration

in the configuration part in top of Key.h user can change and customize library based on what need in project

#### KEY_MULTI_CALLBACK
define KEY_MULTI_CALLBACK if u want have sperate callback functions for each state such as Key_onPressed, Key_onHold, Key_onReleased

#### KEY_ACTIVE_STATE
user can have different Key Active State for each key (pin)

#### KEY_NONE_CALLBACK
give user Key_State_None callback, must enable KEY_MULTI_CALLBACK None callback fire periodically

#### KEY_USE_DEINIT
user must define deinitPin function in Key_Driver

#### KEY_CONFIG_IO
if Key is based on pair of GPIO and Pin num must enable it for arduino must disable it

#### Key_IO
hold key io
user can change it to GPIO_TypeDef or anything else that system want

#### Key_Pin
hold key pin num or pin bit
user can change it to uint8_t for 8-bit systems like AVR

#### KEY_MAX_NUM
maximum number of keys
-1 for unlimited, lib use linked list 
x for limited keys, lib use pointer array

#### Pre-Implemented KeyPort
in Port folder shows how to port and it's provide some port for MCUs

## Examples 
- [Key-AVR-Example](./Examples/Key-AVR-Example/) shows basic usage of `Key` Library
