#warning "TODO: DESCRIPTION"
/**
 * @file ir_ring_buffer_static.h
 * @author Ian Ress
 * @brief NO DYNAMIC MEMORY ALLOCATION. BY VALUE. NOT MEANT FOR DEVICES WITH LITTLE RAM. CURRENTLY UESE 880 BYTES FOR\
 * STATIC STORAGE.
 * @version 0.1
 * @date 2023-08-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef IR_RING_BUFFER_STATIC_H_
#define IR_RING_BUFFER_STATIC_H_

#include <stdbool.h>
#include <stdint.h>


typedef uint32_t Ring_Buffer_Static_Handle;



bool Ring_Buffer_Static_Ctor(Ring_Buffer_Static_Handle * me, size_t element_size_0, uint32_t number_of_elements_0);


void Ring_Buffer_Static_Destroy(Ring_Buffer_Static_Handle me);


bool Ring_Buffer_Static_Write(Ring_Buffer_Static_Handle me, const void * data, size_t data_size);


bool Ring_Buffer_Static_Read(Ring_Buffer_Static_Handle me, void * data, size_t data_size);

/* false is invalid handle supplied */
bool Ring_Buffer_Static_Is_Empty(Ring_Buffer_Static_Handle me);

/* true if invalid handle stupplied*/
bool Ring_Buffer_Static_Is_Full(Ring_Buffer_Static_Handle me);


#endif /* IR_RING_BUFFER_STATIC_H_ */
