/**
 * @file ring_buffer_static.c
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


/* Translation Unit */
#include "ring_buffer_static.h"

/* STD-C Libraries. */
#include <string.h>     /* size_t, memcpy */



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ RING BUFFER CLASS DEFINITION. NOTE HOW THIS IS COMPLETELY PRIVATE --------------------------*/
/*---------------------------- AND ENCAPSULATED FROM THE APPLICATION. ALL USERS GET IS A CLASS HANDLE. ----------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The Ring Buffer Object. Note how this is defined in the Source File so it is completely
 * encapsulated and private from the external Application.
 */
struct Ring_Buffer_t
{
    Ring_Buffer_Static_Handle * handle;         /* Handle using the Ring Buffer. Address comparison ensures multiple Handles can't use the same Ring Buffer. */
    uint8_t buffer[RING_BUFFER_STATIC_SIZE];
    volatile size_t head;
    volatile size_t tail;
    size_t element_size;                        /* Number of Bytes */
    size_t capacity;                            /* Number of Bytes */
    bool is_empty;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------- AVAILABLE RING BUFFERS FOR USE IN THE APPLICATION. NOTICE HOW THIS MEMORY ALLOCATION ----------------*/
/*---------------------------- IS DEFINED AT COMPILE-TIME SO WE CAN AVOID DYNAMIC MEMORY ALLOCATION. ------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#if defined(APPLICATION_UNIT_TEST_)
    /**
     * These Unit Test Memory Regions need the Ring_Buffer_t type in order to be defined.
     * It is done this way instead of exposing the Ring_Buffer_t type in the Header File
     * in order to keep the Source Code consistent and the type encapsulated. 
     */

    uint8_t Test_RB_Instances_Memory_Region[(RB_INSTANCES_MEMORY_EXTENSION_BYTES) + \
                                            (NUMBER_OF_STATIC_RING_BUFFERS * sizeof(struct Ring_Buffer_t)) + \
                                            (RB_INSTANCES_MEMORY_EXTENSION_BYTES)];

    uint8_t Test_RB_Instances_In_Use_Memory_Region[(RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES) + \
                                                    (NUMBER_OF_STATIC_RING_BUFFERS * sizeof(bool)) + \
                                                    (RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES)];

    const size_t Test_RB_Instances_Mem_Size         = sizeof(Test_RB_Instances_Memory_Region);
    const size_t Test_RB_Instances_In_Use_Mem_Size  = sizeof(Test_RB_Instances_In_Use_Memory_Region);


    /**
     * @brief Used by Unit Tests to verify no out-of-bounds memory access occurs. This is the
     * same pre-allocated Pool of Ring Buffers available to the Application and behaves exactly
     * the same as the normal version. The only difference is that this is stored in the middle
     * of Test_RB_Instances_Memory_Region[]. I.e.
     * 
     * Unit Test: Test_RB_Instances_Memory_Region[] = [Known Pad Bytes, RB_Instances[], Known Pad Bytes]
     * Normal Application: RB_Instances[]
     */
    static struct Ring_Buffer_t * const RB_Instances = (struct Ring_Buffer_t *)&Test_RB_Instances_Memory_Region[RB_INSTANCES_MEMORY_EXTENSION_BYTES];


    /**
     * @brief Used by Unit Tests to verify no out-of-bounds memory access occurs. This is the
     * same array that stores whether each Ring Buffer is in use and behaves exactly the same as
     * the normal version. The only difference is that this is stored in the middle
     * of Test_RB_Instances_In_Use_Memory_Region[]. I.e.
     * 
     * Unit Test: Test_RB_Instances_In_Use_Memory_Region[] = [Known Pad Bytes, RB_Instances_In_Use[], Known Pad Bytes]
     * Normal Application: RB_Instances_In_Use[]
     */
    static bool * const RB_Instances_In_Use = (bool *)&Test_RB_Instances_In_Use_Memory_Region[RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES];

#else /* Normal Application */

    /**
     * @brief The pre-allocated Pool of Ring Buffers available to the Application. Each array index corresponds 
     * to a unique Ring Buffer. Notice that memory is allocated for this at compile-time to avoid Dynamic Memory 
     * Allocation. When the Constructor is called this Pool is scanned. If there is an available Ring Buffer it 
     * will be reserved for the Caller and will be represented by a generic Ring Buffer Handle, which is the index
     * in this array containing the reserved Ring Buffer.
     */
    static struct Ring_Buffer_t RB_Instances[NUMBER_OF_STATIC_RING_BUFFERS];


    /**
     * @brief Stores whether each Ring Buffer is available or free for use. A true element means that the Ring
     * Buffer is in use. A false element means that Ring Buffer is free.
     */
    static bool RB_Instances_In_Use[NUMBER_OF_STATIC_RING_BUFFERS];

#endif



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ HELPER FUNCTIONS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Returns if the supplied Ring Buffer Handle (object) is valid. Valid means that the 
 * Ring Buffer Handle was initialized successfully using the Constructor. 
 * 
 * @param me Ring Buffer Handle to check.
 * 
 * @return True if the Handle is valid. False if the Handle is invalid. No Ring Buffer methods should
 * execute for an invalid Handle.
 */
static inline bool Is_Valid_Handle(const Ring_Buffer_Static_Handle * me);
static inline bool Is_Valid_Handle(const Ring_Buffer_Static_Handle * me)
{
    /**
     * Note that the evaluation order DOES matter to avoid dereferencing NULL pointer
     * or accessing out-of-bounds memory.
     */
    return ((me) && (*me < NUMBER_OF_STATIC_RING_BUFFERS) && (RB_Instances_In_Use[(*me)]) && (RB_Instances[(*me)].handle == me));
}



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

bool Ring_Buffer_Static_Ctor(Ring_Buffer_Static_Handle * me, size_t element_size_0, uint32_t number_of_elements_0)
{
    bool success = false;

    if (Is_Valid_Handle(me))
    {
        /* Constructor already called on Handle. Return false. */
    }
    else
    {
        if ((me) && (element_size_0) && (number_of_elements_0) && (element_size_0 * number_of_elements_0 <= RING_BUFFER_STATIC_SIZE))
        {
            for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
            {
                if (!RB_Instances_In_Use[i])
                {
                    *me = i;
                    RB_Instances[i].handle = me;
                    RB_Instances[i].head = 0;
                    RB_Instances[i].tail = 0;
                    RB_Instances[i].element_size = element_size_0;
                    RB_Instances[i].capacity = element_size_0 * number_of_elements_0;
                    RB_Instances[i].is_empty = true;
                    RB_Instances_In_Use[i] = true;
                    success = true;
                    break;
                }
            }
        }
    }

    return success;
}


bool Ring_Buffer_Static_Destroy(const Ring_Buffer_Static_Handle * me)
{
    bool success = Ring_Buffer_Static_Clear(me);

    if (success)
    {
        /* Free allocated Buffer and reset Handle address that the Buffer was reserved for. */
        RB_Instances[(*me)].handle = (Ring_Buffer_Static_Handle *)0;
        RB_Instances_In_Use[(*me)] = false;
    }

    return success;
}


bool Ring_Buffer_Static_Clear(const Ring_Buffer_Static_Handle * me)
{
    bool success = false;

    if (Is_Valid_Handle(me))
    {
        memset((void *)RB_Instances[(*me)].buffer, 0, RING_BUFFER_STATIC_SIZE);
        RB_Instances[(*me)].head = 0;
        RB_Instances[(*me)].tail = 0;
        RB_Instances[(*me)].is_empty = true;
        success = true;
    }

    return success;
}


bool Ring_Buffer_Static_Write(const Ring_Buffer_Static_Handle * me, const void * data, size_t data_size)
{
    bool success = false;

    /**
     * data_size is checked to ensure Ring Buffer consists of the same types. Also note
     * that Is_Full also calls Is_Valid_Handle() since this is a Public Method. If further 
     * optimization is needed then the body of Is_Full can be copied into this function.
     */
    if (Is_Valid_Handle(me) && (!Ring_Buffer_Static_Is_Full(me)) && (data) && (data_size == RB_Instances[(*me)].element_size))
    {
        /* Copy data directly from memory so it is passed BY VALUE. */
        memcpy((void *)(RB_Instances[(*me)].buffer + RB_Instances[(*me)].head), data, data_size);
        RB_Instances[(*me)].head = (RB_Instances[(*me)].head + data_size) % RB_Instances[(*me)].capacity;
        RB_Instances[(*me)].is_empty = false;
        success = true;
    }

    return success;
}


bool Ring_Buffer_Static_Read(const Ring_Buffer_Static_Handle * me, void * data, size_t data_size)
{
    bool success = false;

    /**
     * data_size is checked to ensure Ring Buffer consists of the same types. Also note
     * that Is_Empty also calls Is_Valid_Handle() since this is a Public Method. If further 
     * optimization is needed then the body of Is_Empty can be copied into this function.
     */
    if (Is_Valid_Handle(me) && !Ring_Buffer_Static_Is_Empty(me) && (data) && (data_size == RB_Instances[(*me)].element_size))
    {
        /* Copy data directly from memory so it is passed BY VALUE. */
        memcpy(data, (const void *)(RB_Instances[(*me)].buffer + RB_Instances[(*me)].tail), data_size);
        RB_Instances[(*me)].tail = (RB_Instances[(*me)].tail + data_size) % (RB_Instances[(*me)].capacity);

        if (RB_Instances[(*me)].head == RB_Instances[(*me)].tail) 
        {
            RB_Instances[(*me)].is_empty = true;
        }

        success = true;
    }

    return success;
}


uint32_t Ring_Buffer_Static_Get_Number_Of_Elements(const Ring_Buffer_Static_Handle * me)
{
    uint32_t num_of_elements = 0;

    if (Is_Valid_Handle(me))
    {
        /**
         * Note that Is_Full and Is_Empty also call Is_Valid_Handle() since these are Public
         * Methods. If further optimization is needed then the body of Is_Full and Is_Empty 
         * can be copied into this function.
         */
        if (Ring_Buffer_Static_Is_Full(me))
        {
            num_of_elements = RB_Instances[(*me)].capacity;
        }
        else if (Ring_Buffer_Static_Is_Empty(me))
        {
            num_of_elements = 0;
        }
        else
        {
            num_of_elements = RB_Instances[(*me)].head / RB_Instances[(*me)].element_size;
        }
    }

    return num_of_elements;
}


bool Ring_Buffer_Static_Is_Empty(const Ring_Buffer_Static_Handle * me)
{
    bool empty = false;

    if (Is_Valid_Handle(me))
    {
        empty = RB_Instances[(*me)].is_empty;
    }

    return empty;
}


bool Ring_Buffer_Static_Is_Full(const Ring_Buffer_Static_Handle * me)
{
    bool full = true;
    
    if (Is_Valid_Handle(me))
    {
        full = (RB_Instances[(*me)].head == RB_Instances[(*me)].tail) && !(RB_Instances[(*me)].is_empty);
    }

    return full;
}
