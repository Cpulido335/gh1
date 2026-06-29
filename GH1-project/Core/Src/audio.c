#include "audio.h"
#include "fatfs.h"
#include <string.h>

//config
#define AUDIO_BUF_SIZE 4096   // must be even
#define WAV_HEADER_SIZE 44

// ===== EXTERNAL HANDLES (from CubeMX / main.c) =====
extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim7;

//internal state
static FIL file;
static uint16_t audio_buffer[AUDIO_BUF_SIZE];
static UINT bytes_read;
static volatile uint8_t playing = 0;

//AUDIO implementation

//fread/ point the fptr at the 44th/45th byte of the song.wav on the SD card
//begin loading the 2nd buffer with those bytes
//trigger an interrupt BUFFER_FILLED_UP_NOW
//once the buffer is filled up the DAC will convert those those bytes to analog signal with timer7 that ticks 44.1khz


//while the DAC has just started playing start loading the 1st buffer again with the next bytes

//when the DAC finishes playing the second buffer, swap the second and first.

//once the second and first are swapped start loading up the first again and repeat until song is finished

void Audio_TestTone(void)
{
    for (uint32_t i = 0; i < AUDIO_BUF_SIZE; i++)
    {
        //simple square wave
        if (i & 0x20)
            audio_buffer[i] = 3000;
        else
            audio_buffer[i] = 1000;
    }

    HAL_TIM_Base_Start(&htim7);

    HAL_DAC_Start_DMA(
        &hdac,
        DAC_CHANNEL_1,
        (uint32_t*)audio_buffer,
        AUDIO_BUF_SIZE,
        DAC_ALIGN_12B_R
    );
}

static uint8_t raw[AUDIO_BUF_SIZE * 2];

static void fill_buffer(uint16_t *buf, uint32_t samples)
{
    UINT br = 0;
    FRESULT res;

    res = f_read(&file, raw, samples * 2, &br);

    static volatile uint32_t last_fread_result = 0;
    static volatile uint32_t last_bytes_read   = 0;
    static volatile uint32_t fill_mode         = 0;
    static volatile int16_t  peak_sample       = 0;

    last_fread_result = res;
    last_bytes_read   = br;

    //SAFETY: silence if read fails
    if (res != FR_OK || br == 0)
    {
        fill_mode = 0;

        for (uint32_t i = 0; i < samples; i++)
            buf[i] = 2048;

        return;
    }

    fill_mode = 1;

    uint32_t samples_read = br / 2;

    //SAFE gain (start low)
    const float gain = 0.03f;   //0.01 = quiet, 0.05 = medium, 0.07  = loud

    for (uint32_t i = 0; i < samples_read; i++)
    {
        int16_t sample =
            (int16_t)(raw[i * 2] | (raw[i * 2 + 1] << 8));

        //track peak safely
        if (sample > peak_sample) peak_sample = sample;

        //apply gain BEFORE DAC conversion
        sample = (int16_t)(sample * gain);

        //convert to unsigned DAC range (12-bit centered at 2048)
        int32_t dac = (int32_t)sample;

        dac = (dac >> 3) + 2048;   // scale down to avoid clipping

        //clamp (EXTREMELY IMPORTANT)
        if (dac > 4095) dac = 4095;
        if (dac < 0)    dac = 0;

        buf[i] = (uint16_t)dac;
    }

    //fill remainder with silence
    for (uint32_t i = samples_read; i < samples; i++)
    {
        buf[i] = 2048;
    }
}


void Audio_Start(const char *filename)
{
	//TEST START
	  FIL test;
	    UINT br;
	    uint8_t buf[16];
	    FRESULT res;

	    res = f_open(&test, "exporttestm.wav", FA_READ);

	    if (res == FR_OK)
	    {
	        res = f_read(&test, buf, 16, &br);
	        f_close(&test);
	    }
	    else
	    {
	        int breakpoint = 1;
	        // res contains error reason
	    }
	//TEST END


	if (f_open(&file, filename, FA_READ) != FR_OK)
        return;

    //skip WAV header
    f_lseek(&file, WAV_HEADER_SIZE);

    //pre-fill entire buffer
    fill_buffer(audio_buffer, AUDIO_BUF_SIZE);

    playing = 1;


    //start timer (drives DAC via TRGO) //start the timer before starting DMA
    HAL_TIM_Base_Start(&htim7);

    //start DAC DMA (circular mode assumed)
    HAL_DAC_Start_DMA(&hdac,
                      DAC_CHANNEL_1,
                      (uint32_t *)audio_buffer,
                      AUDIO_BUF_SIZE,
                      DAC_ALIGN_12B_R);

}


void Audio_Stop(void)
{
    playing = 0;

    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
    HAL_TIM_Base_Stop(&htim7);

    f_close(&file);
}

//DMA callbacks
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    if (!playing) return;

    fill_buffer(&audio_buffer[0], AUDIO_BUF_SIZE / 2);
}


void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    if (!playing) return;

    fill_buffer(&audio_buffer[AUDIO_BUF_SIZE / 2], AUDIO_BUF_SIZE / 2);
}
