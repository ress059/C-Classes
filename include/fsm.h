/**
 * @file fsm.h
 * @author Ian Ress
 * @brief Base Finite State Machine (FSM) Class. The concept of State is represented with Function Pointers to
 * allow unique function handlers and explicit Entry and Exit Events for each State. This class can handle nested
 * State Transitions that occur in the Entry Event but not the Exit Events.
 * 
 * This is meant to be inherited by Application-specific FSMs by declaring the Fsm struct as its first 
 * member. To use these base class methods, explicit upcasts are done. For example:
 * 
 * /--------------------------------- KeyboardFsm inheriting Base Fsm Class ------------------------------/
 * #include "fsm.h"
 * 
 * typedef struct
 * {
 *      // Inherit base FSM Class
 *      Fsm parent;
 * 
 *      // Additional Subclass members for fsm
 *      uint8_t layer;
 * } Keyboard_Fsm;
 * 
 * 
 * enum
 * {
 *      KEY_PRESS_EVENT
 * } Keyboard_Event_Signals;
 * 
 * 
 * typedef struct
 * {
 *      // Inherit base Event Class
 *      Event parent; // Additional Keyboard_Event_Signals will be here.
 * 
 *      // Additional Subclass members for event
 *      uint8_t keycode;
 * } Keyboard_Event;
 * 
 * 
 * /-------------------------- Define your own KeyboardFsm State Handlers and Ctor -----------------/
 * static void Keyboard_Ctor(Keyboard_Fsm * const me, Fsm_Handler init_state_handler_0, uint32_t max_state_transitions_0, uint8_t layer_0)
 * {
 *      Fsm_Ctor((Fsm *)me, init_state_handler_0, max_state_transitions_0);
 *      me->layer = layer_0;
 * }
 * 
 * // Init and all other State Handlers will have this same format.
 * static Fsm_Status KEYBOARD_INIT_STATE_HANDLER(Keyboard_Fsm * const me, const Keyboard_Event * const e)
 * {
 *      // Application-specific definition
 * }
 * 
 * 
 * /---------------------------- Use Fsm Base Class Methods on KeyboardFsm  --------------------------/
 * static const Keyboard_Event keypress_a_evt = {.parent = {KEY_PRESS_EVENT}, .keycode = 0x55};
 * static KeyboardFsm keyboard;
 * 
 * Keyboard_Ctor(&keyboard, (Fsm_Handler)KEYBOARD_INIT_STATE_HANDLER, 3, 0);
 * Fsm_Init((Fsm *)keyboard);
 * Fsm_Dispatch((Fsm *)keyboard, (const Event *)keypress_a_evt);
 * 
 * Explicit upcasting to Fsm and Event is guaranteed to be a legal and safe operation because the C Standard
 * mandates that the first member of a struct and the struct itself share the same memory address. Also note how
 * upcasting to Fsm_Handler function pointer is also safe due to this same reasoning. Note that this requires the
 * user to declare the base class as the first struct member and declare State Handler functions with the appropriate
 * function signatures.
 * 
 * This is derived from Miro Samek's Modern Embedded System Programming Course: 
 * https://github.com/QuantumLeaps/modern-embedded-programming-course/tree/main
 * @version 0.1
 * @date 2023-03-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef FSM_H_
#define FSM_H_


/* STD-C Libraries */
#include <stdbool.h>
#include <stdint.h>

/* Run-time Asserts */
#include "asserter.h"

/* Inherit Event Base Class */
#include "event.h"



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ FORWARD DECLARATION OF FSM BASE CLASS NEEDED TO DEFINE FSM HANDLER ------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

typedef struct Fsm Fsm;



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------ FSM STATE HANDLER. DEFINES HOW THE FSM STATE REACTS TO DISPATCHED EVENTS ------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Each FSM State Handler must return one of these Statuses. The FSM Dispatcher uses this 
 * information to figure out the proper State Transitions, Exit Events, and Entry Events to execute.
 */
typedef enum
{
    FSM_TRAN_STATUS,        /* Dispatched Event caused a State Transition. Must only return by calling FSM_TRAN(). Run Exit Event of previous State and Entry Event of State transitioned into. */
    FSM_HANDLED_STATUS,     /* Dispatched Event was handled. Stay in current State. */
    FSM_IGNORED_STATUS,     /* Dispatched Event was ignored. Stay in current State. */
    FSM_ERROR_STATUS        /* Event causes invalid FSM behavior. Dispatcher will trigger a run-time assert that never returns. */
} Fsm_Status;


/**
 * @brief The FSM State Handler. Each FSM State is defined with a unique function of this signature
 * that handles Dispatched Events. See fsm.h File Description for more type/inheritance details.
 */
typedef Fsm_Status (*Fsm_Handler)(Fsm * const me, const Event * const e);



/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------- BASE FSM CLASS ------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The Base FSM Class. The concept of State is represented by Fsm_Handler function
 * pointer. Meant to be inherited by application-specific FSMs. See fsm.h File Description 
 * for more type/inheritance details.
 */
struct Fsm
{
    /* Base FSM Class */
    Fsm_Handler state;                  /* Function Pointer defining FSM's current State behavior. Handles dispatched Events. */
    uint32_t max_state_transitions;     /* The maximum number of State Transitions you can take in a row before assertion fires. */

    /* Additional SubClass Members */
};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The FSM Base Class Constructor. See fsm.h File Description for more type/inheritance details.
 * 
 * @warning This Constructor MUST be called before any other methods are used.
 * @warning This Class does not use Dynamic Memory Allocation. Therefore memory for the Fsm Class 
 * Instance MUST be pre-allocated before the Constructor is called.
 * 
 * @param me Pointer to Fsm Class Instance. This cannot be NULL. Memory for the Class 
 * Instance pointed to must be allocated before using this function.
 * @param init_state_handler_0 Initial State assigned to the FSM. This cannot be NULL. This function 
 * MUST transition to a new State by calling FSM_TRAN(). This Constructor only assigns this State, 
 * it does not run it. Fsm_Begin() runs this assigned Initial State.
 * @param max_state_transitions_0 The maximum number of State Transitions you can take in a row
 * before an assertion fires. For example if this is 2, a dispatched Event cannot cause 3 State
 * Transitions to occur in a row. This must be greater than 0.
 */
void Fsm_Ctor(Fsm * const me, Fsm_Handler init_state_handler_0, uint32_t max_state_transitions_0);


/**
 * @brief Runs the Initial State Handler assigned in Fsm_Ctor(). See fsm.h File Description for 
 * more type/inheritance details.
 * 
 * @warning Fsm_Ctor must be called before using this function.
 * 
 * @param me Pointer to Fsm Class Instance. This cannot be NULL. Memory for the Class 
 * Instance pointed to must be allocated before using this function.
 */
void Fsm_Begin(Fsm * const me);


/**
 * @brief Dispatches an Event to the FSM. See fsm.h File Description for more type/inheritance 
 * details.
 * 
 * @warning Fsm_Ctor must be called before using this function.
 * 
 * @param me Pointer to Fsm Class Instance. This cannot be NULL. Memory for the Class 
 * Instance pointed to must be allocated before using this function.
 * @param e Pointer to Event to dispatch. This cannot be NULL. Memory for the Event
 * pointed to must be allocated before using this function.
 */
void Fsm_Dispatch(Fsm * const me, const Event * const e);


/**
 * @brief Call in Application FSM State Handlers to transition FSM into a new state.
 * This function must be used. The FSM's state should never be changed directly.
 * 
 * @warning Fsm_Ctor must be called before using this function.
 * 
 * @param me Pointer to Fsm Class Instance. This cannot be NULL. Memory for the Class 
 * Instance pointed to must be allocated before using this function.
 * @param target State to transition into. This is a State Handler function. This
 * cannot be NULL.
 * 
 * @return The return value is used internally by the Fsm Dispatcher.
 */
static inline Fsm_Status FSM_TRAN(Fsm * const me, Fsm_Handler target);
static inline Fsm_Status FSM_TRAN(Fsm * const me, Fsm_Handler target)
{
    RUNTIME_ASSERT((me && target));
    me->state = target;
    return FSM_TRAN_STATUS;
}


#endif /* FSM_H_ */
