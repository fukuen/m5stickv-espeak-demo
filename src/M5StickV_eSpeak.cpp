#include "M5StickV_eSpeak.h"
#include "fpioa.h"
#include "gpio.h"
#include "sysctl.h"
#include "i2s.h"
#include "setlengths.h"

#define FRAME_LEN 2048
uint16_t tx_buf[FRAME_LEN * 2];

M5StickVeSpeak::M5StickVeSpeak(){

}

M5StickVeSpeak::~M5StickVeSpeak(){
    
}

int SynthCallback(short *wav, int numsamples, espeak_EVENT *events) {
    if (wav == NULL) {
        return 1; // NULL means done.
    }

    for (int i = 0; i < numsamples; i++) {
        tx_buf[2 * i    ] = (uint16_t)wav[i];
        tx_buf[2 * i + 1] = 0;
    }
    dmac_wait_done(DMAC_CHANNEL1);
    i2s_play(I2S_DEVICE_2, DMAC_CHANNEL1, (uint8_t*)tx_buf, numsamples * 2, 512, 16, 2);

    return 0; // 0 continues synthesis, 1 aborts 
}

int
M5StickVeSpeak::begin()
{
    // enable SPK
    fpioa_set_function(25, FUNC_GPIO1);
    gpio_set_drive_mode(1, GPIO_DM_OUTPUT);
    gpio_set_pin(1, GPIO_PV_HIGH);

    fpioa_set_function(17, FUNC_I2S2_OUT_D1);
    fpioa_set_function(14, FUNC_I2S2_WS);	
    fpioa_set_function(15, FUNC_I2S2_SCLK);

    //i2s init
    i2s_init(I2S_DEVICE_2, I2S_TRANSMITTER, 0xC);

    i2s_tx_channel_config(I2S_DEVICE_2, I2S_CHANNEL_1,
            RESOLUTION_16_BIT, SCLK_CYCLES_32,
            TRIGGER_LEVEL_4, STANDARD_MODE);
//            TRIGGER_LEVEL_4, RIGHT_JUSTIFYING_MODE);

	int sample_rate = espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, NULL, 0);
    i2s_set_sample_rate(I2S_DEVICE_2, (uint32_t)sample_rate);

	espeak_SetSynthCallback(&SynthCallback);
//	espeak_ng_STATUS status = espeak_ng_SetVoiceByName("ja");
	SetParameter(espeakRATE, 60, 0);
	SetParameter(espeakVOLUME, 10, 0);

    return 0;
}

int
M5StickVeSpeak::speak(char * text)
{
	espeak_Synth(text, strlen(text)+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL);

	espeak_Synchronize();

    return 0;
}

int
M5StickVeSpeak::end()
{
	espeak_Terminate();

    return 0;
}
