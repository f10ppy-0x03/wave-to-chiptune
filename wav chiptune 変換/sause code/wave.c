#include"wave.h"

int search_filename(char **filename){																//ファイル名をとってくる関数
	int i=0,c;
	int max=100;
	char *filename2;
	
	printf("ファイル名を入力\n");																	//とりあえず文字数100から始める
	if((*filename=(char *)malloc(max))==NULL){
		printf("エラーが発生:ファイルネーム分のメモリを確保できませんでした。\n");
		return -1;
	}
	
		while((c=fgetc(stdin))!='\n' && c!= EOF){													//エンターとか終了まで続ける。文字数足りなくなったら二倍にしていく
		(*filename)[i++]=(char)c;
		
		if(i==max-1){
			max=max*2;
			filename2=realloc(*filename,max);
			if(filename2==NULL){
				printf("エラーが発生:ファイルネーム分のメモリを確保できませんでした\n");
				return -1;
			}
		*filename=filename2;
		}
	
	}
	
	(*filename)[i] = '\0';																			//エンターを終了文字に置き換える
	return 0;
}

int controll_file(char *filename,struct file_data *data){											//ファイルを開いてヘッダ解析する関数
	
	FILE *fp;
	char *file=malloc(HEADERSIZE);																	//var0.2
	int size;
	
	fp=fopen(filename,"rb");																		//引数のファイルを開く
	if(fp==NULL){
		printf("エラーが発生:ファイルが存在しないか、開けません。\n");
		free(file);
		free(filename);
		return -1;
	}
	
	size=fread(file,sizeof(char),HEADERSIZE,fp);													//ファイルの中身を44文字読み取る(var 0.2)
	for(int i=0;i<size;i++){
		printf("%x ",(unsigned char)file[i]);
	}
	
	if(judgefile(file,data)==-1){																	//judgefileでファイルのヘッダを解析する
		printf("エラーが発生:wavファイルではありません\n");
		free(file);
		free(filename);
		fclose(fp);
		return -1;
	}
	
	alloc_filedata(data,fp);																		//alloc_freedataでファイルデータ分のメモリ確保
	free(file);
	free(filename);
	fclose(fp);
	return 0;
}

int judgefile(char *file,struct file_data *data){													//ファイルのヘッダ解析をする関数(var0.2)
	

	
	if((memcmp(file,RIFF,4))==0){																	//RIFFを見つける
		printf("\nRIFF:見つかりました\n");
		data->riff=1;
		
		data->audio_format=read_16(file,AUDIO_FORMAT_HEAD);											//オーディオフォーマット解析
		switch(data->audio_format){
			case PCM:	printf("オーディオフォーマット:PCMです。\n");	break;
			case A_LOW:	printf("オーディオフォーマット:A-LOWです。\n");	break;
			case U_LOW:	printf("オーディオフォーマット:U-LOWです。\n");	break;
			default:	printf("オーディオフォーマット:解析不可です\n");	return -1;
		}
		
		data->num_channels=read_16(file,CHANNELS_HEAD);												//チャンネル解析
		switch(data->num_channels){
			case MONORAL:	printf("チャンネル:モノラルです。\n");	break;
			case STEREO:	printf("チャンネル:ステレオです。\n");	break;
			default:	printf("チャンネル:解析不可です\n");	return -1;
		}
		
		data->sample_rate=read_32(file,SAMPLE_RATE_HEAD);											//サンプリング周波数解析
		switch(data->sample_rate){
			case SAMPLE8K:	printf("サンプリング周波数:8khzです。\n");	break;
			case SAMPLE44K1:	printf("サンプリング周波数:44.1khzです。\n");	break;
			default:	printf("サンプリング周期:解析不可です\n");	return -1;
		}
		
		data->bits_per_sample=read_16(file,BITS_PER_SAMPLE_HEAD);									//ビット/サンプル解析
		printf("ビット/サンプル:%u bytes/sampleです。\n",(unsigned int)data->bits_per_sample);
		
		data->block_size=read_16(file,BLOCK_SIZE_HEAD);												//ブロックサイズ解析
		printf("ブロックサイズ:%u bytesです。\n",(unsigned int)data->block_size);
		
		data->byterate=read_32(file,BYTERATE_HEAD);													//バイトレート解析
		printf("バイトレート:%u byte/secです。\n",(unsigned int)data->byterate);
		
		data->subchunk2size=read_32(file,SUBCHUNK2SIZE_HEAD);										//サブチャンク2サイズ解析
		printf("チャンク2:%u byteです。\n",(unsigned int)data->subchunk2size);
		
	}else{
		printf("RIFF見つかりませんでした\n");														
		return -1;
	}

	return 0;
}

int alloc_filedata(struct file_data *data,FILE *fp){												//ファイルデータ分のメモリサイズを確保する関数
	
	if((data->file_music_8=malloc(data->subchunk2size))==NULL){
		printf("ファイルデータ分の確保失敗");
		return -1;
	}
	
	fseek(fp,HEADERSIZE,SEEK_SET);
	
	if(fread(data->file_music_8,sizeof(char),data->subchunk2size,fp)!=data->subchunk2size){
		printf("読み込みエラー");
		return -1;
	}
	return 0;
}
	

unsigned int read_16(char *file,int byte){															//リトルエンディアンなので変換する(16bit)
	return ((unsigned char)file[byte]|((unsigned char)file[byte+1]<<8));
}

unsigned int read_32(char *file,int byte){															//リトルエンディアンなので変換する(32bit)
	return ((unsigned char)file[byte]|((unsigned char)file[byte+1]<<8)|((unsigned char)file[byte+2]<<16)|((unsigned char)file[byte+3]<<24));
}

unsigned int re_16(char *file,int byte){															//変換したぶんをint型にして戻す(16bit)
	return ((unsigned char)file[byte+1]|((unsigned char)file[byte]<<8));
}

unsigned int re_32(char *file,int byte){															//変換したぶんをint型にし戻す(32bit)
	return ((unsigned char)file[byte+3]|((unsigned char)file[byte+2]<<8)|((unsigned char)file[byte+1]<<16)|((unsigned char)file[byte]<<24));
}

void write_16(unsigned char *file,int x){															//int型だと扱いにくいのでunsigned char型にする(16bit)
	file[0]=x&0xFF;
	file[1]=(x>>8)&0xFF;
	return;
}

void write_32(unsigned char *file,int x){															//int型だと扱いにくいのでunsigned char型にする(32bit)
	file[0]=x & 0xFF;
	file[1]=(x>>8) & 0xFF;
	file[2]=(x>>16) & 0xFF;
	file[3]=(x>>24) & 0xFF;
	return;
}