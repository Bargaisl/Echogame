#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "math.h"
#include "world.h"
#include "particles.h"
#include "audio.h"
#include <stdbool.h>

#define MAX_ENEMIES 140
#define MAX_PROJECTILES 120
#define MAX_METEORS 20
#define MAX_THUNDERS 20
#define MAX_TSUNAMIS 10
#define MAX_LAVA_ZONES 20
#define REQUIRED_CRYSTALS 5
#define NUM_SKILLS 16
#define NUM_ARTIFACTS 5

typedef enum {
    LANG_EN, // English
    LANG_RU  // Russian
} GameLanguage;

typedef enum {
    STATE_MENU,
    STATE_GUIDE,
    STATE_SETTINGS,
    STATE_PLAYING,
    STATE_LEVELUP,
    STATE_ARTIFACT_MODAL,
    STATE_WIN,
    STATE_GAMEOVER
} GameStateEnum;

typedef enum {
    ENEMY_CLASS_STALKER,
    ENEMY_CLASS_SENTINEL,
    ENEMY_CLASS_BLINKER
} EnemyClass;

typedef enum {
    ENEMY_CHASE,
    ENEMY_INVESTIGATE_DECOY
} EnemyState;

typedef struct {
    Vec3 pos;
    Vec3 vel;
    EnemyState state;
    Vec3 target_pos;
    float visibility;
    float pulse_timer;
    float health;
    float max_health;
    int monster_level;
    float radius_scale;
    EnemyClass type_class;
    bool active;
} Enemy;

typedef struct {
    Vec3 pos;
    Vec3 vel;
    float yaw;
    float pitch;
    float health;
    float max_health;
    float silence_charge;
    bool in_silence_mode; // LEFT SHIFT: Ghost / Stealth Mode (Passes through monsters like a phantom!)
    bool charging_pulse;
    float pulse_charge_time;
    bool pulse_active;
    float pulse_cooldown; // Cooldown timer for Echolocation Pulse Knockback
    Vec3 pulse_origin;
    float pulse_radius;
    float pulse_max_radius;
    float pulse_intensity;
    int crystals_collected;

    // Progression
    int level;
    float mana;
    float mana_to_next;
    int skill_levels[NUM_SKILLS];
    bool artifacts_unlocked[NUM_ARTIFACTS];

    // Spell Timers
    float bolt_timer;
    float fireball_timer;
    float meteor_timer;
    float thunder_timer;
    float tsunami_timer;
    float ray_timer;
    float lava_timer;
    float armageddon_timer;

    // Laser Ray
    bool ray_active;
    float ray_duration;
    Vec3 ray_start;
    Vec3 ray_end;
} Player;

typedef struct {
    Vec3 pos;
    Vec3 vel;
    float damage;
    float life;
    bool is_fireball;
    bool active;
} MagicProjectile;

typedef struct {
    Vec3 pos;
    Vec3 target_pos;
    float height;
    float radius;
    float damage;
    bool active;
} MeteorSpell;

typedef struct {
    Vec3 pos;
    float damage;
    float life;
    bool active;
} ThunderSpell;

typedef struct {
    Vec3 pos;
    Vec3 dir;
    float width;
    float dist;
    float max_dist;
    float damage;
    bool active;
} TsunamiSpell;

typedef struct {
    Vec3 pos;
    float radius;
    float duration;
    float damage_timer;
    bool active;
} LavaZoneSpell;

typedef struct {
    Vec3 pos;
    Vec3 vel;
    float timer;
    float pulse_radius;
    float pulse_intensity;
    bool active;
} DecoyGrenade;

typedef struct {
    int bg_time_loc;
    int bg_res_loc;
    int echo_time_loc;
    int echo_res_loc;
    int echo_orig_loc;
    int echo_rad_loc;
    int echo_int_loc;
    int post_time_loc;
    int post_res_loc;
    int post_dist_loc;
} ShaderLocs;

typedef struct {
    GameStateEnum state;
    int guide_tab;
    float master_volume;
    GameLanguage language;
    uint32_t rng_state;
    float total_game_time;
    float survival_timer;
    int total_kills;
    float screen_shake;
    float distortion_level;
    int levelup_cards[3];
    int unlocked_artifact_idx;
    int world_stage;   // Current stage/level (1, 2, 3, ...)
    int max_stage;                 // High score max stage reached
    float stage_radius;            // Current arena radius
    float stage_timer;             // Time elapsed in current stage
    bool transitioning;            // True while portal animation runs
    float transition_progress;     // 0.0‑1.0 portal animation progress
    bool player_frozen;            // True while player movement is disabled
    float pull_timer;              // Timer for forced pull after 0.5 s

    Player player;
    Enemy enemies[MAX_ENEMIES];
    MagicProjectile projectiles[MAX_PROJECTILES];
    MeteorSpell meteors[MAX_METEORS];
    ThunderSpell thunders[MAX_THUNDERS];
    TsunamiSpell tsunamis[MAX_TSUNAMIS];
    LavaZoneSpell lavas[MAX_LAVA_ZONES];
    DecoyGrenade decoy;

    Camera3D camera;
    ParticleSystem particles;
    World world;
    AudioEngine audio;
    ShaderLocs shader_locs;
} GameState;

void game_init(GameState *g);
void game_reset(GameState *g);
void game_init_shader_locs(GameState *g, Shader bg_shader, Shader echo_shader, Shader post_shader);
void game_update(GameState *g, float dt);
void game_draw(GameState *g, Shader bg_shader, Shader echo_shader, Shader post_shader, RenderTexture2D target);
void game_close(GameState *g);

#endif // GAME_H
