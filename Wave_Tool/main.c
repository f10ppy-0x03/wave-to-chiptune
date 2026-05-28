#include"wave.h"

int main(){

	SetConsoleOutputCP(CP_UTF8);
	  

	WAVE_FILE base_file,change_file;
	char filename[FILENAME_MAX+1];
	 
	if ( !Search_Filename(filename)) return -1;

	if(!Controll_File(filename,&base_file)) return -1;

	Tools(&base_file);


	
}