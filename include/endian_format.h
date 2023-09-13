/**
 * @file endian_format.h
 * @brief Endianness macros that allow data to always be in Little or Big Endian format, regardless of the target's 
 * Endianness. Note that the target's Endianness must be defined in order for this to work. Either CPU_BIG_ENDIAN or
 * CPU_LITTLE_ENDIAN must be defined to 1. It is reccomended to do so in another Header File that serves as a wrapper,
 * which will be included in the application instead of endian_format.h directly. For example:
 * 
 * 
 * /------------------- application_endian.h -------------------/
 * #ifndef APPLICATION_ENDIAN_H_
 * #define APPLICATION_ENDIAN_H_
 * 
 *     #define CPU_LITTLE_ENDIAN               1
 *     #include "endian_format.h"
 * 
 * #endif
 * 
 * 
 * /---------------------------- main.c ------------------------/
 * #include "application_endian.h"
 * 
 * // Use endian macros via application_endian.h wrapper.
 * uint16_t var = LE16_COMPILETIME(0x1234);
 * 
 * 
 * @version 0.1
 * @date 2023-05-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef ENDIAN_FORMAT_H_
#define ENDIAN_FORMAT_H_

/* STD-C Libraries */
#include <stddef.h> /* size_t */
#include <stdint.h>

/* Byte Swapping Macros and Functions */
#include "byte_swap.h"



/*----------------------------------------------------------------------------------------------------------------------*/
/*--------------- MACROS THAT ALWAYS PUT VALUES IN SPECIFIED ENDIANNESS, REGARDLESS OF TARGET'S ENDIANNESS -------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

#if ((CPU_LITTLE_ENDIAN) > 0) /* Compiling on Little Endian machine */

    /**
     * @brief Returns the input value such that it's stored in Little Endian format, regardless of the target's
     * Endianness. The return value of this macro is meant to be used to initialize variables at compile-time. 
     * I.e. uint16_t LE_var = LE16_COMPILETIME(0x1234); Since we are compiling on a Little Endian machine, the 
     * input will already be stored in Little Endian format. Therefore this macro does nothing.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for variables determined at run-time. Use LE16_RUNTIME(x)
     * instead. Only use this for int16_t and uint16_t values.
     * 
     * @param x The int16_t or uint16_t value to store in Little Endian.
     * 
     */
    #define LE16_COMPILETIME(x)                             (x)


    /**
     * @brief Returns a copy of the input variable such that it's stored in Little Endian format, regardless
     * of the target's Endianness. This macro is meant to be used on variables determined at run-time. Since 
     * we are compiling on a Little Endian machine, the input will already be stored in Little Endian format. 
     * Therefore this macro does nothing.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for values determined at compile-time. Use LE16_COMPILETIME(x)
     * instead. Only use this for int16_t and uint16_t variables.
     * 
     * @param x The int16_t or uint16_t run-time variable to store in Little Endian. This macro
     * will not edit this input parameter directly. It will return a copy.
     * 
     */
    #define LE16_RUNTIME(x)                                 (x)


    /**
     * @brief Returns the input value such that it's stored in Little Endian format, regardless of the target's
     * Endianness. The return value of this macro is meant to be used to initialize variables at compile-time. 
     * I.e. uint32_t LE_var = LE32_COMPILETIME(0x12345678); Since we are compiling on a Little Endian machine, 
     * the input will already be stored in Little Endian format. Therefore this macro does nothing.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for variables determined at run-time. Use LE32_RUNTIME(x)
     * instead. Only use this for int32_t and uint32_t values. This macro will not work for IEEE-754 floats.
     * 
     * @param x The int32_t or uint32_t value to store in Little Endian.
     * 
     */
    #define LE32_COMPILETIME(x)                             (x)


    /**
     * @brief Returns a copy of the input variable such that it's stored in Little Endian format, regardless
     * of the target's Endianness. This macro is meant to be used on variables determined at run-time. Since 
     * we are compiling on a Little Endian machine, the input will already be stored in Little Endian format. 
     * Therefore this macro does nothing.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for values determined at compile-time. Use LE32_COMPILETIME(x)
     * instead. Only use this for int32_t and uint32_t variables. This macro will not work for IEEE-754 floats.
     * 
     * @param x The int32_t or uint32_t run-time variable to store in Little Endian. This macro
     * will not edit this input parameter directly. It will return a copy.
     * 
     */
    #define LE32_RUNTIME(x)                                 (x)


    /**
     * @brief Returns the input value such that it's stored in Big Endian format, regardless of the target's
     * Endianness. The return value of this macro is meant to be used to initialize variables at compile-time. 
     * I.e. uint16_t BE_var = BE16_COMPILETIME(0x1234); Since we are compiling on a Little Endian machine, this 
     * macro will return the input with its bytes swapped. I.e. uint16_t BE_var = BE16_COMPILETIME(0x1234) 
     * evaluates to uint16_t BE_var = 0x3412;
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for variables determined at run-time. Use BE16_RUNTIME(x)
     * instead. Only use this for int16_t and uint16_t values.
     * 
     * @param x The int16_t or uint16_t value to store in Big Endian.
     * 
     */
    #define BE16_COMPILETIME(x)                             SWAP16_COMPILETIME(x)


    /**
     * @brief Returns a copy of the input variable such that it's stored in Big Endian format, regardless
     * of the target's Endianness. This macro is meant to be used on variables determined at run-time. Since 
     * we are compiling on a Little Endian machine, this macro will return a copy of the input variable with 
     * its bytes swapped.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for values determined at compile-time. Use BE16_COMPILETIME(x)
     * instead. Only use this for int16_t and uint16_t variables.
     * 
     * @param x The int16_t or uint16_t run-time variable to store in Big Endian. This macro
     * will not edit this input parameter directly. It will return a copy.
     * 
     */
    #define BE16_RUNTIME(x)                                 Swap16_RunTime(x)


    /**
     * @brief Returns the input value such that it's stored in Big Endian format, regardless of the target's
     * Endianness. The return value of this macro is meant to be used to initialize variables at compile-time. 
     * I.e. uint32_t BE_var = BE32_COMPILETIME(0x12345678); Since we are compiling on a Little Endian machine, this 
     * macro will return the input with its bytes swapped. I.e. uint32_t BE_var = BE32_COMPILETIME(0x12345678);
     * evaluates to uint32_t BE_var = 0x78563412;
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for variables determined at run-time. Use BE32_RUNTIME(x)
     * instead. Only use this for int32_t and uint32_t values. This macro will not work for IEEE-754 floats.
     * 
     * @param x The int32_t or uint32_t value to store in Big Endian.
     * 
     */
    #define BE32_COMPILETIME(x)                             SWAP32_COMPILETIME(x)


    /**
     * @brief Returns a copy of the input variable such that it's stored in Big Endian format, regardless
     * of the target's Endianness. This macro is meant to be used on variables determined at run-time. Since 
     * we are compiling on a Little Endian machine, this macro will return a copy of the input variable with 
     * its bytes swapped.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for values determined at compile-time. Use BE32_COMPILETIME(x)
     * instead. Only use this for int32_t and uint32_t variables. This macro will not work for IEEE-754 floats.
     * 
     * @param x The int32_t or uint32_t run-time variable to store in Big Endian. This macro
     * will not edit this input parameter directly. It will return a copy.
     * 
     */
    #define BE32_RUNTIME(x)                                 Swap32_RunTime(x)

#elif ((CPU_BIG_ENDIAN) > 0) /* Compiling on Big Endian machine */

    /**
     * @brief Returns the input value such that it's stored in Little Endian format, regardless of the target's
     * Endianness. The return value of this macro is meant to be used to initialize variables at compile-time. 
     * I.e. uint16_t LE_var = LE16_COMPILETIME(0x1234); Since we are compiling on a Big Endian machine, this 
     * macro will return the input with its bytes swapped. I.e. uint16_t LE_var = LE16_COMPILETIME(0x1234) 
     * evaluates to uint16_t LE_var = 0x3412;
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for variables determined at run-time. Use LE16_RUNTIME(x)
     * instead. Only use this for int16_t and uint16_t values.
     * 
     * @param x The int16_t or uint16_t value to store in Little Endian.
     * 
     */
    #define LE16_COMPILETIME(x)                             SWAP16_COMPILETIME(x)


    /**
     * @brief Returns a copy of the input variable such that it's stored in Little Endian format, regardless
     * of the target's Endianness. This macro is meant to be used on variables determined at run-time. Since 
     * we are compiling on a Big Endian machine, this macro will return a copy of the input variable with 
     * its bytes swapped.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for values determined at compile-time. Use LE16_COMPILETIME(x)
     * instead. Only use this for int16_t and uint16_t variables.
     * 
     * @param x The int16_t or uint16_t run-time variable to store in Little Endian. This macro
     * will not edit this input parameter directly. It will return a copy.
     * 
     */
    #define LE16_RUNTIME(x)                                 Swap16_RunTime(x)


    /**
     * @brief Returns the input value such that it's stored in Little Endian format, regardless of the target's
     * Endianness. The return value of this macro is meant to be used to initialize variables at compile-time. 
     * I.e. uint32_t LE_var = LE32_COMPILETIME(0x12345678); Since we are compiling on a Big Endian machine, this 
     * macro will return the input with its bytes swapped. I.e. uint32_t LE_var = LE32_COMPILETIME(0x12345678);
     * evaluates to uint32_t LE_var = 0x78563412;
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for variables determined at run-time. Use LE32_RUNTIME(x)
     * instead. Only use this for int32_t and uint32_t values. This macro will not work for IEEE-754 floats.
     * 
     * @param x The int32_t or uint32_t value to store in Little Endian.
     * 
     */
    #define LE32_COMPILETIME(x)                             SWAP32_COMPILETIME(x)


    /**
     * @brief Returns a copy of the input variable such that it's stored in Little Endian format, regardless
     * of the target's Endianness. This macro is meant to be used on variables determined at run-time. Since 
     * we are compiling on a Big Endian machine, this macro will return a copy of the input variable with 
     * its bytes swapped.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for values determined at compile-time. Use LE32_COMPILETIME(x)
     * instead. Only use this for int32_t and uint32_t variables. This macro will not work for IEEE-754 floats.
     * 
     * @param x The int32_t or uint32_t run-time variable to store in Little Endian. This macro
     * will not edit this input parameter directly. It will return a copy.
     * 
     */
    #define LE32_RUNTIME(x)                                 Swap32_RunTime(x)


    /**
     * @brief Returns the input value such that it's stored in Big Endian format, regardless of the target's
     * Endianness. The return value of this macro is meant to be used to initialize variables at compile-time. 
     * I.e. uint16_t BE_var = BE16_COMPILETIME(0x1234); Since we are compiling on a Big Endian machine, the 
     * input will already be stored in Big Endian format. Therefore this macro does nothing.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for variables determined at run-time. Use BE16_RUNTIME(x)
     * instead. Only use this for int16_t and uint16_t values.
     * 
     * @param x The int16_t or uint16_t value to store in Big Endian.
     * 
     */
    #define BE16_COMPILETIME(x)                             (x)


    /**
     * @brief Returns a copy of the input variable such that it's stored in Big Endian format, regardless
     * of the target's Endianness. This macro is meant to be used on variables determined at run-time. Since 
     * we are compiling on a Big Endian machine, the input will already be stored in Big Endian format. 
     * Therefore this macro does nothing.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for values determined at compile-time. Use BE16_COMPILETIME(x)
     * instead. Only use this for int16_t and uint16_t variables.
     * 
     * @param x The int16_t or uint16_t run-time variable to store in Big Endian. This macro
     * will not edit this input parameter directly. It will return a copy.
     * 
     */
    #define BE16_RUNTIME(x)                                 (x)


    /**
     * @brief Returns the input value such that it's stored in Big Endian format, regardless of the target's
     * Endianness. The return value of this macro is meant to be used to initialize variables at compile-time. 
     * I.e. uint32_t BE_var = BE32_COMPILETIME(0x12345678); Since we are compiling on a Big Endian machine, 
     * the input will already be stored in Big Endian format. Therefore this macro does nothing.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for variables determined at run-time. Use BE32_RUNTIME(x)
     * instead. Only use this for int32_t and uint32_t values. This macro will not work for IEEE-754 floats.
     * 
     * @param x The int32_t or uint32_t value to store in Big Endian.
     * 
     */    
    #define BE32_COMPILETIME(x)                             (x)


    /**
     * @brief Returns a copy of the input variable such that it's stored in Big Endian format, regardless
     * of the target's Endianness. This macro is meant to be used on variables determined at run-time. Since 
     * we are compiling on a Big Endian machine, the input will already be stored in Big Endian format. 
     * Therefore this macro does nothing.
     * 
     * @attention Do not delete this from the application code. This is needed to make
     * the application portable with both Little and Big Endian targets.
     * 
     * @warning Do not use this for values determined at compile-time. Use BE32_COMPILETIME(x)
     * instead. Only use this for int32_t and uint32_t variables. This macro will not work for IEEE-754 floats.
     * 
     * @param x The int32_t or uint32_t run-time variable to store in Big Endian. This macro
     * will not edit this input parameter directly. It will return a copy.
     * 
     */
    #define BE32_RUNTIME(x)                                 (x)

#else
    #error "In order to use Endianness Macros the CPU Endianness must be defined. See File Description of endian_format.h."
#endif

#endif /* ENDIAN_FORMAT_H_ */
