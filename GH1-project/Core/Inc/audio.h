#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// ===== PUBLIC FUNCTIONS =====
void Audio_TestTone();
void Audio_Start(const char *filename);
void Audio_Stop(void);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_H
