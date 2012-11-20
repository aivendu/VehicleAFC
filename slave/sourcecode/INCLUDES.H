/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name: includes.h
** Last modified Date:  2004-05-20
** Last Version: 1.0
** Descriptions: uC/OS-II Include File
**
**------------------------------------------------------------------------------------------------------
** Created By: Chenmingji
** Created date:   2004-05-20
** Version: 1.0
** Descriptions: First version
**
**------------------------------------------------------------------------------------------------------
** Modified by: Chenmingji
** Modified date: 2004-09-17
** Version: 1.02
** Descriptions: Change for IRQ.S; Support C++
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
********************************************************************************************************/
/********************************/
/*      uC/OS-II的特殊代码       */
/*      uC/OS-II specital code  */
/********************************/
#ifndef __INCLUDES_H 
#define __INCLUDES_H
	
#include   "config.h" 

	#ifdef __cplusplus
	extern "C" {
	#endif
#include    "arm\os_cpu.h"
#include    "os\os_cfg.h"
#include    "os\ucos_ii.h"
	#ifdef __cplusplus
	}
	#endif

#include    <string.h>
   
   /********************************/
   /*	   ARM的特殊代码		   */
   /*	   ARM specital code	   */
   /********************************/
   //这一段无需改动
   //This segment should not be modify
   
#include    "LPC2294.h" 
   
   /********************************/
   /*	  应用程序配置			   */
   /*Application Program Configurations*/
   /********************************/
   //以下根据需要改动
   //This segment could be modified as needed.
#include    <stdio.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <setjmp.h>
#include    <rt_misc.h> 
   
   
   /********************************/
   /*	  本例子的配置			   */
   /*Configuration of the example */
   /********************************/
#include "queue.h"
#include "head.h"
#include "comm.h"
#include "config_manage.h"
#include "uart0.h"
#include "uart1.h"
#include "i2c0.h"
#include "spi0.h"
#include "eeprom.h"
#include "io_driver.h"
#include "test.h"
#include "ic_card.h"
#include "device_state.h"
#include "sys_state.h"
#include "target.h"              //This line may not be deleted 这一句不能删除
#include "note_machine.h"
#include "print_machine.h"
#include "coin_machine.h"
#include "crt_188_dy1.h"
#include "gps.h"
#include "trade.h"
#include "syn6288.h"
#include "chip_communication.h"
//#include "isd1760.h"
   /* 由于不同情况中断OSIntExit()对堆栈的使用情况不同，必须加上这个宏定义    */
   /* for any types of interrupt , OSIntExit() using the stacks is different , so ,must add this define*/
#define OSIntCtxSw()    return

#endif
/**********************end****************************************/
