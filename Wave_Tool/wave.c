#include"wave.h"

int Search_Filename(char *filename){																//ファイル名をとってくる関数
	int i=0,c;
	
	printf("ファイル名を入力:");																	//とりあえず文字数100から始める

	while((c=fgetc(stdin))!='\n' && c!= EOF) (filename)[i++]=(char)c;
	
	(filename)[i] = '\0';																			//エンターを終了文字に置き換える
	return 1;
}


int Controll_File(char *filename,WAVE_FILE *data){											//ファイルを開いてヘッダ解析する関数
	
	uint8_t chunk_size_flag=0;
	int size;
	
(*data).fp_base=fopen(filename,"rb");																		//引数のファイルを開く
	if((*data).fp_base==NULL){

		printf("エラーが発生:ファイルが存在しないか、開けません。\n");
		return -1;
	}


	(*data).fp_now=(*data).fp_base;


	if(!Chank_Finder(data)){
		printf("エラーが発生:wavファイルではありません\n");
		return -1;
	}

	return 1;
}

int Chank_Finder(WAVE_FILE *data){

	uint8_t header_size_flag=0;
	uint8_t data_buffer[4];

	while(!header_size_flag){
		fread(data_buffer,sizeof(uint8_t),4,data->fp_now);

		if(memcmp(data_buffer,"RIFF",4)==0){

			printf("RIFFchunk found\n");
			data->header.riff_header.Riff_position=(uint8_t)ftell(data->fp_now)-4;
	
		}else if(memcmp(data_buffer,"fmt ",4)==0){

			printf("FMTchunk found\n");
			data->header.format_chunk.chunk_position=(uint8_t)ftell(data->fp_now)-4;

		}else if(memcmp(data_buffer,"fact",4)==0){

			printf("factチャンク発見\n");
			data->header.fact_chunk.chunk_position=(uint8_t)ftell(data->fp_now)-4;

		}else if(memcmp(data_buffer,"PEAK",4)==0){

			printf("PEAKチャンク発見\n");
			data->header.option_peak.chunk_position=(uint8_t)ftell(data->fp_now)-4;

		}else if(memcmp(data_buffer,"data",4)==0){

			printf("データチャンク発見\n");
			data->header.data_chunk.chunk_position=(uint8_t)ftell(data->fp_now)-4;
			header_size_flag=1;

		}

		else fseek(data->fp_now,-3,SEEK_CUR);

		if(ftell(data->fp_now)-ftell(data->fp_base)>=100){

			printf("WAVEファイルが壊れているか、別のファイルです\n");
			return -1;

		}

	}


	data->header_size=data->header.data_chunk.chunk_position+8;


	return 1;

}