// avc.cpp : Defines the entry point for the console application.
//
#include "stdio.h"
typedef short                       INT16;
typedef int       			INT32;
typedef unsigned int               UINT32;


#define APP_INPUT_BUFF_SIZE  (0x1000)
#define AVC_GAIN_OFFSET (2)
#define AVC_THRESHOLD (22)

static UINT32 avc_gain_offset = (0x1<<12)*AVC_GAIN_OFFSET;

INT16 app_in_buff_left[APP_INPUT_BUFF_SIZE];//0x1000

INT16 avc_threshold_table[]= {
    14164,12623,11250,10027,8936,7965,7098,6326,5638,5025,
    4479,3991,3557,3170,2826,2518,2244,2000,1783,1589,
    1416,1262,1125,1002,893,796,709,632,563,502,
    447,399,355,317,282,251,224,200,178,158,141
};
void serv_app_msg_avc(INT16 *databufferIn,INT32 len)
{
    static UINT32 avc_av_fs_count_l = 0,avc_av_fs_sum_l = 0,avc_av_fs_average_l = 0,avc_gain_l = (1<<12);
    static UINT32 avc_av_fs_count_r = 0,avc_av_fs_sum_r = 0,avc_av_fs_average_r = 0,avc_gain_r = (1<<12);
    UINT32 i;
    for(i = 0; i < len; i++) {
        if(avc_av_fs_count_l++ ==48000) {
            avc_av_fs_average_l =avc_av_fs_sum_l/48000;
            avc_av_fs_count_l=0;
            avc_av_fs_sum_l=0;
            if(avc_av_fs_average_l > avc_threshold_table[AVC_THRESHOLD]) {
                avc_gain_l = (avc_threshold_table[AVC_THRESHOLD]<<8)/avc_av_fs_average_l;
                avc_gain_l *= avc_gain_offset;
                avc_gain_l >>= 8;
            } else {
                avc_gain_l=avc_gain_offset;
            }
            printf("0x99998888\n");
            printf("%x,%d\n",avc_av_fs_average_l,avc_av_fs_average_l);
            printf("%x,%d\n",avc_gain_l,avc_gain_l);
        }
        if(databufferIn[i] >=0) {
            avc_av_fs_sum_l += databufferIn[i];
        } else {
            avc_av_fs_sum_l += (- databufferIn[i]);
        }
        databufferIn[i] =(databufferIn[i]*avc_gain_l)>>12;
    }

}
int main(int argc, char* argv[])
{
	FILE *fp_in,*fp_out,*fp_test;
	int cnt,n;
	fp_in  = fopen("avc.bin","rb");//48_1K_16bit.bin 44_1_1k_16bit.bin 96_1k_16bit.bin resampe_test_8K.bin 16k_16bit.bin Aa22050-l-channel.pcm
	if (fp_in == NULL)
	{
		printf("aaaaab\n");
		return -1;
	}
	fp_out  = fopen("48_1K_16bit_out.bin","wb");//48_1K_16bit_out.bin 44_1K_16bit_out.bin 96_1K_16bit_out.bin 
	if (fp_out == NULL)
	{	
		printf("ddddd\n");
		return -2;
	}
	while(1)
	{
		n = fread(&app_in_buff_left,2,APP_INPUT_BUFF_SIZE,fp_in);
		printf("read n = %d\n",n);
		if (n != APP_INPUT_BUFF_SIZE)
		{
			serv_app_msg_avc(app_in_buff_left,n);
			fwrite(app_in_buff_left,2,n,fp_out);
			printf("finished \n");
			break;
		}
		serv_app_msg_avc(app_in_buff_left,n);
		fwrite(app_in_buff_left,2,n,fp_out);
	}
	
	fclose(fp_in);
	fclose(fp_out);
	printf("Hello World!\n");
	return 0;
}

