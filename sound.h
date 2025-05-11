#ifndef SOUND_H
#define SOUND_H


#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"

init_audio_device();
steps_sound = load_sound("resources/Steps.wav");
play_sound(steps_sound);

load_music_stream("resources/Music.wav");



close_audio_device();
#endif