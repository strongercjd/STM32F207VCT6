
#ifndef __SOFTWARE_TASK_H__
#define __SOFTWARE_TASK_H__

#include "stdint.h"

//任务个数和占用内存相关
#define CONFIG_TASK_NUMBER 8 //任务个数,1~254，任务号越低，优先级越高
#define RR_EVENTS_TIMER (0x0001)

#define TCB_SIZE ((CONFIG_TASK_NUMBER >> 3) + 1)

#define SW_NULL 0


typedef uint8_t (*fun_callback)(uint8_t task_id, uint16_t events, void *p);


typedef struct
{
  uint32_t tick;       //任务计时??
  uint32_t tick_delay; //任务执行间隔
  // events为2字节事件标志，每bit表示一个事件，最大可表示16个事件，
  //其中bit0为定时器事件，置1有效，置0无效
  uint16_t events;
  fun_callback fun; //任务执行函数
} S_TASK;


//使用方法流程
// step1:SW_task_tick_cnt()放定示器计时
// step2:SW_task_scheduler()死循环调度，或用定时器调度
// step3:SW_task_create()建立任务；
// step4:SW_task_start_scheduler()使能调度
// events为2字节事件标志，每bit表示一个事件，最大可表示16个事件
//其中bit0为定时器事件，置1有效，置0无效



extern void SW_task_init(void);
extern void SW_task_start_scheduler(void);
extern void SW_task_tick_cnt(void); //放定时器计时
extern void SW_task_set_events(uint8_t task_id, uint16_t event);
extern void SW_task_set_events_irq(uint8_t task_id, uint16_t event);
extern uint8_t SW_task_create(uint8_t task_id, fun_callback callback, uint32_t tick_delay);
extern void SW_task_scheduler(void);

#endif
