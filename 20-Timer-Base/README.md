# STM32F207VC-Template
通过SysTick实现LED灯闪烁



IAR版本：iar for arm 7.2.2  [点击下载](https://blog.csdn.net/firefly_cjd/article/details/77285224)

Keil版本：Keil 5 V5.25



**备注：**

为解决在IAR编译环境下类似Label 'Reset_Handler' is defined pubweak in a section implicitly declared root一系列错误，对每个向量表做以下修改
```
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0
        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER(1)
```
修改为
```
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0
        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
```