/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "timer2.h"

/*-------------------------------------------------*/
/*函数名：定时器2初始化  45s                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_Cmd(TIM2,DISABLE);                              //关定时器22
	TIM_SetCounter(TIM2, 0);                            //清定时器
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);           //使能TIM2时钟	
    TIM_TimeBaseInitStructure.TIM_Period = 40000-1; 	          //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1;              //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1分频
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);            //按配置设置TIM2
	
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);                    //清除中断标志位
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                      //允许定时器2更新中断    
//	TIM_Cmd(TIM2,ENABLE);                                         
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;                 //定时器2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;    //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;           //子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //使能中断
	NVIC_Init(&NVIC_InitStructure);                               //按配置设置 
}


