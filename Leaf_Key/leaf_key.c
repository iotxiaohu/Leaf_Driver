#include "leaf_key.h"
#include <stdlib.h>

#include "main.h"

Key_Device Key_Device_t = {NULL, NULL};	//ȫ�ֱ���


/* ��Ӷ�ȡ��ƽ�ĺ���
 * @description ˳��͵͵�Զ��н��г�ʼ��,����
 * @prame �ڵ�
 * @prame ��ȡ����
 */
void key_add_ReadLevel(KeyNode *node, int (*ReadLevel)(void))
{
	node->ReadLevel = ReadLevel;
	node->next = NULL;
	
	//�޽ڵ�--->����������Ϊ��һ���ڵ�
	//�нڵ�--->����������ӵ�����
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

/* ��Ӱ��µĵ�ƽ 
 * @description ���ð��µĵ�ƽ
 * @prame �ڵ�
 * @prame ���µĵ�ƽ,1--�ߵ�ƽ, 0--�͵�ƽ
 */
void key_add_PressLevel(KeyNode *node, unsigned char PressLevel)
{
	node->PressLevel = PressLevel;
}

/* ��Ӷ̰����� */
void key_add_ShortPress_cb(KeyNode *node, void (*ShortPress_cb)(void))
{
	node->ShortPress_cb = ShortPress_cb;
}

/* ��������
 * @description ���ð��µĵ�ƽ
 * @prame �ڵ�
* @prame ���µ�ʱ��, ��λ: ����(10ms)
 */
void key_add_LongPress_cb(KeyNode *node, void (*ShortPress_cb)(void), unsigned short time_cs)
{
	node->LongPress_cb = ShortPress_cb;
	node->LongTime = time_cs;
}


/* ɨ���ж� */
void key_transfer(void)
{
	KeyNode *p = Key_Device_t.front;
	for(; p != NULL; p = p->next)
	{
		//��ȡ��ǰ��ƽ
		p->level = p->ReadLevel();
		
		if(p->level == p->prelevel)
		{
			/*����һ�����, ������ʱ��*/
			if(p->time[0]<0xFFFF) p->time[0]++;
		}
		else
		{
			/* ����һ�β���, �ʹ��¼��� */
			p->time[4] = p->time[3];
			p->time[3] = p->time[2];
			p->time[2] = p->time[1];
			p->time[1] = p->time[0];
			p->time[0] = 1;
		}
		
		/* �жϰ�����״̬ �̰�*/
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
/*===========================��������============================*/
/*===============================================================*/

/* ��ȡ��ƽ�ĺ��� */
static int read_key1(void)
{
	return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
}

/* ���Զ̰� */
static void key1_ShortPress_cb(void)
{
	printf("Key1 Short Press\n");
	return;
}

/* ���Գ��� */
static void key1_LongPress_cb(void)
{
	printf("Key1 Long Press\n");
	return;
}
/*======================================================*/



void m_key_test(void)
{
	new_key(key1);															//��� һ������
	key_add_ReadLevel(GetKeyNode(key1), &read_key1);			//���� "��ȡ��ƽ����"
	key_add_PressLevel(GetKeyNode(key1), 0);						//���� "���µĵ�ƽ"
	key_add_ShortPress_cb(GetKeyNode(key1), &key1_ShortPress_cb);				//���� "�̰��ص�����"	
	key_add_LongPress_cb(GetKeyNode(key1), &key1_LongPress_cb, 500);			//���� "�����ص�����"	
	
	while(1)
	{
		key_transfer();
		HAL_Delay(10);
	}
}


