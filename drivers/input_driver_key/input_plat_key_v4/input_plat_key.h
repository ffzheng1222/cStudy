


#ifndef __INPUT_PLAT_KEY_H_
#define __INPUT_PLAT_KEY_H_ 






//���һ�����������Ķ���	irqno,gpio,name,code,flags
struct key_desc{
	int 			irqno;		//�����������жϺ�
	int 			gpio;		//������Ӧ��GPIO��
	char 			*name;		//����������
	int 			code;		//������������	
	unsigned long 	flags;		//���������жϵķ�ʽ
};





//���˽�����ݶ���
struct my_platdata_key{
	int my_key_num;
	struct key_desc *pdesc;
	
};







#endif
