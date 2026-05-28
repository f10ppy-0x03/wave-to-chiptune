#include "wave.h"

int Tools(WAVE_FILE *data){

uint8_t binary_header[data->header_size];

fseek(data->fp_now, 0, SEEK_SET);
fread(binary_header,sizeof(uint8_t),data->header_size,data->fp_now);

	
	if(memcmp(binary_header,RIFF,4)==0){																	//RIFFを見つける
		printf("RIFF:見つかりました\n");

		data->header.format_chunk.w_format_tag=READ_16(binary_header,data->header.format_chunk.chunk_position+8);	
		
		data->header.format_chunk.n_channnels=READ_16(binary_header,data->header.format_chunk.chunk_position+10);												//チャンネル解析
		
		data->header.format_chunk.sample_per_second=READ_32(binary_header,data->header.format_chunk.chunk_position+12);	
		
		data->header.format_chunk.avg_bytes_per_second=READ_32(binary_header,data->header.format_chunk.chunk_position+16);

		data->header.format_chunk.block_align=READ_16(binary_header,data->header.format_chunk.chunk_position+20);	
		
		data->header.format_chunk.w_bits_per_sample=READ_16(binary_header,data->header.format_chunk.chunk_position+22);									//ビット/サンプル解析												//ブロックサイズ解析
		
		data->header.data_chunk.chunk_size=READ_32(binary_header,data->header.data_chunk.chunk_position+4);
		
		Audio_Format(data);
		Channnels(data);
		Sampling_Frequout(data);
		Block_Size(data);
		Data_Chunk_Size(data);



		
	}else{
		printf("%s \n",RIFF);
		printf("%c %c %c %c \n",(char)binary_header[0],(char)binary_header[1],(char)binary_header[2],(char)binary_header[3]);
		printf("RIFF見つかりませんでした\n");														
		return -1;
	}

	return 0;
}


int8_t Audio_Format(WAVE_FILE *data){

	switch(data->header.format_chunk.w_format_tag){

		case WFORMAT_TAG_UNKNOWN:
        	printf("オーディオフォーマット:UNKNOWNです。\n");
            break;

		case PCM:
        	printf("オーディオフォーマット:PCMです。\n");	
            break;

		case WFORMAT_TAG_MICROSOFT_ADPCM:
        	printf("オーディオフォーマット:MICROSOFT_ADPCMです。\n");	
            break;

		case WFORMAT_TAG_IEEE_FLOAT:
        	printf("オーディオフォーマット:IEEE_FLOATです。\n");	
            break;

		case A_LOW:
        	printf("オーディオフォーマット:A-LOWです。\n");	
            break;

		case U_LOW:
        	printf("オーディオフォーマット:U-LOWです。\n");	
            break;

		case WFORMAT_TAG_IMA_ADPCM:
        	printf("オーディオフォーマット:IMA_ADPCMです。\n");	
            break;

		case WFORMAT_TAG_G723_ADPCM:
        	printf("オーディオフォーマット:G723_ADPCMです。\n");	
            break;

		case WFORMAT_TAG_GSM_6_10:
        	printf("オーディオフォーマット:GSM_6_10です。\n");	
            break;

		case WFORMAT_TAG_G_721_ADPCM:
        	printf("オーディオフォーマット:G_721_ADPCMです。\n");	
            break;

		case WFORMAT_TAG_MPEG:
        	printf("オーディオフォーマット:MPEGです。\n");	
            break;

		case WFORMAT_TAG_EXPERIMENTAL:
        	printf("オーディオフォーマット:EXPERIMENTALです。\n");	
            break;

		default:
        	printf("オーディオフォーマット:解析不可です\n");	
            return -1;

    }


    return 1;

}


int8_t Channnels(WAVE_FILE *data){

	switch(data->header.format_chunk.n_channnels){

		case MONORAL:
        	printf("チャンネル:モノラルです。\n");	
            break;

		case STEREO:
        	printf("チャンネル:ステレオです。\n");	
            break;

		default:
        	printf("チャンネル:解析不可です\n");	
            return -1;
        
	}

    return 1;

}


int8_t Sampling_Frequout(WAVE_FILE *data){

	switch(data->header.format_chunk.sample_per_second){
		case SAMPLE8K:
        	printf("サンプリング周波数:8khzです。\n");	
            break;

		case SAMPLE44K1:
        	printf("サンプリング周波数:44.1khzです。\n");	
            break;

		default:
        	printf("サンプリング周期:解析不可です\n");	
            return -1;

	}

    return 1;

}


int8_t Block_Size(WAVE_FILE *data){

	printf("ビット/サンプル:%u bits/sampleです。\n",(unsigned int)data->header.format_chunk.w_bits_per_sample);
	printf("ブロックサイズ:%u bytesです。\n",(unsigned int)data->header.format_chunk.block_align);
	printf("バイトレート:%u byte/secです。\n",(unsigned int)data->header.format_chunk.avg_bytes_per_second);

    return 1;

}


void Data_Chunk_Size(WAVE_FILE *data){

printf("チャンク:%u byteです。\n",(unsigned int)data->header.data_chunk.chunk_size);
return ;

}