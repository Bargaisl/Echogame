#include "particles.h"
#include <math.h>
#include <string.h>

void particles_init(ParticleSystem *ps, uint32_t seed) {
    if (!ps) return;
    memset(ps->pool, 0, sizeof(ps->pool));
    ps->rng_state = seed ? seed : 7777;
}

void particles_spawn_3d(ParticleSystem *ps, Vec3 pos, Vec3 vel, Color color, float size, float life) {
    if (!ps) return;
    int slot = -1;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!ps->pool[i].active) {
            slot = i;
            break;
        }
    }
    if (slot == -1) return;

    Particle *p = &ps->pool[slot];
    p->pos = pos;
    p->vel = vel;
    p->color = color;
    p->size = size;
    p->life = life;
    p->max_life = life;
    p->active = true;
}

void particles_burst_3d(ParticleSystem *ps, Vec3 pos, Color color, int count, float speed_min, float speed_max, float size_min, float size_max, float life_max) {
    if (!ps) return;
    for (int i = 0; i < count; i++) {
        float theta = rnd_float(&ps->rng_state) * 3.14159265f * 2.0f;
        float phi = acosf(2.0f * rnd_float(&ps->rng_state) - 1.0f);
        float speed = rnd_range(&ps->rng_state, speed_min, speed_max);

        Vec3 vel = (Vec3){
            sinf(phi) * cosf(theta) * speed,
            sinf(phi) * sinf(theta) * speed,
            cosf(phi) * speed
        };

        float sz = rnd_range(&ps->rng_state, size_min, size_max);
        float lf = rnd_range(&ps->rng_state, life_max * 0.4f, life_max);

        // Neon palette colors (Pink #FF00AA, Cyan #00F0FF, Purple #AA00FF, White)
        Color neon_c = color;
        float c_roll = rnd_float(&ps->rng_state);
        if (c_roll < 0.30f) neon_c = (Color){255, 0, 170, color.a};
        else if (c_roll < 0.60f) neon_c = (Color){0, 240, 255, color.a};
        else if (c_roll < 0.85f) neon_c = (Color){170, 0, 255, color.a};
        else neon_c = (Color){255, 255, 255, color.a};

        particles_spawn_3d(ps, pos, vel, neon_c, sz, lf);
    }
}

void particles_update_3d(ParticleSystem *ps, float dt) {
    if (!ps) return;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &ps->pool[i];
        if (!p->active) continue;

        p->life -= dt;
        if (p->life <= 0.0f) {
            p->active = false;
            continue;
        }

        // Spiral & Spline Curved Trajectories
        float t = (p->max_life - p->life) * 8.0f;
        Vec3 spiral_force = (Vec3){cosf(t * 3.0f) * 14.0f, sinf(t * 4.0f) * 10.0f, sinf(t * 3.0f) * 14.0f};

        p->pos = vec3_add(p->pos, vec3_scale(p->vel, dt));
        p->pos = vec3_add(p->pos, vec3_scale(spiral_force, dt));
        p->vel = vec3_scale(p->vel, 0.94f);
    }
}

void particles_draw_3d(ParticleSystem *ps) {
    if (!ps) return;
    BeginBlendMode(BLEND_ADDITIVE);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &ps->pool[i];
        if (!p->active) continue;

        float alpha_ratio = (p->life / p->max_life);
        Color c = p->color;
        c.a = (unsigned char)(255.0f * alpha_ratio);

        Vector3 pos3d = (Vector3){p->pos.x, p->pos.y, p->pos.z};
        DrawSphere(pos3d, p->size * alpha_ratio, c);
    }

    EndBlendMode();
}
