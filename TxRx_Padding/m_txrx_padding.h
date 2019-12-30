#ifndef _M_TXRX_PADDING_H_
#define _M_TXRX_PADDING_H_


#define ASCII_SOH 0x01	//开始
#define ASCII_EOT 0x04	//结束
#define ASCII_ESC 0x1B	//填充-转义

/*=========================User Code 3===========================*/

/***************************用户设置******************************/

#define USING_attribute								//能否使用__attribute__关键词
#define ADD_SUM_CHECK								//是否进行和校验

#define FRAME_MAX				100					/* 帧最大长度				*/
#define PADDING_FRAME_MAX	FRAME_MAX * 2 + 4 /* 字符填充之后帧最大长度	*/
#define QUENUE_MAX			256					/* 队列的深度				*/

/*---------------------------------------------------------------*/


/* 解析结构体 */
/*|----|****|****|****|****|----|----| */
/*|----|head|----|----|----|tail|----| */
typedef struct
{	
	int head;
	int state;
	int tail;
	unsigned char buf[QUENUE_MAX];
	unsigned char cacheRx[FRAME_MAX];				//解析出来的帧
	int cacheRx_len;
	unsigned char cacheTx[PADDING_FRAME_MAX];		//发送的帧
		
	int soh_flag;									//有帧头标志
	int esc_flag;									//转义标志	
	
	/* function */
	void (* m_send)(unsigned char *,int);			//用户的发送函数
	void (* m_deal_frame)(unsigned char *,int);		//用户的处理函数
}StRx;



/*=========================User Code 4===========================*/

/* 声明为外部变量 */
extern StRx StRx_t;

/*---------------------------------------------------------------*/

#ifndef USING_attribute
void StRx_init(void);
#endif



void m_add_padding_send_data(StRx* Order, unsigned char *tx_buf, int tx_buf_len);
int  StRx_push(StRx* Order, unsigned char *add_data, int add_data_len);
void m_deal_receive(StRx* Order);

#endif
