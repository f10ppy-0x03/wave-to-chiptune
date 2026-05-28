//wave.c
#include <windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<math.h>
#include<limits.h>
							
#define READ_16(data,base) ( ( (uint32_t) (*(data+base+1) ) << 8) | \
							   (uint32_t) (*(data+base) ) )  

#define READ_32(data,base) ( ( (uint32_t) (*(data+base+3) ) << 24) | \
							 ( (uint32_t) (*(data+base+2) ) << 16) | \
							 ( (uint32_t) (*(data+base+1) ) << 8)  | \
							   (uint32_t) (*(data+base) ) ) 


//ヘッダ解析に必要な定数
#define HEADERSIZE		44


//RiffID is only "RIFF".
#define	RIFF		"RIFF"

//Format is "WAVE" or "AVI".
#define WAVE_FORMAT		"WAVE"
#define AVI_FORMAT		"AVI"

//ChunkID is only "fmt".
#define FMT		"fmt"

//If liner PCM,chunkSize is 0x10.
#define LINER_PCM 0x10

//Wave format tag specifys the type of audio format.
#define WFORMAT_TAG_UNKNOWN		0x0
#define PCM		0x01
#define WFORMAT_TAG_MICROSOFT_ADPCM			0x02
#define WFORMAT_TAG_IEEE_FLOAT			0x03	
#define A_LOW			0x06
#define U_LOW			0x07
#define WFORMAT_TAG_IMA_ADPCM			0x11
#define WFORMAT_TAG_G723_ADPCM			0x16
#define WFORMAT_TAG_GSM_6_10			0x31
#define WFORMAT_TAG_G_721_ADPCM			0x40
#define WFORMAT_TAG_MPEG			0x50
#define WFORMAT_TAG_EXPERIMENTAL	0xFFFF

//Channnels is MONORAL or STEREO.
#define MONORAL			0x0001
#define STEREO			0x0002

//This sample rate is WAV sample rate.For example 44k1 is 44.1k sampling rate.
#define SAMPLE8K		0x1F40
#define SAMPLE22K05		0x5622
#define SAMPLE24K		0x5DC0
#define SAMPLE32K		0x7D00
#define SAMPLE44K1		0xAC44
#define SAMPLE48K		0xBB80


																									//ヘッダのデータの先頭の位置
#define AUDIO_FORMAT_HEAD		20
#define CHANNELS_HEAD			22
#define SAMPLE_RATE_HEAD		24
#define BYTERATE_HEAD			28
#define	BLOCK_SIZE_HEAD			32
#define BITS_PER_SAMPLE_HEAD	34
#define SUBCHUNK2SIZE_HEAD		40








typedef struct {

	uint8_t Riff_position;

	uint8_t RiffID;
	uint32_t filesize;
	const char* format;

}RIFFHeader;


typedef struct {

	uint8_t chunk_position;

	uint8_t chunk_ID;
	uint32_t chunk_size;
	uint16_t w_format_tag;
	uint16_t n_channnels;
	uint32_t sample_per_second;
	uint32_t avg_bytes_per_second;
	uint16_t block_align;
	uint16_t w_bits_per_sample;
	uint16_t cb_size; 

}FMTChunk;


typedef struct{
	uint8_t fact_chank  :1;
	uint8_t peak_chank	:1;
}OptionFlag;


typedef struct{

	uint8_t chunk_position;

	uint8_t chunk_ID;
	uint32_t chunk_size;
	uint32_t dw_sample_length;

}FactChunk;


typedef struct{

	uint8_t chunk_position;

	uint8_t chunk_ID;
	uint32_t chunk_size;
	uint32_t version;
	uint32_t time_stamp;

}PeakChunk;


typedef struct{

	uint8_t chunk_position;

	uint8_t chunk_ID;
	uint32_t chunk_size;
	uint8_t padding;
}DataChank;


typedef struct{

	RIFFHeader riff_header;
	FMTChunk format_chunk;
	FactChunk fact_chunk;
	PeakChunk option_peak;
	DataChank data_chunk;

	OptionFlag option_flag;

}Header;


typedef struct{

	uint8_t* start_data_pointer;
	uint8_t bit16_flag;

}PCM8bitData;

typedef struct{

	float* start_data_pointer;
	float* file_music;
	float* end_data_pointer;

}IEEEFloatData;


typedef struct{

	FILE *fp_now,*fp_base;
	int8_t header_size;
	Header header;
	PCM8bitData pcm_data;
	IEEEFloatData ieee_float_data;
	int8_t *binary_data;

}WAVE_FILE;
















int Search_Filename(char *);
int Controll_File(char *filename,WAVE_FILE *);
int Chank_Finder(WAVE_FILE *data);




int Tools(WAVE_FILE *data);
int8_t Audio_Format(WAVE_FILE *data);
int8_t Channnels(WAVE_FILE *data);
int8_t Sampling_Frequout(WAVE_FILE *data);
int8_t Block_Size(WAVE_FILE *data);
void Data_Chunk_Size(WAVE_FILE *data);







































