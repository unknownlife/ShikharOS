#include "keyboard_map.h"

#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C

extern unsigned char keyboard_map[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);
extern void print_menu(void);
extern void enter_num(int);
extern void calc(void);
extern int chk(char);
extern char numtochar(int);
extern void convert_answer(int);


unsigned int current_loc = 0;
int choice=0,num1=0,num2=0;
int functioncount=0;
#define CAPSLOCK_KEY_CODE 0x3A

char *vidptr = (char*)0xb8000;

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];


void idt_init(void)
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);

	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

void kb_init(void)
{
	write_port(0x21 , 0xFD);
}

void kprint(const char *str)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = 0x02;
	}
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = 0x02;
	}
}
void casechange(void)
{    
              if(functioncount>=0 && functioncount<26)
{

                  vidptr[current_loc++]=(char)(65+functioncount);
                  vidptr[current_loc++]=0x02;
                         functioncount++;
}
else
                   functioncount=0;
                       return;
}
void keyboard_handler_main(void)
{
	unsigned char status;
	char keycode;
	write_port(0x20, 0x20);
	status = read_port(KEYBOARD_STATUS_PORT);
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(keycode < 0)
			return;

		if(keycode == ENTER_KEY_CODE) {
			kprint_newline();
			return;
		}
             	 if(keycode == CAPSLOCK_KEY_CODE)
                 {
                  casechange();
                     return;
                 }

		char ch=keyboard_map[(unsigned char) keycode];
		if(ch=='\n'){
			kprint_newline();
			}
		else if(ch=='\b'){
			current_loc--;
			current_loc--;
			vidptr[current_loc++] = ' ';
			vidptr[current_loc++] = 0x00;
			}
		else{
			vidptr[current_loc++] = ch;
			vidptr[current_loc++] = 0x02;
			if(choice==0){
				choice=chk(ch);
				enter_num(0);
				}
			else if(num1==0){
				num1=chk(ch);
				enter_num(1);
				}
			else if(num2==0)num2=chk(ch);			
			}
	}
}

void kmain(void)
{	
	const char *str = "Simple Algebric Calculator";
	clear_screen();
	kprint(str);
	kprint_newline();
	idt_init();
	kb_init();
	print_menu();
	while(1){
		if(choice!=0&&num1!=0&&num2!=0)calc();
	}
}

void print_menu(void){
	const char *add = "1.Addition";
	kprint(add);
	kprint_newline();
	const char *sub = "2.Subtract";
	kprint(sub);
	kprint_newline();
	const char *mul = "3.Multiply";
	kprint(mul);
	kprint_newline();
	const char *mod = "4.Modulus";
	kprint(mod);
	kprint_newline();
	const char *ln= "Enter your Choice : ";
	kprint(ln);
}

void enter_num(int x){
	if(x==0){
		const char *str=" Enter First Number : ";
		kprint(str);
	}
	else if(x==1){
		const char *str=" Enter Second Number : ";
		kprint(str);
	}
}

int chk(char ch){
	int num;
	switch(ch){
	case '0':num=0;
		break;
	case '1':num=1;
		break;
	case '2':num=2;
		break;
	case '3':num=3;
		break;
	case '4':num=4;
		break;
	case '5':num=5;
		break;
	case '6':num=6;
		break;
	case '7':num=7;
		break;
	case '8':num=8;
		break;
	case '9':num=9;
		break;
	}
return num;
}

void calc(void){
	int res=0;
	switch(choice){
		case 1:res=num1+num2;
			break;
		case 2:res=num1-num2;
			break;
		case 3:res=num1*num2;
			break;
		case 4:res=num1%num2;
			break;
	}
	convert_answer(res);
}

void convert_answer(int res){
	const char *result="Result : ";	
	int a,b;	
	a=res/10;
	b=res%10;
	kprint_newline();
	kprint(result);
	vidptr[current_loc++] = numtochar(a);
	vidptr[current_loc++] = 0x02;
	vidptr[current_loc++] = numtochar(b);
	vidptr[current_loc++] = 0x02;
	while(1);
}

char numtochar(int a){
	char ch;
	switch(a){
	case 0:ch='0';
		break;
	case 1:ch='1';
		break;
	case 2:ch='2';
		break;
	case 3:ch='3';
		break;
	case 4:ch='4';
		break;
	case 5:ch='5';
		break;
	case 6:ch='6';
		break;
	case 7:ch='7';
		break;
	case 8:ch='8';
		break;
	case 9:ch='9';
		break;
	}
return ch;
}
