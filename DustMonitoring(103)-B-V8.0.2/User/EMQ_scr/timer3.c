/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "timer3.h"

/*-------------------------------------------------*/
/*函数名：定时器3初始化   30ms定时中断             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);           //使能TIM3时钟	
    TIM_TimeBaseInitStructure.TIM_Period = 300-1; 	              //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=7200-1;               //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1分频
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);            //按配置设置TIM3
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);                     //清除中断标志位
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                       //允许定时器3更新中断
	TIM_Cmd(TIM3,DISABLE);                                         //先关闭定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;                 //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;       //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;              //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //使能中断
	NVIC_Init(&NVIC_InitStructure);                               //按配置设置 
}


