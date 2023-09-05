/**
 * @file test_ring_buffer_static.c
 * @author Ian Ress
 * @brief Unit Tests for the Static Ring Buffer module which does not use Dynamic Memory Allocation. 
 * Each Ring Buffer has a fixed-size and an array of these Ring Buffers are initialized at compile-time.
 * This serves as a pool that the Application can reserve from. See the file description of ring_buffer_static.h/.c
 * for more details.
 * @version 0.1
 * @date 2023-08-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */


/* STD-C Libraries */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>     /* rand */
#include <string.h>     /* memset, size_t */
#include <time.h>       /* time */

/* Unit Test Framework */
#define UNITY_INCLUDE_PRINT_FORMATTED
#include "unity.h"

/* Module Under Test */
#include "ring_buffer_static.h"



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------- UNIT TEST SETUP VALUES ------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Used to verify the Ring Buffer Module under test does not access out-of-bounds memory
 * when editing the pre-allocated Ring Buffer Objects. This is the value stored in the pre and 
 * postpended bytes of Test_RB_Instances_Memory_Region[]. For example if this is 0x33 then 
 * Test_RB_Instances_Memory_Region[] would be: [0x33, 0x33,... RB_Instances[], 0x33,.. 0x33]
 * If out-of-bounds memory access occurred then some of the pre and postpended regions would be
 * overwritten with new values. We can easily check for this and throw an appropriate error.
 */
#define RB_INSTANCES_PREPOSTPEND_VALUES                           0x33


/**
 * @brief Used to verify the Ring Buffer Module under test does not access out-of-bounds memory
 * when editing the Ring Buffer status array. This is the value stored in the pre and postpended 
 * bytes of Test_RB_Instances_In_Use_Memory_Region[]. For example if this is 0x44 then 
 * Test_RB_Instances_In_Use_Memory_Region[] would be: [0x44, 0x44,... RB_Instances_In_Use[], 0x44,.. 0x44]
 * If out-of-bounds memory access occurred then some of the pre and postpended regions would be
 * overwritten with new values. We can easily check for this and throw an appropriate error.
 */
#define RB_INSTANCES_IN_USE_PREPOSTPEND_VALUES                    0x44


#define DATA_PREPOSTPEND_LENGTH                                   50
#define READ_DATA_PREPOSTPEND_VALUE                               0x67
#define WRITE_DATA_PREPOSTPEND_VALUE                              0x58



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------- RING BUFFER HANDLES FOR UNIT TESTS --------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Purely used for organization to easily test all Ring Buffers the Module Under Test 
 * pre-allocates. This is not the Ring Buffer Object which is private. This is the Handle that 
 * would be used in the normal Application and initialization members passed to the Constructor.
 */
typedef struct
{
   Ring_Buffer_Static_Handle me;       /* Public Handle used by normal Application. */
   size_t element_size;                /* element size passed to Constructor. */
   uint32_t number_of_elements;        /* number of elements passed to Constructor. */
} Test_Ring_Buffer_Static_Handle_t;


/**
 * @brief Collection of Test Ring Buffer Handles. Purely used for organization to easily 
 * test all Ring Buffers the Module Under Test pre-allocates. Notice that the number of
 * elements in this array equals the number of Ring Buffer Objects that the Module Under 
 * Test pre-allocates. Therefore if this value changes, this array will automatically
 * update and we will still be testing all the pre-allocated Ring Buffer Objects.
 */
static Test_Ring_Buffer_Static_Handle_t Test_Ring_Buffer_Handles[NUMBER_OF_STATIC_RING_BUFFERS];



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------- HELPERS -----------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The Ring Buffer Module Under Test pre-allocates Ring Buffer Objects for use at compile-time
 * to avoid Dynamic Memory Allocation. Since these are stored in an array, this function verifies 
 * no out-of-bounds memory access occurred while editing these Objects.
 */
static inline void Test_RB_Objects_Memory_Access(void);
static inline void Test_RB_Objects_Memory_Access(void)
{
   TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(RB_INSTANCES_PREPOSTPEND_VALUES, &Test_RB_Instances_Memory_Region[0], RB_INSTANCES_MEMORY_EXTENSION_BYTES,
                                       "Wrote to out-of-range memory when updating RB_Instances[]!");

   TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(RB_INSTANCES_PREPOSTPEND_VALUES, ((&Test_RB_Instances_Memory_Region[0]) + (Test_RB_Instances_Mem_Size - RB_INSTANCES_MEMORY_EXTENSION_BYTES)),
                                       RB_INSTANCES_MEMORY_EXTENSION_BYTES, "Wrote to out-of-range memory when updating RB_Instances[]!");

   TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(RB_INSTANCES_IN_USE_PREPOSTPEND_VALUES, &Test_RB_Instances_In_Use_Memory_Region[0], RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES,
                                       "Wrote to out-of-range memory when updating RB_Instances_In_Use[]!");

   TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(RB_INSTANCES_IN_USE_PREPOSTPEND_VALUES, ((&Test_RB_Instances_In_Use_Memory_Region[0]) + (Test_RB_Instances_In_Use_Mem_Size - RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES)),
                                       RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES, "Wrote to out-of-range memory when updating RB_Instances_In_Use[]!");
}



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------- RUNS AT THE START AND END OF EACH TEST --------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

void setUp(void) 
{
   /* Prepend test Memory Region that holds Ring Buffers with known values to test out-of-bounds access. */
   memset((void *)&Test_RB_Instances_Memory_Region[0], RB_INSTANCES_PREPOSTPEND_VALUES, RB_INSTANCES_MEMORY_EXTENSION_BYTES);
   memset((void *)&Test_RB_Instances_In_Use_Memory_Region[0], RB_INSTANCES_IN_USE_PREPOSTPEND_VALUES, RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES);

   /**
    * Postpend test Memory Region that holds Ring Buffers with known values to test out-of-bounds access.
    * We have to use mem size variables instead of sizeof because we are only given access to a pointer.
    */
   for (uint32_t i = (Test_RB_Instances_Mem_Size - 1); i > (Test_RB_Instances_Mem_Size - RB_INSTANCES_MEMORY_EXTENSION_BYTES - 1); i--)
   {
      Test_RB_Instances_Memory_Region[i] = RB_INSTANCES_PREPOSTPEND_VALUES;
   }
   for (uint32_t i = (Test_RB_Instances_In_Use_Mem_Size - 1); i > (Test_RB_Instances_In_Use_Mem_Size - RB_INSTANCES_IN_USE_MEMORY_EXTENSION_BYTES - 1); i--)
   {
      Test_RB_Instances_In_Use_Memory_Region[i] = RB_INSTANCES_IN_USE_PREPOSTPEND_VALUES;
   }

   /* Ensure all Ring Buffers supply valid values to the Constructor when called. */
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      Test_Ring_Buffer_Handles[i].element_size = 1;
      Test_Ring_Buffer_Handles[i].number_of_elements = RING_BUFFER_STATIC_SIZE;
   }
}

void tearDown(void) 
{
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      /**
       * Note that some of these Destroys will fail because we won't be calling the Constructor on all
       * of our Ring Buffers for each Test so we don't care about its output.
       */
      (void)Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&(Test_Ring_Buffer_Handles[i].me));
   }

   /* Clear our test Memory Regions. */
   memset((void *)&Test_RB_Instances_Memory_Region[0], 0, Test_RB_Instances_Mem_Size);
   memset((void *)&Test_RB_Instances_In_Use_Memory_Region[0], 0, Test_RB_Instances_In_Use_Mem_Size);
}



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- UNIT TESTS BEGIN -----------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------- Ring_Buffer_Static_Ctor() ------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Tests if the Ring Buffer Constructor fails when different invalid Input Parameters are fed into
 * it.
 */
static void Test_Ring_Buffer_Static_Ctor_Invalid_Parameters(void);
static void Test_Ring_Buffer_Static_Ctor_Invalid_Parameters(void)
{
   /*---------------------------------------------------------------------------------------------------*/
   /*----------------------------- Invalid Argument - NULL pointer supplied. ---------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor((Ring_Buffer_Static_Handle *)0, Test_Ring_Buffer_Handles[0].element_size, 
                                             Test_Ring_Buffer_Handles[0].number_of_elements));

   /*---------------------------------------------------------------------------------------------------*/
   /*------------------------------ Invalid Argument - Element Size = 0. -------------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_Ring_Buffer_Handles[0].element_size = 0;
   Test_Ring_Buffer_Handles[0].number_of_elements = 5;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[0].me, Test_Ring_Buffer_Handles[0].element_size, 
                                             Test_Ring_Buffer_Handles[0].number_of_elements));

   /*---------------------------------------------------------------------------------------------------*/
   /*---------------------------- Invalid Argument - Number of Elements = 0. ---------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_Ring_Buffer_Handles[0].element_size = 1;
   Test_Ring_Buffer_Handles[0].number_of_elements = 0;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[0].me, Test_Ring_Buffer_Handles[0].element_size, 
                                             Test_Ring_Buffer_Handles[0].number_of_elements));
   
   /*---------------------------------------------------------------------------------------------------*/
   /*-------------------------------- Invalid Argument - Too large storage. ----------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_Ring_Buffer_Handles[0].element_size = (RING_BUFFER_STATIC_SIZE / 2) + 2;
   Test_Ring_Buffer_Handles[0].number_of_elements = 2;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[0].me, Test_Ring_Buffer_Handles[0].element_size, 
                                             Test_Ring_Buffer_Handles[0].number_of_elements));

   Test_Ring_Buffer_Handles[0].element_size = RING_BUFFER_STATIC_SIZE + 1;
   Test_Ring_Buffer_Handles[0].number_of_elements = 1;
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[0].me, Test_Ring_Buffer_Handles[0].element_size, 
                                             Test_Ring_Buffer_Handles[0].number_of_elements));
}


/**
 * @brief To avoid Dynamic Memory Allocation, the Ring Buffer Module under test allocates memory 
 * for a fixed number of Buffers at compile-time. Once this pool of Ring Buffers are all in use, 
 * a new Instance cannot be constructed. Calling the Constructor on an already Initialized Instance
 * should also result in failure.
 */
static void Test_Ring_Buffer_Static_Ctor_Invalid_Handles(void);
static void Test_Ring_Buffer_Static_Ctor_Invalid_Handles(void)
{
   /* Extra Invalid Handle. Guarantee supplied Constructor parameters are always valid. */
   Ring_Buffer_Static_Handle valid_handle;
   size_t valid_element_size = 1;
   uint32_t valid_number_of_elements = RING_BUFFER_STATIC_SIZE;

   /*---------------------------------------------------------------------------------------------------*/
   /*------------ Call valid Constructor until all Ring Buffers are reserved. Verify calling -----------*/
   /*---------- Constructor afterwards is unsuccessful since no more Ring Buffers are available. -------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[i].me, Test_Ring_Buffer_Handles[i].element_size, 
                                                Test_Ring_Buffer_Handles[i].number_of_elements));
   }
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&valid_handle, valid_element_size, valid_number_of_elements));

   /*---------------------------------------------------------------------------------------------------*/
   /*--------- Verify calling Constructor on already Initialized Ring Buffers results in failures. -----*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[i].me, Test_Ring_Buffer_Handles[i].element_size, 
                                                Test_Ring_Buffer_Handles[i].number_of_elements));
   }
}


/**
 * @brief To avoid Dynamic Memory Allocation, the Ring Buffer Module Under Test allocates memory 
 * for a fixed number of Buffers at compile-time. Therefore the Module Under Test will be indexing
 * within this array when editing Ring Buffer Objects. This Test verifies out-of-bounds memory writes 
 * did not occur when accessing the first and last Ring Buffer Object in the array. Note that if 
 * out-of-bounds access occured in the middle of the array then the other Ring Buffers simply would 
 * not work so that is also easily verifiable in the remaining tests.
 */
static void Test_Ring_Buffer_Static_Ctor_Memory_Access(void);
static void Test_Ring_Buffer_Static_Ctor_Memory_Access(void)
{
   /*---------------------------------------------------------------------------------------------------*/
   /*------------ Call valid Constructor until all Ring Buffers are reserved. Verify Constructor -------*/
   /*------ did not write to out-of-bounds memory when Constructing the first and last Ring Buffer -----*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[i].me, Test_Ring_Buffer_Handles[i].element_size, 
                                                Test_Ring_Buffer_Handles[i].number_of_elements));
   }
   Test_RB_Objects_Memory_Access();
}



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------- Ring_Buffer_Static_Clear() ------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Ring Buffer Clear function clears the buffer's contents, and resets head/tail. Note
 * however that the Class Handle is still usable. We verify this Clear function can only be 
 * done on Handles that have been Initialized via the Constructor, and that the Handles are still
 * usable after the Clear function is called. We also verify this Clear function does not access 
 * out-of-bounds memory when clearing the buffer contents.
 */
static void Test_Ring_Buffer_Static_Clear(void);
static void Test_Ring_Buffer_Static_Clear(void)
{
   /* Extra Invalid Handle */
   Ring_Buffer_Static_Handle invalid_handle;

   /*---------------------------------------------------------------------------------------------------*/
   /*---------------------- Verify Clear fails since Constructor has not been called yet. --------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Clear((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[i].me));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*--------- Call Constructor. Unity setUp makes the input parameters to the Constructor valid. ------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[i].me, Test_Ring_Buffer_Handles[i].element_size, 
                                                Test_Ring_Buffer_Handles[i].number_of_elements));
   }
   
   /*---------------------------------------------------------------------------------------------------*/
   /*-------------- Verify Clear succeeds since Constructor has been successfully called on Handle. ----*/
   /*---------------- Verify Clear fails on invalid handle that was not passed in the Constructor. -----*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Clear((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[i].me));
   }
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Clear((const Ring_Buffer_Static_Handle *)&invalid_handle));

   /*---------------------------------------------------------------------------------------------------*/
   /*-------------------- Verify Handle can still be used after Clear function is called. --------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Clear((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[i].me));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*------------------------- Verify Clear did not access out-of-bounds memory. -----------------------*/
   /*--- Unity setUp prepends and postpends known values to the test memory region to test against. ----*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_RB_Objects_Memory_Access();
}



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- Ring_Buffer_Static_Destroy() ------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The Destructor clears the buffers's contents, resets head/tail, and frees the Ring Buffer
 * allocated to the Handle. We verify this Destroy function can only be done on Handles that have 
 * been Initialized via the Constructor, and that the Handles are unusable after they are Destroyed.
 * We also verify this Destructor does not access out-of-bounds memory.
 */
static void Test_Ring_Buffer_Static_Destroy(void);
static void Test_Ring_Buffer_Static_Destroy(void)
{
   /* Extra Invalid Handle */
   Ring_Buffer_Static_Handle invalid_handle;

   /*---------------------------------------------------------------------------------------------------*/
   /*---------------------- Verify Destroy fails since Constructor has not been called yet. ------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[i].me));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*--------- Call Constructor. Unity setUp makes the input parameters to the Constructor valid. ------*/
   /*---------------------------- Verify Destroy still fails on invalid handle. ------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[i].me, Test_Ring_Buffer_Handles[i].element_size, 
                                                Test_Ring_Buffer_Handles[i].number_of_elements));
   }
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&invalid_handle));

   /*---------------------------------------------------------------------------------------------------*/
   /*--------------------- Destroy 1st Handle and verify it is successful. Leave others. ---------------*/
   /*------------------------ Verify Destroy fails on Handle that was just destroyed. ------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   TEST_ASSERT_TRUE(Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[0].me));
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[0].me));

   /*---------------------------------------------------------------------------------------------------*/
   /*--------------------- Destroy remaining Handles and verify it is successful for all. --------------*/
   /*------------------------ Verify Destroy fails on Handle that was just destroyed. ------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 1; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[i].me));
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[i].me));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*-------------------- Verify Destroy function did not access out-of-bounds memory. -----------------*/
   /*--- Unity setUp prepends and postpends known values to the test memory region to test against. ----*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_RB_Objects_Memory_Access();
}


/**
 * @brief Continuation of Test_Ring_Buffer_Static_Destroy Test however the Constructor and Destructor 
 * are now repeatedly called. We verify all Constructor and Destructor calls are successful and no
 * out-of-bounds memory access occurred.
 */
static void Test_Ring_Buffer_Static_Ctor_And_Destroy(void);
static void Test_Ring_Buffer_Static_Ctor_And_Destroy(void)
{
   /* Number of times a Constructor and Destructor call executes. */
   uint32_t ctor_destroy_iterations = 10;

   /* Note that this is an array index so it will be from 0 to NUMBER_OF_STATIC_RING_BUFFERS - 1 */
   uint32_t random_buffer = rand() % NUMBER_OF_STATIC_RING_BUFFERS;

   /*---------------------------------------------------------------------------------------------------*/
   /*----------------------- Call valid Constructor on all available Ring Buffers. ---------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[i].me, Test_Ring_Buffer_Handles[i].element_size, 
                                                Test_Ring_Buffer_Handles[i].number_of_elements));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*------------ Repeatedly Destroy and Reconstruct the Ring Buffer that was randomly selected. -------*/
   /*----------------------------------- Verify it is successful everytime -----------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < ctor_destroy_iterations; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[random_buffer].me));
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[random_buffer].me, Test_Ring_Buffer_Handles[random_buffer].element_size, 
                                                Test_Ring_Buffer_Handles[random_buffer].number_of_elements));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*----- Repeatedly Destroy and Reconstruct all Ring Buffers. Verify it is successful everytime. -----*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < ctor_destroy_iterations; i++)
   {
      for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
      {
         TEST_ASSERT_TRUE(Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me));
         TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[buf].me, Test_Ring_Buffer_Handles[buf].element_size, 
                                                   Test_Ring_Buffer_Handles[buf].number_of_elements));
      }
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*----------------------------- Verify no out-of-bounds memory access occurred. ---------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_RB_Objects_Memory_Access();
}



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- Ring_Buffer_Static_Write() --------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The Ring Buffers can only be written to if the Handles were successfully Constructed,
 * the Buffer is not full, valid data is passed, and the specified write size is equal to the 
 * element size parameter specified in the Constructor. We verify Writes only occur in these 
 * scenarios, and that out-of-bounds memory access does not occur. Note that the memory region
 * tested is for the internal Ring Buffers, not the data we are writing from. I.e. we do not test 
 * the memory region surrounding 'dummy_value'. This is done in the Ring Buffer ReadWrite test.
 */
static void Test_Ring_Buffer_Static_Write(void);
static void Test_Ring_Buffer_Static_Write(void)
{
   /* Extra Invalid Handle */
   Ring_Buffer_Static_Handle invalid_handle;

   /* Dummy Values to write to the buffers. */
   uint16_t dummy_value = 50;

   /* Override input parameters set in Unity setUp with our own for this test. */
   size_t element_size_0 = sizeof(dummy_value);
   uint32_t number_of_elements_0 = 10;

   /*---------------------------------------------------------------------------------------------------*/
   /*------ Call Constructor on first Handle for these sets of Tests. We override input parameters -----*/ 
   /*-------------------------------- set in Unity setUp with our own. ---------------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[0].me, element_size_0, number_of_elements_0));

   /*---------------------------------------------------------------------------------------------------*/
   /*-------------------------------- Verify Write fails on invalid handle. ----------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&invalid_handle, 
                                             (const void *)&dummy_value, element_size_0));

   /*---------------------------------------------------------------------------------------------------*/
   /*--------------- Verify Write on NULL data pointer fails. All other parameters valid. --------------*/
   /*---------- NOTE FOR DEBUGGING: YOU WILL GET SEG FAULT WITH NO ERROR MESSAGE IF THIS FAILS ---------*/
   /*---------------------------------------------------------------------------------------------------*/
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[0].me, 
                                             (const void *)0, element_size_0));

   /*---------------------------------------------------------------------------------------------------*/
   /*----- Verify Write fails when data size input is different than element size specified in Ctor. ---*/
   /*---------------------------------------------------------------------------------------------------*/
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[0].me, 
                                             (const void *)&dummy_value, element_size_0 + 1));

   /*---------------------------------------------------------------------------------------------------*/
   /*---- Verify Write passes on first Buffer. Clear this Handle. Verify Write is still successful -----*/
   /*---------------------- since Handle is still usable after it is cleared. --------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   TEST_ASSERT_TRUE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[0].me, 
                                             (const void *)&dummy_value, element_size_0));

   TEST_ASSERT_TRUE(Ring_Buffer_Static_Clear((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[0].me));

   TEST_ASSERT_TRUE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[0].me, 
                                             (const void *)&dummy_value, element_size_0));

   /*---------------------------------------------------------------------------------------------------*/
   /*----------- Destroy the first Buffer Handle. Verify Write to it now fails since this Handle -------*/
   /*---------------- has been Destroyed. Construct all available Buffer Handles for next test. --------*/
   /*---------------------------------------------------------------------------------------------------*/
   TEST_ASSERT_TRUE(Ring_Buffer_Static_Destroy((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[0].me));

   TEST_ASSERT_FALSE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[0].me, 
                                             (const void *)&dummy_value, element_size_0));

   for (uint32_t i = 0; i < NUMBER_OF_STATIC_RING_BUFFERS; i++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[i].me, element_size_0, number_of_elements_0));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*------ Now pass in valid parameters. Verify Writes pass on all Buffers until they get full. -------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t i = 0; i < number_of_elements_0; i++)
   {
      for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
      {
         TEST_ASSERT_TRUE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, 
                                                   (const void *)&dummy_value, element_size_0));
      }
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*--------------------------- Verify Writes fail since buffers are now full. ------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, 
                                                (const void *)&dummy_value, element_size_0));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*-------------------------- Clear all Buffers. Verify Writes are now successful. -------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Clear((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me));
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, 
                                                (const void *)&dummy_value, element_size_0));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*------- Verify Writes did not access out-of-bounds memory. Unity setUp prepends and postpends -----*/
   /*------- known values to the test memory region to test against. Note this is the memory region ----*/
   /*----- of the internal Ring Buffers, not the data we are writing from. I.e. we do not test the -----*/
   /*------------- memory region surrounding 'dummy_value'. This is done in ReadWrite test. ------------*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_RB_Objects_Memory_Access();
}



/*-------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------- Ring_Buffer_Static_Read() ---------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Main test of the Ring Buffer Module. We write the same data values to each buffer and read 
 * it back. We verify the values read back are the same as what was written and that no out-of-bounds 
 * memory access occurred on the data or on the internal Ring Buffer Objects. For variety and completeness
 * each buffer is assigned a random number of elements it can store. So in this test the data values are 
 * the same but the amount of data that can be stored in each buffer is random.
 * 
 * @note The full Test is:
 * 1) Verify invalid parameters supplied to Ring_Buffer_Static_Read() fails.
 * 2) Write single data element to all buffers and read it back. Verify data read back is the
 * same as what was written and no out-of-bounds memory access occurred.
 * 3) Repeat Test #2 but do multiple full read-write sequences. Write data until all buffers 
 * are full (reached max number of elements supplied in Constructor). Then read data back - 
 * with each read testing if the data is the same as what was written and no out-of-bounds 
 * access occurred. Do this until all buffers are fully empty. Repeat this Test for 
 * 'number_of_readwrite_sequences' times.
 */
static void Test_Ring_Buffer_Static_Read_Write(void);
static void Test_Ring_Buffer_Static_Read_Write(void)
{
   /*---------------------------------------------------------------------------------------------------*/
   /*--- Setup. Seed the "random number generator" and define the number of read-write sequences this --*/
   /*---- test should conduct. A single read-write sequence means writing to the buffer until it is ----*/
   /*-- full then reading data back until it is completely empty. This value should be greater than 0 --*/
   /*---------------------------------------------------------------------------------------------------*/
   srand(time(NULL));
   const uint32_t number_of_readwrite_sequences = 3;

   /*---------------------------------------------------------------------------------------------------*/
   /*------------- Setup. Create test data that will be written and read back. Notice this data --------*/
   /*------------------------------ is the same for each read-write operation. -------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   typedef struct
   {
      /* Test Type with internal padding. */
      uint8_t val1;
      uint16_t val2;
      uint32_t val3;
   } test_type;

   const test_type write_data_values = 
   {
      /* Random Values. Make sure LSB and MSB don't equal the pre and postpend dummy values. */
      .val1 = 0x62,
      .val2 = 0xFFC0,
      .val3 = 0xD32F0532
   };

   /*---------------------------------------------------------------------------------------------------*/
   /*------- Setup. Create Test Memory Regions that store the Written and Read Data. We use this -------*/
   /*---- to verify no out-of-bounds memory access occurred when accessing data. If a Write accessed ---*/
   /*--- out-of-bounds memory then the data read back will not be the same as 'write_data_values'. -----*/
   /*------- If a Read accessed out-of-bounds memory then the pre and postpended values will get -------*/
   /*------------------------------------------ overwritten. -------------------------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   uint8_t write_data_memory_region[DATA_PREPOSTPEND_LENGTH + sizeof(test_type) + DATA_PREPOSTPEND_LENGTH];
   uint8_t read_data_memory_region[DATA_PREPOSTPEND_LENGTH + sizeof(test_type) + DATA_PREPOSTPEND_LENGTH];
   uint8_t * const write_data = &write_data_memory_region[DATA_PREPOSTPEND_LENGTH];
   uint8_t * const read_data = &read_data_memory_region[DATA_PREPOSTPEND_LENGTH];

   /* Copy Data Contents and known prepend/postpend values into Memory Regions. */
   memset((void *)&read_data_memory_region[0], READ_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);
   memset((void *)&read_data_memory_region[DATA_PREPOSTPEND_LENGTH + sizeof(test_type)], READ_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);

   memset((void *)&write_data_memory_region[0], WRITE_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);
   memset((void *)&write_data_memory_region[DATA_PREPOSTPEND_LENGTH + sizeof(test_type)], WRITE_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);
   memcpy((void *)&write_data[0], (const void *)&write_data_values, sizeof(test_type));

   /*---------------------------------------------------------------------------------------------------*/
   /*---- Call valid Constructor on all available Ring Buffers. Override parameters in Unity setUp -----*/
   /*--- with our own. For variety, the number of elements is randomly generated for each Ring Buffer --*/
   /*----- It is guaranteed this parameter will always be valid since it is calculated such that -------*/
   /*-- the Ring Buffer Under Test will never hold more than RING_BUFFER_STATIC_SIZE number of bytes. --*/
   /*---------------------------------------------------------------------------------------------------*/
   Ring_Buffer_Static_Handle invalid_handle;
   const uint32_t max_number_of_elements  = floor(RING_BUFFER_STATIC_SIZE / sizeof(test_type));

   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      uint32_t rand_number_of_elements                   = (rand() % max_number_of_elements) + 1;
      Test_Ring_Buffer_Handles[buf].element_size         = sizeof(test_type); /* Added just in case but currently unused. We use sizeof instead. */
      Test_Ring_Buffer_Handles[buf].number_of_elements   = rand_number_of_elements;

      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[buf].me, sizeof(test_type), 
                                                Test_Ring_Buffer_Handles[buf].number_of_elements));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*-------------- Verify Read on invalid Handle fails since Constructor wasn't called. ---------------*/
   /*---------------------------------------------------------------------------------------------------*/
   TEST_ASSERT_FALSE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&invalid_handle, (void *)&read_data[0], sizeof(test_type)));

   /*---------------------------------------------------------------------------------------------------*/
   /*---------------------- Verify Read on all Buffers fail since they are all empty. ------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)&read_data[0],
                                                sizeof(test_type)));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*---------------- Write once to the Buffers so they are not Empty then verify other ----------------*/
   /*--------------------------------- invalid parameter combinations. ---------------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (const void *)&write_data[0],
                                                sizeof(test_type)));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*--------------- Verify Read on NULL data pointer fails. All other parameters valid. ---------------*/
   /*---------- NOTE FOR DEBUGGING: YOU MAY GET SEG FAULT WITH NO ERROR MESSAGE IF THIS FAILS ----------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)0,
                                                sizeof(test_type)));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*----- Verify Read fails when data size input is different than element size specified in Ctor. ----*/
   /*------------------------------------- All other parameters valid. ---------------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)&read_data[0],
                                                sizeof(test_type) + 1));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*---------- Read back Data. Verify it is successful. Verify contents read are the same as ----------*/
   /*--- what was written. If a Write accessed out-of-bounds memory or the Read was done incorrectly ---*/
   /*----- then the data read back will not be the same as 'write_data_values'. If a Read accessed -----*/
   /*---------- out-of-bounds memory then the pre and postpended values will get overwritten. ----------*/
   /*---------------------------------------------------------------------------------------------------*/
   char fail_msg[250];

   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      /**
       * Unfortunately we have to generate this fail message every iteration in order to print the 
       * Buffer Number and the randomly generated capacity that caused it to fail. Note this is a 
       * costly operation so if the Unit Test takes too long it is because of these sprintf calls.
       */
      sprintf(fail_msg, "Did not read back same data as what was written either due to Read Failure, Write accessing out-of-bounds memory,"
               " or Read accessing out-of-bounds memory. Buffer # = %u Element Size = %zu Buffer Capacity = %u\n", buf, 
               sizeof(test_type), Test_Ring_Buffer_Handles[buf].number_of_elements);

      TEST_ASSERT_TRUE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)&read_data[0],
                                                sizeof(test_type)));

      /* Test if data read back equals written data. Tests if Read was successful and Write did not access out-of-bounds memory. */
      TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(&write_data_values, &read_data[0], sizeof(test_type), fail_msg);

      /* Test if Read wrote to out-of-bounds memory. */
      TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(READ_DATA_PREPOSTPEND_VALUE, &read_data_memory_region[0], DATA_PREPOSTPEND_LENGTH, fail_msg);

      TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(READ_DATA_PREPOSTPEND_VALUE, &read_data_memory_region[(DATA_PREPOSTPEND_LENGTH + sizeof(test_type))],
                                          DATA_PREPOSTPEND_LENGTH, fail_msg);

      /* Reset Read Data buffer and fail message. */
      memset((void *)&read_data[0], 0, sizeof(test_type));
      memset((void *)fail_msg, 0, sizeof(fail_msg));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*------- Repeat the same Test as above but do full read-write sequences on all the Buffers. --------*/
   /*--- A read-write sequence is writing till the Buffer is completely full and Reading back data -----*/
   /*---- until it is completely empty. Do this for 'number_of_readwrite_sequences'. Obviously this ----*/
   /*--------- not optimal code. We are trying to fully test all Ring Buffer read/write cases. ---------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t iter = 0; iter < number_of_readwrite_sequences; iter++)
   {
      /* Perform Writes until all Buffers are completely full. Also assert Write fails when Buffer becomes full. */
      for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
      {
         for (size_t ele = 0; ele < Test_Ring_Buffer_Handles[buf].number_of_elements; ele++)
         {
            TEST_ASSERT_TRUE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (const void *)&write_data[0],
                                                      sizeof(test_type)));
         }
         /* Assert Write fails since this Buffer should now be completely full. */
         TEST_ASSERT_FALSE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (const void *)&write_data[0],
                                                   sizeof(test_type)));
      }

      /* Read back data until all Buffers are completely empty. On each Read do a comparison and out-of-bounds memory test. 
      Also assert Read fails when Buffer becomes empty. */
      for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
      {
         sprintf(fail_msg, "Did not read back same data as what was written either due to Read Failure, Write accessing out-of-bounds memory,"
                  " or Read accessing out-of-bounds memory. Buffer # = %u Element Size = %zu Buffer Capacity = %u\n", buf, 
                  sizeof(test_type), Test_Ring_Buffer_Handles[buf].number_of_elements);

         for (size_t ele = 0; ele < Test_Ring_Buffer_Handles[buf].number_of_elements; ele++)
         {
            TEST_ASSERT_TRUE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)&read_data[0],
                                                      sizeof(test_type)));

            /* Test if data read back equals written data. Tests if Read was successful and Write did not access out-of-bounds memory. */
            TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(&write_data_values, &read_data[0], sizeof(test_type), fail_msg);

            /* Test if Read wrote to out-of-bounds memory. */
            TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(READ_DATA_PREPOSTPEND_VALUE, &read_data_memory_region[0], DATA_PREPOSTPEND_LENGTH, fail_msg);

            TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(READ_DATA_PREPOSTPEND_VALUE, &read_data_memory_region[(DATA_PREPOSTPEND_LENGTH + sizeof(test_type))],
                                                DATA_PREPOSTPEND_LENGTH, fail_msg);

            /* Reset Read Data Buffer. */
            memset((void *)&read_data[0], 0, sizeof(test_type));
         }

         /* Assert Read fails since Buffer is now completely empty. Reset fail message. */
         TEST_ASSERT_FALSE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)&read_data[0],
                                                   sizeof(test_type)));
         memset((void *)fail_msg, 0, sizeof(fail_msg));
      }
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*------- Verify these repeated Read-Write sequences did not cause out-of-bounds memory access ------*/
   /*------- Note this is for the Ring Buffer Objects stored by the Module. I.e. RB_Instances[] --------*/
   /*---------- and RB_Instances_In_Use[]. Our previous out-of-bounds tests were on the DATA. ----------*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_RB_Objects_Memory_Access();
}


/**
 * @brief Similar to previous Tests where data is written and read back to verify the values are
 * the same and no out-of-bounds memory access occurred on the Data. However this tests the edge-case 
 * of the Buffer being completely full where number of elements = RING_BUFFER_STATIC_SIZE and element
 * size is 1 byte.
 * 
 * @note Full in Test_Ring_Buffer_Static_Read_Write test meant the Ring Buffer has reached 
 * the max number of elements that was specified in the Constructor - not necessarily the max memory 
 * capacity allocated to the buffer at compile-time. Here we test the absolute max memory capacity.
 * I.e. RING_BUFFER_STATIC_SIZE bytes of data is filled in the buffer.
 */
static void Test_Ring_Buffer_Static_Read_Write_Max_Number_Of_Elements(void);
static void Test_Ring_Buffer_Static_Read_Write_Max_Number_Of_Elements(void)
{
   /*---------------------------------------------------------------------------------------------------*/
   /*------------ Setup. Define element size and number of elements to specify in Constructor. ---------*/
   /*---------------------------------------------------------------------------------------------------*/
   const size_t element_size_0            = 1;
   const uint32_t number_of_elements_0    = RING_BUFFER_STATIC_SIZE;

   /*---------------------------------------------------------------------------------------------------*/
   /*------- Setup. Create Test Memory Regions that store the Written and Read Data. We use this -------*/
   /*---- to verify no out-of-bounds memory access occurred when accessing data. If a Write accessed ---*/
   /*--- out-of-bounds memory then the data read back will not be the same as 'write_data_values'. -----*/
   /*------- If a Read accessed out-of-bounds memory then the pre and postpended values will get -------*/
   /*------------------------------------------ overwritten. -------------------------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   /* The Test Data. */
   const uint8_t write_data_values = 0x91;

   /* Test Memory Regions. */
   uint8_t write_data_memory_region[DATA_PREPOSTPEND_LENGTH + sizeof(write_data_values) + DATA_PREPOSTPEND_LENGTH];
   uint8_t read_data_memory_region[DATA_PREPOSTPEND_LENGTH + sizeof(write_data_values) + DATA_PREPOSTPEND_LENGTH];
   uint8_t * const write_data = &write_data_memory_region[DATA_PREPOSTPEND_LENGTH];
   uint8_t * const read_data = &read_data_memory_region[DATA_PREPOSTPEND_LENGTH];

   /* Copy Data Contents and known prepend/postpend values into Memory Regions. */
   memset((void *)&read_data_memory_region[0], READ_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);
   memset((void *)&read_data_memory_region[DATA_PREPOSTPEND_LENGTH + sizeof(write_data_values)], READ_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);

   memset((void *)&write_data_memory_region[0], WRITE_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);
   memset((void *)&write_data_memory_region[DATA_PREPOSTPEND_LENGTH + sizeof(write_data_values)], WRITE_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);
   memcpy((void *)&write_data[0], (const void *)&write_data_values, sizeof(write_data_values));

   /*---------------------------------------------------------------------------------------------------*/
   /*---- Call valid Constructor on all available Ring Buffers. Override parameters in Unity setUp -----*/
   /*-------------------------------- with our own to test this Edge Case. -----------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[buf].me, element_size_0, number_of_elements_0));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*-------- Write one byte of data at a time to all Ring Buffers until they are all full. ------------*/
   /*------------ Verify Writes on all Buffers afterwards fail since they are now full. ----------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      for (size_t ele = 0; ele < number_of_elements_0; ele++)
      {
         TEST_ASSERT_TRUE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (const void *)&write_data[0],
                                                   element_size_0));
      }
      /* Assert Write fails since this Buffer should now be completely full. */
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (const void *)&write_data[0],
                                                element_size_0));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*---------- Read back Data. Verify it is successful. Verify contents read are the same as ----------*/
   /*--- what was written. If a Write accessed out-of-bounds memory or the Read was done incorrectly ---*/
   /*----- then the data read back will not be the same as 'write_data_values'. If a Read accessed -----*/
   /*---------- out-of-bounds memory then the pre and postpended values will get overwritten. ----------*/
   /*---------------------------------------------------------------------------------------------------*/
   char fail_msg[250];

   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      for (size_t ele = 0; ele < number_of_elements_0; ele++)
      {
         sprintf(fail_msg, "Did not read back same data as what was written either due to Read Failure, Write accessing out-of-bounds memory,"
                  " or Read accessing out-of-bounds memory. Buffer # = %u Element # = %zu\n", buf, ele);

         TEST_ASSERT_TRUE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)&read_data[0],
                                                   element_size_0));

         /* Test if data read back equals written data. Tests if Read was successful and Write did not access out-of-bounds memory. */
         TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(&write_data_values, &read_data[0], sizeof(write_data_values), fail_msg);

         /* Test if Read wrote to out-of-bounds memory. */
         TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(READ_DATA_PREPOSTPEND_VALUE, &read_data_memory_region[0], DATA_PREPOSTPEND_LENGTH, fail_msg);

         TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(READ_DATA_PREPOSTPEND_VALUE, &read_data_memory_region[DATA_PREPOSTPEND_LENGTH + sizeof(write_data_values)],
                                             DATA_PREPOSTPEND_LENGTH, fail_msg);

         /* Reset Read Data buffer and fail message. */
         memset((void *)&read_data[0], 0, sizeof(write_data_values));
         memset((void *)fail_msg, 0, sizeof(fail_msg));
         
      }
      /* Assert Read fails since this Buffer should now be completely empty. */
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)&read_data[0],
                                                element_size_0));  
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*------- Verify this did not cause out-of-bounds memory access for the Ring Buffer Objects ---------*/
   /*------------- stored by the Module. I.e. RB_Instances[] and RB_Instances_In_Use[]. ----------------*/
   /*---------------------- Our previous out-of-bounds tests were on the DATA. -------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_RB_Objects_Memory_Access();
}


/**
 * @brief Same Test as Test_Ring_Buffer_Static_Read_Write_Max_Number_Of_Elements however now the
 * number of elements = 1 and the element size is RING_BUFFER_STATIC_SIZE bytes.
 */
static void Test_Ring_Buffer_Static_Read_Write_Max_Element_Size(void);
static void Test_Ring_Buffer_Static_Read_Write_Max_Element_Size(void)
{
   /*---------------------------------------------------------------------------------------------------*/
   /*------------ Setup. Define element size and number of elements to specify in Constructor. ---------*/
   /*---------------------------------------------------------------------------------------------------*/
   const size_t element_size_0            = RING_BUFFER_STATIC_SIZE;
   const uint32_t number_of_elements_0    = 1;

   /*---------------------------------------------------------------------------------------------------*/
   /*------- Setup. Create Test Memory Regions that store the Written and Read Data. We use this -------*/
   /*---- to verify no out-of-bounds memory access occurred when accessing data. If a Write accessed ---*/
   /*--- out-of-bounds memory then the data read back will not be the same as 'write_data_values'. -----*/
   /*------- If a Read accessed out-of-bounds memory then the pre and postpended values will get -------*/
   /*------------------------------------------ overwritten. -------------------------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   /* The Test Data. */
   const uint8_t write_data_values[RING_BUFFER_STATIC_SIZE] = {0x91};

   /* Test Memory Regions. */
   uint8_t write_data_memory_region[DATA_PREPOSTPEND_LENGTH + RING_BUFFER_STATIC_SIZE + DATA_PREPOSTPEND_LENGTH];
   uint8_t read_data_memory_region[DATA_PREPOSTPEND_LENGTH + RING_BUFFER_STATIC_SIZE + DATA_PREPOSTPEND_LENGTH];
   uint8_t * const write_data = &write_data_memory_region[DATA_PREPOSTPEND_LENGTH];
   uint8_t * const read_data = &read_data_memory_region[DATA_PREPOSTPEND_LENGTH];

   /* Copy Data Contents and known prepend/postpend values into Memory Regions. */
   memset((void *)&read_data_memory_region[0], READ_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);
   memset((void *)&read_data_memory_region[DATA_PREPOSTPEND_LENGTH + RING_BUFFER_STATIC_SIZE], READ_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);

   memset((void *)&write_data_memory_region[0], WRITE_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);
   memset((void *)&write_data_memory_region[DATA_PREPOSTPEND_LENGTH + RING_BUFFER_STATIC_SIZE], WRITE_DATA_PREPOSTPEND_VALUE, DATA_PREPOSTPEND_LENGTH);
   memcpy((void *)&write_data[0], (const void *)&write_data_values[0], RING_BUFFER_STATIC_SIZE);

   /*---------------------------------------------------------------------------------------------------*/
   /*---- Call valid Constructor on all available Ring Buffers. Override parameters in Unity setUp -----*/
   /*-------------------------------- with our own to test this Edge Case. -----------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[buf].me, element_size_0, number_of_elements_0));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*--- Write one data element to all Ring Buffers. One Write will cause all Buffers to be full. ------*/
   /*------------ Verify Writes on all Buffers afterwards fail since they are now full. ----------------*/
   /*---------------------------------------------------------------------------------------------------*/
   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      TEST_ASSERT_TRUE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (const void *)&write_data_values[0],
                                                element_size_0));

      /* Assert Write fails since this Buffer should now be completely full. */
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Write((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (const void *)&write_data_values[0],
                                                element_size_0));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*---------- Read back Data. Verify it is successful. Verify contents read are the same as ----------*/
   /*--- what was written. If a Write accessed out-of-bounds memory or the Read was done incorrectly ---*/
   /*----- then the data read back will not be the same as 'write_data_values'. If a Read accessed -----*/
   /*---------- out-of-bounds memory then the pre and postpended values will get overwritten. ----------*/
   /*---------------------------------------------------------------------------------------------------*/
   char fail_msg[250];

   for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   {
      sprintf(fail_msg, "Did not read back same data as what was written either due to Read Failure, Write accessing out-of-bounds memory,"
               " or Read accessing out-of-bounds memory. Buffer # = %u\n", buf);

      TEST_ASSERT_TRUE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)&read_data[0],
                                                element_size_0));

      /* Assert Read fails since this Buffer should now be completely empty. */
      TEST_ASSERT_FALSE(Ring_Buffer_Static_Read((const Ring_Buffer_Static_Handle *)&Test_Ring_Buffer_Handles[buf].me, (void *)&read_data[0],
                                                element_size_0));

      /* Test if data read back equals written data. Tests if Read was successful and Write did not access out-of-bounds memory. */
      TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(&write_data_values[0], &read_data[0], RING_BUFFER_STATIC_SIZE, fail_msg);

      /* Test if Read wrote to out-of-bounds memory. */
      TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(READ_DATA_PREPOSTPEND_VALUE, &read_data_memory_region[0], DATA_PREPOSTPEND_LENGTH, fail_msg);

      TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(READ_DATA_PREPOSTPEND_VALUE, &read_data_memory_region[DATA_PREPOSTPEND_LENGTH + RING_BUFFER_STATIC_SIZE],
                                          DATA_PREPOSTPEND_LENGTH, fail_msg);

      /* Reset Read Data buffer and fail message. */
      memset((void *)&read_data[0], 0, RING_BUFFER_STATIC_SIZE);
      memset((void *)fail_msg, 0, sizeof(fail_msg));
   }

   /*---------------------------------------------------------------------------------------------------*/
   /*------- Verify this did not cause out-of-bounds memory access for the Ring Buffer Objects ---------*/
   /*------------- stored by the Module. I.e. RB_Instances[] and RB_Instances_In_Use[]. ----------------*/
   /*---------------------- Our previous out-of-bounds tests were on the DATA. -------------------------*/
   /*---------------------------------------------------------------------------------------------------*/
   Test_RB_Objects_Memory_Access();
}




/*-------------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------- Ring_Buffer_Static_Get_Number_Of_Elements() -----------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------*/

#warning "TODO"
static void Test_Ring_Buffer_Static_Get_Number_Of_Elements_Element_Size_1(void);
static void Test_Ring_Buffer_Static_Get_Number_Of_Elements_Element_Size_1(void)
{
   /*---------------------------------------------------------------------------------------------------*/
   /*-------- Setup. Seed "random number generator" and create dummy read/write buffers. We don't ------*/
   /*---- verify read/write contents and out-of-bounds access on the data in this test since it was ----*/ 
   /*----------------- done previously. We just need a place to store write and read data --------------*/
   /*---------------------------------------------------------------------------------------------------*/
   srand(time(NULL));
   uint8_t write_data[NUMBER_OF_STATIC_RING_BUFFERS];
   uint8_t read_data[NUMBER_OF_STATIC_RING_BUFFERS];

   /*---------------------------------------------------------------------------------------------------*/
   /*---- Generate random number of elements and random element size for all Buffers but guarantee -----*/
   /*---------------- Buffers always have enough capacity to hold the requested data sizes. ------------*/
   /*---------------------------------------------------------------------------------------------------*/
   // Ring_Buffer_Static_Handle invalid_handle;

   // for (uint32_t buf = 0; buf < NUMBER_OF_STATIC_RING_BUFFERS; buf++)
   // {
   //    /* Max capacity of each Buffer is RING_BUFFER_STATIC_SIZE bytes. Guarantee we never request more than this. */
   //    size_t rand_element_size                           = (rand() % RING_BUFFER_STATIC_SIZE) + 1;
   //    uint32_t max_number_of_elements                    = floor(RING_BUFFER_STATIC_SIZE / rand_element_size); 
   //    uint32_t rand_number_of_elements                   = (rand() % max_number_of_elements) + 1;
   //    Test_Ring_Buffer_Handles[buf].element_size         = rand_element_size;
   //    Test_Ring_Buffer_Handles[buf].number_of_elements   = rand_number_of_elements;

   //    TEST_ASSERT_TRUE(Ring_Buffer_Static_Ctor(&Test_Ring_Buffer_Handles[buf].me, Test_Ring_Buffer_Handles[buf].element_size, 
   //                                              Test_Ring_Buffer_Handles[buf].number_of_elements));
   // }


   /* Verify 0 is returned on invalid handle */

   /* Verify 0 is returned on empty handles */


   /* Write until Full. With each write verify Element count incremetns by one. */

   /* Read until empty. With each read verify Eleemtn count decrements by one. */

   /* Repeat Write/Read sequence for certain number of times. */
}




int main(void) 
{
   UNITY_BEGIN();
   RUN_TEST(Test_Ring_Buffer_Static_Ctor_Invalid_Parameters);
   RUN_TEST(Test_Ring_Buffer_Static_Ctor_Invalid_Handles);
   RUN_TEST(Test_Ring_Buffer_Static_Ctor_Memory_Access);
   RUN_TEST(Test_Ring_Buffer_Static_Clear);
   RUN_TEST(Test_Ring_Buffer_Static_Destroy);
   RUN_TEST(Test_Ring_Buffer_Static_Ctor_And_Destroy);
   RUN_TEST(Test_Ring_Buffer_Static_Write);
   RUN_TEST(Test_Ring_Buffer_Static_Read_Write);
   RUN_TEST(Test_Ring_Buffer_Static_Read_Write_Max_Number_Of_Elements);
   RUN_TEST(Test_Ring_Buffer_Static_Read_Write_Max_Element_Size);
   return UNITY_END();
}