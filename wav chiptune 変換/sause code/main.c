#include"wave.h"

int main(){

	SetConsoleOutputCP(CP_UTF8);
	  
	char *filename=NULL;
	struct file_data data_var;
	struct file_data *data=&data_var;
	struct data_change change_var;
	struct data_change *change=&change_var;
	struct chiptune_data chiptune_var;
	struct chiptune_data *chiptune=&chiptune_var;
	 
	search_filename(&filename);
	controll_file(filename,data);
	
	change_sign_16bit(data,change);
	printf("fhase1");
	chiptune_wave(change);
	writing(data,change,chiptune);
	printf("成功しました");
}