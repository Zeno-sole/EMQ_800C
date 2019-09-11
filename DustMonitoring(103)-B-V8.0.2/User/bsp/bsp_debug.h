#ifndef  BSP_DEBUG
#define  BSP_DEBUG

//#define      FUNC_DEBUG_
//#define    GPRS_DEBUG_


#ifdef FUNC_DEBUG_
		#define FUNC_DEBUG(format, ...) printf (format, ##__VA_ARGS__)
#else
    #define FUNC_DEBUG(format,...)
#endif

#ifdef GPRS_DEBUG_
		#define GPRS_DEBUG(format, ...) printf (format, ##__VA_ARGS__)
#else
    #define GPRS_DEBUG(format,...)
#endif


#endif /* BSP_DEBUG */


