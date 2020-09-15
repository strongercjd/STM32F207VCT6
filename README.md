# STM32F207VCT6
> 此仓库所有的硬件和软件资料，只用于学习参考使用，在普通室温环境下使用没问题，对于参考此仓库中资料造成商业产品或工业产品故障，本人不负责

本仓库代码多个工程同时使用01-Libraries中的库文件，如果每个工程都有个库文件，会导致整个git仓库文件很大，拉取缓慢。

| name                 | 描述                                   |
| :------------------- | -------------------------------------- |
| 00-Hardware          | 学习STM32F207使用硬件的PCB和原理图     |
| 01-Libraries         | STM32F207使用的库文件                  |
| 02-Template          | Keil和IAR的模板工程                    |
| 03-ASM               | 使用汇编函数编写接口                   |
| 04-Delay             | 使用查询定时器计数器的方式实现延时函数 |
| 05-LCD               | 使用FSMC驱动LCD屏幕代码                |
| 06-GPIO-Input-Output | GPIO输入输出代码                       |
| 07-GPIO-JTAG         | GPIO复用JTAG代码                       |
| 08-GPIO-bit-band     | GPIO位带代码                           |
| 09-EXTI              | 中断控制器代码，使用GPIO中断             |
| 10-CRC&RNG           | STM32硬件CRC单元和随机数单元            |
| 11-SPI               | 使用STM32的SPI控制器读取norflash       |
| 12-SPI-Simulation    | 使用模拟SPI读取norflash               |
| 13-IIC               | 使用模拟IIC读取PCF8536             |
| 14-Flash-Program     | STM32内部Flash编程            |
| 15-Flash-Write-Protection| STM32内部Flash写保护       |
| 16-Flash-Read-Protection | STM32内部Flash读保护（**没有完成**）      |
| 17-DMA-M-To-M | DMA内存到内存传输 |
| 18-IWDG       | 独立看门狗 |
| 19-WWDG       | 窗口看门狗 |
| 20-Timer-Base | 定时器的基础定制中断功能 |
| 21-Timer-PWMOUT | 定时器的PWM输出功能 |
| 22-Timer-InputCapture | 定时器输入捕获功能 |
| 23-Timer-Touchpad | 触摸按键功能 |
| 24-ADC | ADC外设代码 |

