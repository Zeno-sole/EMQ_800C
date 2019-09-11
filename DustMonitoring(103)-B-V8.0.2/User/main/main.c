#include "bsp.h"

OS_TCB				  AppTaskStartTCB;
OS_TCB          AppTaskCollectionTCB;
OS_TCB          AppTaskSendTCB;
OS_TCB          AppTaskGET1TCB;

static CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static CPU_STK  AppTaskCollectionStk[APP_CFG_TASK_COLLECTION_STK_SIZE];
static CPU_STK  AppTaskSendStk[APP_CFG_TASK_SEND_STK_SIZE];
__align(8) static CPU_STK  AppTaskGET1Stk[APP_CFG_TASK_Get1_SIZE];

static void	  	AppTaskStart(void  *p_arg);       //��Ӧ�ó�������
static void			AppTaskCollection(void  *p_arg);  //���ݲɼ�����
static void			AppTaskSend(void  *p_arg);  //���ݷ�������
static void			AppTaskGET1(void  *p_arg);         //
//OS_MUTEX     AppTaskObjMutex;//�����ź�������

int main(void){
	
    OS_ERR   err;
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif
		//.....
    Mem_Init();                                                 /* Initialize Memory Managment Module                   */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)"STM32F103VC",(CPU_ERR*)&cpu_err);
#endif

    BSP_IntDisAll();                                            /* Disable all Interrupts.                              */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */
    App_OS_SetAllHooks();

    OSTaskCreate(&AppTaskStartTCB,                              /* Create the start task                                */
                  "App Task Start",
                  AppTaskStart,
                  0,
                  APP_CFG_TASK_START_PRIO,
                  &AppTaskStartStk[0u],
                  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                  APP_CFG_TASK_START_STK_SIZE,
                  0,
                  0,
                  0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                  &err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    while(1){                                                   /* Should Never Get Here.                               */
    }
}

/**
 * @brief  Ӧ�ó���GPRS Poll����
 * @param  ��
 * @retval ��
 */
static  void  AppTaskStart (void *p_arg){
	
	OS_ERR  os_err;
	(void)p_arg;
	
	BSP_Init();                                                 /* Initialize BSP functions                             */
	CPU_Init();                                                 /* Initialize the uC/CPU services                       */

	#if OS_CFG_STAT_TASK_EN > 0u
		OSStatTaskCPUUsageInit(&os_err);                          /* CPUͳ�������ʼ�� */
	#endif

	#ifdef CPU_CFG_INT_DIS_MEAS_EN
		CPU_IntDisMeasMaxCurReset();
	#endif
	
	//OSMutexCreate(&AppTaskObjMutex,"Mutex Test",&os_err);//���������ź���
	/*-------------------------------------------------------------------------------*/
	OSTaskCreate(&AppTaskCollectionTCB,
								"App Task Collection TCB",
								AppTaskCollection,
								0,
								APP_CFG_TASK_COLLECTION_PRIO,
								&AppTaskCollectionStk[0],
								AppTaskCollectionStk[APP_CFG_TASK_COLLECTION_STK_SIZE / 10u],
								APP_CFG_TASK_COLLECTION_STK_SIZE,
								0,
								0,
								0,
								(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
								&os_err);
	/*-------------------------------------------------------------------------------*/						
	OSTaskCreate(&AppTaskSendTCB,
								"App Task Send TCB",
								AppTaskSend,
								0,
								APP_CFG_TASK_SEND_PRIO,
								&AppTaskSendStk[0],
								AppTaskSendStk[APP_CFG_TASK_SEND_STK_SIZE / 10u],
								APP_CFG_TASK_SEND_STK_SIZE,
								0,
								0,
								0,
								(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
								&os_err);
	/*-------------------------------------------------------------------------------*/

	OSTaskCreate(&AppTaskGET1TCB,
								"App Task GET1 TCB",
								AppTaskGET1,
								0,
								APP_CFG_TASK_Get1_PRIO,
								&AppTaskGET1Stk[0],
								AppTaskGET1Stk[APP_CFG_TASK_Get1_SIZE / 10u],
								APP_CFG_TASK_Get1_SIZE,
								0,
								0,
								0,
								(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
								&os_err);	
	/*-------------------------------------------------------------------------------*/	
	ModbusMaster_begin();//��ʼ��Modbus����
	OSTaskSuspend((OS_TCB*)&AppTaskGET1TCB,&os_err);//��������2
	while(1){
		ParseCommand_Config();
		OSTimeDlyHMSM(0u, 0u, 0u, 30u, OS_OPT_TIME_HMSM_STRICT, &os_err);	
	}
}

/**
 * @brief  ���ݲɼ�������LED��ʾ��
 * @param  ��
 * @retval ��
 */
static void AppTaskCollection(void* p_arg)
{
	OS_ERR  os_err;
 EMQ_Connect();
	while(1){
		
		EMQ_POLL();
	
		OSTimeDlyHMSM(0u, 0u, 0u, 60u, OS_OPT_TIME_HMSM_STRICT, &os_err);
	}
}


static void AppTaskSend(void* p_arg)
{
	OS_ERR  os_err;
	
	OSTimeDlyHMSM(0u, 0u, 10u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
	DisplayNumberMode();//��ʾ��ź�ģʽ
	OSTimeDlyHMSM(0u, 0u, 2u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);

	while(1){
		
		UpdateDisplay();
//		printf("AppTaskSend\r\n");
//		ParsePM25Command();
		OSTimeDlyHMSM(0u, 0u, 0u, 60u, OS_OPT_TIME_HMSM_STRICT, &os_err);
	}
}
static void AppTaskGET1(void* p_arg)
{
	OS_ERR  os_err;
	
	printf("EMQ_Connect\r\n");
	while(1){
		


		OSTimeDlyHMSM(0u, 0u, 0u, 200u, OS_OPT_TIME_HMSM_STRICT, &os_err);
	}
}
