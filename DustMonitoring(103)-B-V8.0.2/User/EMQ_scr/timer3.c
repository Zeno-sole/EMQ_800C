/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "timer3.h"

/*-------------------------------------------------*/
/*����������ʱ��3��ʼ��   30ms��ʱ�ж�             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);           //ʹ��TIM3ʱ��	
    TIM_TimeBaseInitStructure.TIM_Period = 300-1; 	              //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=7200-1;               //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);            //����������TIM3
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);                     //����жϱ�־λ
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                       //����ʱ��3�����ж�
	TIM_Cmd(TIM3,DISABLE);                                         //�ȹرն�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;                 //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;       //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;              //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);                               //���������� 
}


