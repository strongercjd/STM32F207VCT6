/**
 * @brief 软件任务调度器
 * 
 */
#include "software_task.h"


static volatile uint8_t SW_Task_TCB[TCB_SIZE]; //任务控制块,每个字节标识8个任务

static volatile S_TASK task_info[CONFIG_TASK_NUMBER];
static volatile uint8_t scheduler_flag = 0;
/**
 * @brief 调度器初始化
 * 
 */
void SW_task_init(void)
{
  uint8_t i = 0;
  for (i = 0; i < CONFIG_TASK_NUMBER; i++)
  {
    task_info[i].events = 0;
    task_info[i].fun = 0;
    task_info[i].tick = 0;
    task_info[i].tick_delay = 0;
  }
}
/**
 * @brief 开启任务调度器
 * 
 */
void SW_task_start_scheduler(void)
{
  scheduler_flag = 1;
}

static uint8_t SW_task_set(uint8_t task_id)
{
  uint8_t div_len = 0;
  uint8_t div = 0;

  if (task_id >= (TCB_SIZE << 3))
  {
    return 1;
  }

  div_len = task_id >> 3;
  div = task_id & 0x07;
  SW_Task_TCB[div_len] |= (1 << div);

  return 0;
}
static uint8_t SW_task_clr(uint8_t task_id)
{
  uint8_t div_len = 0;
  uint8_t div = 0;

  if (task_id >= (TCB_SIZE << 3))
  {
    return 1;
  }

  div_len = task_id >> 3;
  div = task_id & 0x07;
  SW_Task_TCB[div_len] &= ~(1 << div);

  return 0;
}
static void SW_task_scan(void)
{
  uint8_t task_id = 0;

  do
  {
    if ((task_info[task_id].tick_delay) &&
        (task_info[task_id].tick >= task_info[task_id].tick_delay))
    {
      task_info[task_id].tick = 0;
      task_info[task_id].events |= RR_EVENTS_TIMER;
      SW_task_set(task_id);
    }
    task_id++;
  } while (task_id < CONFIG_TASK_NUMBER);
}

void SW_task_tick_cnt(void)
{
  uint8_t task_id = 0;

  if (0 == scheduler_flag)
    return;

  do
  {
    task_info[task_id++].tick++;
  } while (task_id < CONFIG_TASK_NUMBER);

  SW_task_scan();
}
void SW_task_set_events(uint8_t task_id, uint16_t event)
{
  if (task_id < CONFIG_TASK_NUMBER)
  {
    task_info[task_id].events |= event;
    SW_task_set(task_id);
  }
}
void SW_task_set_events_irq(uint8_t task_id, uint16_t event)
{
  //禁止中断
  if (task_id < CONFIG_TASK_NUMBER)
  {
    task_info[task_id].events |= event;
    SW_task_set(task_id);
  }
  //开启中断
}
uint8_t SW_task_create(uint8_t task_id, fun_callback fun, uint32_t tick_delay)
{
  if (task_id >= CONFIG_TASK_NUMBER || fun == SW_NULL)
  {
    return 1;
  }

  task_info[task_id].fun = fun;
  task_info[task_id].events = 0; //无事件

  task_info[task_id].tick = 0;
  task_info[task_id].tick_delay = tick_delay;

  return 0;
}

static uint8_t SW_task_get(void)
{
  uint8_t i = 0;
  uint8_t j = 0;
  uint8_t tmp = 0;

  do
  {
    if (SW_Task_TCB[i]) //有任务
    {
      tmp = SW_Task_TCB[i];
      for (j = 0; j < 8; j++)
      {
        if (tmp & 0x01)
        {
          return (i * 8 + j);
        }
        tmp >>= 1;
      }
    }
    i++;
  } while (i < TCB_SIZE);

  return 0xFF;
}

void SW_task_scheduler(void)
{
  uint8_t task_id = 0;
  uint16_t events = 0;

  // 获取当前需要执行的任务
  task_id = SW_task_get();

  if (task_id > CONFIG_TASK_NUMBER)
  {
    return;
  }

  // 该函数有做参数检查，task_id返回FF也不会出错
  SW_task_clr(task_id);
  if (task_info[task_id].fun == SW_NULL)
    return;

  // 如获取的任务存在(已初始化)，则执行任务??
  events = task_info[task_id].events;
  task_info[task_id].events = 0;
  task_info[task_id].fun(task_id, events, SW_NULL);
}
