#ifndef BLINKER_ASSISTANT_H
#define BLINKER_ASSISTANT_H

#if defined(BLINKER_ALIGENIE_LIGHT)
    #if defined(BLINKER_ALIGENIE_OUTLET)
        #undef BLINKER_ALIGENIE_OUTLET
    #endif
    
    #if defined(BLINKER_ALIGENIE_MULTI_OUTLET)
        #undef BLINKER_ALIGENIE_MULTI_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_SWITCH)
        #undef BLINKER_ALIGENIE_SWITCH
    #endif

    #if defined(BLINKER_ALIGENIE_SENSOR)
        #undef BLINKER_ALIGENIE_SENSOR
    #endif

    #if defined(BLINKER_ALIGENIE_FAN)
        #undef BLINKER_ALIGENIE_FAN
    #endif

    #if defined(BLINKER_ALIGENIE_AIRCONDITION)
        #undef BLINKER_ALIGENIE_AIRCONDITION
    #endif

    #define BLINKER_ALIGENIE
#elif defined(BLINKER_ALIGENIE_OUTLET)
    #if defined(BLINKER_ALIGENIE_MULTI_OUTLET)
        #undef BLINKER_ALIGENIE_MULTI_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_LIGHT)
        #undef BLINKER_ALIGENIE_LIGHT
    #endif

    #if defined(BLINKER_ALIGENIE_SWITCH)
        #undef BLINKER_ALIGENIE_SWITCH
    #endif

    #if defined(BLINKER_ALIGENIE_SENSOR)
        #undef BLINKER_ALIGENIE_SENSOR
    #endif

    #if defined(BLINKER_ALIGENIE_FAN)
        #undef BLINKER_ALIGENIE_FAN
    #endif

    #if defined(BLINKER_ALIGENIE_AIRCONDITION)
        #undef BLINKER_ALIGENIE_AIRCONDITION
    #endif

    #define BLINKER_ALIGENIE
#elif defined(BLINKER_ALIGENIE_MULTI_OUTLET)
    #if defined(BLINKER_ALIGENIE_OUTLET)
        #undef BLINKER_ALIGENIE_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_LIGHT)
        #undef BLINKER_ALIGENIE_LIGHT
    #endif

    #if defined(BLINKER_ALIGENIE_SWITCH)
        #undef BLINKER_ALIGENIE_SWITCH
    #endif

    #if defined(BLINKER_ALIGENIE_SENSOR)
        #undef BLINKER_ALIGENIE_SENSOR
    #endif

    #if defined(BLINKER_ALIGENIE_FAN)
        #undef BLINKER_ALIGENIE_FAN
    #endif

    #if defined(BLINKER_ALIGENIE_AIRCONDITION)
        #undef BLINKER_ALIGENIE_AIRCONDITION
    #endif

    #define BLINKER_ALIGENIE
#elif defined(BLINKER_ALIGENIE_SWITCH)
    #if defined(BLINKER_ALIGENIE_LIGHT)
        #undef BLINKER_ALIGENIE_LIGHT
    #endif

    #if defined(BLINKER_ALIGENIE_OUTLET)
        #undef BLINKER_ALIGENIE_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_MULTI_OUTLET)
        #undef BLINKER_ALIGENIE_MULTI_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_SENSOR)
        #undef BLINKER_ALIGENIE_SENSOR
    #endif

    #if defined(BLINKER_ALIGENIE_FAN)
        #undef BLINKER_ALIGENIE_FAN
    #endif

    #if defined(BLINKER_ALIGENIE_AIRCONDITION)
        #undef BLINKER_ALIGENIE_AIRCONDITION
    #endif

    #define BLINKER_ALIGENIE
#elif defined(BLINKER_ALIGENIE_SENSOR)
    #if defined(BLINKER_ALIGENIE_LIGHT)
        #undef BLINKER_ALIGENIE_LIGHT
    #endif

    #if defined(BLINKER_ALIGENIE_OUTLET)
        #undef BLINKER_ALIGENIE_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_MULTI_OUTLET)
        #undef BLINKER_ALIGENIE_MULTI_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_SWITCH)
        #undef BLINKER_ALIGENIE_SWITCH
    #endif

    #if defined(BLINKER_ALIGENIE_FAN)
        #undef BLINKER_ALIGENIE_FAN
    #endif

    #if defined(BLINKER_ALIGENIE_AIRCONDITION)
        #undef BLINKER_ALIGENIE_AIRCONDITION
    #endif
    
    #define BLINKER_ALIGENIE
#elif defined(BLINKER_ALIGENIE_FAN)
    #if defined(BLINKER_ALIGENIE_LIGHT)
        #undef BLINKER_ALIGENIE_LIGHT
    #endif

    #if defined(BLINKER_ALIGENIE_OUTLET)
        #undef BLINKER_ALIGENIE_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_MULTI_OUTLET)
        #undef BLINKER_ALIGENIE_MULTI_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_SWITCH)
        #undef BLINKER_ALIGENIE_SWITCH
    #endif

    #if defined(BLINKER_ALIGENIE_SENSOR)
        #undef BLINKER_ALIGENIE_SENSOR
    #endif

    #if defined(BLINKER_ALIGENIE_AIRCONDITION)
        #undef BLINKER_ALIGENIE_AIRCONDITION
    #endif
    
    #define BLINKER_ALIGENIE
#elif defined(BLINKER_ALIGENIE_AIRCONDITION)
    #if defined(BLINKER_ALIGENIE_LIGHT)
        #undef BLINKER_ALIGENIE_LIGHT
    #endif

    #if defined(BLINKER_ALIGENIE_OUTLET)
        #undef BLINKER_ALIGENIE_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_MULTI_OUTLET)
        #undef BLINKER_ALIGENIE_MULTI_OUTLET
    #endif

    #if defined(BLINKER_ALIGENIE_SWITCH)
        #undef BLINKER_ALIGENIE_SWITCH
    #endif

    #if defined(BLINKER_ALIGENIE_SENSOR)
        #undef BLINKER_ALIGENIE_SENSOR
    #endif

    #if defined(BLINKER_ALIGENIE_FAN)
        #undef BLINKER_ALIGENIE_FAN
    #endif
    
    #define BLINKER_ALIGENIE
#endif

#if defined(BLINKER_DUEROS_LIGHT)
    #if defined(BLINKER_DUEROS_OUTLET)
        #undef BLINKER_DUEROS_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_MULTI_OUTLET)
        #undef BLINKER_DUEROS_MULTI_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_SWITCH)
        #undef BLINKER_DUEROS_SWITCH
    #endif

    #if defined(BLINKER_DUEROS_SENSOR)
        #undef BLINKER_DUEROS_SENSOR
    #endif

    #if defined(BLINKER_DUEROS_FAN)
        #undef BLINKER_DUEROS_FAN
    #endif

    #if defined(BLINKER_DUEROS_AIRCONDITION)
        #undef BLINKER_DUEROS_AIRCONDITION
    #endif

    #define BLINKER_DUEROS
#elif defined(BLINKER_DUEROS_OUTLET)
    #if defined(BLINKER_DUEROS_MULTI_OUTLET)
        #undef BLINKER_DUEROS_MULTI_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_LIGHT)
        #undef BLINKER_DUEROS_LIGHT
    #endif

    #if defined(BLINKER_DUEROS_SWITCH)
        #undef BLINKER_DUEROS_SWITCH
    #endif

    #if defined(BLINKER_DUEROS_SENSOR)
        #undef BLINKER_DUEROS_SENSOR
    #endif

    #if defined(BLINKER_DUEROS_FAN)
        #undef BLINKER_DUEROS_FAN
    #endif

    #if defined(BLINKER_DUEROS_AIRCONDITION)
        #undef BLINKER_DUEROS_AIRCONDITION
    #endif

    #define BLINKER_DUEROS
#elif defined(BLINKER_DUEROS_MULTI_OUTLET)
    #if defined(BLINKER_DUEROS_OUTLET)
        #undef BLINKER_DUEROS_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_LIGHT)
        #undef BLINKER_DUEROS_LIGHT
    #endif

    #if defined(BLINKER_DUEROS_SWITCH)
        #undef BLINKER_DUEROS_SWITCH
    #endif

    #if defined(BLINKER_DUEROS_SENSOR)
        #undef BLINKER_DUEROS_SENSOR
    #endif

    #if defined(BLINKER_DUEROS_FAN)
        #undef BLINKER_DUEROS_FAN
    #endif

    #if defined(BLINKER_DUEROS_AIRCONDITION)
        #undef BLINKER_DUEROS_AIRCONDITION
    #endif

    #define BLINKER_DUEROS
#elif defined(BLINKER_DUEROS_SENSOR)
    #if defined(BLINKER_DUEROS_LIGHT)
        #undef BLINKER_DUEROS_LIGHT
    #endif

    #if defined(BLINKER_DUEROS_SWITCH)
        #undef BLINKER_DUEROS_SWITCH
    #endif

    #if defined(BLINKER_DUEROS_OUTLET)
        #undef BLINKER_DUEROS_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_MULTI_OUTLET)
        #undef BLINKER_DUEROS_MULTI_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_FAN)
        #undef BLINKER_DUEROS_FAN
    #endif

    #if defined(BLINKER_DUEROS_AIRCONDITION)
        #undef BLINKER_DUEROS_AIRCONDITION
    #endif

    #define BLINKER_DUEROS
#elif defined(BLINKER_DUEROS_FAN)
    #if defined(BLINKER_DUEROS_LIGHT)
        #undef BLINKER_DUEROS_LIGHT
    #endif

    #if defined(BLINKER_DUEROS_SWITCH)
        #undef BLINKER_DUEROS_SWITCH
    #endif

    #if defined(BLINKER_DUEROS_OUTLET)
        #undef BLINKER_DUEROS_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_MULTI_OUTLET)
        #undef BLINKER_DUEROS_MULTI_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_SENSOR)
        #undef BLINKER_DUEROS_SENSOR
    #endif

    #if defined(BLINKER_DUEROS_AIRCONDITION)
        #undef BLINKER_DUEROS_AIRCONDITION
    #endif

    #define BLINKER_DUEROS
#elif defined(BLINKER_DUEROS_AIRCONDITION)
    #if defined(BLINKER_DUEROS_LIGHT)
        #undef BLINKER_DUEROS_LIGHT
    #endif

    #if defined(BLINKER_DUEROS_SWITCH)
        #undef BLINKER_DUEROS_SWITCH
    #endif

    #if defined(BLINKER_DUEROS_OUTLET)
        #undef BLINKER_DUEROS_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_MULTI_OUTLET)
        #undef BLINKER_DUEROS_MULTI_OUTLET
    #endif

    #if defined(BLINKER_DUEROS_SENSOR)
        #undef BLINKER_DUEROS_SENSOR
    #endif

    #if defined(BLINKER_DUEROS_FAN)
        #undef BLINKER_DUEROS_FAN
    #endif

    #define BLINKER_DUEROS
#endif

#if defined(BLINKER_MIOT_LIGHT)
    #if defined(BLINKER_MIOT_OUTLET)
        #undef BLINKER_MIOT_OUTLET
    #endif
    
    #if defined(BLINKER_MIOT_MULTI_OUTLET)
        #undef BLINKER_MIOT_MULTI_OUTLET
    #endif

    #if defined(BLINKER_MIOT_SWITCH)
        #undef BLINKER_MIOT_SWITCH
    #endif

    #if defined(BLINKER_MIOT_SENSOR)
        #undef BLINKER_MIOT_SENSOR
    #endif

    #if defined(BLINKER_MIOT_FAN)
        #undef BLINKER_MIOT_FAN
    #endif

    #if defined(BLINKER_MIOT_AIRCONDITION)
        #undef BLINKER_MIOT_AIRCONDITION
    #endif

    #define BLINKER_MIOT
#elif defined(BLINKER_MIOT_OUTLET)
    #if defined(BLINKER_MIOT_MULTI_OUTLET)
        #undef BLINKER_MIOT_MULTI_OUTLET
    #endif

    #if defined(BLINKER_MIOT_LIGHT)
        #undef BLINKER_MIOT_LIGHT
    #endif

    #if defined(BLINKER_MIOT_SWITCH)
        #undef BLINKER_MIOT_SWITCH
    #endif

    #if defined(BLINKER_MIOT_SENSOR)
        #undef BLINKER_MIOT_SENSOR
    #endif

    #if defined(BLINKER_MIOT_FAN)
        #undef BLINKER_MIOT_FAN
    #endif

    #if defined(BLINKER_MIOT_AIRCONDITION)
        #undef BLINKER_MIOT_AIRCONDITION
    #endif

    #define BLINKER_MIOT
#elif defined(BLINKER_MIOT_MULTI_OUTLET)
    #if defined(BLINKER_MIOT_OUTLET)
        #undef BLINKER_MIOT_OUTLET
    #endif

    #if defined(BLINKER_MIOT_LIGHT)
        #undef BLINKER_MIOT_LIGHT
    #endif

    #if defined(BLINKER_MIOT_SWITCH)
        #undef BLINKER_MIOT_SWITCH
    #endif

    #if defined(BLINKER_MIOT_SENSOR)
        #undef BLINKER_MIOT_SENSOR
    #endif

    #if defined(BLINKER_MIOT_FAN)
        #undef BLINKER_MIOT_FAN
    #endif

    #if defined(BLINKER_MIOT_AIRCONDITION)
        #undef BLINKER_MIOT_AIRCONDITION
    #endif

    #define BLINKER_MIOT
#elif defined(BLINKER_MIOT_SWITCH)
    #if defined(BLINKER_MIOT_LIGHT)
        #undef BLINKER_MIOT_LIGHT
    #endif

    #if defined(BLINKER_MIOT_OUTLET)
        #undef BLINKER_MIOT_OUTLET
    #endif

    #if defined(BLINKER_MIOT_MULTI_OUTLET)
        #undef BLINKER_MIOT_MULTI_OUTLET
    #endif

    #if defined(BLINKER_MIOT_SENSOR)
        #undef BLINKER_MIOT_SENSOR
    #endif

    #if defined(BLINKER_MIOT_FAN)
        #undef BLINKER_MIOT_FAN
    #endif

    #if defined(BLINKER_MIOT_AIRCONDITION)
        #undef BLINKER_MIOT_AIRCONDITION
    #endif

    #define BLINKER_MIOT
#elif defined(BLINKER_MIOT_SENSOR)
    #if defined(BLINKER_MIOT_LIGHT)
        #undef BLINKER_MIOT_LIGHT
    #endif

    #if defined(BLINKER_MIOT_OUTLET)
        #undef BLINKER_MIOT_OUTLET
    #endif

    #if defined(BLINKER_MIOT_MULTI_OUTLET)
        #undef BLINKER_MIOT_MULTI_OUTLET
    #endif

    #if defined(BLINKER_MIOT_SWITCH)
        #undef BLINKER_MIOT_SWITCH
    #endif

    #if defined(BLINKER_MIOT_FAN)
        #undef BLINKER_MIOT_FAN
    #endif

    #if defined(BLINKER_MIOT_AIRCONDITION)
        #undef BLINKER_MIOT_AIRCONDITION
    #endif
    
    #define BLINKER_MIOT
#elif defined(BLINKER_MIOT_FAN)
    #if defined(BLINKER_MIOT_LIGHT)
        #undef BLINKER_MIOT_LIGHT
    #endif

    #if defined(BLINKER_MIOT_OUTLET)
        #undef BLINKER_MIOT_OUTLET
    #endif

    #if defined(BLINKER_MIOT_MULTI_OUTLET)
        #undef BLINKER_MIOT_MULTI_OUTLET
    #endif

    #if defined(BLINKER_MIOT_SWITCH)
        #undef BLINKER_MIOT_SWITCH
    #endif

    #if defined(BLINKER_MIOT_SENSOR)
        #undef BLINKER_MIOT_SENSOR
    #endif

    #if defined(BLINKER_MIOT_AIRCONDITION)
        #undef BLINKER_MIOT_AIRCONDITION
    #endif
    
    #define BLINKER_MIOT
#elif defined(BLINKER_MIOT_AIRCONDITION)
    #if defined(BLINKER_MIOT_LIGHT)
        #undef BLINKER_MIOT_LIGHT
    #endif

    #if defined(BLINKER_MIOT_OUTLET)
        #undef BLINKER_MIOT_OUTLET
    #endif

    #if defined(BLINKER_MIOT_MULTI_OUTLET)
        #undef BLINKER_MIOT_MULTI_OUTLET
    #endif

    #if defined(BLINKER_MIOT_SWITCH)
        #undef BLINKER_MIOT_SWITCH
    #endif

    #if defined(BLINKER_MIOT_SENSOR)
        #undef BLINKER_MIOT_SENSOR
    #endif

    #if defined(BLINKER_MIOT_FAN)
        #undef BLINKER_MIOT_FAN
    #endif
    
    #define BLINKER_MIOT
#endif

#endif
