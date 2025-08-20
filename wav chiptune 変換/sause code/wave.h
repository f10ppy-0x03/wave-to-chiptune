//wave.c
#include <windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<math.h>
																									//ヘッダ解析に必要な定数
#define HEADERSIZE	44
#define	RIFF		"RIFF"

#define PCM			0x0001
#define A_LOW		0x0006
#define U_LOW		0x0007
#define MONORAL		0x0001
#define STEREO		0x0002
#define SAMPLE8K	0x00001F40
#define SAMPLE44K1	0x0000AC44

																									//ヘッダのデータの先頭の位置
#define AUDIO_FORMAT_HEAD		20
#define CHANNELS_HEAD			22
#define SAMPLE_RATE_HEAD		24
#define BYTERATE_HEAD			28
#define	BLOCK_SIZE_HEAD			32
#define BITS_PER_SAMPLE_HEAD	34
#define SUBCHUNK2SIZE_HEAD		40

struct file_data{																					//元ファイルのデータ
	int riff,audio_format,num_channels,sample_rate,bits_per_sample,block_size,byterate,subchunk2size;
	unsigned char *file_music_8;
	
};

struct data_change{																					//chiptune変換後の音声データ
	unsigned char *unsigned_8bit_data,*unsigned_8bit_data_l,*unsigned_8bit_data_r;
	int subchunk2size;
};

struct chiptune_data{																				//変換後の全データ
	int chunk_size,fmt_chunk,audio_format,num_channels,sample_rate,byterate,block_size,bits_per_sample,subchunk2size;
	
	unsigned char *chunk_size_b,*fmt_chunk_b,*audio_format_b,*num_channels_b,*sample_rate_b,*byterate_b,*block_size_b,*bits_per_sample_b,*subchunk2size_b;
	unsigned char *chiptune_music;
	unsigned char *binary;
};

int search_filename(char **);
int controll_file(char *,struct file_data *);
int judgefile(char *,struct file_data *);
unsigned int read_16(char *,int);
unsigned int read_32(char *,int);
int alloc_filedata(struct file_data *,FILE *);



//trans.c
void change_sign_16bit(struct file_data *,struct data_change *);
void change_unsign_8bit(struct file_data *,struct data_change *);
void chiptune_wave(struct data_change *);
void chiptune(struct data_change *,int ,int ,int );
void least_squares_method(struct data_change *,int ,int );
void write_16(unsigned char *,int );
void write_32(unsigned char *,int );


//writing.c
char *create_newfile(FILE **);
void header_and_data(struct file_data *,struct data_change *,struct chiptune_data *);
int write_data(struct chiptune_data *);
int write_newfile(struct chiptune_data *,FILE *);
int writing(struct file_data *,struct data_change *,struct chiptune_data *);
void allocer(struct chiptune_data *);
void freer(struct chiptune_data *);
