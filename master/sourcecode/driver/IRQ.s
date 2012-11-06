;/****************************************Copyright (c)**************************************************
;**                               Guangzou ZLG-MCU Development Co.,LTD.
;**                                      graduate school
;**                                 http://www.zlgmcu.com
;**
;**--------------File Info-------------------------------------------------------------------------------
;** File Name: IRQ.s
;** Last modified Date:  2004-06-14
;** Last Version: 1.1
;** Descriptions: The irq handle that what allow the interrupt nesting. 
;**
;**------------------------------------------------------------------------------------------------------
;** Created By: Chenmingji
;** Created date:   2004-09-17
;** Version: 1.0
;** Descriptions: First version
;**
;**------------------------------------------------------------------------------------------------------
;** Modified by:
;** Modified date:
;** Version:
;** Descriptions:
;**
;********************************************************************************************************/


		    INCLUDE		\driver\irq.inc			; Inport the head file 引入头文件

    CODE32

    AREA    IRQ,CODE,READONLY


;/* 以下添加中断句柄，用户根据实际情况改变 */
;/* Add interrupt handler here，user could change it as needed */

;/*中断*/
;/*Interrupt*/
IRQ_Handler	HANDLER IRQ_Exception
;Uart0 中断定义
Uart0_Handler  HANDLER   UART0_Exception
;Uart1 中断定义
Uart1_Handler  HANDLER   UART1_Exception
;spi0 中断定义
Spi0_Handler	HANDLER	Spi0_Exception
;i2c0 中断定义
I2c0_Handler	HANDLER	I2c0_Exception
;/*定时器0中断*/
;/*Time0 Interrupt*/
Timer0_Handler  HANDLER Timer0_Exception

    END
;/*********************************************************************************************************
;**                            End Of File
;********************************************************************************************************/
