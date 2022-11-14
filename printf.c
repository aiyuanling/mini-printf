//  #include <stdio.h>
// #include <float.h>
// #include <limits.h>
// #include <stdarg.h>
// #define bool char
// #define true 1
// #define false 0
// #if (CHAR_BIT==8)
// 	typedef  char                   int8_t  ;
// 	typedef  unsigned char          uint8_t ;
// #else
// 	#error sizeof(char)不等于1字节
// #endif
// #if (USHRT_MAX==65535)
// 	typedef  short                  int16_t ;
// 	typedef  unsigned short         uint16_t;
// #else
// 	#error sizeof(short)不等于2字节
// #endif

// #if (UINT_MAX==4294967295)
// 	typedef  int                    int32_t ;
// 	typedef  unsigned int           uint32_t;
// #elif (ULONG_MAX==4294967295)
// 	typedef  long                   int32_t ;
// 	typedef  unsigned long          uint32_t;
// #else 
// 	#error sizeof(int)或sizeof(long) 不等于4字节
// #endif
// #if (LONG_MAX==9223372036854775807)
// 	typedef  long                   int64_t ;
// 	typedef  unsigned long          uint64_t;
// #else 
// 	//#error 不支持 64位
// #endif
// 	struct STT_TAG_float_4byte 
// 	{
// 	  uint16_t body_low  :16     ;
// 	  uint16_t body_high :(23-16);
// 	  uint16_t exp       : 8;
// 	  uint16_t sign      : 1;
// 	};typedef  struct STT_TAG_float_4byte STT_float_4byte;
// #include "printf.h"


///////////////////////////////////////////////////////////////////////////////////////////////////



 char putchar(char buf)
 {
 	 extern uint8_t UART_seed(const uint8_t buf);
 	 return UART_seed(buf);
 }
static bool printf_out_fun(const char buf, char (*fun)(char) ,char **out_buf,int* ret,int out_max)
{
        if(fun !=NULL ){
        	fun(buf);
        };
        if(out_buf !=NULL && (*out_buf) !=NULL ){
	        **out_buf=buf;
	        (*out_buf)++;
        };
        (*ret)++;
        return (((out_max - (*ret))<=0) ? true : false);
}

int printf_number(bool add0x,bool isbig,bool sign,
				  unsigned char scale,int out_max,
				  unsigned long buf ,char *out_buf,
				   char (*fun)(char) )
{
	int ret;
	unsigned int num;
	char i;
	unsigned long	temp;
	ret=0;
	num=0;
	if(out_max <= 0 || scale <= 1U ){return ret;};
    if (  sign && ( ((long)buf) <0 )  ){// 如果为负数打印，负号
        if( printf_out_fun('-', fun ,&out_buf,&ret,out_max) ){
        	return ret;
        };
		buf = -buf;// 取相反数
    };
    if ( add0x == true ){
        if( printf_out_fun('0', fun ,&out_buf,&ret,out_max) ){
        	return ret;
        };
        if( printf_out_fun('x', fun ,&out_buf,&ret,out_max) ){
        	return ret;
        };
	};
    if ( buf == 0 ){// 数字0的长度为1
        if( printf_out_fun('0', fun ,&out_buf,&ret,out_max) ){
        	return ret;
        };
    };// 计算buf.d长度
    	temp=buf;
    	while (temp>0) {
    		num++;
            temp /= ((unsigned long)scale);
        };
		while (num>0) {
			num--;
			temp=1;
		    for (i = 0; i < num; i++)
		    {
		        temp *= ((unsigned long)scale);
		    };
		    i =(char)(buf / temp);
	        if( printf_out_fun( (i + (i<=9 ? '0': ( isbig ? ('A'- 10) : ('a'- 10) ) )) 
	        	                , fun 
	        	                ,&out_buf,&ret,out_max) ){
	        	return ret;
	        };
            buf %= temp;
        };
        return ret;             
}


static bool isNaN(long double  v) {
    return (v != v);
}
int printf_float(bool isbig,bool isE,char out_see,char E_see,int out_max,long double buf , char (*fun)(char) ,char *out_buf)
{
	int16_t flag;
	int16_t  num;
	int8_t  temp;
  	int ret;
  	char * Pstr;
  	if(out_max <= 0 ){return 0;};

  	ret=0;
  	if(out_see>out_max){
  		out_see=out_max;
  	};
  	if (out_see<0){
  		out_see=FLT_DIG;
  	};
  	if( isNaN(buf)  ){
  		Pstr= ( isbig ? "NAN" : "nan" );
  		GOTO_special_END: ;
  		num=0;
  		while(num<3){
		 	if( printf_out_fun( *Pstr , fun ,&out_buf,&ret,out_max) ){
	        	return ret;
	        };
	        Pstr++;
	        num++;
  		};
  		return ret;
  	};
  	flag=0;
  	if( buf < 0 ){
  		flag=1;
  	 	buf = -buf ;
	 	if( printf_out_fun('-', fun ,&out_buf,&ret,out_max) ){
        	return ret;
        };
  	};
  	if( buf  >  LDBL_MAX  ){
  		if(flag==0){
		 	if( printf_out_fun( '+' , fun ,&out_buf,&ret,out_max) ){
	        	return ret;
	        };
    	};
        Pstr= ( isbig ? "INF" : "inf" );
  		goto GOTO_special_END;
  	} ;
  	 num=0;
  	if( buf <1 ){
		while(buf<1){
			buf *= 10;
			num--;
		};
  	 }
  	 else if(buf >9){
		while(buf>9){
			buf /= 10;
			num++;
		};
  	 };
	flag=0;
	if(isE==1){
			temp=buf;
	 	if( printf_out_fun( temp+'0' , fun ,&out_buf,&ret,out_max) ){
        	return ret;
        };
	}
	else{
		if(num<0){
		 	if( printf_out_fun( '0' , fun ,&out_buf,&ret,out_max) ){
	        	return ret;
	        };
		}
		else{
			flag=num;
	        while( flag >-1 ){
	        	flag--;
	        	temp=buf;
	        	
				if( printf_out_fun(temp+'0', fun ,&out_buf,&ret,out_max) ){
					return ret;
		        };
		        buf -= temp;
		        buf *= 10;
	        };
		};
	};

 	if( (out_see>0 ? printf_out_fun('.', fun ,&out_buf,&ret,out_max) : (isE==1 ? 0 : 1) ) ){
    	return ret;
    };   

    if(isE==0 && num<0){
    	flag=-num;
	    while( out_see >=1 && flag>1){
	    	flag--;
	    	out_see--;
			if( printf_out_fun('0', fun ,&out_buf,&ret,out_max) ){
				return ret;
	        };
	    };
    };

	    while( out_see >=1 ){
	    	out_see--;
	    	temp=buf;
			buf -= temp;
	        buf *= 10;
			temp=buf;
			if( printf_out_fun(temp + '0', fun ,&out_buf,&ret,out_max) ){
				return ret;
	        };
	       
	    };


	if(isE==1){
		if(num <0){
			num = -num;
			temp='-';
              
		}
		else {
			temp='+';
		};
		if( printf_out_fun( ( isbig ? 'E' : 'e' ), fun ,&out_buf,&ret,out_max) ){
			return ret;
        };
		if( printf_out_fun( temp, fun ,&out_buf,&ret,out_max) ){
			return ret;
        };	
		if(E_see>=4){
	 		if( printf_out_fun( (((num/1000)%10) +'0'), fun ,&out_buf,&ret,out_max) ){
				return ret;
	        }; 
		} 
		if(E_see>=3){
			if( printf_out_fun( (((num/100)%10) +'0'), fun ,&out_buf,&ret,out_max) ){
				return ret;
	        };
		};
		if( printf_out_fun( (((num/10)%10) +'0'), fun ,&out_buf,&ret,out_max) ){
			return ret;
        };
  		if( printf_out_fun( ((num%10) +'0'), fun ,&out_buf,&ret,out_max) ){
			return ret;
        };
	};
	return ret;
}


int out_vsprintf(int out_max, char (*fun)(char) ,char *str, const char *format, va_list arg)
{

	union UNN_TAG_buf
	{
	   int    i;
	   int    d;
	   int    o;
	   int    x;
	   int    X;
	   unsigned int    u;
	   double f;
	   double e;
	   double E;
	   double g;
	   double G;
	   char   c;
	   char  *s;
	   void  *p;
	   unsigned long  lu;
	   long  ld;
	   char str[4];
	}  buf;//xdata
	int ret ;// 返回打印字符的个数
	char* pStr ;// 指向format
	bool flag;
    if ( format == NULL ){
    	return -1;
    };
    if ( out_max <= 0){
    	return 0;
    };

    ret=0;
    pStr = (char*)format;
    
    while (*pStr != '\0' && ret<INT_MAX ){
		if ( *pStr != '%'){
			GOTO_putchar: ;
	  		if( printf_out_fun(*pStr, fun ,&str,&ret,out_max) ){
				return ret;
	        };
			pStr++;

		}
		else{

				pStr++;
				GOTO_case: ;
				switch (*pStr)
	            {
	            	case '\0':{return ret;};
	            	case 'C':;
					case 'c':{  
				  		if( printf_out_fun((char)va_arg(arg, int), fun ,&str,&ret,out_max) ){
							return ret;
				        };
		            	pStr++;
	                };break;
	                case 'S':;
	            	case 's':{
		            	buf.s = va_arg(arg, char*);
		            	while( (*(buf.s) !='\0') ){ // && ret<32767
				  		if( printf_out_fun(*(buf.s), fun ,&str,&ret,out_max) ){
							return ret;
				        };
		            		(buf.s)++;
		            	};
		            	pStr++;
	            	};break; 
	            	case 'I':;
	            	case 'i':;
					case 'D':;
	            	case 'd':{
	            		GOTO_case_D: ;
	            		(buf.str)[0]=false;(buf.str)[1]=true;(buf.str)[2]=10;
	            		GOTO_case_I: ;
	            		buf.i= printf_number(0,(buf.str)[0],(buf.str)[1],
	            								(buf.str)[2],(out_max - ret),
	            								( flag==1?va_arg(arg, unsigned long):va_arg(arg, int)),str,
	            								fun);
	            		flag=0;
	            		GOTO_case_I_next: ;
	            		if(str !=NULL ){
	            			str += buf.i ;
	            		};
	            		ret += buf.i ;
	            		if( (out_max - ret)<=0 ){return ret;};
	            		pStr++;
	             	};break;
	             	case 'O':;
	            	case 'o':{
	            		(buf.str)[0]=false;(buf.str)[1]=true;(buf.str)[2]=8;
	            	};goto GOTO_case_I;
	            	case 'X':{
	            		(buf.str)[0]=true;(buf.str)[1]=true;(buf.str)[2]=16;
	            	};goto GOTO_case_I;
	            	case 'x':{
	            		(buf.str)[0]=false;(buf.str)[1]=true;(buf.str)[2]=16;
	            	};goto GOTO_case_I;
	            	case 'U':;
	            	case 'u':{
	            		(buf.str)[0]=false;(buf.str)[1]=false;(buf.str)[2]=10;
	            	};goto GOTO_case_I;
	            	case 'B':;
	            	case 'b':{
	            		(buf.str)[0]=false;(buf.str)[1]=true;(buf.str)[2]=2;
	            	};goto GOTO_case_I;
	            	case 'P':;
	            	case 'p':{
	            		buf.i= printf_number(1,0,0,
	            								16,(out_max - ret),
	            								(unsigned long)va_arg(arg, void*),str,
	            								fun);
	            	};goto GOTO_case_I_next;   
	            	case 'L':;
	            	case 'l':{
	            		switch (*(pStr+1)){
		            	case 'I':;
		            	case 'i':;
						case 'D':;
		            	case 'd':;
		             	case 'O':;
		            	case 'o':;
		            	case 'X':;
		            	case 'x':;
		            	case 'U':;
		            	case 'u':;
						case 'B':;
	            	    case 'b':flag=1;pStr++;goto GOTO_case;
	            	    case 'F':;
	            	    case 'f':;
						case 'E':;
						case 'e':;
						case 'G':;
						case 'g':pStr++;goto GOTO_case;
	            	    default:flag=1;goto GOTO_case_D;
	            		};
	            	};
//int printf_float(bool isbig,bool isE,char out_see,char E_see,int out_max,long double buf , char (*fun)(char) ,char *out_buf);
    				case 'F':{
	            		(buf.str)[0]=1;(buf.str)[1]=0;
	            		GOTO_case_F: ;
	            			buf.i=printf_float((buf.str)[0],(buf.str)[1],6,2,(out_max - ret),va_arg(arg, long double),fun,str);
							if(str !=NULL ){
		            			str += buf.i ;
		            		};
		            		ret += buf.i ;
		            		if( (out_max - ret)<=0 ){return ret;};
		            		pStr++;
	            	};break;
	            	case 'f':{(buf.str)[0]=0;(buf.str)[1]=0;};goto GOTO_case_F;
	            	case 'E':{(buf.str)[0]=1;(buf.str)[1]=1;};goto GOTO_case_F;
	            	case 'e':{(buf.str)[0]=0;(buf.str)[1]=1;};goto GOTO_case_F;
	            	case 'G':{(buf.str)[0]=1;(buf.str)[1]=0;};goto GOTO_case_F;
	            	case 'g':{(buf.str)[0]=0;(buf.str)[1]=0;};goto GOTO_case_F;
					default:
		            	goto GOTO_putchar;
	            };
		};
    };
	return ret;
}
int printf(char *format, ...)
{
   extern int out_vsprintf(int out_max, char (*fun)(char) ,char *str, const char *format, va_list arg);
   va_list args;
   int ret;
   va_start(args, format);
   //vprintf(format, args);
   ret=out_vsprintf(INT_MAX,(char(*)(char))putchar,NULL,format,args);
   va_end(args);
   return ret;
}
int sprintf  (char *out_buf, const char *format, ...)
{
   extern int out_vsprintf(int out_max, char (*fun)(char) ,char *str, const char *format, va_list arg);
   va_list args;
   int ret;
   va_start(args, format);
   //vprintf(format, args);
   ret=out_vsprintf(INT_MAX,NULL,out_buf,format,args);
   va_end(args);
   return ret;
}
int vprintf  (const char *format, va_list args)
{
   extern int out_vsprintf(int out_max, char (*fun)(char) ,char *str, const char *format, va_list arg);

   return out_vsprintf(INT_MAX,(char(*)(char))putchar,NULL,format,args);

}
int vsprintf (char *out_buf, const char *format, va_list args)
{
   extern int out_vsprintf(int out_max, char (*fun)(char) ,char *str, const char *format, va_list arg);

   return out_vsprintf(INT_MAX,NULL,out_buf,format,args);

}
