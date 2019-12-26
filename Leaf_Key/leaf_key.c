#include "leaf_key.h"
#include <stdlib.h>

#include "main.h"

Key_Device Key_Device_t = {NULL, NULL};	//全局变量


/* 添加读取电平的函数
 * @description 顺便偷偷对队列进行初始化,哈哈
 * @prame 节点
 * @prame 读取函数
 */
void key_add_ReadLevel(KeyNode *node, int (*ReadLevel)(void))
{
	node->ReadLevel = ReadLevel;
	node->next = NULL;
	
	//无节点--->把新来的作为第一个节点
	//有节点--->把新来的添加到后面
	if(Key_Device_t.front == NULL)
	{
		Key_Device_t.front = node;
		Key_Device_t.rear = node;
	}
	else
	{
		Key_Device_t.rear->next = node;
		Key_Device_t.rear = node;
	}
}

/* 添加按下的电平 
 * @description 设置按下的电平
 * @prame 节点
 * @prame 按下的电平,1--高电平, 0--低电平
 */
void key_add_PressLevel(KeyNode *node, unsigned char PressLevel)
{
	node->PressLevel = PressLevel;
}

/* 添加短按功能 */
void key_add_ShortPress_cb(KeyNode *node, void (*ShortPress_cb)(void))
{
	node->ShortPress_cb = ShortPress_cb;
}

/* 添长按功能
 * @description 设置按下的电平
 * @prame 节点
* @prame 按下的时间, 单位: 厘秒(10ms)
 */
void key_add_LongPress_cb(KeyNode *node, void (*ShortPress_cb)(void), unsigned short time_cs)
{
	node->LongPress_cb = ShortPress_cb;
	node->LongTime = time_cs;
}


/* 扫描判断 */
void key_transfer(void)
{
	KeyNode *p = Key_Device_t.front;
	for(; p != NULL; p = p->next)
	{
		//获取当前电平
		p->level = p->ReadLevel();
		
		if(p->level == p->prelevel)
		{
			/*跟上一次相等, 就增加时间*/
			if(p->time[0]<0xFFFF) p->time[0]++;
		}
		else
		{
			/* 跟上一次不等, 就从新计数 */
			p->time[4] = p->time[3];
			p->time[3] = p->time[2];
			p->time[2] = p->time[1];
			p->time[1] = p->time[0];
			p->time[0] = 1;
		}
		
		/* 判断按键的状态 短按*/
		if((p->level == p->PressLevel)&&(p->time[0] == 2)&&(p->time[1] > 2)&&(p->ShortPress_cb != 0))
		{
			p->ShortPress_cb();
		}
		if((p->level == p->PressLevel)&&(p->time[0] == p->LongTime)&&(p->LongPress_cb != 0))
		{
			p->LongPress_cb();
		}		
		p->prelevel = p->level;
	}
}





/*===============================================================*/
/*===========================测试例程============================*/
/*===============================================================*/

/* 读取电平的函数 */
static int read_key1(void)
{
	return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
}

/* 测试短按 */
static void key1_ShortPress_cb(void)
{
	printf("Key1 Short Press\n");
	return;
}

/* 测试长按 */
static void key1_LongPress_cb(void)
{
	printf("Key1 Long Press\n");
	return;
}
/*======================================================*/



void m_key_test(void)
{
	new_key(key1);															//添加 一个按键
	key_add_ReadLevel(GetKeyNode(key1), &read_key1);			//设置 "读取电平函数"
	key_add_PressLevel(GetKeyNode(key1), 0);						//设置 "按下的电平"
	key_add_ShortPress_cb(GetKeyNode(key1), &key1_ShortPress_cb);				//设置 "短按回调函数"	
	key_add_LongPress_cb(GetKeyNode(key1), &key1_LongPress_cb, 500);			//设置 "长按回调函数"	
	
	while(1)
	{
		key_transfer();
		HAL_Delay(10);
	}
}


