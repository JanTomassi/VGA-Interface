/**
 * @file    scheduler.c
 * @author  Jan Tomassi
 * @version V0.0.1
 * @date    02/10/2022
 * @brief   Implementation of a simple scheduler to manage user defined function
 */

#include "scheduler.h"
#include <stddef.h>

task tasks[SCH_NUM_TASK] = {0};

/**
 * @addtogroup VGA-Interface
 * @{
 * @addtogroup Scheduler
 * @{
 */

/**
 * @brief Add task to task array to be scheduled
 *
 * @param period system tick interval after witch the function should be called
 * @param TickFct pointer to the function to call
 * @return int16_t location in the array
 */
const int16_t schAddTask(const uint32_t period, void (*TickFct)())
{
    for (int8_t task_num = 0; task_num < SCH_NUM_TASK; task_num++)
    {
        if (tasks[task_num].TickFct == NULL)
        {
            tasks[task_num].period = period;
            tasks[task_num].elapsedTime = 0;
            tasks[task_num].TickFct = TickFct;
            return task_num;
        }
    }
    return -1;
}

/**
 * @brief Add task to specific index in the task array to be scheduled
 *
 * @param period system tick interval at witch the function should be called
 * @param task_num index of the task
 * @param TickFct pointer to the function to call
 * @return int16_t location in the array
 */
const int16_t schAddIndexTask(const uint32_t period, uint8_t task_num, void (*TickFct)())
{
    tasks[task_num].period = period;
    tasks[task_num].elapsedTime = 0;
    tasks[task_num].TickFct = TickFct;
    return task_num;
}

/**
 * @brief Remove the task with index task_num
 *
 * @param task_num task index in the array
 */
void schRemoveTask(uint8_t task_num)
{
    tasks[task_num].period = 0;
    tasks[task_num].elapsedTime = 0;
    tasks[task_num].TickFct = 0;
}

/**
 * @brief Remove all the task
 *
 * @param task_num task index in the array
 */
void schRemoveAllTask(int8_t task_num)
{
    for (int8_t task_num = 0; task_num < SCH_NUM_TASK; task_num++)
    {

        tasks[task_num].period = 0;
        tasks[task_num].elapsedTime = 0;
        tasks[task_num].TickFct = NULL;
    }
}

/**
 * @brief Tick all the task
 *
 */
void schTickTask(void)
{
    for (int8_t task_num = 0; task_num < SCH_NUM_TASK; task_num++)
    {
        if (tasks[task_num].TickFct != NULL)
        {
            tasks[task_num].elapsedTime++;
        }
    }
}

/**
 * @brief Run all the tasks that as elapsed there time
 *
 */
void schRunTask(void)
{
    for (int8_t task_num = 0; task_num < SCH_NUM_TASK; task_num++)
    {
        if (tasks[task_num].TickFct != NULL &&
            tasks[task_num].period <= tasks[task_num].elapsedTime)
        {
            tasks[task_num].elapsedTime = 0;
            tasks[task_num].TickFct();
        }
    }
}
///@}
///@}