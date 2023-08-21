#warning "TODO: DESCRIPTION"
/**
 * @file ir_ring_buffer_static.c
 * @author Ian Ress
 * @brief NO DYNAMIC MEMORY ALLOCATION. BY VALUE. NOT MEANT FOR DEVICES WITH LITTLE RAM. CURRENTLY UESE 880 BYTES FOR\
 * STATIC STORAGE.
 * @version 0.1
 * @date 2023-08-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "ir_ring_buffer_static.h"
#include <stdlib.h>
#include <string.h>

#warning "TODO: DESCRIPTION"
#define NUMBER_OF_RING_BUFFERS                                  4

/* Each buffer allocated this nubmer of bytes. If a ring buffer is reserved that uses a buffer size
less than this than 200 bytes are still reserved and the extras are unused. */
#define BUFFER_SIZE                                             200


#warning "TODO: DESCRIPTION"
struct Ring_Buffer_t
{
    uint8_t buffer[BUFFER_SIZE];
    volatile size_t head;
    volatile size_t tail;
    size_t element_size; /* Number of Bytes */
    size_t capacity; /* Number of Bytes */
    bool is_empty;
};


#warning "TODO: DESCRIPTION"
static struct Ring_Buffer_t RB_Instances[NUMBER_OF_RING_BUFFERS];
static bool RB_Instances_In_Use[NUMBER_OF_RING_BUFFERS];



bool Ring_Buffer_Static_Ctor(Ring_Buffer_Static_Handle * me, size_t element_size_0, uint32_t number_of_elements_0)
{
    bool success = false;

    if ((element_size_0) && (number_of_elements_0 > 1) && (element_size_0 * number_of_elements_0 <= BUFFER_SIZE))
    {
        for (uint32_t i = 0; i < NUMBER_OF_RING_BUFFERS; i++)
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
    if (me < NUMBER_OF_RING_BUFFERS)
    {
        RB_Instances_In_Use[me] = false;
    }   
}


bool Ring_Buffer_Static_Write(Ring_Buffer_Static_Handle me, const void * data, size_t data_size)
{
    bool success = false;

    /* data_size is checked to ensure Ring Buffer consists of the same types */
    if ((me < NUMBER_OF_RING_BUFFERS) && !Ring_Buffer_Static_Is_Full(me) && RB_Instances_In_Use[me] && (data) && (data_size == RB_Instances[me].element_size))
    {
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
    if ((me < NUMBER_OF_RING_BUFFERS) && !Ring_Buffer_Static_Is_Empty(me) && (RB_Instances_In_Use[me]) && (data) && (data_size == RB_Instances[me].element_size))
    {
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

    if (me < NUMBER_OF_RING_BUFFERS)
    {
        empty = RB_Instances[me].is_empty;
    }

    return empty;
}


bool Ring_Buffer_Static_Is_Full(Ring_Buffer_Static_Handle me)
{
    bool full = true;

    if (me < NUMBER_OF_RING_BUFFERS)
    {
        full = (RB_Instances[me].head == RB_Instances[me].tail) && !(RB_Instances[me].is_empty);
    }

    return full;
}
