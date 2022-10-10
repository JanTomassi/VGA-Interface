#ifndef __SCHEDULER_H
#define __SCHEDULER_H

<<<<<<< HEAD
<<<<<<< HEAD
#include <stddef.h>
#include <stdint.h>
=======
#include "stm32f4_discovery.h"
#include "sys.h"
>>>>>>> 91c65aa (First implementation of a non preemptive Scheduler)
=======
#include <stddef.h>
#include <stdint.h>
>>>>>>> 2e4e481 (Remove unused header from scheduler)
/**
 * @brief Number of max scheduled task
 * @warning Max is 127
 */
#define SCH_NUM_TASK (5)

typedef struct task
{
<<<<<<< HEAD
<<<<<<< HEAD
    uint32_t period;            // Rate at which the task should tick
    uint32_t elapsedTime;       // Time since task's last tick
    void (*TickFct)(void); // Function to call for task's tick
} task;

const int16_t schAddTask(const uint32_t period, void (*TickFct)());
const int16_t schAddIndexTask(const uint32_t period, uint8_t task_num, void (*TickFct)());
void schRemoveTask(uint8_t task_num);
=======
    u32 period;            // Rate at which the task should tick
    u32 elapsedTime;       // Time since task's last tick
    void (*TickFct)(void); // Function to call for task's tick
} task;

sc16 schAddTask(uc32 period, void (*TickFct)());
sc16 schAddIndexTask(uc32 period, u8 task_num, void (*TickFct)());
void schRemoveTask(u8 task_num);
>>>>>>> 91c65aa (First implementation of a non preemptive Scheduler)
=======
    uint32_t period;            // Rate at which the task should tick
    uint32_t elapsedTime;       // Time since task's last tick
    void (*TickFct)(void); // Function to call for task's tick
} task;

const int16_t schAddTask(const uint32_t period, void (*TickFct)());
const int16_t schAddIndexTask(const uint32_t period, uint8_t task_num, void (*TickFct)());
void schRemoveTask(uint8_t task_num);
>>>>>>> 2e4e481 (Remove unused header from scheduler)
void schTickTask(void);
void schRunTask(void);

#endif