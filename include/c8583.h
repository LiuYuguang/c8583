#ifndef _C8583_H_
#define _C8583_H_

//https://en.wikipedia.org/wiki/ISO_8583

typedef struct c8583_s c8583_t;

//初始化结构体
c8583_t* newC8583();
void deleteC8583(c8583_t *p);

ssize_t setC8583Tpdu(c8583_t *c,unsigned char *buf,size_t size);
ssize_t getC8583Tpdu(c8583_t *c,unsigned char *buf,size_t size);
ssize_t setC8583MsgType(c8583_t *c,unsigned char* buf,size_t size);
ssize_t getC8583MsgType(c8583_t *c,unsigned char* buf,size_t size);
ssize_t setC8583FieldData(c8583_t *c,unsigned char fieldno,unsigned char *buf,size_t size);
ssize_t getC8583FieldData(c8583_t *c,unsigned char fieldno,unsigned char *buf,size_t size);

ssize_t unpackC8583Data(c8583_t *c,unsigned char *buf,size_t size);
ssize_t packC8583Data(c8583_t *c,unsigned char* buf,size_t size);
#endif