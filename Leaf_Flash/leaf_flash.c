#include "leaf_flash.h"
#include "main.h"



/* 擦除某页 */
int Erase_page(uint32_t pageaddr, uint32_t num)
{
	HAL_FLASH_Unlock();
	
	/* 2/4擦除FLASH*/
	FLASH_EraseInitTypeDef FlashSet;
	FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;
	FlashSet.PageAddress = pageaddr;
	FlashSet.NbPages = num;
	
	/*设置PageError，调用擦除函数*/
	uint32_t PageError = 0;
	HAL_FLASHEx_Erase(&FlashSet, &PageError);
	
	HAL_FLASH_Lock();
	return 1;
}



/* 写若干个数据 */
void WriteFlash(uint32_t addr, uint32_t * buff, int size)
{	
	/* 1/4解锁FLASH*/
	HAL_FLASH_Unlock();
	
	for(int i = 0; i<size; i++)	
	{
		/* 3/4对FLASH烧写*/
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + 4 * i, buff[i]);	
	}

	/* 4/4锁住FLASH*/
	HAL_FLASH_Lock();
}



/* 读若干个数据 */
void ReadFlash(uint32_t addr, uint8_t * buff, uint16_t byte_size)
{
	for(int i =0; i<byte_size; i++)
	{
		buff[i] = *(__IO uint8_t*)(addr + i);
	}
	return;
}



/* 读指定数据 */
int Read_LeafFlash(L_node node, unsigned char* des, unsigned int size)
{
	unsigned int addr;
	unsigned int data;
	
	uint32_t Head = size<<16|0x01;
	uint32_t Tail = size<<16|0x04;
	
	unsigned int word_size = (size + 3)/4;

	
	/* 寻找到头 */
	for(addr = node.head_addr + node.page_num * PAGE_SIZE - 4; addr >= node.head_addr; addr = addr - 4)
	{
		/* 读取一个数 */
		data = *(__IO uint32_t*)(addr);
		
		if(data == Tail)///<定位到头了
		{
			data = *(__IO uint32_t*)(addr - word_size * 4 - 4);
			if(data == Head)
			{
				addr = addr - word_size * 4 - 4;
				break;
			}
		}
	}
	
	/* 读数据 */
	ReadFlash(addr + 4, des, size);
	
	return 1;
}



/* 寻找可用地址 并且返回剩余可写的字数 */
unsigned int Find_write_addr(L_node node, unsigned int *addr)
{
	unsigned int data;
	unsigned int renum = 0;	
	
	for(addr[0] = node.head_addr + node.page_num * PAGE_SIZE  - 4; addr[0] >= node.head_addr; addr[0] -= 4)
	{
		/* 读取一个数 */
		data = *(__IO uint32_t*)(addr[0]);
		
		if(data != 0xFFFFFFFF)
		{
			addr[0] += 4;
			return renum;
		}
		renum++;
	}
	addr[0] = node.head_addr;
	return renum;
}



/* 写指定数据 */
int Write_LeafFlash(L_node node, unsigned char *src, unsigned int size)
{
	uint32_t Head = size<<16|0x01;
	uint32_t Tail = size<<16|0x04;
	uint32_t addr;
	
	/* 空间不够 */
	unsigned int word_size = (size + 3)/4;
	if(Find_write_addr(node, &addr) < word_size + 2)
	{
		Erase_page(node.head_addr, node.page_num);
		addr = node.head_addr;	
	}

	/*===============================开始写===================================*/
	HAL_FLASH_Unlock();
	
	/* 写头 */
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, Head);
	addr = addr + 4;
	
	/* 写数据 */
	for(int i = 0; i < word_size; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, ((uint32_t *)src)[i]);
		addr = addr + 4;
	}
	
	/* 写尾 */
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, Tail);
	HAL_FLASH_Lock();
	
	/*=============================确认写进去的没问题=========================*/
	/* 暂时不要 */
	return 1;
}



/* ==================================================== */
/* ==================================================== */
/* ===================下面是测试程序=================== */
/* ==================================================== */
/* ==================================================== */

#ifdef LEAF_FLASH_TEST

/* 测试结构体 */
void test_struct(void)///<测试结构体
{
	typedef struct
	{
		int a;
		uint8_t b;
		uint16_t c;
	}Test;
	
	Test test_a1 = {0x12345678, 0xab, 0x9876};
	Test test_a2;
	
	L_node test1 = {PAGE122	,1};
	
	Write_LeafFlash(test1, (unsigned char*)&test_a1, sizeof(test_a1));	
	if(Read_LeafFlash(test1, (unsigned char*)&test_a2, sizeof(test_a2)) == 1)
	{
		printf("test_a2.a = %x\n",test_a2.a);
		printf("test_a2.b = %x\n",test_a2.b);
		printf("test_a2.c = %x\n\n",test_a2.c);
	}	
}

/* 测试unsigned short 变量 */
void test_uint16(void)///<测试unsigned short
{
	unsigned short b1 = 0x7788;
	unsigned short b2;
	L_node test2 = {PAGE123	,1};
	Write_LeafFlash(test2, (unsigned char*)&b1, sizeof(b1));	
	if(Read_LeafFlash(test2, (unsigned char*)&b2, sizeof(b2)) == 1)
	{
		printf("b2 = %x\n\n",b2);
		
	}	
}

/* 测试int变量 */
void test_int(void)///<测试int
{
	int c1 = -235;
	int c2;
	L_node test3 = {PAGE124	,1};
	Write_LeafFlash(test3, (unsigned char*)&c1, sizeof(c1));	
	if(Read_LeafFlash(test3, (unsigned char*)&c2, sizeof(c2)) == 1)
	{
		printf("c2 = is:%d\n\n",c2);
	}
}

/* 测试unsigned char数组 */
void test_uint8(void)///<测试unsigned char*
{
	unsigned char d1[6] = {0x11,0x22,0x33,0x44,0x55,0x66};
	unsigned char d2[6];
	
	L_node test4 = {PAGE125	,1};
	Write_LeafFlash(test4, (unsigned char*)&d1, sizeof(d1));	
	if(Read_LeafFlash(test4, d2, sizeof(d2)) == 1)
	{
		printf("d2[] = {%x,%x,%x,%x,%x,%x}\n\n",d2[0],d2[1],d2[2],d2[3],d2[4],d2[5]);
	}	
}

/* 测试int数组 */
void test_uint32(void)///<测试unsigned int*
{
	unsigned int e1[5] = {1,3,5,7,9};
	unsigned int e2[5];
	
	L_node test5 = {PAGE126	,2};
	Write_LeafFlash(test5, (unsigned char*)&e1, sizeof(e1));
	
	if(Read_LeafFlash(test5, (unsigned char*)e2, sizeof(e2)) == 1)
	{
		printf("e2[] = {%d,%d,%d,%d,%d}\n\n",e2[0],e2[1],e2[2],e2[3],e2[4]);
	}
}



/* 测试程序入口 */
void test(void)
{	
	test_struct();
	test_uint16();
	test_int();
	test_uint8();
	test_uint32();
	
}
#endif

