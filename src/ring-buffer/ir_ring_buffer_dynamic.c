#warning "TODO: DESCRIPTION"
/**
 * @file ir_ring_buffer_dynamic.c
 * @author Ian Ress
 * @brief BY VALUE
 * @version 0.1
 * @date 2023-08-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "ir_ring_buffer_dynamic.h"
#include <stdlib.h>
#include <string.h>


#warning "TODO: DESCRIPTION"
struct Ring_Buffer_t
{
    uint8_t * buffer;
    volatile size_t head;
    volatile size_t tail;
    size_t element_size; /* Number of Bytes */
    size_t capacity; /* Number of Bytes */
    bool is_empty;
};



bool Ring_Buffer_Dynamic_Ctor(Ring_Buffer_Dynamic_Handle me, size_t element_size_0, uint32_t number_of_elements_0)
{
    bool success = false;

    #warning "Handle Overflow case of element_size_0 * number_of_elements_0"
    if ((element_size_0) && (number_of_elements_0 > 1) && (element_size_0 * number_of_elements_0 < SIZE_MAX))
    {
        me = malloc(sizeof(struct Ring_Buffer_t));

        if (me)
        {
            me->buffer = malloc((element_size_0 * number_of_elements_0));

            if (!me->buffer)
            {
                free(me);
            }

            else
            {
                me->head = 0;
                me->tail = 0;
                me->element_size = element_size_0;
                me->capacity = element_size_0 * number_of_elements_0;
                me->is_empty = true;
                success = true;
            }
        }        
    }

    return success;
}


void Ring_Buffer_Dynamic_Destroy(Ring_Buffer_Dynamic_Handle me)
{
    if (me)
    {
        free(me->buffer); /* free NULL checks me->buffer */
        free(me);
    }   
}


bool Ring_Buffer_Dynamic_Write(Ring_Buffer_Dynamic_Handle me, const void * data, size_t data_size)
{
    bool success = false;

    /* data_size is checked to ensure Ring Buffer consists of the same types */
    if ((me) && !Ring_Buffer_Dynamic_Is_Full(me) && (me->buffer) && (data) && (data_size == me->element_size))
    {
        memcpy((void *)(me->buffer + me->head), data, data_size);
        me->head = (me->head + data_size) % me->capacity;
        me->is_empty = false;
        success = true;
    }

    return success;
}


bool Ring_Buffer_Dynamic_Read(Ring_Buffer_Dynamic_Handle me, void * data, size_t data_size)
{
    bool success = false;

    /* data_size is checked to ensure Ring Buffer consists of the same types */
    if ((me) && !Ring_Buffer_Dynamic_Is_Empty(me) && (me->buffer) && (data) && (data_size == me->element_size))
    {
        memcpy(data, (const void *)(me->buffer + me->tail), data_size);
        me->tail = (me->tail + data_size) % (me->capacity);

        if (me->head == me->tail)
        {
            me->is_empty = true;
        }

        success = true;
    }

    return success;
}


bool Ring_Buffer_Dynamic_Is_Empty(Ring_Buffer_Dynamic_Handle me)
{
    bool empty = false;

    if (me)
    {
        empty = me->is_empty;
    }

    return empty;
}


bool Ring_Buffer_Dynamic_Is_Full(Ring_Buffer_Dynamic_Handle me)
{
    bool full = true;

    if (me)
    {
        full = (me->head == me->tail) && !(me->is_empty);
    }

    return full;
}
