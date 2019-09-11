/*-------------------------------------------------*/

/*-------------------------------------------------*/
/*-----------------------------------------------------*/
/*           ��ʱ��6Դ�ļ�,��1������                   */
/*-----------------------------------------------------*/


#include "timer6.h"

/*-------------------------------------------------*/
/*����������ʱ��6��ʼ��  45s                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_Cmd(TIM6,DISABLE);                              //�ض�ʱ��6
	TIM_SetCounter(TIM6, 0);                            //�嶨ʱ��
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);           //ʹ��TIM6ʱ��	
    TIM_TimeBaseInitStructure.TIM_Period = 20000-1; 	              //�Զ���װ��ֵ 10s 20k/2k  
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1;              //��ʱ����Ƶ    2KHz
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);            //����������TIM6
	
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);                    //����жϱ�־λ
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);                      //����ʱ��4�����ж�    
	TIM_Cmd(TIM6,ENABLE);                                         
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;                 //��ʱ��6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;              //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);                               //���������� 
}


