#ifndef ECHO_WORLD_H
#define ECHO_WORLD_H

#include "raylib.h"
#include "math.h"
#include <stdbool.h>

#define MAX_WORLD_OBJECTS 1024

typedef enum {
    OBJ_NONE,
    OBJ_WALL,
    OBJ_CRYSTAL,
    OBJ_SILENCE_NODE,
    OBJ_PORTAL,
    OBJ_BRIDGE,
    OBJ_PLATFORM,
    OBJ_ARCH,
    OBJ_MANA_BLUE,
    OBJ_MANA_YELLOW,
    OBJ_CHEST,
    OBJ_HEART
} WorldObjectType;

typedef struct {
    WorldObjectType type;
    Vec3 pos;
    float radius;
    float height;
    bool active;
    float pulse_visibility;
    float visibility_timer;
} WorldObject;

typedef struct {
    WorldObject objects[MAX_WORLD_OBJECTS];
    int count;
    uint32_t seed;
    int total_crystals;
    Vec3 portal_pos;
} World;

void world_init(World *w, uint32_t seed, int stage);
void world_update_3d(World *w, float dt, Vec3 player_pos, Vec3 pulse_origin, float pulse_radius, float pulse_intensity);
void world_draw_3d(World *w, Vec3 player_pos, int stage);
bool world_check_wall_collision_3d(World *w, Vec3 pos, float radius);
void world_spawn_mana_orb(World *w, Vec3 pos, bool is_yellow);
void world_spawn_chest(World *w, Vec3 pos);
void world_spawn_heart(World *w, Vec3 pos);

#endif // ECHO_WORLD_H
