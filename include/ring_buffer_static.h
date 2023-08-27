/**
 * @file ring_buffer_static.h
 * @author Ian Ress
 * @brief Generic Ring Buffer that is passed BY VALUE without the use of Dynamic Memory Allocation. Each element
 * of the buffer must be the same size. To avoid Dynamic Memory Allocation, each Ring Buffer has a fixed-size and an 
 * array of Ring Buffers is initialized at compile-time. When the Application calls the Ring Buffer Constructor, a 
 * Ring Buffer from this pool (array) is reserved and the returned Handle is the index in this array containing the 
 * reserved Ring Buffer. DO NOT EDIT THE VALUE OF THIS HANDLE DIRECTLY. This Handle serves as a Ring Buffer Object 
 * (instance of Ring Buffer Class) that can be input into the public methods. No other parts of the Application can 
 * use this Ring Buffer Handle until the Destructor is called.
 * 
 * To pass data by value functions take in a void pointer and copy memory contents directly to and from the buffer.
 * @version 0.1
 * @date 2023-08-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef RING_BUFFER_STATIC_H_
#define RING_BUFFER_STATIC_H_


/* STD-C Libraries */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------- MAXIMUM SIZES (MAXIMUM MEMORY ALLOCATED FOR RING BUFFER CLASS) ----------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The number of Ring Buffer Objects that are initialized. In order to avoid Dynamic Memory Allocation, this 
 * Ring Buffer Class initializes an array of Ring Buffers at compile-time. This is the number of elements in that array.
 * 
 */
#define NUMBER_OF_STATIC_RING_BUFFERS                                       4


/**
 * @brief The maximum size (number of bytes) of each Ring Buffer Object's buffer. Since no Dynamic Memory Allocation 
 * is used, a fixed-sized buffer has to be used so Memory is allocated at compile-time. Ring Buffers requesting storage
 * more than this number of bytes cannot be constructed. Also note that if a Ring Buffer is reserved that needs a
 * buffer size much less than this amount, BUFFER_SIZE number of bytes is still reserved and the extras are unused.
 * 
 * @note This would normally be in the Source File. However it is included in the Header File to make it possible
 * for the Application to throw an error at COMPILE-TIME if a too large buffer is requested.
 * 
 */
#define RING_BUFFER_STATIC_SIZE                                             200



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------- RING BUFFER CLASS HANDLE. USED AS THE CLASS OBJECT ----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Ring Buffer Object Handle which is similar to a Class Instance.
 * 
 * @warning Do NOT edit this Handle directly. This is meant to be a Constant Handle that the Application
 * only uses to feed into the Ring Buffer functions defined in this Class.
 * 
 */
typedef uint32_t Ring_Buffer_Static_Handle;



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------- PUBLIC FUNCTIONS -------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Ring Buffer Constructor.
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
 * were supplied or the requested buffer was too large to store. To avoid Dynamic Memory Allocation each Ring
 * Buffer has a fixed size. The requested storage cannot be greater than this fixed size.
 *
 */
bool Ring_Buffer_Static_Ctor(Ring_Buffer_Static_Handle * me, size_t element_size_0, uint32_t number_of_elements_0);


/**
 * @brief Ring Buffer Handle Destructor.
 * 
 * @param me Ring Buffer Handle. Constructor must have been called to use Handle in this function. It 
 * is the Application's responsibility to ensure this Handle is not used once it is destroyed.
 * 
 */
void Ring_Buffer_Static_Destroy(Ring_Buffer_Static_Handle me);


/**
 * @brief Clears the contents of the Ring Buffer and resets HEAD and TAIL to 0. Note that this only
 * resets the Ring Buffer so the Handle is still usable. If it is desired to completely destroy the
 * Ring Buffer the Destroy function should be called instead.
 * 
 * @param me Ring Buffer Handle. Constructor must have been called to use Handle in this
 * function. Note that after this function call this Handle is still usable.
 * 
 */
void Ring_Buffer_Static_Reset(Ring_Buffer_Static_Handle me);


/**
 * @brief Returns the number of elements CURRENTLY stored in the Ring Buffer. This is different
 * from the maximum number of elements (number_of_elements_0) initialized in the Constructor.
 * 
 * @param me Ring Buffer Handle. Constructor must have been called to use Handle in this
 * function.
 * 
 * @return Returns 0 if the Ring Buffer is currently empty of an invalid Handle was supplied.
 * Otherwise returns the number of elements.
 * 
 */
uint32_t Ring_Buffer_Static_Get_Number_Of_Elements(Ring_Buffer_Static_Handle me);


/**
 * @brief Writes data BY VALUE to the Ring Buffer and increments HEAD.
 * 
 * @attention This function will directly copy memory into the Buffer. The @ref data memory block must
 * be equal to the number of bytes specified in @ref data_size otherwise behavior is undefined.
 * 
 * @param me Ring Buffer Handle. Constructor must have been called to use Handle in this
 * function.
 * @param data The starting memory block of data to write to the Buffer. This function will copy the memory
 * contents to perform a write by value.
 * @param data_size Number of Bytes being written. This must equal @ref element_size_0 specified in the
 * Constructor when the Object was initialized.
 * 
 * @return True if the Write was successful. False if unsuccessful. An unsuccessful write will occur if
 * the Buffer is Full, an invalid Handle is supplied, a NULL data pointer is supplied, or the specified 
 * data size is not equal to @ref element_size_0 specified when the Constructor was called.
 * 
 */
bool Ring_Buffer_Static_Write(Ring_Buffer_Static_Handle me, const void * data, size_t data_size);


/**
 * @brief Reads data from the Ring Buffer, increments TAIL, and updates empty status.
 * 
 * @attention This function will directly write to the @ref data memory block reserved for the read.
 * This block must equal the number of bytes specified in @ref data_size otherwise behavior is undefined.
 * 
 * @param me Ring Buffer Handle. Constructor must have been called to use Handle in this
 * function.
 * @param data The starting memory block to store data in.
 * @param data_size Number of Bytes being read. This must equal @ref element_size_0 specified in the
 * Constructor when the Object was initialized.
 * 
 * @return True if the Read was successful. False if unsuccessful. An unsuccessful read will occur if
 * the Buffer is Empty, an invalid Handle is supplied, a NULL data pointer is supplied, or the specified 
 * data size is not equal to @ref element_size_0 specified when the Constructor was called.
 * 
 */
bool Ring_Buffer_Static_Read(Ring_Buffer_Static_Handle me, void * data, size_t data_size);


/**
 * @brief Returns if the Ring Buffer is Empty. Empty means there are no elements in
 * the Buffer. Reads cannot be performed if Empty.
 * 
 * @param me Ring Buffer Handle. Constructor must have been called to use Handle in this
 * function.
 * 
 * @return True if Empty. False if not Empty or an invalid Handle was supplied.
 * 
 */
bool Ring_Buffer_Static_Is_Empty(Ring_Buffer_Static_Handle me);


/**
 * @brief Returns if the Ring Buffer is Full. Writes cannot be performed if Full.
 * 
 * @param me Ring Buffer Handle. Constructor must have been called to use Handle in this
 * function.
 * 
 * @return True if Full or an invalid Handle was supplied. False otherwise.
 * 
 */
bool Ring_Buffer_Static_Is_Full(Ring_Buffer_Static_Handle me);


#endif /* RING_BUFFER_STATIC_H_ */
