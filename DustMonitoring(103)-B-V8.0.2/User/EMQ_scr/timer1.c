/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "timer1.h"

/*-------------------------------------------------*/
/*����������ʱ��1��ʼ�� 15s                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Ping_TIM1Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	TIM_Cmd(TIM1,DISABLE);                                        //�ض�ʱ��1
	TIM_SetCounter(TIM1, 0);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);           //ʹ��TIM1ʱ��	
    TIM_TimeBaseInitStructure.TIM_Period = 15000-1; 	          //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1;              //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);            //����������TIM1	
		
    TIM_ClearITPendingBit(TIM1,TIM_IT_Update);                    //����жϱ�־λ
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);                      //����ʱ��1�����ж�
	TIM_Cmd(TIM1,ENABLE);                                         //����ʱ��1  
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;              //��ʱ��1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;              //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);                               //���������� 	                                      
}

/*-------------------------------------------------*/
/*��������3��ping 2s                              */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Ping3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_Cmd(TIM1,DISABLE);                                        //�ض�ʱ��1 
	TIM_SetCounter(TIM1, 0); 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);           //ʹ��TIM1ʱ��	
    TIM_TimeBaseInitStructure.TIM_Period = 10000-1; 	          //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=7200-1;               //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);            //����������TIM1	
	
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);                    //����жϱ�־λ
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);                      //����ʱ��1�����ж�   
	TIM_Cmd(TIM1,ENABLE);                                         //����ʱ��1  
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;              //��ʱ��1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;              //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);                               //���������� 	                                      
}


