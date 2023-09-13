/**
 * @file fsm.c
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


#include "fsm.h"



/*--------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------- EVENTS FOR INTERNAL USE BY THE FSM DISPATCHER ----------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

static const Event Entry_Event  = {ENTRY_EVENT};
static const Event Exit_Event   = {EXIT_EVENT};
static const Event Init_Event   = {INIT_EVENT};



/*--------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------ PUBLIC FUNCTIONS --------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

void Fsm_Ctor(Fsm * const me, Fsm_Handler init_state_handler_0, uint32_t max_state_transitions_0)
{
    RUNTIME_ASSERT((me && init_state_handler_0 && max_state_transitions_0));
    me->state = init_state_handler_0;
}


void Fsm_Begin(Fsm * const me)
{
    /**
     * Note that this function attempts to protect against everything however the following are errors that
     * cannot currently be handled:
     * 
     * 1) User directly changing me->state instead of using FSM_TRAN() function.
     */
    Fsm_Status status = FSM_ERROR_STATUS;
    Fsm_Handler prev_state;
    uint32_t i;

    /**
     * Please note how a NULL assertion is first done SEPARATELY on the me pointer. This guarantees
     * the assert handler will execute instead of a crash due to a Segmentation Fault when accessing 
     * members of me.
     */
    RUNTIME_ASSERT((me));
    RUNTIME_ASSERT(((me->max_state_transitions) && (me->state)));

    /* Run Init State Handler. Verify FSM_TRAN() was called and FSM transitioned into non-NULL State. */
    status = (*me->state)(me, &Init_Event);
    RUNTIME_ASSERT((status == FSM_TRAN_STATUS) && (me->state));

    /* Run Entry Event of State transitioned into. Save State in case of another Transition in the Entry Event. */
    prev_state = me->state;
    status = (*me->state)(me, &Entry_Event);

    /* Handle any State Transitions occuring after initial transition. */
    for (i = 0; (status == FSM_TRAN_STATUS && me->state && i < me->max_state_transitions); i++)
    {
        /**
         * Run Exit Event of State that was just transitioned out of.
         * Reject FSM_TRAN() calls in the Exit Event and ensure State was supposed to be exited. I.e. FSM 
         * may be in a State that should never exit so user returns FSM_ERROR_STATUS in Exit Event.
         */
        status = (*prev_state)(me, &Exit_Event);
        RUNTIME_ASSERT(((status != FSM_TRAN_STATUS) && (status != FSM_ERROR_STATUS)));

        /* Run Entry Event of State that was just transitioned into. */
        prev_state = me->state;
        status = (*me->state)(me, &Entry_Event);
    }

    /* Verify FSM did not transition into a NULL State, max transitions weren't exceeded, and FSM did not signal error. */
    RUNTIME_ASSERT(((me->state) && (i <= me->max_state_transitions) && (status != FSM_ERROR_STATUS)));
}


void Fsm_Dispatch(Fsm * const me, const Event * const e)
{
    /**
     * Note that this function attempts to protect against everything however the following are errors that
     * cannot currently be handled:
     * 
     * 1) User directly changing me->state instead of using FSM_TRAN() function.
     */
    Fsm_Status status = FSM_ERROR_STATUS;
    Fsm_Handler prev_state;
    uint32_t i;

    /**
     * Please note how a NULL assertion is first done SEPARATELY on the me pointer. This guarantees
     * the assert handler will execute instead of a crash due to a Segmentation Fault when accessing 
     * members of me. Verify FSM members are valid and reject the Application from dispatching Reserved
     * Events to the FSM.
     */
    RUNTIME_ASSERT(((me) && (e)));
    RUNTIME_ASSERT(((me->max_state_transitions) && (me->state) && (e->sig >= USER_SIG)));

    /* Dispatch Event to State. */
    prev_state = me->state;
    status = (*me->state)(me, e);

    /* Handle State Transitions. */
    for (i = 0; (status == FSM_TRAN_STATUS && me->state && i < me->max_state_transitions); i++)
    {
        /**
         * Run Exit Event of State that was just transitioned out of.
         * Reject FSM_TRAN() calls in the Exit Event and ensure State was supposed to be exited. I.e. FSM 
         * may be in a State that should never exit so user returns FSM_ERROR_STATUS in Exit Event.
         */
        status = (*prev_state)(me, &Exit_Event);
        RUNTIME_ASSERT(((status != FSM_TRAN_STATUS) && (status != FSM_ERROR_STATUS)));

        /* Run Entry Event of State that was just transitioned into. */
        prev_state = me->state;
        status = (*me->state)(me, &Entry_Event);
    }

    /* Verify FSM did not transition into a NULL State, max transitions weren't exceeded, and FSM did not signal error. */
    RUNTIME_ASSERT(((me->state) && (i <= me->max_state_transitions) && (status != FSM_ERROR_STATUS)));
}
