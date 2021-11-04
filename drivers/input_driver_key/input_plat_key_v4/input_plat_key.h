


#ifndef __INPUT_PLAT_KEY_H_
#define __INPUT_PLAT_KEY_H_ 






//设计一个描述按键的对象	irqno,gpio,name,code,flags
struct key_desc{
	int 			irqno;		//按键触发的中断号
	int 			gpio;		//按键对应的GPIO口
	char 			*name;		//按键的名字
	int 			code;		//代表按键的名字	
	unsigned long 	flags;		//按键触发中断的方式
};





//设计私有数据对象
struct my_platdata_key{
	int my_key_num;
	struct key_desc *pdesc;
	
};







#endif
