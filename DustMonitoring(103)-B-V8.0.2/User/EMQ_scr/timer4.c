/*-------------------------------------------------*/
/*-------------------------------------------------*/

#include "timer4.h"

/*-------------------------------------------------*/
/*函数名：定时器4初始化   30s                      */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM4_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	TIM_Cmd(TIM4,DISABLE);                              //关定时器22
	TIM_SetCounter(TIM4, 0);                            //清定时器	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);           //使能TIM4时钟	
  TIM_TimeBaseInitStructure.TIM_Period = 60000-1; 	          //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=54000-1;              //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1分频
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);            //按配置设置TIM4
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);                    //清除中断标志位
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                      //允许定时器4更新中断    
	TIM_Cmd(TIM4,ENABLE);                                         

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;                 //定时器4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;    //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;           //子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //使能中断
	NVIC_Init(&NVIC_InitStructure);                               //按配置设置 
}


