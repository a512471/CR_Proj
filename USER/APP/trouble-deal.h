#ifndef __TROUBLE_DEAL_H__
#define __TROUBLE_DEAL_H__

#include "includes.h" 

//////壁障元件标志位//////
#define IrDa1           0x0001 
#define IrDa2           0x0002 
#define IrDa3           0x0004 
#define IrDa4           0x0008 
#define IrDa5           0x0010

#define Touch_Switch1   0x0020
#define Touch_Switch2   0x0040
#define Touch_Switch3   0x0080
#define Touch_Switch4   0x0100

#define All_Trouble_Checkers 0x01FF //所有壁障器件的总数，每一位表示一个器件

void Trouble_Check_init(void);

#endif






