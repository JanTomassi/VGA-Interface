#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>

/**
 * @brief Number of max scheduled task
 * @warning Max is 127
 */
#define SCH_NUM_TASK (5)

typedef struct task
{
    uint32_t period;            // Rate at which the task should tick
    uint32_t elapsedTime;       // Time since task's last tick
    void (*TickFct)(void); // Function to call for task's tick
} task;

const int16_t schAddTask(const uint32_t period, void (*TickFct)());
const int16_t schAddIndexTask(const uint32_t period, uint8_t task_num, void (*TickFct)());
void schRemoveTask(uint8_t task_num);
void schTickTask(void);
void schRunTask(void);

#endif