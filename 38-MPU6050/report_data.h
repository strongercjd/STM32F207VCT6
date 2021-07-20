#ifndef __REPORT_DATA_H
#define __REPORT_DATA_H

#include "stm32f2xx.h"


extern void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz);
extern void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);

#endif 