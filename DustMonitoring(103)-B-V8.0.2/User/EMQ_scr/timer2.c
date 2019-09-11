/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "timer2.h"

/*-------------------------------------------------*/
/*����������ʱ��2��ʼ��  45s                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_Cmd(TIM2,DISABLE);                              //�ض�ʱ��22
	TIM_SetCounter(TIM2, 0);                            //�嶨ʱ��
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);           //ʹ��TIM2ʱ��	
    TIM_TimeBaseInitStructure.TIM_Period = 40000-1; 	          //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1;              //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);            //����������TIM2
	
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);                    //����жϱ�־λ
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                      //����ʱ��2�����ж�    
//	TIM_Cmd(TIM2,ENABLE);                                         
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;                 //��ʱ��2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;    //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;           //�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);                               //���������� 
}


