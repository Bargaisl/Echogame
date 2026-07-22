#include "audio.h"
#include "math.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern void log_debug(const char *msg);

#define SAMPLE_RATE 48000
#define CHANNELS 2

static uint32_t audio_rng = 987654321;
static AudioEngine *g_audio_engine = NULL;

static float synth_sine(float freq, float time) {
    return sinf(TAU * freq * time);
}

static float synth_saw(float freq, float time) {
    float phase = fmodf(freq * time, 1.0f);
    return 2.0f * phase - 1.0f;
}

static float synth_square(float freq, float time) {
    return (sinf(TAU * freq * time) >= 0.0f) ? 0.7f : -0.7f;
}

static float synth_noise(void) {
    return rnd_range(&audio_rng, -1.0f, 1.0f);
}

// Pentatonic frequencies (Hz) for procedural arpeggiator
static const float ARPEGGIO_NOTES[] = {
    110.00f, // A2
    130.81f, // C3
    146.83f, // D3
    164.81f, // E3
    196.00f, // G3
    220.00f, // A3
    261.63f, // C4
    293.66f, // D4
    329.63f  // E4
};
static const int NUM_ARPEGGIO_NOTES = 9;

// Asynchronous Audio Callback with 3D Spatial Panning
static void audio_stream_callback(void *buffer, unsigned int frames) {
    if (!g_audio_engine || !g_audio_engine->active) {
        memset(buffer, 0, frames * CHANNELS * sizeof(int16_t));
        return;
    }

    int16_t *pcm_buffer = (int16_t *)buffer;
    float dt_sample = 1.0f / (float)SAMPLE_RATE;

    for (unsigned int i = 0; i < frames; i++) {
        g_audio_engine->time_accumulator += dt_sample;
        if (g_audio_engine->time_accumulator >= 3600.0f) {
            g_audio_engine->time_accumulator = fmodf(g_audio_engine->time_accumulator, 3600.0f);
        }
        float t = g_audio_engine->time_accumulator;
        float sample_l = 0.0f;
        float sample_r = 0.0f;

        // 1. Procedural Ambient Music Generation
        float bass_freq = 55.0f;
        float bass_sub = synth_sine(bass_freq, t) * 0.25f + synth_saw(bass_freq * 0.5f, t) * 0.1f;

        float step_time = 0.20f;
        int step_idx = (int)(t / step_time) % NUM_ARPEGGIO_NOTES;
        float note_t = fmodf(t, step_time);
        float arp_env = expf(-note_t * 8.0f);
        float arp_freq = ARPEGGIO_NOTES[step_idx];
        float arp_lead = synth_sine(arp_freq, t) * arp_env * 0.15f;

        float music_sample = (bass_sub + arp_lead) * 0.5f;
        sample_l += music_sample;
        sample_r += music_sample;

        // 2. Active 3D Spatial Sound Effects Synthesis
        for (int v = 0; v < 16; v++) {
            if (!g_audio_engine->sfx_voices[v].active) continue;

            float st = g_audio_engine->sfx_voices[v].time;
            float dur = g_audio_engine->sfx_voices[v].duration;
            float vl = g_audio_engine->sfx_voices[v].vol_l;
            float vr = g_audio_engine->sfx_voices[v].vol_r;
            float sfx_sample = 0.0f;

            if (st >= dur) {
                g_audio_engine->sfx_voices[v].active = false;
                continue;
            }

            float env = 1.0f - (st / dur);

            switch (g_audio_engine->sfx_voices[v].type) {
                case SFX_PULSE_BOOM: {
                    float freq = FAST_LERP(250.0f, 40.0f, st / dur);
                    sfx_sample = (synth_sine(freq, t) * 0.7f + synth_square(freq * 0.5f, t) * 0.3f) * env;
                    break;
                }
                case SFX_DECOY_LAND: {
                    float freq = FAST_LERP(400.0f, 60.0f, st / dur);
                    sfx_sample = (synth_sine(freq, t) * 0.8f + synth_saw(freq * 0.6f, t) * 0.2f) * env;
                    break;
                }
                case SFX_ECHO_REFLECT: {
                    float freq = 880.0f + sinf(st * 40.0f) * 100.0f;
                    sfx_sample = synth_sine(freq, t) * env * 0.4f;
                    break;
                }
                case SFX_CRYSTAL_COLLECT: {
                    float freq = 659.25f + floorf((st / dur) * 3.0f) * 200.0f;
                    sfx_sample = (synth_sine(freq, t) + synth_saw(freq * 0.5f, t) * 0.3f) * env * 0.5f;
                    break;
                }
                case SFX_SILENCE_SHIELD: {
                    sfx_sample = synth_noise() * env * 0.2f + synth_sine(150.0f, t) * env * 0.3f;
                    break;
                }
                case SFX_ENEMY_ATTACK: {
                    sfx_sample = synth_noise() * env * 0.6f + synth_saw(90.0f, t) * env * 0.4f;
                    break;
                }
                case SFX_PLAYER_HURT: {
                    sfx_sample = synth_square(60.0f, t) * env * 0.7f;
                    break;
                }
                case SFX_PORTAL_WIN: {
                    float f1 = 440.0f, f2 = 554.37f, f3 = 659.25f;
                    sfx_sample = (synth_sine(f1, t) + synth_sine(f2, t) + synth_sine(f3, t)) * (env / 3.0f) * 0.6f;
                    break;
                }
            }

            sample_l += sfx_sample * vl;
            sample_r += sfx_sample * vr;
            g_audio_engine->sfx_voices[v].time += dt_sample;
        }

        sample_l = FAST_CLAMP(sample_l * g_audio_engine->global_volume, -1.0f, 1.0f);
        sample_r = FAST_CLAMP(sample_r * g_audio_engine->global_volume, -1.0f, 1.0f);

        pcm_buffer[i * 2]     = (int16_t)(sample_l * 32767.0f);
        pcm_buffer[i * 2 + 1] = (int16_t)(sample_r * 32767.0f);
    }
}

bool audio_init(AudioEngine *ae) {
    if (!ae) return false;
    log_debug("audio_init: clearing memory...");
    memset(ae, 0, sizeof(AudioEngine));

    log_debug("audio_init: Calling InitAudioDevice...");
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        log_debug("audio_init: InitAudioDevice failed!");
        return false;
    }
    log_debug("audio_init: Audio device ready.");

    log_debug("audio_init: Loading audio stream (48000 Hz, 16-bit, 2 Channels)...");
    ae->stream = LoadAudioStream(SAMPLE_RATE, 16, CHANNELS);
    if (!IsAudioStreamValid(ae->stream)) {
        log_debug("audio_init: LoadAudioStream failed!");
        return false;
    }
    log_debug("audio_init: Audio stream loaded successfully.");

    ae->global_volume = 0.8f;
    ae->noise_level = 0.0f;
    ae->active = true;

    g_audio_engine = ae;
    log_debug("audio_init: Setting audio callback...");
    SetAudioStreamCallback(ae->stream, audio_stream_callback);
    log_debug("audio_init: Playing audio stream...");
    PlayAudioStream(ae->stream);
    log_debug("audio_init: Done.");

    return true;
}

void audio_play_sfx(AudioEngine *ae, SoundEffectType sfx, float volume) {
    if (!ae) return;
    for (int i = 0; i < 16; i++) {
        if (!ae->sfx_voices[i].active) {
            ae->sfx_voices[i].type = sfx;
            ae->sfx_voices[i].time = 0.0f;
            ae->sfx_voices[i].vol_l = volume;
            ae->sfx_voices[i].vol_r = volume;
            ae->sfx_voices[i].active = true;

            switch (sfx) {
                case SFX_PULSE_BOOM:      ae->sfx_voices[i].duration = 0.8f; break;
                case SFX_DECOY_LAND:     ae->sfx_voices[i].duration = 0.7f; break;
                case SFX_ECHO_REFLECT:    ae->sfx_voices[i].duration = 0.35f; break;
                case SFX_CRYSTAL_COLLECT: ae->sfx_voices[i].duration = 0.5f; break;
                case SFX_SILENCE_SHIELD:  ae->sfx_voices[i].duration = 1.0f; break;
                case SFX_ENEMY_ATTACK:    ae->sfx_voices[i].duration = 0.4f; break;
                case SFX_PLAYER_HURT:     ae->sfx_voices[i].duration = 0.3f; break;
                case SFX_PORTAL_WIN:      ae->sfx_voices[i].duration = 1.5f; break;
            }
            break;
        }
    }
}

void audio_play_sfx_3d(AudioEngine *ae, SoundEffectType sfx, Vec3 sound_pos, Vec3 listener_pos, Vec3 listener_forward, float volume) {
    if (!ae) return;

    // Calculate 3D Distance Attenuation
    float dist = vec3_dist(sound_pos, listener_pos);
    float atten = 1.0f / (1.0f + (dist / 140.0f) * (dist / 140.0f));
    float final_vol = volume * atten;

    if (final_vol < 0.02f) return; // Silent at distance

    // Calculate 3D Panning (Left vs Right)
    Vec3 to_sound = vec3_normalize(vec3_sub(sound_pos, listener_pos));
    Vec3 right = vec3_normalize((Vec3){listener_forward.z, 0.0f, -listener_forward.x});
    float dot_right = vec3_dot(to_sound, right);

    float vol_l = FAST_CLAMP(0.5f - dot_right * 0.5f, 0.15f, 1.0f) * final_vol;
    float vol_r = FAST_CLAMP(0.5f + dot_right * 0.5f, 0.15f, 1.0f) * final_vol;

    for (int i = 0; i < 16; i++) {
        if (!ae->sfx_voices[i].active) {
            ae->sfx_voices[i].type = sfx;
            ae->sfx_voices[i].time = 0.0f;
            ae->sfx_voices[i].vol_l = vol_l;
            ae->sfx_voices[i].vol_r = vol_r;
            ae->sfx_voices[i].active = true;

            switch (sfx) {
                case SFX_PULSE_BOOM:      ae->sfx_voices[i].duration = 0.8f; break;
                case SFX_DECOY_LAND:     ae->sfx_voices[i].duration = 0.7f; break;
                case SFX_ECHO_REFLECT:    ae->sfx_voices[i].duration = 0.35f; break;
                case SFX_CRYSTAL_COLLECT: ae->sfx_voices[i].duration = 0.5f; break;
                case SFX_SILENCE_SHIELD:  ae->sfx_voices[i].duration = 1.0f; break;
                case SFX_ENEMY_ATTACK:    ae->sfx_voices[i].duration = 0.4f; break;
                case SFX_PLAYER_HURT:     ae->sfx_voices[i].duration = 0.3f; break;
                case SFX_PORTAL_WIN:      ae->sfx_voices[i].duration = 1.5f; break;
            }
            break;
        }
    }
}

void audio_emit_noise(AudioEngine *ae, float noise_amount) {
    if (!ae) return;
    ae->noise_level += noise_amount;
    if (ae->noise_level > 1.0f) ae->noise_level = 1.0f;
}

void audio_update(AudioEngine *ae, float dt) {
    if (!ae || !ae->active) return;
    ae->noise_level -= dt * 0.4f;
    if (ae->noise_level < 0.0f) ae->noise_level = 0.0f;
}

void audio_close(AudioEngine *ae) {
    if (!ae) return;
    g_audio_engine = NULL;
    if (IsAudioStreamValid(ae->stream)) {
        StopAudioStream(ae->stream);
        UnloadAudioStream(ae->stream);
    }
    CloseAudioDevice();
    ae->active = false;
}
