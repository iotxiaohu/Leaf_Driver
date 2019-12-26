#ifndef _LEAF_KEY_H_
#define _LEAF_KEY_H_

/* 每个按键的参数 */
typedef struct key_node
{
	unsigned char PressLevel:1;		//按下时的电平
	unsigned char level:1;				//表示当前是高电平或者低电平
	unsigned char prelevel:1;			//表示当前是高电平或者低电平
	unsigned short time[5];				//当前电平持续时间
	unsigned short LongTime;			//设置的长按时间
	
	int (*ReadLevel)(void);				//读取电平状态
	void (*ShortPress_cb)(void);		//短按回调函数
	void (*LongPress_cb)(void);		//长按回调函数
	struct key_node *next;
}KeyNode;

#define GetKeyNode(name) &KeyNode_##name	//结构体地址
#define new_key(name) KeyNode KeyNode_##name;


/* 按键结构体 */
typedef struct key_device
{
	KeyNode *front;
	KeyNode *rear;
}Key_Device;


void key_add_ReadLevel(KeyNode *node, int (*ReadLevel)(void));
void key_add_PressLevel(KeyNode *node, unsigned char PressLevel);
void key_add_ShortPress_cb(KeyNode *node, void (*ShortPress_cb)(void));
void key_add_LongPress_cb(KeyNode *node, void (*ShortPress_cb)(void), unsigned short time_ms);


void key_transfer(void);




void m_key_test(void);
#endif
