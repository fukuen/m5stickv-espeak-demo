#ifndef __M5STICKV_ESPEAK_H
#define __M5STICKV_ESPEAK_H

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/speak_lib.h>

#include "sysctl.h"

class M5StickVeSpeak{
public:
    M5StickVeSpeak();
    ~M5StickVeSpeak();
    int begin();
    int speak(char * text);
    int end();
    
};

#endif