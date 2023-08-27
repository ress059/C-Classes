/**
 * @file ring_buffer_static.c
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
 * 
 */
struct Ring_Buffer_t
{
    uint8_t buffer[RING_BUFFER_STATIC_SIZE];
    volatile size_t head;
    volatile size_t tail;
    size_t element_size;    /* Number of Bytes */
    size_t capacity;        /* Number of Bytes */
    bool is_empty;
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*--------------------- AVAILABLE RING BUFFERS FOR USE IN THE APPLICATION. NOTICE HOW THIS MEMORY ALLOCATION ----------------*/
/*---------------------------- IS DEFINED AT COMPILE-TIME SO WE CAN AVOID DYNAMIC MEMORY ALLOCATION. ------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The Pool of Ring Buffers available to the Application. Each array index corresponds to a unique
 * Ring Buffer. Notice that memory is allocated for this at compile-time to avoid Dynamic Memory Allocation. 
 * When the Constructor is called this Pool is scanned. If there is an available Ring Buffer it will be 
 * reserved for the Caller and will be represented by a generic Ring Buffer Handle, which is the index
 * in this array containing the reserved Ring Buffer.
 * 
 */
static struct Ring_Buffer_t RB_Instances[NUMBER_OF_STATIC_RING_BUFFERS];


/**
 * @brief Stores whether each Ring Buffer is available or free for use. A true element means that the Ring
 * Buffer is in use. A false element means that Ring Buffer is free.
 * 
 */
static bool RB_Instances_In_Use[NUMBER_OF_STATIC_RING_BUFFERS];



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS ---------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

bool Ring_Buffer_Static_Ctor(Ring_Buffer_Static_Handle * me, size_t element_size_0, uint32_t number_of_elements_0)
{
    bool success = false;

    if (me)
    {
        /**
         * Initially set to out-of-range value as extra precaution. If Ctor fails, 
         * an out-of-range Handle will not run any other Methods.
         */
        *me = NUMBER_OF_STATIC_RING_BUFFERS + 1;
    }

    if ((me) && (element_size_0) && (number_of_elements_0 > 1) && (element_size_0 * number_of_elements_0 <= RING_BUFFER_STATIC_SIZE))
    {
        for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
        {
            if (!RB_Instances_In_Use[i])
            {
                *me = i;
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
    return success;
}


void Ring_Buffer_Static_Destroy(Ring_Buffer_Static_Handle me)
{
    if (me < NUMBER_OF_STATIC_RING_BUFFERS)
    {
        RB_Instances_In_Use[me] = false;
    }   
}


void Ring_Buffer_Static_Reset(Ring_Buffer_Static_Handle me)
{
    if (me < NUMBER_OF_STATIC_RING_BUFFERS)
    {
        memset((void *)RB_Instances[me].buffer, 0, RING_BUFFER_STATIC_SIZE);
        RB_Instances[me].head = 0;
        RB_Instances[me].tail = 0;
        RB_Instances[me].is_empty = true;
    }
}


uint32_t Ring_Buffer_Static_Get_Number_Of_Elements(Ring_Buffer_Static_Handle me)
{
    uint32_t num_of_elements = 0;

    if ((me < NUMBER_OF_STATIC_RING_BUFFERS) && (RB_Instances_In_Use[me]))
    {
        if (Ring_Buffer_Static_Is_Full(me))
        {
            num_of_elements = RB_Instances[me].capacity;
        }
        else if (Ring_Buffer_Static_Is_Empty(me))
        {
            num_of_elements = 0;
        }
        else
        {
            num_of_elements = RB_Instances[me].head / RB_Instances[me].element_size;
        }
    }

    return num_of_elements;
}


bool Ring_Buffer_Static_Write(Ring_Buffer_Static_Handle me, const void * data, size_t data_size)
{
    bool success = false;

    /* data_size is checked to ensure Ring Buffer consists of the same types */
    if ((me < NUMBER_OF_STATIC_RING_BUFFERS) && (!Ring_Buffer_Static_Is_Full(me)) && (RB_Instances_In_Use[me]) && (data) && (data_size == RB_Instances[me].element_size))
    {
        /* Copy data directly from memory so it is passed BY VALUE. */
        memcpy((void *)(RB_Instances[me].buffer + RB_Instances[me].head), data, data_size);
        RB_Instances[me].head = (RB_Instances[me].head + data_size) % RB_Instances[me].capacity;
        RB_Instances[me].is_empty = false;
        success = true;
    }

    return success;
}


bool Ring_Buffer_Static_Read(Ring_Buffer_Static_Handle me, void * data, size_t data_size)
{
    bool success = false;

    /* data_size is checked to ensure Ring Buffer consists of the same types */
    if ((me < NUMBER_OF_STATIC_RING_BUFFERS) && !Ring_Buffer_Static_Is_Empty(me) && (RB_Instances_In_Use[me]) && (data) && (data_size == RB_Instances[me].element_size))
    {
        /* Copy data directly from memory so it is passed BY VALUE. */
        memcpy(data, (const void *)(RB_Instances[me].buffer + RB_Instances[me].tail), data_size);
        RB_Instances[me].tail = (RB_Instances[me].tail + data_size) % (RB_Instances[me].capacity);

        if (RB_Instances[me].head == RB_Instances[me].tail) 
        {
            RB_Instances[me].is_empty = true;
        }

        success = true;
    }

    return success;
}


bool Ring_Buffer_Static_Is_Empty(Ring_Buffer_Static_Handle me)
{
    bool empty = false;

    if (me < NUMBER_OF_STATIC_RING_BUFFERS)
    {
        empty = RB_Instances[me].is_empty;
    }

    return empty;
}


bool Ring_Buffer_Static_Is_Full(Ring_Buffer_Static_Handle me)
{
    bool full = true;

    if (me < NUMBER_OF_STATIC_RING_BUFFERS)
    {
        full = (RB_Instances[me].head == RB_Instances[me].tail) && !(RB_Instances[me].is_empty);
    }

    return full;
}
