#include "platform.h"

const struct audio audio_info;
void devaudio_stop(uint8_t channel) {}
int devaudio_wait(uint8_t channel) {}
int devaudio_play(void) {}
int devaudio_ioctl(uarg_t op, void *val) {}
void devaudio_tick(void) {}
