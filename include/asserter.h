/**
 * @file asserter.h
 * @brief Requires GCC v2.5 and greater or C11 and greater if not using GCC. General Asserter for compile-time 
 * and run-time asserts. Run-time asserter calls an assert handler callback function that must be defined by 
 * the application if this module is used. The prototype of this handler function requires a noreturn function 
 * attribute which is compiler-specific. As a result, this is encapsulated in the ATTRIBUTE_NORETURN_ preprocessor 
 * define whose definition adjusts depending on the compiler being used.
 * 
 * General Guidelines for Assertions:
 * 1) Always use assertions for bugs. I.e. NULL pointer dereference, out-of-bounds access, invalid 
 * function parameters.
 * 
 * 2) Never use assertions for undesired run-time behavior that can occur. I.e. out-of-range temperature
 * reading, communication loss, emergency message received from other device.
 * 
 * 3) Always keep assertions in production builds. Preprocessor define can be used to differentiate
 * between debug and production assert handler definitions.
 * 
 * 4) Thouroughly test assert handler definition under multiple failure conditions since this is the
 * last line of defence if your program fails. I.e. Stack Overflow, misaligned access, preemption.
 * 
 * 5) Assert handler definition should handle BOTH hardware (NMI faults) and software bugs. 
 *
 * @version 0.1
 * @date 2023-09-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef ASSERTER_H_
#define ASSERTER_H_



/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- ASSERT MACROS ----------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/


#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    /**
     * @brief Produces a compilation error if the assert fails by using C23 static assert.
     * 
     * @param check_ Assertion to check. If this is True the assertion passes. If this
     * if False the assertion fails and triggers a compilation error. This expression must 
     * be able to be evaluated at compile-time.
     * 
     * @warning Do not use this for run-time asserts. Doing so is undefined behavior.
     * Use RUNTIME_ASSERT macro instead.
     */
    #define COMPILETIME_ASSERT(check_)              static_assert((check_), "Compile-time Assertion fired!")
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    /**
     * @brief Produces a compilation error if the assert fails by using C11 static assert.
     * 
     * @param check_ Assertion to check. If this is True the assertion passes. If this
     * if False the assertion fails and triggers a compilation error. This expression must 
     * be able to be evaluated at compile-time.
     * 
     * @warning Do not use this for run-time asserts. Doing so is undefined behavior.
     * Use RUNTIME_ASSERT macro instead.
     */
    #define COMPILETIME_ASSERT(check_)              _Static_assert((check_), "Compile-time Assertion fired!")
#else
    /**
     * @brief Produces a compilation error if the assert fails by expanding to 
     * (void)sizeof(char[-1]).
     * 
     * @param check_ Assertion to check. If this is True the assertion passes. If this
     * if False the assertion fails and triggers a compilation error. This expression must 
     * be able to be evaluated at compile-time.
     * 
     * @warning Do not use this for run-time asserts. Doing so is undefined behavior.
     * Use RUNTIME_ASSERT macro instead.
     */
    #define COMPILETIME_ASSERT(check_)              (void)sizeof(char[((check_) ? 1 : -1)])
#endif



/**
 * @brief Executes a user-defined assert handler callback function if the assert
 * fails. This callback function does not return and must be defined by the application 
 * if this module is used.
 * 
 * @param check_ Assertion to check. If this is True the assertion passes. If this
 * if False the assertion fails and a user-defined assert handler callback function
 * executes. This must be a run-time expression.
 * 
 * @warning Do not use this for compile-time asserts. Doing so is undefined behavior.
 * Use COMPILETIME_ASSERT macro instead.
 */
#define RUNTIME_ASSERT(check_)                      ((check_) ? ((void)0) : My_Assert_Handler())



/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------ ASSERT HANDLER FUNCTION CALLED IF RUNTIME ASSERTS FAIL -------------------------------*/
/*--------- THIS MUST BE DEFINED BY THE APPLICATION USING THIS MODULE OTHERWISE WE ATTEMPT TO DEFINE IT OURSELVES -----------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

#ifndef ATTRIBUTE_NORETURN_ 
    #if defined(__GNUC__) && ( (__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5) )   /* Use GCC extension if compiling with GCC. Requires GCC v2.5 or greater. */
        #define ATTRIBUTE_NORETURN_                 __attribute__((noreturn))
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
        #define ATTRIBUTE_NORETURN_                 [[noreturn]]
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        #define ATTRIBUTE_NORETURN_                 _Noreturn
    #else
        #error "noreturn function attribute required. Must be compiling with GCC v2.5 or greater. Otherwise requires C11 or greater."
    #endif
#endif


/**
 * @brief User-defined handler function that executes if RUNTIME_ASSERT macro fails. This function 
 * does not return and must be defined by the application using this module. This should be able
 * to handle BOTH hardware (NMI Faults) and Software bugs. An example definition would be resetting 
 * the Stack Pointer, performing Application-specific shutdown behavior, and resetting the CPU.
 * 
 * @note For Unit Tests this function definition can be a Mock where it is verified this is 
 * called under the appropriate conditions.
 * 
 * @warning This serves as the last line of defence for your program if something goes wrong.
 * As a result, the definition should be tested under multiple failure conditions (Stack Overflow,
 * misaligned access, preemption, etc).
 * 
 */
ATTRIBUTE_NORETURN_ void My_Assert_Handler(void);


#endif /* ASSERTER_H_ */
