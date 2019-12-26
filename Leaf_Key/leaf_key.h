#ifndef _LEAF_KEY_H_
#define _LEAF_KEY_H_

/* ÿ�������Ĳ��� */
typedef struct key_node
{
	unsigned char PressLevel:1;		//����ʱ�ĵ�ƽ
	unsigned char level:1;				//��ʾ��ǰ�Ǹߵ�ƽ���ߵ͵�ƽ
	unsigned char prelevel:1;			//��ʾ��ǰ�Ǹߵ�ƽ���ߵ͵�ƽ
	unsigned short time[5];				//��ǰ��ƽ����ʱ��
	unsigned short LongTime;			//���õĳ���ʱ��
	
	int (*ReadLevel)(void);				//��ȡ��ƽ״̬
	void (*ShortPress_cb)(void);		//�̰��ص�����
	void (*LongPress_cb)(void);		//�����ص�����
	struct key_node *next;
}KeyNode;

#define GetKeyNode(name) &KeyNode_##name	//�ṹ���ַ
#define new_key(name) KeyNode KeyNode_##name;


/* �����ṹ�� */
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
