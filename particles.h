#ifndef ECHO_PARTICLES_H
#define ECHO_PARTICLES_H

#include "raylib.h"
#include "math.h"
#include <stdbool.h>

#define MAX_PARTICLES 4096

typedef struct {
    Vec3 pos;
    Vec3 vel;
    Color color;
    float size;
    float life;
    float max_life;
    bool active;
} Particle;

typedef struct {
    Particle pool[MAX_PARTICLES];
    int count;
    uint32_t rng_state;
} ParticleSystem;

void particles_init(ParticleSystem *ps, uint32_t seed);
void particles_spawn_3d(ParticleSystem *ps, Vec3 pos, Vec3 vel, Color color, float size, float life);
void particles_burst_3d(ParticleSystem *ps, Vec3 pos, Color color, int count, float speed_min, float speed_max, float size_min, float size_max, float life);
void particles_update_3d(ParticleSystem *ps, float dt);
void particles_draw_3d(ParticleSystem *ps);

#endif // ECHO_PARTICLES_H
