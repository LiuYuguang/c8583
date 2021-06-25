#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "c8583.h"

#define FIELD_SIZE 128
struct c8583_s
{
	unsigned char tpdu[5];
	unsigned char msgtype[2];
	
	unsigned char* fieldData[FIELD_SIZE];
	size_t fieldSize[FIELD_SIZE];
};

const unsigned int default_field_length[FIELD_SIZE]=
{    
64/8,19,   6,    12,   12,   12,   10,   8,     //1-8
8,    8,    6,    6,    4,    4,    4,    4,    //9-16
4,    4,    3,    3,    3,    3,    3,    3,    //17-24
2,    2,    1,    1+8,  1+8,  1+8,  1+8,  11,   //25-32
11,   28,   37,   104,  12,   6,    2,    3,    //33-40
8,    15,   40,   25,   76,   999,  999,  999,  //41-48
3,    3,    3,    64/8,16,   120,  999,  999,   //49-56
999,  999,  999,  999,  999,  999,  999,  64/8, //57-64
1,    1,    2,    3,    3,    3,    4,    4,    //65-72
6,    10,   10,   10,   10,   10,   10,   10,   //73-80
10,   12,   12,   12,   12,   16,   16,   16,   //81-88
16,   42,   1,    2,    5,    7,    42,   64/8, //89-96
1+16, 25,   11,   11,   17,   28,   28,   100,  //97-104
999,  999,  999,  999,  999,  999,  999,  999,  //105-112
999,  999,  999,  999,  999,  999,  999,  999,  //112-120
999,  999,  999,  999,  999,  999,  999,  64/8, //121-128
};

// 0 normal 1 llvar 2 lllvar
#define FIX		(1)
#define LLVAR	(1<<1)//Where 0 < LL < 100, means two leading digits LL specify the field length of field VAR
#define LLLVAR	(1<<2)//Where 0 < LLL < 1000, means three leading digits LLL specify the field length of field VAR
//LL and LLL are hex or ASCII. A VAR field can be compressed or ASCII depending of the data element type.

const unsigned char  default_field_var_type[FIELD_SIZE]=
{
FIX,    LLVAR,  FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    //1-8
FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    //9-16
FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    //17-24
FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    LLVAR,  //25-32
LLVAR,  LLVAR,  LLVAR,  LLLVAR, FIX,    FIX,    FIX,    FIX,    //33-40
FIX,    FIX,    FIX,    LLVAR,  LLVAR,  LLLVAR, LLLVAR, LLLVAR, //41-48
FIX,    FIX,    FIX,    FIX,    FIX,    LLLVAR, LLLVAR, LLLVAR, //49-56
LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, FIX,    //57-64
FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    //65-72
FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,	//73-80
FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    //81-88
FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    FIX,    //89-96
FIX,    FIX,    LLVAR,  LLVAR,  LLVAR,  LLVAR,  LLVAR,  LLLVAR, //97-104
LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, //105-112
LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, //112-120
LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, LLLVAR, FIX,    //121-128
};

#define TYPEA	(1)//Alpha, including blanks
#define TYPEN	(1<<1)//Numeric values only
#define TYPEXN	(1<<2)//Numeric (amount) values, where the first byte is either 'C' to indicate a positive or Credit value, or 'D' to indicate a negative or Debit value, followed by the numeric value (using n digits)
#define TYPES	(1<<3)//Special characters only
#define TYPEAN	(TYPEA|TYPEN)//Alphanumeric
#define TYPEAS	(TYPEA|TYPES)//Alpha & special characters only
#define TYPENS	(TYPEN|TYPES)//Numeric and special characters only
#define TYPEANS	(TYPEA|TYPEN|TYPES)//Alphabetic, numeric and special characters.
#define TYPEB	(1<<4)//Binary data
#define TYPEZ	(1<<5)//Tracks 2 and 3 code set as defined in ISO/IEC 7813 and ISO/IEC 4909 respectively
const unsigned char default_field_type[FIELD_SIZE]=
{
TYPEB,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   //1-8
TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   //9-16
TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   //17-24
TYPEN,   TYPEN,   TYPEN,   TYPEXN,  TYPEXN,  TYPEXN,  TYPEXN,  TYPEN,   //25-32
TYPEN,   TYPENS,  TYPEZ,   TYPEN,   TYPEAN,  TYPEAN,  TYPEAN,  TYPEAN,  //33-40
TYPEANS, TYPEANS, TYPEANS, TYPEAN,  TYPEAN,  TYPEAN,  TYPEAN,  TYPEAN,  //41-48
TYPEAN,  TYPEAN,  TYPEAN,  TYPEB,   TYPEN,   TYPEAN,  TYPEANS, TYPEANS, //49-56
TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEB,   //57-64
TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   //65-72
TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   //73-80
TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   TYPEN,   //81-88
TYPEN,   TYPEN,   TYPEAN,  TYPEAN,  TYPEAN,  TYPEAN,  TYPEAN,  TYPEB,   //89-96
TYPEXN,  TYPEANS, TYPEN,   TYPEN,   TYPEANS, TYPEANS, TYPEANS, TYPEANS, //97-104
TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, //105-112
TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, //112-120
TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEANS, TYPEB,   //121-128
};

//0 right justified 1 left justified
#define RIGHT_JUSTIFIED (1)
#define LEFT_JUSTIFIED  (1<<1)
const unsigned char default_justified[FIELD_SIZE]=
{
RIGHT_JUSTIFIED, LEFT_JUSTIFIED,  RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //1-8
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //9-16
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //17-24
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //25-32
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, LEFT_JUSTIFIED,  LEFT_JUSTIFIED,  RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //33-40
LEFT_JUSTIFIED,  LEFT_JUSTIFIED,  RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //41-48
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //49-56
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //57-64
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //65-72
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //73-80
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //81-88
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //89-96
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //97-104
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //105-112
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //112-120
RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, RIGHT_JUSTIFIED, //121-128
};

int bcd2int(unsigned char *b,int l){
	int n=0;
	int i;
	for(i=0;i<l;i++){
		n*=100;

		if((b[i]&0x0f)>0x09 || (b[i]&0xf0)>0x90){
			break;
		}

		n +=( ((b[i]>>4)&0x0f)*10 + b[i]&0x0f );
	}
	return n;
}

int int2bcd(int n,unsigned char *b,int b_size){
	int i,j;
	for(i=b_size-1;i>=0 && n>=0;i--,n/=100){
		j = n%100;
		b[i] = (j/10)<<4|(j%10);
	}
	return i;
}

void bcd2asc(unsigned char *in_data,int in_data_length,unsigned char *out_data,int out_data_length,int justified){
	static unsigned char bcd [256][2] = {
		{0x30,0x30},{0x30,0x31},{0x30,0x32},{0x30,0x33},{0x30,0x34},{0x30,0x35},{0x30,0x36},{0x30,0x37},{0x30,0x38},{0x30,0x39},{0x30,0x61},{0x30,0x62},{0x30,0x63},{0x30,0x64},{0x30,0x65},{0x30,0x66},//0-15
		{0x31,0x30},{0x31,0x31},{0x31,0x32},{0x31,0x33},{0x31,0x34},{0x31,0x35},{0x31,0x36},{0x31,0x37},{0x31,0x38},{0x31,0x39},{0x31,0x61},{0x31,0x62},{0x31,0x63},{0x31,0x64},{0x31,0x65},{0x31,0x66},//16-31
		{0x32,0x30},{0x32,0x31},{0x32,0x32},{0x32,0x33},{0x32,0x34},{0x32,0x35},{0x32,0x36},{0x32,0x37},{0x32,0x38},{0x32,0x39},{0x32,0x61},{0x32,0x62},{0x32,0x63},{0x32,0x64},{0x32,0x65},{0x32,0x66},//32-47
		{0x33,0x30},{0x33,0x31},{0x33,0x32},{0x33,0x33},{0x33,0x34},{0x33,0x35},{0x33,0x36},{0x33,0x37},{0x33,0x38},{0x33,0x39},{0x33,0x61},{0x33,0x62},{0x33,0x63},{0x33,0x64},{0x33,0x65},{0x33,0x66},//48-63
		{0x34,0x30},{0x34,0x31},{0x34,0x32},{0x34,0x33},{0x34,0x34},{0x34,0x35},{0x34,0x36},{0x34,0x37},{0x34,0x38},{0x34,0x39},{0x34,0x61},{0x34,0x62},{0x34,0x63},{0x34,0x64},{0x34,0x65},{0x34,0x66},//64-79
		{0x35,0x30},{0x35,0x31},{0x35,0x32},{0x35,0x33},{0x35,0x34},{0x35,0x35},{0x35,0x36},{0x35,0x37},{0x35,0x38},{0x35,0x39},{0x35,0x61},{0x35,0x62},{0x35,0x63},{0x35,0x64},{0x35,0x65},{0x35,0x66},//80-95
		{0x36,0x30},{0x36,0x31},{0x36,0x32},{0x36,0x33},{0x36,0x34},{0x36,0x35},{0x36,0x36},{0x36,0x37},{0x36,0x38},{0x36,0x39},{0x36,0x61},{0x36,0x62},{0x36,0x63},{0x36,0x64},{0x36,0x65},{0x36,0x66},//96-
		{0x37,0x30},{0x37,0x31},{0x37,0x32},{0x37,0x33},{0x37,0x34},{0x37,0x35},{0x37,0x36},{0x37,0x37},{0x37,0x38},{0x37,0x39},{0x37,0x61},{0x37,0x62},{0x37,0x63},{0x37,0x64},{0x37,0x65},{0x37,0x66},//16-31
		{0x38,0x30},{0x38,0x31},{0x38,0x32},{0x38,0x33},{0x38,0x34},{0x38,0x35},{0x38,0x36},{0x38,0x37},{0x38,0x38},{0x38,0x39},{0x38,0x61},{0x38,0x62},{0x38,0x63},{0x38,0x64},{0x38,0x65},{0x38,0x66},//16-31
		{0x39,0x30},{0x39,0x31},{0x39,0x32},{0x39,0x33},{0x39,0x34},{0x39,0x35},{0x39,0x36},{0x39,0x37},{0x39,0x38},{0x39,0x39},{0x39,0x61},{0x39,0x62},{0x39,0x63},{0x39,0x64},{0x39,0x65},{0x39,0x66},//16-31
		{0x61,0x30},{0x61,0x31},{0x61,0x32},{0x61,0x33},{0x61,0x34},{0x61,0x35},{0x61,0x36},{0x61,0x37},{0x61,0x38},{0x61,0x39},{0x61,0x61},{0x61,0x62},{0x61,0x63},{0x61,0x64},{0x61,0x65},{0x61,0x66},//16-31
		{0x62,0x30},{0x62,0x31},{0x62,0x32},{0x62,0x33},{0x62,0x34},{0x62,0x35},{0x62,0x36},{0x62,0x37},{0x62,0x38},{0x62,0x39},{0x62,0x61},{0x62,0x62},{0x62,0x63},{0x62,0x64},{0x62,0x65},{0x62,0x66},//16-31
		{0x63,0x30},{0x63,0x31},{0x63,0x32},{0x63,0x33},{0x63,0x34},{0x63,0x35},{0x63,0x36},{0x63,0x37},{0x63,0x38},{0x63,0x39},{0x63,0x61},{0x63,0x62},{0x63,0x63},{0x63,0x64},{0x63,0x65},{0x63,0x66},//16-31
		{0x64,0x30},{0x64,0x31},{0x64,0x32},{0x64,0x33},{0x64,0x34},{0x64,0x35},{0x64,0x36},{0x64,0x37},{0x64,0x38},{0x64,0x39},{0x64,0x61},{0x64,0x62},{0x64,0x63},{0x64,0x64},{0x64,0x65},{0x64,0x66},//16-31
		{0x65,0x30},{0x65,0x31},{0x65,0x32},{0x65,0x33},{0x65,0x34},{0x65,0x35},{0x65,0x36},{0x65,0x37},{0x65,0x38},{0x65,0x39},{0x65,0x61},{0x65,0x62},{0x65,0x63},{0x65,0x64},{0x65,0x65},{0x65,0x66},//16-31
		{0x66,0x30},{0x66,0x31},{0x66,0x32},{0x66,0x33},{0x66,0x34},{0x66,0x35},{0x66,0x36},{0x66,0x37},{0x66,0x38},{0x66,0x39},{0x66,0x61},{0x66,0x62},{0x66,0x63},{0x66,0x64},{0x66,0x65},{0x66,0x66}//16-31
	};
	int i,j;

	if(justified == RIGHT_JUSTIFIED){
		for(i=in_data_length-1,j=out_data_length-1;i>=0&&j>=1;i-=1,j-=2){
			memcpy(&out_data[j-1],bcd[in_data[i]],2);
		}
		if(i<0||j<0){
			return;
		}
		memcpy(&out_data[j],&bcd[in_data[i]][1],1);
	}else if(justified == LEFT_JUSTIFIED){
		for(i=0,j=0;i<in_data_length&&j<out_data_length-1;i+=1,j+=2){
			memcpy(&out_data[j],bcd[in_data[i]],2);
		}
		if(i>=in_data_length||j>=out_data_length){
			return;
		}
		memcpy(&out_data[j],&bcd[in_data[i]][0],1);
	}
}

int asc2bcd(unsigned char *in_data,int in_data_len,unsigned char *out_data,int out_data_len,int justified){//0 right 1 left
	static unsigned char ascii2hex[256] = {
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //0-9//should be static??
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //10-19
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //20-29
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //30-39
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d,    0,    1, //40-49
		   2,    3,    4,    5,    6,    7,    8,    9, 0x3d, 0x3d, //50-59
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d,   10,   11,   12,   13,   14, //60-69
		  15, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //70-79
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //80-89
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d,   10,   11,   12, //90-99
		  13,   14,   15, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //100-109
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //110-119
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //120-129
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //130-139
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //140-149
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //150-159
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //160-169
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //170-179
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //180-189
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //190-199
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //200-209
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //210-219
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //220-229
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //230-239
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, //240-249
		0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d //250-255
	};

	int i,j;

	if(justified == RIGHT_JUSTIFIED){
		for(i = in_data_len-1,j = out_data_len-1;i>=1&&j>=0;i-=2,j-=1){
			out_data[j] = ascii2hex[in_data[i-1]]<<4 | ascii2hex[in_data[i]];
		}
		if(i<0||j<0){
			return;
		}
		out_data[j] = (out_data[j]&0xf0)| (ascii2hex[in_data[i]]&0x0f);
	}else if(justified == LEFT_JUSTIFIED){
		for(i=0,j=0;i<in_data_len-1&&j<out_data_len;i+=2,j+=1){
			out_data[j] = 1;
			out_data[j] = ascii2hex[in_data[i]]<<4 | ascii2hex[in_data[i+1]];
		}
		if(i>=in_data_len||j>=out_data_len){
			return;
		}
		out_data[j] = (out_data[j]&0x0f) | ((ascii2hex[in_data[i]]<<4)&0xf0);
	}
}

//初始化结构体
c8583_t* newC8583(){
	c8583_t*c = malloc(sizeof(c8583_t));
	if(c == NULL){
		return NULL;
	}
	memset(c,0,sizeof(*c));
	int i;
	for(i=0;i<FIELD_SIZE;i++){
		c->fieldData[i]=NULL;
	}
	return c;
}

void deleteC8583(c8583_t *c){
	int i;
	for(i=0;i<FIELD_SIZE;i++){
		free(c->fieldData[i]);
	}
	free(c);
}

ssize_t setC8583Tpdu(c8583_t *c,unsigned char *buf,size_t size){
	if(size != 2*sizeof(c->tpdu)){
		return -1;
	}
	asc2bcd(buf,size,c->tpdu,sizeof(c->tpdu),LEFT_JUSTIFIED);
	return 0;
}

ssize_t getC8583Tpdu(c8583_t *c,unsigned char *buf,size_t size){
	if(size < 2*sizeof(c->tpdu)){
		return -1;
	}
	bcd2asc(c->tpdu,sizeof(c->tpdu),buf,size,LEFT_JUSTIFIED);
	return 2*sizeof(c->tpdu);
}

ssize_t setC8583MsgType(c8583_t *c,unsigned char* buf,size_t size){
	if(size != 2*sizeof(c->msgtype)){
		return -1;
	}

	asc2bcd(buf,size,c->msgtype,sizeof(c->msgtype),LEFT_JUSTIFIED);
	return 0;
}

ssize_t getC8583MsgType(c8583_t *c,unsigned char* buf,size_t size){
	if(size < 2*sizeof(c->msgtype)){
		return -1;
	}
	bcd2asc(c->msgtype,sizeof(c->msgtype),buf,size,LEFT_JUSTIFIED);
	return 2*sizeof(c->msgtype);
}

ssize_t setC8583FieldData(c8583_t *c,unsigned char fieldno,unsigned char *buf,size_t size){
	int i = fieldno-1;
	if(i < 0)
		return -1;
	if(i >= FIELD_SIZE)	
		return -1;
	if(size > default_field_length[i]){
		return -1;
	}

	if(c->fieldData[i] != NULL){
		free(c->fieldData[i]);
		c->fieldSize[i] = 0;
		c->fieldData[i] = NULL;
	}
	if(size == 0){
		return 0;
	}
	c->fieldSize[i] = size;
	c->fieldData[i] = malloc(sizeof(unsigned char) * size);
	memcpy(c->fieldData[i],buf,size);
	return 0;
}

ssize_t getC8583FieldData(c8583_t *c,unsigned char fieldno,unsigned char *buf,size_t size){
	int i = fieldno-1;
	if(i < 0)
		return -1;
	if(i >= FIELD_SIZE)	
		return -1;
	
	if(size < c->fieldSize[i]){
		return -1;
	}
	
	if(c->fieldData[i] == NULL){
		return 0;
	}

	memcpy(buf,c->fieldData[i],c->fieldSize[i]);
	return c->fieldSize[i];
}

ssize_t unpackC8583Data(c8583_t *c,unsigned char *buf,size_t size){
	int index = 0;

	if(index+sizeof(c->tpdu)>size){
		return -1;
	}
	memcpy(c->tpdu,&buf[index],sizeof(c->tpdu));
	index+=sizeof(c->tpdu);

	if(index+sizeof(c->msgtype)>size){
		return -1;
	}
	memcpy(c->msgtype,&buf[index],sizeof(c->msgtype));
	index+=sizeof(c->msgtype);

	unsigned char bitmap[16] = {0};
	int bitmapSize = 0;
	if(index+8>size){
		return -1;
	}
	memcpy(bitmap,&buf[index],8);
	bitmapSize+=8;
	index+=8;

	if((bitmap[0] & 128) != 0x00 && index+8<=size){
		memcpy(&bitmap[8],&buf[index],8);
		bitmapSize+=8;
	}

	int i,j,fieldno=0;
	int fielddata_len,fielddata_len2;
	unsigned char fieldData[1024] = {0};
	for(i=0;i<bitmapSize;i++){
		for(j=128;j>0;j>>=1,fieldno++){
			if((bitmap[i] & j) == 0x00){
				continue;
			}

			switch(default_field_var_type[fieldno]){
				case FIX:
				{
					fielddata_len = default_field_length[fieldno];
					break;
				}
				case LLVAR:
				{
					if(index+1>size){
						return -1;
					}
					fielddata_len = bcd2int(&buf[index],1);
					index+=1;
					break;
				}
				case LLLVAR:
				{
					if(index+2>size){
						return -1;
					}
					fielddata_len = bcd2int(&buf[index],2);
					index+=2;
					break;
				}
				default:
				{
					return -1;
				}
			}

			switch(default_field_type[fieldno]){
				case TYPEA:
				case TYPES:
				case TYPEAN:
				case TYPEAS:
				case TYPEANS:
				case TYPEB:
				{
					if(index+fielddata_len>size){
						return -1;
					}
					memcpy(fieldData,&buf[index],fielddata_len);
					index += fielddata_len;
					break;
				}
				case TYPENS:
				case TYPEN:
				case TYPEZ:
				{
					fielddata_len2 = (fielddata_len+1)/2;
					if(index+fielddata_len2>size){
						return -1;
					}
					bcd2asc(&buf[index],fielddata_len2,fieldData,fielddata_len,default_justified[fieldno]);
					index+=fielddata_len2;
					break;
				}
				case TYPEXN:
				{
					if(index+1>size){
						return -1;
					}
					fieldData[0] = buf[index];
					index+=1;

					fielddata_len2 = (fielddata_len)/2;
					if(index+fielddata_len2>size){
						return -1;
					}
					bcd2asc(&buf[index],fielddata_len2,&fieldData[1],fielddata_len-1,default_justified[fieldno]);
					index+=fielddata_len2;
					break;
				}
				default:{
					return -1;
				}
			}
			setC8583FieldData(c,fieldno+1,fieldData,fielddata_len);
		}
	}
	return 0;
}

ssize_t packC8583Data(c8583_t *c,unsigned char* buf,size_t size){
	int index=0;

	if(index+sizeof(c->tpdu)>size){
		return -1;
	}
	memcpy(&buf[index],c->tpdu,sizeof(c->tpdu));
	index+=sizeof(c->tpdu);

	if(index+sizeof(c->msgtype)>size){
		return -1;
	}
	memcpy(&buf[index],c->msgtype,sizeof(c->msgtype));
	index+=sizeof(c->msgtype);

	unsigned char bitmap[16] = {0};
	unsigned char offset_map[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	int i,j;
	int fielddata_len,fielddata_len2;
	for(i=0,j=0;i<FIELD_SIZE;i++){
		if(c->fieldData[i] != NULL){
			bitmap[i/8] |= offset_map[i%8];
			j=i;
		}
	}

	if(j >= 64){
		setC8583FieldData(c,1,&bitmap[8],8);
		bitmap[0] |= 128;
	}

	if(index+8>size){
		return -1;
	}
	memcpy(&buf[index],bitmap,8);
	index+=8;

	for(i=0;i<FIELD_SIZE;i++){
		//field not exist
		if(c->fieldData[i] == NULL){
			continue;
		}

		fielddata_len = 0;
		switch(default_field_var_type[i]){
			case FIX:
			{
				fielddata_len = default_field_length[i];
				break;
			}
			case LLVAR:
			{
				fielddata_len = c->fieldSize[i];
				unsigned char fieldlen_bcd[1] = {0};
				int2bcd(fielddata_len,fieldlen_bcd,1);
				if(index+1>size){
					return -1;
				}
				memcpy(&buf[index],fieldlen_bcd,1);
				index+=1;
				break;
			}
			case LLLVAR:
			{
				fielddata_len = c->fieldSize[i];
				unsigned char fieldlen_bcd[2] = {0};
				int2bcd(fielddata_len,fieldlen_bcd,2);
				if(index+2>size){
					return -1;
				}
				memcpy(&buf[index],fieldlen_bcd,2);
				index+=2;
				break;
			}
			default:
			{
				return -1;
			}
		}

		switch (default_field_type[i])
		{
			case TYPEA:
			case TYPES:
			case TYPEAN:
			case TYPEAS:
			case TYPEANS:
			case TYPEB:
			{
				if(index+fielddata_len>size){
					return -1;
				}
				memset(&buf[index],0,fielddata_len);
				memcpy(&buf[index],c->fieldData[i],c->fieldSize[i]);
				index+=fielddata_len;
				break;
			}
			case TYPENS:
			case TYPEN:
			case TYPEZ:
			{
				fielddata_len2 = (fielddata_len+1)/2;
				if(index+fielddata_len2>size){
					return -1;
				}
				memset(&buf[index],0,fielddata_len2);
				asc2bcd(c->fieldData[i],c->fieldSize[i],&buf[index],fielddata_len2,default_justified[i]);
				index+=fielddata_len2;
				break;
			}
			case TYPEXN:
			{
				fielddata_len2 = (fielddata_len)/2;
				if(index+1+fielddata_len2>size){
					return -1;
				}
				buf[index] = c->fieldData[i][0];
				index+=1;
				memset(&buf[index],0,fielddata_len2);
				asc2bcd(&c->fieldData[i][1],c->fieldSize[i]-1,&buf[index],fielddata_len2,default_justified[i]);
				index+=fielddata_len2;
				break;
			}
			default:
			{
				return -1;
			}
		}
	}
	return index;
}