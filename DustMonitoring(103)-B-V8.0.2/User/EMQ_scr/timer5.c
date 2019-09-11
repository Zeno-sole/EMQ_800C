
/*-------------------------------------------------*/
/*-----------------------------------------------------*/
/*           定时器5源文件,有1个函数                   */
/*-----------------------------------------------------*/


#include "timer5.h"

/*-------------------------------------------------*/
/*函数名：定时器5初始化   10s                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM5_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
			
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);           //使能TIM5时钟	
    TIM_TimeBaseInitStructure.TIM_Period = 20000-1; 	              //自动重装载值 10s 20k/2k  
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1;              //定时器分频    2KHz
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1分频
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);            //按配置设置TIM5
	
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);                    //清除中断标志位
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);                      //允许定时器4更新中断    
	TIM_Cmd(TIM5,DISABLE);                                         
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;                 //定时器5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;              //子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //使能中断
	NVIC_Init(&NVIC_InitStructure);                               //按配置设置 
}


