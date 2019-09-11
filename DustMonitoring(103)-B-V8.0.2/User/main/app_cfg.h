#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_PRIO                2
#define  APP_CFG_TASK_COLLECTION_PRIO           3
#define  APP_CFG_TASK_SEND_PRIO           			4
#define  APP_CFG_TASK_Get1_PRIO                 5
/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE            128u
#define  APP_CFG_TASK_COLLECTION_STK_SIZE       128u
#define  APP_CFG_TASK_SEND_STK_SIZE      				128u
#define  APP_CFG_TASK_Get1_SIZE              		128u
#endif
