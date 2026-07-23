#ifndef ECHO_AUDIO_H
#define ECHO_AUDIO_H

#include "raylib.h"
#include "math.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SFX_PULSE_BOOM,
    SFX_ECHO_REFLECT,
    SFX_CRYSTAL_COLLECT,
    SFX_SILENCE_SHIELD,
    SFX_ENEMY_ATTACK,
    SFX_PLAYER_HURT,
    SFX_PORTAL_WIN,
    SFX_DECOY_LAND
} SoundEffectType;

typedef struct {
    SoundEffectType type;
    float time;
    float duration;
    float vol_l;
    float vol_r;
    bool active;
} SfxVoice;

typedef struct {
    AudioStream stream;
    bool active;
    float global_volume;
    float time_accumulator;
    float noise_level; // current noise level for enemy AI
    
    SfxVoice sfx_voices[16];
} AudioEngine;

bool audio_init(AudioEngine *ae);
void audio_reset(AudioEngine *ae);
void audio_update(AudioEngine *ae, float dt);
void audio_play_sfx(AudioEngine *ae, SoundEffectType sfx, float volume);
void audio_play_sfx_3d(AudioEngine *ae, SoundEffectType sfx, Vec3 sound_pos, Vec3 listener_pos, Vec3 listener_forward, float volume);
void audio_emit_noise(AudioEngine *ae, float noise_amount);
void audio_close(AudioEngine *ae);

#endif // ECHO_AUDIO_H
