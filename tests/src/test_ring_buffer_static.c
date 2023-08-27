/**
 * @file test_ring_buffer_static.c
 * @author Ian Ress
 * @brief Unit Tests for Ring Buffer Static Module.
 * @version 0.1
 * @date 2023-08-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "unity.h"
#include "ring_buffer_static.h"

static Ring_Buffer_Static_Handle me;
static size_t element_size = 0;
static uint32_t number_of_elements = 0;


void setUp(void) 
{
    // set stuff up here
}

void tearDown(void) 
{
    // clean stuff up here
}


/**
 * @brief Tests to verify that the returned Ring Buffer Handle is invalid. Note that
 * if the Handle is ever greater than 4 bytes the Test will automatically fail.
 * 
 * @param me Ring Buffer Handle edited by the Constructor.
 */
static inline void Test_Ring_Buffer_Static_Handle_Invalid(Ring_Buffer_Static_Handle me);
static inline void Test_Ring_Buffer_Static_Handle_Invalid(Ring_Buffer_Static_Handle me)
{
   if (sizeof(me) == 1)
   {
      TEST_ASSERT_GREATER_THAN_UINT8(NUMBER_OF_STATIC_RING_BUFFERS, me);
   }
   else if (sizeof(me) == 2)
   {
      TEST_ASSERT_GREATER_THAN_UINT16(NUMBER_OF_STATIC_RING_BUFFERS, me);
   }
   else if (sizeof(me) == 4)
   {
      TEST_ASSERT_GREATER_THAN_UINT32(NUMBER_OF_STATIC_RING_BUFFERS, me);
   }
   else
   {
      TEST_ASSERT_FALSE(true);
   }
}


/**
 * @brief Tests to verify that the returned Ring Buffer Handle is valid. Note that
 * if the Handle is ever greater than 4 bytes the Test will automatically fail.
 * 
 * @param me Ring Buffer Handle edited by the Constructor.
 */
static inline void Test_Ring_Buffer_Static_Handle_Valid(Ring_Buffer_Static_Handle me);
static inline void Test_Ring_Buffer_Static_Handle_Valid(Ring_Buffer_Static_Handle me)
{
   if (sizeof(me) == 1)
   {
      TEST_ASSERT_LESS_OR_EQUAL_UINT8(NUMBER_OF_STATIC_RING_BUFFERS, me);
   }
   else if (sizeof(me) == 2)
   {
      TEST_ASSERT_LESS_OR_EQUAL_UINT16(NUMBER_OF_STATIC_RING_BUFFERS, me);
   }
   else if (sizeof(me) == 4)
   {
      TEST_ASSERT_LESS_OR_EQUAL_UINT32(NUMBER_OF_STATIC_RING_BUFFERS, me);
   }
   else
   {
      TEST_ASSERT_FALSE(true);
   }
}


void test_Ring_Buffer_Static_Ctor(void) 
{
   /*----------------------------- Invalid Argument - NULL pointer supplied. ---------------------------*/
   element_size = 5;
   number_of_elements = 5;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor((Ring_Buffer_Static_Handle *)0, element_size, number_of_elements));

   /*------------------------------ Invalid Argument - Element Size = 0. -------------------------------*/
   element_size = 0;
   number_of_elements = 5;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&me, element_size, number_of_elements));
   Test_Ring_Buffer_Static_Handle_Invalid(me);

   /*------------------------- Invalid Argument - Number of Elements less than 2. ----------------------*/
   element_size = 1;
   number_of_elements = 0;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&me, element_size, number_of_elements));
   Test_Ring_Buffer_Static_Handle_Invalid(me);
   
   element_size = 1;
   number_of_elements = 1;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&me, element_size, number_of_elements));
   Test_Ring_Buffer_Static_Handle_Invalid(me);
   
   /*-------------------------------- Invalid Argument - Too large storage. ----------------------------*/
   element_size = (RING_BUFFER_STATIC_SIZE / 2) + 2;
   number_of_elements = 2;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&me, element_size, number_of_elements));
   Test_Ring_Buffer_Static_Handle_Invalid(me);

   element_size = RING_BUFFER_STATIC_SIZE + 1;
   number_of_elements = 1;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&me, element_size, number_of_elements));
   Test_Ring_Buffer_Static_Handle_Invalid(me);

   /*------------------------------ TODO: HAVE TO TEST IF ALL RING BUFFERS ARE BEING USED --------------*/
}



int main(void) 
{
   UNITY_BEGIN();
   RUN_TEST(test_Ring_Buffer_Static_Ctor);
   return UNITY_END();
}