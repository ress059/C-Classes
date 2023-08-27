#warning "TODO: DESCRIPTION AND EVERYTHING"
/**
 * @file ring_buffer_dynamic.h
 * @author Ian Ress
 * @brief BY VALUE
 * @version 0.1
 * @date 2023-08-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef RING_BUFFER_DYNAMIC_H_
#define RING_BUFFER_DYNAMIC_H_


#include <stdbool.h>
#include <stdint.h>



/**
 * @defgroup rb_dyn Ring Buffer Dynamic Class
 * @brief Ring Buffer Class that uses Dynamic Memory Allocation
 */


/**
 * @defgroup rb_dyn_handle Class Handle
 * @{
 */

/**
 * @brief Ring Buffer Class Handle
 */
typedef struct Ring_Buffer_t * Ring_Buffer_Dynamic_Handle;

/**@}*/ // end rb_dyn_handle


/**
 * @defgroup rb_dyn_public_fuctions Public Functions
 * @{
 */

/**
 * @brief Ring Buffer Constructor.
 * 
 * @attention Constructor uses Dynamic Memory Allocation.
 * 
 * @attention All elements of the buffer must be the same size. This, along with the number of elements
 * in the buffer cannot change once initialized. I.e. @ref element_size_0 and @ref number_of_elements_0
 * cannot change after the Constructor is called.
 * 
 * @param me Ring Buffer Handle to initialize.
 * @param element_size_0 Number of bytes of each element. This must be greater than 0. For example this 
 * would be 2 for a buffer of uint16_t values. 
 * @param number_of_elements_0 Maximum number of elements the buffer can hold. This must be greater than 1.
 * For example if you want a 5 element Ring Buffer of uint16_t values, @ref element_size_0 would be 2 and 
 * @ref number_of_elements_0 would be 5.
 * 
 * @return True if successful. False if unsuccessful. An unsuccessful attempt will occur if invalid arguments
 * were supplied, memory allocation for the Ring Buffer failed, or the requested buffer was too large to store
 * on the hardware target. For example a 2001 byte array on a target with 2kB of memory.
 */
bool Ring_Buffer_Dynamic_Ctor(Ring_Buffer_Dynamic_Handle me, size_t element_size_0, uint32_t number_of_elements_0);


/**
 * @brief Ring Buffer Handle Destructor.
 * 
 * @param me Ring Buffer Handle. It is the Application's responsibility to ensure this Handle is not 
 * used once it is destroyed.
 */
void Ring_Buffer_Dynamic_Destroy(Ring_Buffer_Dynamic_Handle me);


/**
 * @brief Writes data BY VALUE to the Ring Buffer and increments HEAD.
 * 
 * @attention This function will directly copy memory into the Buffer. The @ref data memory block must
 * be equal to the number of bytes specified in @ref data_size otherwise behavior is undefined.
 * 
 * @param me Ring Buffer Handle.
 * @param data The starting memory block of data to write to the Buffer. This function will copy the memory
 * contents to perform a write by value.
 * @param data_size Number of Bytes being written. This must equal @ref element_size_0 specified in the
 * Constructor when the Object was initialized.
 * 
 * @return True if the Write was successful. False if unsuccessful. An unsuccessful write will occur if
 * the Buffer is Full, an invalid Handle is supplied, a NULL data pointer is supplied, or the specified 
 * data size is not equal to @ref element_size_0 specified when the Constructor was called. 
 */
bool Ring_Buffer_Dynamic_Write(Ring_Buffer_Dynamic_Handle me, const void * data, size_t data_size);


/**
 * @brief Reads data from the Ring Buffer, increments TAIL, and updates empty status.
 * 
 * @attention This function will directly write to the @ref data memory block reserved for the read.
 * This block must equal the number of bytes specified in @ref data_size otherwise behavior is undefined.
 * 
 * @param me Ring Buffer Handle.
 * @param data The starting memory block to store data in.
 * @param data_size Number of Bytes being read. This must equal @ref element_size_0 specified in the
 * Constructor when the Object was initialized.
 * 
 * @return True if the Read was successful. False if unsuccessful. An unsuccessful read will occur if
 * the Buffer is Empty, an invalid Handle is supplied, a NULL data pointer is supplied, or the specified 
 * data size is not equal to @ref element_size_0 specified when the Constructor was called.
 */
bool Ring_Buffer_Dynamic_Read(Ring_Buffer_Dynamic_Handle me, void * data, size_t data_size);


/**
 * @brief Returns if the Ring Buffer is Empty. Empty means there are no elements in
 * the Buffer. Reads cannot be performed if Empty.
 * 
 * @param me Ring Buffer Handle.
 * 
 * @return True if Empty. False if not Empty or an invalid Handle was supplied.
 */
bool Ring_Buffer_Dynamic_Is_Empty(const Ring_Buffer_Dynamic_Handle me);


/**
 * @brief Returns if the Ring Buffer is Full. Writes cannot be performed if Full.
 * 
 * @param me Ring Buffer Handle.
 * 
 * @return True if Full or an invalid Handle was supplied. False otherwise.
 */
bool Ring_Buffer_Dynamic_Is_Full(const Ring_Buffer_Dynamic_Handle me);

/**@}*/ // end rb_dyn_public_fuctions

/**@}*/ // end rb_dyn


#endif /* RING_BUFFER_DYNAMIC_H_ */
