#include <stdio.h>
#include <string.h>
#include "c8583.h"

int main(){
    c8583_t *c;
    unsigned char tmp[2048] = {0};
    int tmp_len = 0;
    int i;
    int ret;
    
    c = newC8583();
    setC8583Tpdu(c,"6056781234",10);
    setC8583MsgType(c,"0200",4);

    setC8583FieldData(c,2,"22222",5);
    setC8583FieldData(c,3,"333333",6);
    setC8583FieldData(c,4,"444",3);
    
    setC8583FieldData(c,5,"555",3);
    setC8583FieldData(c,6,"666",3);
    setC8583FieldData(c,7,"777",3);
    setC8583FieldData(c,8,"888",3);
    setC8583FieldData(c,9,"999",3);
    setC8583FieldData(c,10,"000",3);
    setC8583FieldData(c,11,"11",2);
    setC8583FieldData(c,12,"222222",6);
    setC8583FieldData(c,13,"3333",4);
    setC8583FieldData(c,14,"4444",4);
    setC8583FieldData(c,15,"5555",4);
    setC8583FieldData(c,16,"6666",4);
    setC8583FieldData(c,17,"7777",4);
    setC8583FieldData(c,18,"8888",4);
    setC8583FieldData(c,19,"999",3);
    setC8583FieldData(c,20,"000",3);
    setC8583FieldData(c,21,"11",2);
    setC8583FieldData(c,22,"222",3);
    setC8583FieldData(c,23,"333",3);
    setC8583FieldData(c,24,"444",3);
    setC8583FieldData(c,25,"55",2);
    setC8583FieldData(c,26,"66",2);
    setC8583FieldData(c,27,"7",1);
    setC8583FieldData(c,28,"8888",4);
    setC8583FieldData(c,29,"9999",4);
    setC8583FieldData(c,30,"0000",4);
    setC8583FieldData(c,31,"11",2);
    setC8583FieldData(c,32,"222222",6);
    setC8583FieldData(c,33,"3333",4);
    setC8583FieldData(c,34,"4444",4);
    setC8583FieldData(c,35,"5555",4);
    setC8583FieldData(c,36,"6666",4);
    setC8583FieldData(c,37,"777777777777",12);
    setC8583FieldData(c,38,"88888",6);
    setC8583FieldData(c,39,"99",2);
    setC8583FieldData(c,40,"000",3);
    setC8583FieldData(c,41,"11111111",8);
    setC8583FieldData(c,42,"222222",6);
    setC8583FieldData(c,43,"3333333333333333333333333333333333333333",40);
    setC8583FieldData(c,44,"4444",4);
    setC8583FieldData(c,45,"5555",4);
    setC8583FieldData(c,46,"6666",4);
    setC8583FieldData(c,47,"7777",4);
    setC8583FieldData(c,48,"8888",4);
    setC8583FieldData(c,49,"999",3);
    setC8583FieldData(c,50,"000",3);
    setC8583FieldData(c,51,"111",3);
    setC8583FieldData(c,52,"222222",6);
    setC8583FieldData(c,53,"3333",4);
    setC8583FieldData(c,54,"4444",4);
    setC8583FieldData(c,55,"5555",4);
    setC8583FieldData(c,56,"6666",4);
    setC8583FieldData(c,57,"7777",4);
    setC8583FieldData(c,58,"8888",4);
    setC8583FieldData(c,59,"9999",4);
    setC8583FieldData(c,63,"33333333",8);
    setC8583FieldData(c,64,"11111111",8);
    setC8583FieldData(c,65,"1",1);
    setC8583FieldData(c,66,"2",1);
    
    tmp_len = packC8583Data(c,tmp,sizeof(tmp));
    
    printf("tmp_len%d\n",tmp_len);
    for(i=0;i<tmp_len;i++){
        printf("\\x%02x",tmp[i]);
    }
    printf("\n");
    
    deleteC8583(c);

    c = newC8583();
    ret = unpackC8583Data(c,tmp,tmp_len);
    printf("ret%d\n",ret);
    
    for(i=1;i<=128;i++){
        memset(tmp,0,sizeof(tmp));
        tmp_len = getC8583FieldData(c,i,tmp,sizeof(tmp));
        if(tmp_len>0){
            printf("field %d:[%d][%s]\n",i,tmp_len,tmp);
        }
    }
    
    deleteC8583(c);
}