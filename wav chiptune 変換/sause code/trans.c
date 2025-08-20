#include"wave.h"

void change_sign_16bit(struct file_data *data,struct data_change *data_change){						//16bit符号付のデータを8bit符号なしに切り替える
	if(data->num_channels==MONORAL){
		data_change->subchunk2size=data->subchunk2size/2;											//モノラルの場合
		data_change->unsigned_8bit_data=malloc(data_change->subchunk2size);
		
		if(data_change->unsigned_8bit_data==NULL) printf("エラー:サンプル分のメモリー確保に失敗しました\n");
		
		for(int i=0;i<data->subchunk2size;i=i+2){
			short music_data=(short)(data->file_music_8[i+1]<<8)|(short)(data->file_music_8[i]&0xFF);
			data_change->unsigned_8bit_data[i/2]=(unsigned char)((music_data>>8)+128);
		}
	}else{
																									//ステレオの場合
		data_change->subchunk2size=data->subchunk2size/2;																					
		data_change->unsigned_8bit_data_l=malloc(data_change->subchunk2size/2);
		data_change->unsigned_8bit_data_r=malloc(data_change->subchunk2size/2);
		
		for(int i=0;i<data->subchunk2size;i=i+4){
			
			short music_data_l=(short)(data->file_music_8[i+1]<<8)|(short)(data->file_music_8[i]&0xFF);
			short music_data_r=(short)(data->file_music_8[i+3]<<8)|(short)(data->file_music_8[i+2]&0xFF);
			data_change->unsigned_8bit_data_l[i/2]=(unsigned char)((music_data_l>>8)+128);
			data_change->unsigned_8bit_data_r[i/2]=(unsigned char)((music_data_r>>8)+128);
		}	
	}
	return;
	
}
	
void change_unsign_8bit(struct file_data *data,struct data_change *data_change){					//8bit符号なしを8bit符号なしに切り替える
	if(data->num_channels==MONORAL){	
		data_change->subchunk2size=data->subchunk2size;												//モノラルの場合
		data_change->unsigned_8bit_data=malloc(data_change->subchunk2size);
		if(data_change->unsigned_8bit_data==NULL) printf("エラー:サンプル分のメモリー確保に失敗しました\n");
		
		for(int i=0;i<data->subchunk2size;i++){
			data_change->unsigned_8bit_data[i]=data->file_music_8[i];
		}
	}else{	
																									//ステレオの場合
		data_change->subchunk2size=data->subchunk2size;
		data_change->unsigned_8bit_data_l=malloc(data_change->subchunk2size/2);
		data_change->unsigned_8bit_data_r=malloc(data_change->subchunk2size/2);
		
		for(int i=0;i<data->subchunk2size;i=i+2){
			data_change->unsigned_8bit_data_l[i]=data->file_music_8[i];
			data_change->unsigned_8bit_data_r[i]=data->file_music_8[i+1];
		}
	}
	return;
	
}	

void chiptune_wave(struct data_change *data_change){												//wavからchiptuneに変換する
	
	int i=0;
	int base=0;
	int max_sound=127;
	int max_number=0;
	int zero_flag=0;
	int neary_deff=127;
	
	while(i<data_change->subchunk2size){
		
		if(abs((int)data_change->unsigned_8bit_data[i]-128)>=abs(max_sound-128)){					//最大値と最大の時の配列番号を記憶する
			max_sound=(int)data_change->unsigned_8bit_data[i];
			max_number=i;
		}
		
		if(abs((int)data_change->unsigned_8bit_data[i]-128)<=abs(neary_deff)){							//128に一番近い数を記録する。
			neary_deff=(int)data_change->unsigned_8bit_data[i]-128;
			zero_flag=0;
			i++;
			continue;
		}else{																						//変わらなかったら128に一番近い
			zero_flag=1;
		}
		
		if(zero_flag==1 && abs((int)data_change->unsigned_8bit_data[i-1]-128)>=neary_deff && i-base>=20){
			chiptune(data_change,max_number,base,i);
		}else{
			i++;
			continue;
		}
		
		base=i;
		zero_flag=0;
		max_sound=127;
		neary_deff=127;
		i++;
		
	}
}

void chiptune(struct data_change *data_change,int max_number,int base,int max){						//chiptuneにする関数

	least_squares_method(data_change,base,max_number);
	for(int i=max_number+1;i<=max;i++){
		data_change->unsigned_8bit_data[i]=128;														//最大から後は一気に128に落ちていく
	}
	return;
}

void least_squares_method(struct data_change *data_change,int base,int max_number){					//最小二乗法でsin波とかを直線に置き換える関数
	
	double sumx=0;
	double sumy=0;
	double sumxy=0;
	double avaragex=0;
	double avaragey=0;
	double dispersalx=0;
	double dispersalxy=0;
	double range=(double)(max_number-base+1);
	int no_rounding=0;
	
	for(int i=base;i<= max_number;i++){
		sumx+=i;
		sumy+=data_change->unsigned_8bit_data[i];	
	}
	avaragex=sumx/range;
	avaragey=sumy/range;																			//配列の数の平均と中身のデータの平均をとる
	
	sumx=0;
	
	for(int i=base;i<= max_number;i++){
	sumx+=pow(i-avaragex,2);
	sumxy+=(i-avaragex)*(data_change->unsigned_8bit_data[i]-avaragey);
	}
	dispersalx=sumx/range;
	dispersalxy=sumxy/range;																		//配列の数の分散と配列の数とデータの共分散をとる
	
	if(dispersalx==0){ 
    	for(int i=base;i<=max_number;i++){
        	data_change->unsigned_8bit_data[i]=128;
		}
		
	}else{
	
		for(int i=base;i<= max_number;i++){																//最小二乗法の定義に沿って計算する。0以下は0に255以上は255にする。
			no_rounding=(int)((dispersalxy/dispersalx)*(i-base)+(avaragey-(dispersalxy/dispersalx)*avaragex));
			if(no_rounding<0)	no_rounding=0;
			if(no_rounding>255)	no_rounding=255;
			data_change->unsigned_8bit_data[i]=no_rounding;
			if(abs((int)data_change->unsigned_8bit_data[i]-(int)data_change->unsigned_8bit_data[i-1])<=1 && i!=0){
				data_change->unsigned_8bit_data[i]=data_change->unsigned_8bit_data[i-1];
			}
		}
	}
	return;
	
}
					