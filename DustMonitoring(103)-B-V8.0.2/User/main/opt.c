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
	
	if(beginPos != -1){ //ȷ����û�в鵽,û�в鵽����-1
		
		beginPos = beginPos + strlen(str);//���ҿ�ʼ��λ��
		for(i=beginPos;i<strlen(src);i++){
			
			if(src[i]=='\r'){ //���һس�����Ϊ������
				endPos=i;//����\r��λ����Ϊ������λ��
				break;
			}
			
		}
		if(endPos>beginPos){
			
			len = endPos-beginPos; //����Ҫ��ȡ����
			if(len < numofsize){ //��ֹ�ڴ����
				
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

/*------------- ȥ���ַ����������ߵĿո� --------------*/
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


/*------------- ���ַ�������߽�ȡn���ַ� --------------*/
char * left(char *dst,char *src, int n)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len;
    /*p += (len-n);*/   /*���ұߵ�n�ַ���ʼ*/
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; /*�б�Ҫ��?���б�Ҫ*/
    return dst;
}

/*------------- ���ַ������м��ȡn���ַ� --------------*/
char * mid(char *dst,char *src, int nLen,int nPos)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(nLen>len) nLen = len-nPos;    /*�ӵ�nPos�������*/
    if(nPos<0) nPos=0;    /*�ӵ�һ����ʼ*/
    if(nPos>len) return NULL;
    p += nPos;
    while(nLen--) *(q++) = *(p++);
    *(q++)='\0'; /*�б�Ҫ��?���б�Ҫ*/
    return dst;
}

/*------------- ���ַ������ұ߽�ȡn���ַ� --------------*/
char * right(char *dst,char *src, int nLen)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(nLen>len) nLen = len;
    p += (len-nLen);   /*���ұߵ�n���ַ���ʼ*/
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

/*��ʮ�����Ƶ��ַ���ת������*/  
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

