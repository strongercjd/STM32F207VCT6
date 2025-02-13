#include "MultiTimer.h"
#include <stdio.h>

/* Timer handle list head. */
static MultiTimer *timerList = NULL;

/* Timer tick */
static PlatformTicksFunction_t platformTicksFunction = NULL;
/**
 * @brief 注册系统timer tick函数
 *
 * @param ticksFunc
 * @return int
 */
int MultiTimerInstall(PlatformTicksFunction_t ticksFunc)
{
    platformTicksFunction = ticksFunc;
    return 0;
}
/**
 * @brief 定时器开始工作
 *
 * @param timer 定时器控制块
 * @param timing 定时时间
 * @param callback 回调函数
 * @param userData 私有数据
 * @return int 0: success, -1: fail.
 */
int MultiTimerStart(MultiTimer *timer, uint64_t timing, MultiTimerCallback_t callback, void *userData)
{
    if (!timer || !callback)
    {
        return -1;
    }
    /* 这里链表不管理内存，也就是传进来的 *timer的内存不能是临时的
    简单的说：下面的timer1不能是函数内的临时变量
    MultiTimerStart(&timer1, 500, exampleTimer1Callback, "open led");
     */
    MultiTimer **nextTimer = &timerList;
    /* Remove the existing target timer. */
    for (; *nextTimer; nextTimer = &(*nextTimer)->next)
    {
        if (timer == *nextTimer)
        {
            // 找到了对应的MultiTimer，就删除
            *nextTimer = timer->next; /* remove from list */
            break;
        }
    }
    /* 上面的代码不好理解，下面是方便理解的版本
    MultiTimer **nowTimer = &timerList;
    MultiTimer **frontTimer = NULL;;
    nowTimer = &timerList;
    for (; *nowTimer; )
    {
      if (timer == *nowTimer)
      {
        (*frontTimer)->next = (*nowTimer)->next;
        break;
      }
      frontTimer = nowTimer;
      nowTimer = &((*nowTimer)->next);
    }
    */

    /* Init timer. */
    timer->deadline = platformTicksFunction() + timing;
    timer->callback = callback;
    timer->userData = userData;

    /* 插入定时器 ，并且进行排序*/
    for (nextTimer = &timerList;; nextTimer = &(*nextTimer)->next)
    {
        if (!*nextTimer) // 直接追加到末尾
        {
            timer->next = NULL;
            *nextTimer = timer;
            break;
        }
        if (timer->deadline < (*nextTimer)->deadline)
        {
            timer->next = *nextTimer;
            *nextTimer = timer;
            break;
        }
    }
    return 0;
}
/**
 * @brief 删除定时器
 *
 * @param timer
 * @return int
 */
int MultiTimerStop(MultiTimer *timer)
{
    MultiTimer **nextTimer = &timerList;
    /* Find and remove timer. */
    for (; *nextTimer; nextTimer = &(*nextTimer)->next)
    {
        MultiTimer *entry = *nextTimer;
        if (entry == timer)
        {
            *nextTimer = timer->next;
            break;
        }
    }
    return 0;
}

int MultiTimerYield(void)
{
    MultiTimer *entry = timerList;
    for (; entry; entry = entry->next)
    {
        /* Sorted list, just process with the front part. */
        /* 已排序列表，只处理最前面 */
        if (platformTicksFunction() < entry->deadline)
        {
            return (int)(entry->deadline - platformTicksFunction());
        }
        /* remove expired timer from list */
        timerList = entry->next;

        /* call callback */
        if (entry->callback)
        {
            entry->callback(entry, entry->userData);
        }
    }
    return 0;
}

void MultiTimerinfo(void)
{
    MultiTimer *entry = timerList;
    printf("timerList start\r\n");
    for (; entry; entry = entry->next)
    {
        printf("Timer deadline:%d userdata: %s.\r\n", entry->deadline, (char *)(entry->userData));
    }
    printf("timerList end\r\n\r\n");
}
