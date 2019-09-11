#include "bsp.h"



/**
  * @brief  Get The specified string value.
  * @param  tgt: Target string buffer.
  * @param  numofsize: Target string buffer size.
  * @param  src: Source string buffer.
  * @param  str: To find string label.
  * @retval Get success return 1 , else return 0.
  */
uint8_t GetString(char *tgt,uint16_t numofsize,const char *src,const char *str)
{
	int32_t beginPos,endPos; //defined signed type
	uint32_t i,len;
	
	beginPos = StringFind((char *)src,str);
	
	if(beginPos != -1){ //确认有没有查到,没有查到返回-1
		
		beginPos = beginPos + strlen(str);//查找开始的位置
		for(i=beginPos;i<strlen(src);i++){
			
			if(src[i]=='\r'){ //查找回车符作为结束符
				endPos=i;//保存\r的位置作为结束的位置
				break;
			}
			
		}
		if(endPos>beginPos){
			
			len = endPos-beginPos; //计算要截取长度
			if(len < numofsize){ //防止内存溢出
				
				mid(tgt,(char *)src,len,beginPos);
				return 1;
			}
			else{
				return 0;
			}
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

/*------------- 去除字符串左右两边的空格 --------------*/
void trim(char *s){
	
	char *start;  
	char *end;  
	int len = strlen(s);  
		
	start = s;  
	end = s + len - 1;  

	while(1){
		
		char c = *start;  
		if (!isspace(c))  
		break;  

		start++;  
		if(start > end){     
			s[0] = '\0';  
			return;  
		}     
	}     

	while(1){
		
		char c = *end;  
		if (!isspace(c))  
				break;  

		end--;  
		if(start > end){  
			s[0] = '\0';  
			return;  
		}  
	}  

	memmove(s, start, end - start + 1);  
	s[end - start + 1] = '\0';  
}  


/*------------- 从字符串的左边截取n个字符 --------------*/
char * left(char *dst,char *src, int n)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len;
    /*p += (len-n);*/   /*从右边第n字符开始*/
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; /*有必要吗?很有必要*/
    return dst;
}

/*------------- 从字符串的中间截取n个字符 --------------*/
char * mid(char *dst,char *src, int nLen,int nPos)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(nLen>len) nLen = len-nPos;    /*从第nPos个到最后*/
    if(nPos<0) nPos=0;    /*从第一个开始*/
    if(nPos>len) return NULL;
    p += nPos;
    while(nLen--) *(q++) = *(p++);
    *(q++)='\0'; /*有必要吗?很有必要*/
    return dst;
}

/*------------- 从字符串的右边截取n个字符 --------------*/
char * right(char *dst,char *src, int nLen)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(nLen>len) nLen = len;
    p += (len-nLen);   /*从右边第n个字符开始*/
    while(*(q++) == *(p++));
    return dst;
}

//find string in string, return the first start location or -1 if can not find  
int32_t StringFind(const char *pSrc, const char *pDst)  
{
    int i, j;  
    for (i=0; pSrc[i]!='\0'; i++)  
    {  
        if(pSrc[i]!=pDst[0])  
            continue;         
        j = 0;  
        while(pDst[j]!='\0' && pSrc[i+j]!='\0')  
        {  
            j++;  
            if(pDst[j]!=pSrc[i+j])
            break;
        }
        if(pDst[j]=='\0')
            return i;
    }
    return -1;  
}

void CutString(char *pSrc, char *pDst, uint8_t maxSize)
{
	uint8_t nums= 0;
	char *result = NULL;
	result = strtok(pSrc, " ");
	while( result != NULL )
	{
		nums++;
		if(nums == 2){
			if(strlen(result) > 0 && strlen(result) < maxSize){
				trim(result);
				strcpy(pDst, result);
			}
		}
		result = strtok(NULL, " ");
	}
}

/*将十六进制的字符串转成整数*/  
int htoi(char *s)
{
	int i;
	int n = 0;
	if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
	{
		i = 2;
	}
	else
	{
		i = 0;
	}
	for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
	{
		if (tolower(s[i]) > '9')
		{
			n = 16 * n + (10 + tolower(s[i]) - 'a');
		}
		else
		{
			n = 16 * n + (tolower(s[i]) - '0');
		}
	}
	return n;
}

