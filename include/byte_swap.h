/**
 * @file byte_swap.h
 * @author Ian Ress
 * @brief Byte swapping macros and inline functions. Macros must be used for compile-time values. Inline function
 * variants must be used for run-time values.
 * @version 0.1
 * @date 2023-09-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef BYTE_SWAP_H_
#define BYTE_SWAP_H_


#include <stdint.h>



/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------- SWAPPING MACROS FOR COMPILE-TIME VALUES ------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Swaps the Endianness of a 16-bit value that is determined at compile-time. The return
 * value of this macro is meant to be used to initialize variables at compile-time.
 * 
 * @attention Only use this for values that are determined at compile-time. Only use this
 * for int16_t and uint16_t types.
 * 
 * @param x The int16_t or uint16_t value to swap.
 * 
 */
#define SWAP16_COMPILETIME(x)                                   (uint16_t)((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))


/**
 * @brief Swaps the Endianness of a 32-bit value that is determined at compile-time. The return
 * value of this macro is meant to be used to initialize variables at compile-time.
 * 
 * @attention Only use this for values that are determined at compile-time. Only use this
 * for int32_t and uint32_t types. This macro will not work for floats.
 * 
 * @param x The int32_t or uint32_t value to swap.
 * 
 */
#define SWAP32_COMPILETIME(x)                                   (uint32_t)((((x) & 0xFF000000UL) >> 24UL) | (((x) & 0x00FF0000UL) >> 8UL) | \
                                                                        (((x) & 0x0000FF00UL) << 8UL)  | (((x) & 0x000000FFUL) << 24UL))



/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------- SWAPPING FUNCTIONS FOR RUN-TIME VALUES -------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Swaps the Endianness of a 16-bit value that is determined at run-time. This
 * function will not modify the input directly. It will return a copy of the value with 
 * swapped Endianness.
 * 
 * @attention Only use this for values that are determined at run-time. Only use this
 * for int16_t and uint16_t types.
 * 
 * @param val The int16_t or uint16_t value to swap.
 * 
 * @return A copy of the swapped value.
 * 
 */
static inline uint16_t Swap16_RunTime(uint16_t val);
static inline uint16_t Swap16_RunTime(uint16_t val)
{
    uint16_t copy = val;
    uint8_t temp = *( ( ( uint8_t * ) &copy ) );

    *( ( ( uint8_t * ) &copy ) ) = *( ( ( uint8_t * ) &copy ) + 1 );
    *( ( ( uint8_t * ) &copy ) + 1 ) = temp;

    return copy;
}


/**
 * @brief Swaps the Endianness of a 32-bit value that is determined at run-time. This
 * function will not modify the input directly. It will return a copy of the value with 
 * swapped Endianness.
 * 
 * @attention Only use this for values that are determined at run-time. Only use this for
 * int32_t and uint32_t types. This function will not work with floats as the compiler will
 * typecast the input and output to a uint32_t.
 * 
 * @param val The int32_t or uint32_t value to swap.
 * 
 * @return A copy of the swapped value.
 * 
 */
static inline uint32_t Swap32_RunTime(uint32_t val);
static inline uint32_t Swap32_RunTime(uint32_t val)
{
    uint32_t copy = val;
    uint8_t temp = *( ( ( uint8_t * ) &copy ) + 3 );

    *( ( ( uint8_t * ) &copy ) + 3 ) = *( ( ( uint8_t * ) &copy ) );
    *( ( ( uint8_t * ) &copy ) ) = temp;

    temp = *( ( ( uint8_t * ) &copy ) + 2 );
    *( ( ( uint8_t * ) &copy ) + 2 ) = *( ( ( uint8_t * ) &copy ) + 1 );
    *( ( ( uint8_t * ) &copy ) + 1 ) = temp;

    return copy;
}


/**
 * @brief Swaps N bytes of data. This function will directly modify the data since it is passed 
 * by reference.
 * 
 * @attention Only use this for run-time data. Only use this on pointers pointing to a single 
 * value or on a uint8_t array. This will NOT work on arrays that store multi-byte types since 
 * each of the individual bytes will be swapped.
 * 
 * @param data The data to swap. This can be a pointer to a single value or a uint8_t array.
 * @param size The number of bytes to swap. This MUST be the same size as the data passed
 * into this function. A size smaller than the data size will cause not all of the bytes to
 * be swapped. A SIZE LARGER THAN THE DATA SIZE WILL RESULT IN OUT OF BOUNDS MEMORY WRITES.
 * 
 */
static inline void Swap_NBytes_RunTime(void * const data, size_t size);
static inline void Swap_NBytes_RunTime(void * const data, size_t size)
{
    uint8_t * ptr;
    uint8_t temp;

    if ((size > 0) && (data != (void *)0))
    {
        ptr = (uint8_t *)data;
        
        for (size_t i = 0; i < (size/2); i++)
        {
            temp = *(ptr + i);
            *(ptr + i) = *(ptr + size - i - 1);
            *(ptr + size - i - 1) = temp;
        }
    }
}

#endif /* BYTE_SWAP_H_ */
