#ifndef ECHO_MATH_H
#define ECHO_MATH_H

#include <math.h>
#include <stdint.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef TAU
#define TAU 6.28318530717958647692f
#endif

#ifndef DEG2RAD
#define DEG2RAD (PI / 180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG (180.0f / PI)
#endif

// Fast utility macros
#define FAST_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define FAST_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define FAST_CLAMP(v, lo, hi) FAST_MAX((lo), FAST_MIN((hi), (v)))
#define FAST_LERP(a, b, t) ((a) + ((b) - (a)) * (t))

// Fast PRNG (Xorshift32)
static inline uint32_t xorshift32(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static inline float rnd_float(uint32_t *state) {
    return (xorshift32(state) & 0xFFFFFF) / 16777216.0f;
}

static inline float rnd_range(uint32_t *state, float min_val, float max_val) {
    return min_val + rnd_float(state) * (max_val - min_val);
}

static inline int rnd_int(uint32_t *state, int min_val, int max_val) {
    if (min_val >= max_val) return min_val;
    return min_val + (int)(xorshift32(state) % (uint32_t)(max_val - min_val + 1));
}

// 2D Vector operations
typedef struct {
    float x;
    float y;
} Vec2;

static inline Vec2 vec2_add(Vec2 a, Vec2 b) { return (Vec2){a.x + b.x, a.y + b.y}; }
static inline Vec2 vec2_sub(Vec2 a, Vec2 b) { return (Vec2){a.x - b.x, a.y - b.y}; }
static inline Vec2 vec2_scale(Vec2 v, float s) { return (Vec2){v.x * s, v.y * s}; }
static inline float vec2_dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
static inline float vec2_length_sq(Vec2 v) { return v.x * v.x + v.y * v.y; }
static inline float vec2_length(Vec2 v) { return sqrtf(vec2_length_sq(v)); }

static inline Vec2 vec2_normalize(Vec2 v) {
    float len = vec2_length(v);
    if (len > 0.00001f) {
        return (Vec2){v.x / len, v.y / len};
    }
    return (Vec2){0.0f, 0.0f};
}

static inline float vec2_dist(Vec2 a, Vec2 b) {
    return vec2_length(vec2_sub(a, b));
}

// 3D Vector operations
typedef struct {
    float x;
    float y;
    float z;
} Vec3;

static inline Vec3 vec3_add(Vec3 a, Vec3 b) { return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
static inline Vec3 vec3_sub(Vec3 a, Vec3 b) { return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
static inline Vec3 vec3_scale(Vec3 v, float s) { return (Vec3){v.x * s, v.y * s, v.z * s}; }
static inline float vec3_dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline float vec3_length_sq(Vec3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
static inline float vec3_length(Vec3 v) { return sqrtf(vec3_length_sq(v)); }

static inline Vec3 vec3_normalize(Vec3 v) {
    float len = vec3_length(v);
    if (len > 0.00001f) {
        return (Vec3){v.x / len, v.y / len, v.z / len};
    }
    return (Vec3){0.0f, 0.0f, 0.0f};
}

static inline float vec3_dist(Vec3 a, Vec3 b) {
    return vec3_length(vec3_sub(a, b));
}

// Simple 2D Hash Noise for procedural generation
static inline float hash21(float x, float y) {
    float n = sinf(x * 12.9898f + y * 78.233f) * 43758.5453f;
    return n - floorf(n);
}

static inline float noise2d(float x, float y) {
    float ix = floorf(x);
    float iy = floorf(y);
    float fx = x - ix;
    float fy = y - iy;

    float ux = fx * fx * (3.0f - 2.0f * fx);
    float uy = fy * fy * (3.0f - 2.0f * fy);

    float a = hash21(ix, iy);
    float b = hash21(ix + 1.0f, iy);
    float c = hash21(ix, iy + 1.0f);
    float d = hash21(ix + 1.0f, iy + 1.0f);

    return FAST_LERP(FAST_LERP(a, b, ux), FAST_LERP(c, d, ux), uy);
}

#endif // ECHO_MATH_H
