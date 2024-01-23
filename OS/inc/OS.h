/*
 * OS.h
 *
 *  Created on: May 30, 2021
 *      Author: Gabriel
 */

#ifndef INC_OS_OS_H_
#define INC_OS_OS_H_

#include "OS_Common.h"
#include "OS_Heap.h"
#include "OS_Tasks.h"
#include "OS_Callbacks.h"
#include "OS_Tick.h"
#include "OS_Obj.h"
#include "OS_Scheduler.h"
#include "OS_Sem.h"
#include "OS_Mutex.h"
#include "OS_Event.h"
#include "OS_MsgQ.h"

/**********************************************
 * PUBLIC FUNCTIONS
 *********************************************/

/***********************************************************************
 * OS Init Get
 *
 * @brief This function informs other processes that the OS is initialized
 *
 * @return bool : 1 = done
 *
 **********************************************************************/
bool os_init_get();


/***********************************************************************
 * OS Init
 *
 * @brief This function initializes the OS environment, creating the main and idle tasks, initializing their stacks, and initializing OS's heap
 *
 * @param char* main_name				: [in] main task name. A null name creates a nameless task.
 * @param int8_t   main_task_priority   : [in] main stack base priority (0 to 127)
 * @param uint32_t interrput_stack_size : [in] size of the interrupt stack
 * @param char* idle_name				: [in] idle task name. A null name creates a nameless task.
 * @param uint32_t idle_stack_size 		: [in] size of the idle stack
 *
 * @return os_err_e : Error code (0 = OK)
 *
 **********************************************************************/
os_err_e os_init(char* main_name, int8_t main_task_priority, uint32_t interrput_stack_size, char* idle_name, uint32_t idle_stack_size);


#endif /* INC_OS_OS_H_ */
