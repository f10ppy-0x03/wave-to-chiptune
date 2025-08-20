#include "wave.h"

int writing(struct file_data *data,struct data_change *change,struct chiptune_data *chiptune){
	FILE *new_fp;
	char *new_file_name;
	
	while(1){
		new_file_name=create_newfile(&new_fp);
		if(new_file_name==NULL)	break;
		header_and_data(data,change,chiptune);
		write_data(chiptune);
		if(write_newfile(chiptune,new_fp)==-1)		break;
		return 0;
		}
		printf("エラー");
	return -1;
}

char *create_newfile(FILE **new_fp){																//新しいファイルを作成する関数
	char *new_filename;
	
	printf("新規ファイル名を入力>");
	search_filename(&new_filename);
	*new_fp=fopen(new_filename,"wb");																//ファイル名を入力させる
	
	if(*new_fp==NULL){
		printf("エラー:新規ファイル作成失敗\n");
		return NULL;
	}
	
	printf("ファイル作成成功\n");
	return new_filename;
	
}

void header_and_data( struct file_data *file_data,struct data_change *change,struct chiptune_data *chiptune){			//ヘッダとデータをセットする関数
	allocer(chiptune);
	chiptune->bits_per_sample=8;
	write_16(chiptune->bits_per_sample_b,chiptune->bits_per_sample);								//ビット/サンプルのデータをセット
	
	chiptune->sample_rate=file_data->sample_rate;
	write_32(chiptune->sample_rate_b,chiptune->sample_rate);										//サンプルレートのデータをセット
	
	chiptune->audio_format=file_data->audio_format;
	write_16(chiptune->audio_format_b,chiptune->audio_format);										//オーディオフォーマットのデータをセット
	
	if((unsigned int)file_data->bits_per_sample==16){												//元のデータが16bitだった場合
	
		chiptune->subchunk2size=file_data->subchunk2size/2;
		write_32(chiptune->subchunk2size_b,chiptune->subchunk2size);								//サブチャンク2のサイズのデータをセット
		
		chiptune->byterate=file_data->byterate/2;
		write_32(chiptune->byterate_b,chiptune->byterate);											//バイトレートのデータをセット
		
		chiptune->block_size=file_data->block_size/2;
		write_32(chiptune->block_size_b,chiptune->block_size);										//ブロックサイズのデータをセット
		
	}else{																							//元のデータが8bitだった場合
	
		chiptune->subchunk2size=file_data->subchunk2size;
		write_32(chiptune->subchunk2size_b,chiptune->subchunk2size);								//サブチャンク2のサイズのデータをセット
		
		chiptune->byterate=file_data->byterate/2;
		write_32(chiptune->byterate_b,chiptune->byterate);											//バイトレートのデータをセット
		
		chiptune->block_size=file_data->block_size;
		write_32(chiptune->block_size_b,chiptune->block_size);										//ブロックサイズのデータをセット
		
	}
	
	chiptune->chunk_size=chiptune->subchunk2size+HEADERSIZE-8;
	write_32(chiptune->chunk_size_b,chiptune->chunk_size);											//チャンクサイズのデータをセット
	
	if(file_data->num_channels==MONORAL){															//モノラルの場合
		
		chiptune->num_channels=MONORAL;
		write_16(chiptune->num_channels_b,chiptune->num_channels);									//チャンネル数のデータをセット

		chiptune->chiptune_music=(char*)malloc(chiptune->subchunk2size);
		for(int i=0;i<chiptune->subchunk2size;i++){
			chiptune->chiptune_music[i]=change->unsigned_8bit_data[i];								//サンプルのデータをセット
			
		}
	}else{																							//ステレオの場合
		chiptune->num_channels=STEREO;
		write_16(chiptune->num_channels_b,chiptune->num_channels);									//チャンネル数のデータをセット
		
		chiptune->chiptune_music=(char*)malloc(chiptune->subchunk2size);
		for(int i=0;i<chiptune->subchunk2size;i=i+2){
			chiptune->chiptune_music[i]=change->unsigned_8bit_data_l[i];
			chiptune->chiptune_music[i+1]=change->unsigned_8bit_data_r[i];							//サンプルのデータをセット
			
		}
	}
	chiptune->fmt_chunk=16;
	write_32(chiptune->fmt_chunk_b,chiptune->fmt_chunk);											//フォーマットチャンクをセット(var0.1)
	return;
}

int write_data(struct chiptune_data *chiptune_data){
	if((chiptune_data->binary=(char *)malloc((unsigned int)(chiptune_data->chunk_size+8)))==NULL){						//全部のデータを一つにまとめる関数(var0.1)
		printf("エラー:バイナリデータのメモリの確保失敗\n");
		return -1;
	}
	memcpy(chiptune_data->binary,"RIFF",4);
	memcpy(chiptune_data->binary+4,chiptune_data->chunk_size_b,4);
	memcpy(chiptune_data->binary+8,"WAVE",4);
	memcpy(chiptune_data->binary+12,"fmt ",4);
	memcpy(chiptune_data->binary+16,chiptune_data->fmt_chunk_b,4);
	memcpy(chiptune_data->binary+20,chiptune_data->audio_format_b,2);
	memcpy(chiptune_data->binary+22,chiptune_data->num_channels_b,2);
	memcpy(chiptune_data->binary+24,chiptune_data->sample_rate_b,4);
	memcpy(chiptune_data->binary+28,chiptune_data->byterate_b,4);
	memcpy(chiptune_data->binary+32,chiptune_data->block_size_b,2);
	memcpy(chiptune_data->binary+34,chiptune_data->bits_per_sample_b,2);
	memcpy(chiptune_data->binary+36,"data",4);
	memcpy(chiptune_data->binary+40,chiptune_data->subchunk2size_b,4);
	memcpy(chiptune_data->binary+44,chiptune_data->chiptune_music,chiptune_data->subchunk2size);
	return 0;
}

int write_newfile(struct chiptune_data *chiptune,FILE *new_fp){															//まとめたデータをファイルに書き込む関数
	if(fwrite(chiptune->binary,sizeof(char),(unsigned int)(chiptune->chunk_size+8),new_fp)!=(unsigned int)chiptune->chunk_size+8){
		("異常終了");
		fclose(new_fp);
		free(chiptune->binary);
		return -1;
		
	}else{
		printf("正常終了");
		fclose(new_fp);
		free(chiptune->binary);
		return 0;
	}
}

void allocer(struct chiptune_data *chiptune){														//メモリを確保する関数
	chiptune->chunk_size_b=(char *)malloc(sizeof(4));
	chiptune->fmt_chunk_b=(char *)malloc(sizeof(4));
	chiptune->audio_format_b=(char *)malloc(sizeof(2));
	chiptune->num_channels_b=(char *)malloc(sizeof(2));
	chiptune->sample_rate_b=(char *)malloc(sizeof(4));
	chiptune->byterate_b=(char *)malloc(sizeof(4));
	chiptune->block_size_b=(char *)malloc(sizeof(2));
	chiptune->bits_per_sample_b=(char *)malloc(sizeof(2));
	chiptune->subchunk2size_b=(char *)malloc(sizeof(4));
	return;
}

void freer(struct chiptune_data *chiptune){														//メモリを開放する関数
	free(chiptune->chunk_size_b);
	free(chiptune->fmt_chunk_b);
	free(chiptune->audio_format_b);
	free(chiptune->num_channels_b);
	free(chiptune->sample_rate_b);
	free(chiptune->byterate_b);
	free(chiptune->block_size_b);
	free(chiptune->bits_per_sample_b);
	free(chiptune->subchunk2size_b);
	
	
	
	
	
	
	return;
}