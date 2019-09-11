/*-------------------------------------------------*/

/*-------------------------------------------------*/
/*-----------------------------------------------------*/
/*           定时器6源文件,有1个函数                   */
/*-----------------------------------------------------*/


#include "timer6.h"

/*-------------------------------------------------*/
/*函数名：定时器6初始化  45s                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_Cmd(TIM6,DISABLE);                              //关定时器6
	TIM_SetCounter(TIM6, 0);                            //清定时器
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);           //使能TIM6时钟	
    TIM_TimeBaseInitStructure.TIM_Period = 20000-1; 	              //自动重装载值 10s 20k/2k  
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1;              //定时器分频    2KHz
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1分频
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);            //按配置设置TIM6
	
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);                    //清除中断标志位
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);                      //允许定时器4更新中断    
	TIM_Cmd(TIM6,ENABLE);                                         
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;                 //定时器6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;              //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //使能中断
	NVIC_Init(&NVIC_InitStructure);                               //按配置设置 
}


