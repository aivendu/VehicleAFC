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


		    INCLUDE		\driver\irq.inc			; Inport the head file ����ͷ�ļ�

    CODE32

    AREA    IRQ,CODE,READONLY


;/* ��������жϾ�����û�����ʵ������ı� */
;/* Add interrupt handler here��user could change it as needed */

;/*�ж�*/
;/*Interrupt*/
IRQ_Handler	HANDLER IRQ_Exception
;Uart0 �ж϶���
Uart0_Handler  HANDLER   UART0_Exception
;Uart1 �ж϶���
Uart1_Handler  HANDLER   UART1_Exception
;spi0 �ж϶���
Spi0_Handler	HANDLER	Spi0_Exception
;i2c0 �ж϶���
I2c0_Handler	HANDLER	I2c0_Exception
;/*��ʱ��0�ж�*/
;/*Time0 Interrupt*/
Timer0_Handler  HANDLER Timer0_Exception

    END
;/*********************************************************************************************************
;**                            End Of File
;********************************************************************************************************/
