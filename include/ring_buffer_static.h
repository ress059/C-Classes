/**
 * @file ring_buffer_static.h
 * @author Ian Ress
 * @brief Generic Ring Buffer that is passed BY VALUE without the use of Dynamic Memory Allocation. Each element
 * of the buffer must be the same size. To avoid Dynamic Memory Allocation, each Ring Buffer has a fixed-size and an 
 * array of Ring Buffers is initialized at compile-time. When the Application calls the Ring Buffer Constructor, a 
 * Ring Buffer from this pool (array) is reserved and the returned Handle is the index in this array containing the 
 * reserved Ring Buffer. DO NOT EDIT THE VALUE OF THIS HANDLE DIRECTLY. This Handle serves as a Ring Buffer Object 
 * (instance of Ring Buffer Class) that can be input into the public methods. No other parts of the Application can 
 * use the Ring Buffer reserved for this Handle until it is destroyed via a Destructor call.
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
 * @note This would normally be in the Source File. However it is included in the Header File to make it visible for
 * Unit Tests.
 */
#define NUMBER_OF_STATIC_RING_BUFFERS                                       4


/**
 * @brief The maximum size (number of bytes) of each Ring Buffer Object's buffer. Since no Dynamic Memory Allocation 
 * is used, a fixed-sized buffer has to be used so Memory is allocated at compile-time. Ring Buffers requesting storage
 * more than this number of bytes cannot be constructed. Also note that if a Ring Buffer is reserved that needs a
 * buffer size much less than this amount, BUFFER_SIZE number of bytes is still reserved and the extras are unused.
 * 
 * @note This would normally be in the Source File. However it is included in the Header File to make it possible
 * for the Application to throw an error at COMPILE-TIME if a too large buffer is requested and for this info to
 * be available for Unit Tests.
 */
#define RING_BUFFER_STATIC_SIZE                                             200


/**
 * @brief Checks at compile-time whether the requested Ring Buffer is too large. If the Ring Buffer is too large
 * this macro expands to (void)sizeof(char[-1]) which produces a compilation error.
 * 
 * @param element_size Number of bytes of each element. For example this would be 2 for a buffer of uint16_t values. 
 * @param len Number of elements the requested buffer will hold. For example if you want a 5 element Ring Buffer of 
 * uint16_t values, @ref element_size would be 2 and @ref len would be 5. 
 */
#define RING_BUFFER_SIZE_STATIC_ASSERT(element_size, len)                   (void)sizeof(char[ (1 - 2*!!( ((element_size) * (len)) > (RING_BUFFER_STATIC_SIZE) ) ) ])



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------- RING BUFFER CLASS HANDLE. USED AS THE CLASS OBJECT ----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Ring Buffer Object Handle which is similar to a Class Instance.
 * 
 * @warning Do NOT edit this Handle directly. This is meant to be a Constant Handle that the Application
 * only uses to feed into the Ring Buffer functions defined in this Class.
 * 
 * @warning Do NOT change this and typedef the Handle as a uint32_t pointer. Since no Dynamic Memory
 * Allocation is used the Constructor will NOT allocate memory for the uint32_t value that this would
 * be pointing to, leading to undefined behavior.
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
 * @param me Ring Buffer Handle to initialize. Note that the Constructor will change the value pointed
 * to by this Handle.
 * @param element_size_0 Number of bytes of each element. This must be greater than 0. For example this 
 * would be 2 for a buffer of uint16_t values. 
 * @param number_of_elements_0 Maximum number of elements the buffer can hold. This must be greater than 0.
 * For example if you want a 5 element Ring Buffer of uint16_t values, @ref element_size_0 would be 2 and 
 * @ref number_of_elements_0 would be 5.
 * 
 * @return True if successful. False if unsuccessful. An unsuccessful attempt will occur if invalid arguments
 * were supplied or the requested buffer was too large to store. To avoid Dynamic Memory Allocation each Ring
 * Buffer has a fixed size. The requested storage cannot be greater than this fixed size.
 */
bool Ring_Buffer_Static_Ctor(Ring_Buffer_Static_Handle * me, size_t element_size_0, uint32_t number_of_elements_0);


/**
 * @brief Ring Buffer Handle Destructor. Frees the Ring Buffer that was allocated to the Handle.
 * 
 * @param me Ring Buffer Handle. Constructor must have been successfully called on this Handle to
 * use it in this function.
 * 
 * @return True if successful. False if unsuccessful. An unsuccessful attempt will occur if the
 * supplied Handle is not valid.
 */
bool Ring_Buffer_Static_Destroy(const Ring_Buffer_Static_Handle * me);


/**
 * @brief Clears the contents of the Ring Buffer and resets HEAD and TAIL to 0. Note that this only
 * resets the Ring Buffer so the Handle is still usable. If it is desired to completely destroy the
 * Ring Buffer the Destroy function should be called instead.
 * 
 * @param me Ring Buffer Handle. Constructor must have been successfully called on this Handle to
 * use it in this function. Note that the Handle will still be valid after this function call.
 * 
 * @return True if successful. False if unsuccessful. An unsuccessful attempt will occur if the
 * supplied Handle is not valid.
 */
bool Ring_Buffer_Static_Clear(const Ring_Buffer_Static_Handle * me);


/**
 * @brief Writes data BY VALUE to the Ring Buffer and increments HEAD.
 * 
 * @attention This function will directly copy memory into the Buffer. The @ref data memory block must
 * be equal to the number of bytes specified in @ref data_size otherwise behavior is undefined.
 * 
 * @param me Ring Buffer Handle. Constructor must have been successfully called on this Handle to
 * use it in this function.
 * @param data The starting memory block of data to write to the Buffer. This function will copy the memory
 * contents to perform a write by value.
 * @param data_size Number of Bytes being written. This must equal @ref element_size_0 specified in the
 * Constructor when the Object was initialized.
 * 
 * @return True if the Write was successful. False if unsuccessful. An unsuccessful write will occur if
 * the Buffer is Full, an invalid Handle is supplied, a NULL data pointer is supplied, or the specified 
 * data size is not equal to @ref element_size_0 specified when the Constructor was called.
 */
bool Ring_Buffer_Static_Write(const Ring_Buffer_Static_Handle * me, const void * data, size_t data_size);


/**
 * @brief Reads data from the Ring Buffer, increments TAIL, and updates empty status.
 * 
 * @attention This function will directly write to the @ref data memory block reserved for the read.
 * This block must equal the number of bytes specified in @ref data_size otherwise behavior is undefined.
 * 
 * @param me Ring Buffer Handle. Constructor must have been successfully called on this Handle to
 * use it in this function.
 * @param data The starting memory block to store data in.
 * @param data_size Number of Bytes being read. This must equal @ref element_size_0 specified in the
 * Constructor when the Object was initialized.
 * 
 * @return True if the Read was successful. False if unsuccessful. An unsuccessful read will occur if
 * the Buffer is Empty, an invalid Handle is supplied, a NULL data pointer is supplied, or the specified 
 * data size is not equal to @ref element_size_0 specified when the Constructor was called.
 */
bool Ring_Buffer_Static_Read(const Ring_Buffer_Static_Handle * me, void * data, size_t data_size);


/**
 * @brief Returns the number of elements CURRENTLY stored in the Ring Buffer. This is different
 * from the maximum number of elements (number_of_elements_0) initialized in the Constructor.
 * 
 * @param me Ring Buffer Handle. Constructor must have been successfully called on this Handle to
 * use it in this function.
 * 
 * @return Returns 0 if the Ring Buffer is currently empty or an invalid Handle was supplied.
 * Otherwise returns the number of elements.
 */
uint32_t Ring_Buffer_Static_Get_Number_Of_Elements(const Ring_Buffer_Static_Handle * me);


/**
 * @brief Returns if the Ring Buffer is Empty. Empty means there are no elements in
 * the Buffer. Reads cannot be performed if Empty.
 * 
 * @param me Ring Buffer Handle. Constructor must have been successfully called on this Handle to
 * use it in this function.
 * 
 * @return True if Empty. False if not Empty or an invalid Handle was supplied.
 */
bool Ring_Buffer_Static_Is_Empty(const Ring_Buffer_Static_Handle * me);


/**
 * @brief Returns if the Ring Buffer is Full. Writes cannot be performed if Full.
 * 
 * @param me Ring Buffer Handle. Constructor must have been successfully called on this Handle to
 * use it in this function.
 * 
 * @return True if Full or an invalid Handle was supplied. False otherwise.
 */
bool Ring_Buffer_Static_Is_Full(const Ring_Buffer_Static_Handle * me);



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------ ONLY FOR UNIT TESTS. DO NOT USE IN APPLICATION ---------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/
#if defined(APPLICATION_UNIT_TEST_)

    /**
     * @brief The Unit Test Memory Region used to verify no out-of-bounds memory access occurs.
     * This stores all pre-allocated Ring Buffer Objects in the middle and is surrounded by 
     * RB_INSTANCES_MEMORY_EXTENSION_BYTES of known values. For example if 
     * RB_INSTANCES_MEMORY_EXTENSION_BYTES is 1000, then Test_RB_Instances_Memory_Region[] would be: 
     * [1000 Bytes Known Values, RB_Instances[] Objects, 1000 Bytes Known Values]
     * 
     * @note Only a pointer is declared since the size of this array must be defined in the 
     * Source File to keep the Ring Buffer type encapsulated and keep the Source Code consistent.
     * I.e. Only one Ring Buffer type is defined so if it ever changes these values would 
     * automatically update with no intervention required and the type will remain encapsulated.
     */
    extern uint8_t Test_RB_Instances_Memory_Region[];


    /**
     * @brief The number of Bytes to pre and postpend Test_RB_Instances_Memory_Region[] by.
     * For example if this is 1000, then Test_RB_Instances_Memory_Region[] would be: 
     * [1000 Bytes Known Values, RB_Instances[] Objects, 1000 Bytes Known Values]
     */
    #define RB_INSTANCES_MEMORY_EXTENSION_BYTES                                         1000


    /**
     * @brief Number of Bytes Test_RB_Instances_Memory_Region[] is.  
     */
    extern const size_t Test_RB_Instances_Mem_Size;


    /**
     * @brief The Unit Test Memory Region used to verify no out-of-bounds memory access occurs.
     * This stores the statuses of whether each Ring Buffer Object is free or in use. These 
     * statuses are stored in the middle and are surrounded by RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES 
     * of known values. For example if RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES is 50, then 
     * Test_RB_Instances_In_Use_Memory_Region[] would be: 
     * [50 Bytes Known Values, RB_Instances_In_Use[], 50 Bytes Known Values]
     */
    extern uint8_t Test_RB_Instances_In_Use_Memory_Region[];

    /**
     * @brief The number of Bytes to pre and postpend Test_RB_Instances_In_Use_Memory_Region[] by.
     * For example if this is 50, then Test_RB_Instances_In_Use_Memory_Region[] would be: 
     * [50 Bytes Known Values, RB_Instances_In_Use[], 50 Bytes Known Values]
     */
    #define RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES                                  50


    /**
     * @brief Number of Bytes Test_RB_Instances_In_Use_Memory_Region[] is.  
     */
    extern const size_t Test_RB_Instances_In_Use_Mem_Size;

#endif /* APPLICATION_UNIT_TEST_ */


#endif /* RING_BUFFER_STATIC_H_ */
