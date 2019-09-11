/*-------------------------------------------------*/

/*-------------------------------------------------*/

#include "timer1.h"

/*-------------------------------------------------*/
/*函数名：定时器1初始化 15s                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Ping_TIM1Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	TIM_Cmd(TIM1,DISABLE);                                        //关定时器1
	TIM_SetCounter(TIM1, 0);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);           //使能TIM1时钟	
    TIM_TimeBaseInitStructure.TIM_Period = 15000-1; 	          //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=36000-1;              //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1分频
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);            //按配置设置TIM1	
		
    TIM_ClearITPendingBit(TIM1,TIM_IT_Update);                    //清除中断标志位
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);                      //允许定时器1更新中断
	TIM_Cmd(TIM1,ENABLE);                                         //开定时器1  
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;              //定时器1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;              //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //使能中断
	NVIC_Init(&NVIC_InitStructure);                               //按配置设置 	                                      
}

/*-------------------------------------------------*/
/*函数名：3连ping 2s                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Ping3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_Cmd(TIM1,DISABLE);                                        //关定时器1 
	TIM_SetCounter(TIM1, 0); 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);           //使能TIM1时钟	
    TIM_TimeBaseInitStructure.TIM_Period = 10000-1; 	          //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=7200-1;               //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1分频
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);            //按配置设置TIM1	
	
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);                    //清除中断标志位
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);                      //允许定时器1更新中断   
	TIM_Cmd(TIM1,ENABLE);                                         //开定时器1  
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;              //定时器1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;              //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //使能中断
	NVIC_Init(&NVIC_InitStructure);                               //按配置设置 	                                      
}


