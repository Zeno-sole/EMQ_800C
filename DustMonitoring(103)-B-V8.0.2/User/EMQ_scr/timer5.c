
/*-------------------------------------------------*/
/*-----------------------------------------------------*/
/*           ��ʱ��5Դ�ļ�,��1������                   */
/*-----------------------------------------------------*/


#include "timer5.h"

/*-------------------------------------------------*/
/*����������ʱ��5��ʼ��   10s                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM5_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
			
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);           //ʹ��TIM5ʱ��	
    TIM_TimeBaseInitStructure.TIM_Period = 20000-1; 	              //�Զ���װ��ֵ 10s 20k/2k  
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1;              //��ʱ����Ƶ    2KHz
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);            //����������TIM5
	
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);                    //����жϱ�־λ
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);                      //����ʱ��4�����ж�    
	TIM_Cmd(TIM5,DISABLE);                                         
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;                 //��ʱ��5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;              //�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);                               //���������� 
}


