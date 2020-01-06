#ifndef _LEAF_FLASH_H_
#define _LEAF_FLASH_H_


//#define LEAF_FLASH_TEST


#define STM32F103_RB
//#define STM32F103_RC
//#define STM32F103_ZE


#ifdef STM32F103_RB
#define PAGE_SIZE	FLASH_PAGE_SIZE			///<页大小
#define PAGE_NUM	128							///<页数
#define PAGE120 0x0801E000
#define PAGE121 0x0801E400
#define PAGE122 0x0801E800
#define PAGE123 0x0801EC00
#define PAGE124 0x0801F000
#define PAGE125 0x0801F400
#define PAGE126 0x0801F800
#define PAGE127 0x0801FC00
#endif

#ifdef STM32F103_RC
#define PAGE_SIZE	FLASH_PAGE_SIZE			///<页大小
#define PAGE_NUM	128							///<页数
#define PAGE120 0x0803C000
#define PAGE121 0x0803C800
#define PAGE122 0x0803D000
#define PAGE123 0x0803D800
#define PAGE124 0x0803E000
#define PAGE125 0x0803E800
#define PAGE126 0x0803F000
#define PAGE127 0x0803F800

#endif



/* 定义一个结构体 */
typedef struct
{
	unsigned int head_addr;	///<变量存储的起始页
	unsigned int page_num;	///<变量存储页大小
}L_node;



/* 读取函数 */
int Read_LeafFlash(L_node node, unsigned char* des, unsigned int size);



/* 写入函数 */
int Write_LeafFlash(L_node node, unsigned char *src, unsigned int size);


/* 测试函数 */
#ifdef LEAF_FLASH_TEST
void test(void);
#endif








#endif
