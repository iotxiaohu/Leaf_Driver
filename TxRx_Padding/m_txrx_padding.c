/**
  ******************************************************************************
  * 
  * 使用说明
  *
  ******************************************************************************
  *
  * 1. 修改 User Code 1 中的自己头文件声明、定义自己的结构体
  * 2. 修改 User Code 2 中发送函数和帧处理函数
  * 3. 修改 User Code 3 中的宏定义,帧大小,队列深度等
  * 4. 修改 User Code 4 将自己定义的变量声明为外部变量
  * 5. 在自己的数据接收函数里面调用 StRx_push() 函数将收到的数据添加进队列 
  * 6. 在合适的地方调用 m_deal_receive() 函数处理接受到的数据
  * 7. 当要发送数据的时候 请调动 m_add_padding_send_data() 函数进行数据发送
  * 
  *
  ******************************************************************************
  */


#include "m_txrx_padding.h"

/*=========================User Code 1===========================*/

#include "main.h"	// 声明自己 User Code 2 中要用到函数的头文件

StRx StRx_t;		// 定义自己的结构体

/*---------------------------------------------------------------*/

#ifdef USING_attribute
 
 
 
 /**
  * @bieaf 初始化结构体参数(此函数在main之前执行) 
  * @detail 如果不能调用这个函数,就必须在最开始初始化这个函数
  *
  */
void __attribute__((constructor)) StRx_init(void)
{
	StRx_t.head = 0;
	StRx_t.tail = 0;
	StRx_t.state = 0;
	StRx_t.esc_flag = 0;
	StRx_t.soh_flag = 0;
	
/*=========================User Code 2===========================*/
	
	/* 函数初始化 */
	StRx_t.m_send = m_send_data;			//用户自己的 发送函数 
	StRx_t.m_deal_frame = m_deal_frame;		//用户自己的 处理函数 
	
/*---------------------------------------------------------------*/
	
	return;
}

#else

/* 初始化单个设备 */
void StRx_init(void)
{

	StRx_t.head = 0;
	StRx_t.tail = 0;
	StRx_t.state = 0;
	StRx_t.esc_flag = 0;
	StRx_t.soh_flag = 0;

/*=========================User Code 2===========================*/
	
	/* 函数初始化 */
	StRx_t.m_send = m_send_data;			//用户自己的 发送函数 
	StRx_t.m_deal_frame = m_deal_frame;		//用户自己的 处理函数 
/*---------------------------------------------------------------*/
	return;
}
#endif


 
 
/**
 * @bieaf 初始化结构体参数(此函数在main之前执行) 
 *
 * @param 结构体索引
 * @return 返回是否为满
 * 	@retval 0 非满
 * 	@retval 1 满
 */
static int StRx_full(StRx* Order)
{
	if (((Order->tail + 1) % QUENUE_MAX) == Order->head)
	{
		return 1;
	}
	return 0;
}




/**
 * @bieaf 判断是否为空 空返回1，满返回0
 *
 * @param 结构体索引
 * @return 返回是否为空
 * 	@retval 0 非空
 * 	@retval 1 空
 */
static int StRx_empty(StRx* Order)
{
	if (Order->head == Order->tail)
	{
		return 1;
	}
	return 0;
}



/**
 * @bieaf 添加数据
 *
 * @param 结构体索引
 * @param add_data		源数据
 * @param add_data_len	源数据大小
 * @return 返回本次操作的状态
 * 	@retval 0 操作失败
 * 	@retval 1 操作成功
 */
int StRx_push(StRx* Order, unsigned char *add_data, int add_data_len)
{
	int i = 0;
	
	if(add_data_len == 0) return 0;	// 为空
	
	for(i = 0; i< add_data_len; i++)
	{
		/* buf不满，就添加数据 */
		if(StRx_full(Order) == 0)
		{
			Order->buf[Order->tail] = add_data[i];
			Order->tail = (Order->tail + 1) % QUENUE_MAX;
		}
		else
		{
			return 0;		// 满了就添加不进去了
		}
	}
	return 1;				// 添加成功
}



/**
 * @bieaf 读取一个数据
 *
 * @param 结构体索引
 * @param 目的数据
 * @return 返回本次操作的状态
 * 	@retval 0 操作失败
 * 	@retval 1 操作成功
 */
static unsigned char StRx_pop(StRx* Order, unsigned char *des)
{
	if (StRx_empty(Order) == 1)
	{
		return 0;
	}
	*des = Order->buf[Order->head];
	Order->head = (Order->head + 1) % QUENUE_MAX;
	
	return 1;	
}


#ifdef ADD_SUM_CHECK



/**
 * @bieaf 对数据进行和校验
 *
 * @param tx_buf		源数据
 * @param tx_buf_len 源数据大小
 * @return 返回校验结果
 * 	@retval 0 校验失败
 * 	@retval 1 校验成功
 */
static int m_sum_check(unsigned char *tx_buf, int tx_buf_len)
{
	int i = 0;
	unsigned char sum = 0;
		
	for(i = 0; i<tx_buf_len-1;i++)
	{
		sum += tx_buf[i];
	}
	if(sum == tx_buf[tx_buf_len -1]) return 1;
	return 0;
}



/**
 * @bieaf 计算和校验
 * @detail 
 *
 * @param tx_buf 		源数据
 * @param tx_buf_len	源数据大小
 * @return 和校验的值
 */
 static unsigned char m_calculation_sum_check(unsigned char *tx_buf, int tx_buf_len)
{
	int i = 0;
	unsigned char sum = 0;
	for(i = 0; i<tx_buf_len; i++)
	{
		sum += tx_buf[i];
	}
	return sum;
}
#endif



/**
 * @bieaf 接收处理函数
 *
 * @param 结构体参数
 */
void m_deal_receive(StRx* Order)
{
	unsigned char head_data;
		
	while(StRx_pop(Order, &head_data))
	{
		if((Order->esc_flag != 1)&&(head_data == ASCII_SOH))		//【无ESC】+【SOH】
		{
				/* 头 */
				Order->soh_flag = 1;
				Order->cacheRx_len = 0;
		}
		else if((Order->esc_flag != 1)&&(head_data == ASCII_ESC))	//【无ESC】+【ESC】
		{
				/* 不管 */
				Order->esc_flag = 1;
		}
		else if((Order->esc_flag != 1)&&(head_data == ASCII_EOT))	//【无ESC】+【EOT】
		{
			/* 尾巴 */
			if(Order->soh_flag == 1)
			{
				/* 一帧数据完结 */
				
#ifdef ADD_SUM_CHECK			//使用和校验
				
				/* 检查和校验 */
				if(m_sum_check(Order->cacheRx, Order->cacheRx_len) == 1)
				{					
					Order->m_deal_frame(Order->cacheRx, Order->cacheRx_len -1);
				}
#else
				Order->m_deal_frame(Order->cacheRx, Order->cacheRx_len);
#endif
						
				Order->cacheRx_len = 0;
				Order->soh_flag = 0;
			}
		}
		else
		{
			Order->cacheRx[Order->cacheRx_len % FRAME_MAX] = head_data;
			Order->cacheRx_len++;
			Order->esc_flag = 0;
		}
	}
}



/**
 * @bieaf 进行字符填充之后再发送出去
 *
 * @param Order 		结构体参数
 * @param tx_buf 		发送的数据指针
 * @param tx_buf_len 发送的数据大小
 */
void m_add_padding_send_data(StRx* Order, unsigned char *tx_buf, int tx_buf_len)
{
	int state = 0;
	int i = 0;
	unsigned char sum = 0;

	if(tx_buf_len > FRAME_MAX) return;	//长度太长,就发送失败
	
#ifdef ADD_SUM_CHECK					//使用和校验
	
	/* 进行和校验 */
	sum = m_calculation_sum_check(tx_buf, tx_buf_len);			
	
#endif	
	
	/* 添加帧头 */
	Order->cacheTx[state] = ASCII_SOH;
	state++;
	
	/* 对数据进行填充 */
	for(i = 0; i<tx_buf_len; i++)
	{
		if((tx_buf[i]==ASCII_SOH)||(tx_buf[i]==ASCII_EOT)||(tx_buf[i]==ASCII_ESC))
		{
			Order->cacheTx[state] = ASCII_ESC;
			state++;
		}
		Order->cacheTx[state] = tx_buf[i];
		state++;
	}
		
#ifdef ADD_SUM_CHECK			//使用和校验
	
	/* 添加和校验 */
	if((sum == ASCII_SOH)||(sum == ASCII_EOT)||(sum == ASCII_ESC))
	{
		Order->cacheTx[state] = ASCII_ESC;
		state++;
	}
	Order->cacheTx[state] = sum;
	state++;
	
#endif
	
	/* 添加帧尾 */
	Order->cacheTx[state] = ASCII_EOT;
	state++;
	
	/* 开始发送 */
	Order->m_send(Order->cacheTx, state);
	
	return;
}



/**
 * @bieaf 测试例程
 *
 */
void text_example(void)
{
	/* 适当的时侯调用这个函数, 建议 10ms 一次 */
	m_deal_receive(&StRx_t);
}

