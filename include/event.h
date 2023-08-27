/**
 * @file event.h
 * @brief The Event Base Class. This can be used to dispatch Event Messages to Object Queues. Objects can
 * define their own Message Type with unique members by inheriting this Base Class. This is done by creating
 * a new Object-specific Event struct and making the Event struct defined here as its first member. Event 
 * Base Class methods can be used by SubClasses that inherit it by explicitly upcasting to an Event. For example:
 * 
 * // Function taking in Base Event Class 
 * void foo(const Event * const e);
 * 
 * // SubClass inheriting Event Base Class
 * typedef struct 
 * {
 *      // Inherit Base Event Class
 *      Event super;
 *      
 *      // SubClass Members
 *      int x;
 *      int y;
 * } Event_Subclass;
 * 
 * 
 * // We can use Event Methods by explicitly upcasting to an Event. 
 * Event_Subclass me;
 * foo((const Event *)me);
 * 
 * This is guaranteed to be a legal and safe operation since the C-Standard mandates that the first member of
 * a struct and the struct itself share the same Memory Address. This REQUIRES that SubClasses make Event 
 * their first struct member.
 * @version 0.1
 * @date 2023-08-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef EVENT_BASE_CLASS_H_
#define EVENT_BASE_CLASS_H_


/* STD-C Libraries */
#include <stdint.h>


/*---------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------- EVENT SIGNAL. THIS WILL BE AN ENUMERATION. -----------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Event Signal. This will be an Event enumeration that Objects can uniquely define. 
 * 
 */
typedef int16_t Signal;


/*---------------------------------------------------------------------------------------------------------------------------*/
/*------------------------ RESERVED EVENT SIGNALS FOR INTERNAL USE. THE APPLICATION CANNOT USE THESE SIGNALS ----------------*/
/*------------------------------------------ AND MUST START THEIR EVENT ENUMERATIONS AT USER_SIG ----------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Reserved Event Signals that Objects are not able to use. When an Object is defining
 * their own Event Signals, they MUST start their enumeration at USER_SIG.
 * 
 */
enum Reserved_Signals
{
    INIT_EVENT      = -4,   /* DO NOT USE. This Event signals an Initial State Transition */
    ENTRY_EVENT     = -3,   /* DO NOT USE. State Transition occurred. Triggers the Entry Event of the State transitioned into. */
    EXIT_EVENT      = -2,   /* DO NOT USE. State Transition occurred. Triggers the Exit Event of the State transitioned out of. */
    IDLE_EVENT      = -1,   /* DO NOT USE. Background processing. */

    /********************/
    USER_SIG        = 0     /* First dispatch Event signal available to the users. This MUST start at 0 for this implementation. */
};



/*---------------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------- THE BASE EVENT CLASS ------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief The Event Base Class that other Object Events can inherit. SubClasses will inherit
 * this by defining their own Event type and making the Event struct its first member. The
 * Event struct MUST be the first member of the inherited type. See event.h file description
 * for more details.
 * 
 */
typedef struct
{
    /* Base Event Class Members */
    Signal sig;

    /* SubClass Members */
} Event;


#endif /* EVENT_BASE_CLASS_H_ */
