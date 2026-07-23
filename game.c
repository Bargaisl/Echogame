#include "game.h"
#include "font.h"
#include "shaders.h"
#include "rlgl.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

extern void log_debug(const char *msg);

// ========== TRANSLATION SYSTEM (ENGLISH & RUSSIAN) ==========
static const char *TR_START[]     = {"START GAME",           "НАЧАТЬ ИГРУ"};
static const char *TR_START_KEY[] = {"SPACE / CLICK: START", "ПРОБЕЛ / КЛИК: СТАРТ"};
static const char *TR_GUIDE_BTN[] = {"[G] GUIDE & SPELLS",   "[G] ГАЙД И ЗАКЛИНАНИЯ"};
static const char *TR_SETT_BTN[]  = {"[S] SETTINGS",         "[S] НАСТРОЙКИ И ЯЗЫК"};
static const char *TR_CONTROLS[]  = {"WASD:MOVE MOUSE:LOOK LMB:PULSE RMB:DECOY LSHIFT:GHOST",
                                     "WASD: ДВИЖЕНИЕ | МЫШЬ: ОБЗОР | ЛКМ: ИМПУЛЬС | SHIFT: ПРИЗРАК"};
static const char *TR_MONSTERS[]  = {"TENTACLE IMPS | RED BEASTS | BLACK HOLES",
                                     "ЧЕРНЫЕ БЕСЫ | КРАСНЫЕ МУТАНТЫ | ЧЕРНЫЕ ДЫРЫ"};
static const char *TR_SETT_TITLE[]= {"SETTINGS & LANGUAGE",  "НАСТРОЙКИ И ЯЗЫК"};
static const char *TR_SELECT_LNG[]= {"SELECT LANGUAGE:",     "ВЫБОР ЯЗЫКА ИГРЫ:"};
static const char *TR_ENGLISH[]   = {"[1] ENGLISH (US)",     "[1] ENGLISH (US)"};
static const char *TR_RUSSIAN[]   = {"[2] RUSSIAN (RU)",     "[2] РУССКИЙ (RU)"};
static const char *TR_VOLUME[]    = {"MASTER VOLUME (+/-):", "ГРОМКОСТЬ (+/-):"};
static const char *TR_BACK_HINT[] = {"ESC / [S]: BACK",      "ESC / [S]: НАЗАД"};
static const char *TR_FULLSCREEN[]= {"[F] FULLSCREEN:",      "[F] ПОЛНОЭКРАННЫЙ РЕЖИМ:"};
static const char *TR_FS_OFF[]    = {"WINDOWED",             "В ОКНЕ"};
static const char *TR_FS_ON[]     = {"FULLSCREEN",           "ПОЛНЫЙ ЭКРАН"};
static const char *TR_ACTIVE[]    = {"[ ON ]",               "[ ВКЛ ]"};
static const char *TR_HEALTH[]    = {"HEALTH",               "ЗДОРОВЬЕ"};
static const char *TR_MANA[]      = {"MANA XP",              "МАНА XP"};
static const char *TR_SILENCE[]   = {"SILENCE",              "ТИШИНА (ПРИЗРАК)"};
static const char *TR_PULSE[]     = {"ECHO PULSE",           "ЭХО-ИМПУЛЬС"};
static const char *TR_READY[]     = {"READY",                "ГОТОВ"};
static const char *TR_VICTORY[]   = {"VICTORY!",             "ПОБЕДА!"};
static const char *TR_WIN_DESC[]  = {"YOU ESCAPED THE VOID!","ВЫ СБЕЖАЛИ ИЗ БЕЗДНЫ!"};
static const char *TR_GAMEOVER[]  = {"CONSUMED BY SHADOWS",  "ПОГЛОЩЕН ТЕНЯМИ"};
static const char *TR_GO_DESC[]   = {"THE SILENCE ENDED...", "ТИШИНА ОКОНЧИЛАСЬ..."};
static const char *TR_RESTART[]   = {"PRESS ANY KEY FOR LOBBY", "НАЖМИТЕ ЛЮБУЮ КЛАВИШУ ДЛЯ ВЫХОДА В ЛОББИ"};
static const char *TR_LEVELUP[]   = {"LEVEL UP! CHOOSE AN UPGRADE", "НОВЫЙ УРОВЕНЬ! ВЫБЕРИ НАВЫК"};
static const char *TR_CLICK_SEL[] = {"[ CLICK TO SELECT ]",  "[ КЛИКНИ ДЛЯ ВЫБОРА ]"};

#define T(arr) ((arr)[g->language])

// Helper: calculate UTF-8 text pixel width at given scale
static int text_width(const char *text, int scale) {
    return text_utf8_width(text, scale);
}

// Helper: center text X inside a rectangle
static int center_x(const char *text, int box_x, int box_w, int scale) {
    int tw = text_width(text, scale);
    return box_x + (box_w - tw) / 2;
}

// Helper: center text Y inside a rectangle
static int center_y(int box_y, int box_h, int scale) {
    return box_y + (box_h - 8 * scale) / 2;
}

// Helper: draw vibrant software neon cursor in menu/UI states
static void draw_neon_cursor(Vector2 mpos) {
    DrawCircle((int)mpos.x, (int)mpos.y, 4.0f, (Color){0, 255, 200, 255});
    DrawCircleLines((int)mpos.x, (int)mpos.y, 10.0f, (Color){0, 240, 255, 220});
    DrawLine((int)mpos.x - 12, (int)mpos.y, (int)mpos.x - 6, (int)mpos.y, (Color){0, 255, 200, 200});
    DrawLine((int)mpos.x + 6, (int)mpos.y, (int)mpos.x + 12, (int)mpos.y, (Color){0, 255, 200, 200});
    DrawLine((int)mpos.x, (int)mpos.y - 12, (int)mpos.x, (int)mpos.y - 6, (Color){0, 255, 200, 200});
    DrawLine((int)mpos.x, (int)mpos.y + 6, (int)mpos.x, (int)mpos.y + 12, (Color){0, 255, 200, 200});
}

typedef struct {
    const char *title_en;
    const char *title_ru;
    const char *category_en;
    const char *category_ru;
    const char *pros_en;
    const char *pros_ru;
    const char *cons_en;
    const char *cons_ru;
} SkillCardInfo;

static const SkillCardInfo SKILL_CARDS[NUM_SKILLS] = {
    {"MAGIC BOLT",       "МАГИЧЕСКИЙ БОЛТ",   "OFFENSIVE", "АТАКА",     "+Homing Plasma Bullets",       "+Самонаводящиеся снаряды",    "+Fast Cooldown Rate",         "+Высокая скорострельность"},
    {"FIREBALL",         "ОГНЕННЫЙ ШАР",      "OFFENSIVE", "АТАКА",     "+Explosive 3D Fire Sphere",    "+Взрывной огненный шар",       "+Massive AoE Blast Damage",   "+Огромный урон по области"},
    {"THUNDERSTORM",     "ГРОЗОВОЙ ШТОРМ",    "OFFENSIVE", "АТАКА",     "+3D Sky Lightning Strikes",    "+Молнии с неба по врагам",     "+Hits Top Priority Target",   "+Удары по сильным врагам"},
    {"METEOR",           "МЕТЕОРИТ",          "OFFENSIVE", "АТАКА",     "+Flaming Sky Meteor Impact",   "+Падение пылающего метеора",   "+Huge Crater Radius Damage",  "+Огромный урон в кратере"},
    {"SATELLITE",        "СПУТНИКИ",          "OFFENSIVE", "АТАКА",     "+3D Orbiting Contact Spheres", "+Сферы вокруг мага",           "+Continuous Shield Damage",   "+Постоянная защита вращения"},
    {"ARCANE RAY",       "ЛАЗЕРНЫЙ ЛУЧ",      "OFFENSIVE", "АТАКА",     "+Piercing Laser Beam",         "+Пронизывающий луч",           "+Obliterates All On Line",    "+Уничтожает всех на линии"},
    {"LAVA ZONE",        "ЗОНА ЛАВЫ",         "OFFENSIVE", "АТАКА",     "+Burning Ground Magma Pools",  "+Огненные лужи на земле",      "+Continuous Burn Damage",     "+Периодический урон лавы"},
    {"TSUNAMI",          "ЦУНАМИ",            "OFFENSIVE", "АТАКА",     "+Giant 3D Tidal Wave",         "+Гигантская волна цунами",     "+Cleans Entire Arena Field",  "+Сносит орду по всей арене"},
    {"ELECTRIC ZONE",    "ЭЛЕКТРО-ПОЛЕ",      "OFFENSIVE", "АТАКА",     "+High-Voltage Shock Ring",     "+Кольцо молний вокруг игрока", "+Protection Aura Damage",     "+Защитная аура урона"},
    {"SHIELD",           "ЗАЩИТНЫЙ ЩИТ",      "UTILITY",   "ЗАЩИТА",    "+Protective Energy Shield",    "+Энергетический купол",        "+Absorbs Heavy Direct Hits",  "+Поглощает прямые удары"},
    {"ARMAGEDDON",       "АРМАГЕДДОН",        "UTILITY",   "ОСОБОЕ",    "+Screen-Clearing Blast",       "+Взрыв на весь экран",         "+Obliterates Entire Horde",   "+Уничтожает всю орду врагов"},
    {"MAGIC CIRCLE",     "МАГИЧЕСКИЙ КРУГ",   "UTILITY",   "УСИЛЕНИЕ",  "+ATK & Casting Speed Ring",    "+Круг силы под ногами",        "+Ground Power Boost Zone",    "+25% к урону всей магии"},
    {"INTELLIGENCE",     "ИНТЕЛЛЕКТ",         "PASSIVE",   "ПАССИВНОЕ", "+20% Attack Power (ATK)",       "+20% к урону всей магии",      "+All Magic Spell Damage",     "+Усиливает все заклинания"},
    {"FAST CASTING",     "БЫСТРОЕ КОЛДОВСТВО","PASSIVE",   "ПАССИВНОЕ", "-25% Cooldown For Spells",    "-25% перезарядка магии",       "+Faster Fire Rate Speed",     "+Высокая скорострельность"},
    {"HASTE & EXPLORER", "УСКОРЕНИЕ & МАГНИТ","PASSIVE",   "ПАССИВНОЕ", "+25% Move Speed & Pickup",     "+25% к скорости бега",         "+Item Magnet Radius Boost",   "+Притягивание предметов"},
    {"ARCANA MASTERY",   "МАСТЕР МАГИИ",      "SPECIAL",   "ОСОБОЕ",    "+30% Spell Size & Crit",       "+30% к размеру заклинаний",    "+Ultimate Magic Power",       "+Критический урон магии"}
};

typedef struct {
    const char *title;
    const char *desc;
} ArtifactInfo;

static const ArtifactInfo ARTIFACT_LIST[NUM_ARTIFACTS] = {
    {"BOOTS OF HERMES", "+30% MOVEMENT SPEED BOOST"},
    {"AMULET OF VITALITY", "+50 MAX HP & FULL HEAL"},
    {"CHRONOS HOURGLASS", "-35% SPELL COOLDOWN TIME"},
    {"ORB OF REFLECTION", "+35% ALL SPELL DAMAGE"},
    {"HEART OF PHOENIX", "PASSIVE HP REGEN (+3 HP/SEC)"}
};

// INITIAL ENEMY CAP
#define INITIAL_ENEMIES 25
#define ENEMY_RAMP_INTERVAL 12.0f
#define ENEMY_RAMP_AMOUNT 5

static void trigger_level_up(GameState *g) {
    if (!g) return;
    g->state = STATE_LEVELUP;
    EnableCursor();

    g->levelup_cards[0] = rnd_int(&g->rng_state, 0, NUM_SKILLS - 1);
    g->levelup_cards[1] = (g->levelup_cards[0] + 1 + rnd_int(&g->rng_state, 0, NUM_SKILLS - 2)) % NUM_SKILLS;
    g->levelup_cards[2] = (g->levelup_cards[1] + 1 + rnd_int(&g->rng_state, 0, NUM_SKILLS - 2)) % NUM_SKILLS;
    if (g->levelup_cards[2] == g->levelup_cards[0]) {
        g->levelup_cards[2] = (g->levelup_cards[2] + 1) % NUM_SKILLS;
    }
}

static void trigger_artifact_unlock(GameState *g) {
    if (!g) return;
    int art_idx = -1;
    for (int i = 0; i < NUM_ARTIFACTS; i++) {
        if (!g->player.artifacts_unlocked[i]) { art_idx = i; break; }
    }
    if (art_idx == -1) art_idx = rnd_int(&g->rng_state, 0, NUM_ARTIFACTS - 1);

    g->player.artifacts_unlocked[art_idx] = true;
    g->unlocked_artifact_idx = art_idx;
    g->state = STATE_ARTIFACT_MODAL;
    EnableCursor();

    if (art_idx == 1) {
        g->player.max_health += 50.0f;
        g->player.health = g->player.max_health;
    }
    audio_play_sfx(&g->audio, SFX_PORTAL_WIN, 1.0f);
}

static void add_mana(GameState *g, float amount) {
    if (!g) return;
    g->player.mana += amount;
    if (g->player.mana >= g->player.mana_to_next) {
        g->player.mana -= g->player.mana_to_next;
        g->player.level++;
        g->player.mana_to_next *= 1.25f;
        trigger_level_up(g);
    }
}

// SPAWN MONSTERS: 25% FRONT ARC, 75% SIDES & REAR (Balanced spawning with front space)
static void spawn_enemy_instance(GameState *g, int idx, float progress) {
    Enemy *e = &g->enemies[idx];

    float cam_yaw = g->player.yaw * DEG2RAD;
    float offset_angle = 0.0f;

    // 25% chance to spawn in front arc (-30 to +30 deg), 75% to spawn sides/rear
    if (rnd_float(&g->rng_state) < 0.25f) {
        offset_angle = rnd_range(&g->rng_state, -0.15f * PI, 0.15f * PI);
    } else {
        offset_angle = rnd_range(&g->rng_state, 0.40f * PI, 1.60f * PI);
    }

    float angle = cam_yaw + offset_angle;
    float dist = rnd_range(&g->rng_state, 300.0f, 540.0f);
    float y = rnd_range(&g->rng_state, 6.0f, 15.0f);

    e->pos = (Vec3){g->player.pos.x + sinf(angle) * dist, y, g->player.pos.z + cosf(angle) * dist};
    e->vel = (Vec3){0.0f, 0.0f, 0.0f};
    e->state = ENEMY_CHASE;
    e->target_pos = g->player.pos;
    e->visibility = 1.0f;
    e->pulse_timer = rnd_range(&g->rng_state, 0.0f, 3.5f);
    e->active = true;

    float lvl_roll = rnd_float(&g->rng_state);
    if (lvl_roll < 0.60f) {
        e->monster_level = 1;
        e->radius_scale = 10.0f;
        e->health = 25.0f + progress * 30.0f;
    } else if (lvl_roll < 0.90f) {
        e->monster_level = 2;
        e->radius_scale = 20.0f;
        e->health = 75.0f + progress * 90.0f;
    } else {
        e->monster_level = 3;
        e->radius_scale = 42.0f;
        e->health = 300.0f + progress * 250.0f;
    }
    e->max_health = e->health;

    int roll = rnd_int(&g->rng_state, 0, 9);
    if (roll < 5) e->type_class = ENEMY_CLASS_STALKER;
    else if (roll < 8) e->type_class = ENEMY_CLASS_SENTINEL;
    else e->type_class = ENEMY_CLASS_BLINKER;
}

void game_reset(GameState *g) {
    if (!g) return;

    GameLanguage saved_lang = g->language;

    if (g->world_stage < 1) g->world_stage = 1;
    if (g->world_stage > g->max_stage) g->max_stage = g->world_stage;

    g->language = saved_lang;

    audio_reset(&g->audio);

    g->rng_state = 54321 + (uint32_t)(g->world_stage * 123);
    g->total_game_time = 0.0f;
    g->survival_timer = 0.0f;
    g->total_kills = 0;
    g->screen_shake = 0.0f;
    g->distortion_level = 0.0f;
    // Initialize portal transition state
    g->transitioning = false;
    g->player_frozen = false;
    g->transition_progress = 0.0f;
    g->pull_timer = -1.0f;
    g->player.pos = (Vec3){0.0f, 10.0f, 0.0f};
    g->player.vel = (Vec3){0.0f, 0.0f, 0.0f};
    g->player.yaw = 0.0f;
    g->player.pitch = 0.0f;
    g->player.health = 100.0f;
    g->player.max_health = 100.0f;
    g->player.silence_charge = 100.0f;
    g->player.in_silence_mode = false;
    g->player.charging_pulse = false;
    g->player.pulse_charge_time = 0.0f;
    g->player.pulse_active = false;
    g->player.pulse_cooldown = 0.0f;
    g->player.crystals_collected = 0;

    g->player.level = 1;
    g->player.mana = 0.0f;
    g->player.mana_to_next = 60.0f;
    g->player.bolt_timer = 0.0f;
    g->player.fireball_timer = 0.0f;
    g->player.meteor_timer = 0.0f;
    g->player.thunder_timer = 0.0f;
    g->player.tsunami_timer = 0.0f;
    g->player.ray_timer = 0.0f;
    g->player.lava_timer = 0.0f;
    g->player.armageddon_timer = 0.0f;
    g->player.ray_active = false;

    memset(g->player.skill_levels, 0, sizeof(g->player.skill_levels));
    memset(g->player.artifacts_unlocked, 0, sizeof(g->player.artifacts_unlocked));
    memset(&g->decoy, 0, sizeof(DecoyGrenade));
    memset(g->projectiles, 0, sizeof(g->projectiles));
    memset(g->meteors, 0, sizeof(g->meteors));
    memset(g->thunders, 0, sizeof(g->thunders));
    memset(g->tsunamis, 0, sizeof(g->tsunamis));
    memset(g->lavas, 0, sizeof(g->lavas));

    g->camera.position = (Vector3){0.0f, 10.0f, 0.0f};
    g->camera.target = (Vector3){0.0f, 10.0f, 1.0f};
    g->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    g->camera.fovy = 75.0f;
    g->camera.projection = CAMERA_PERSPECTIVE;

    particles_init(&g->particles, 12345);
    world_init(&g->world, 9999, g->world_stage);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (i < INITIAL_ENEMIES) {
            spawn_enemy_instance(g, i, 0.0f);
        } else {
            g->enemies[i].active = false;
        }
    }
}

void game_init(GameState *g) {
    if (!g) return;
    memset(g, 0, sizeof(GameState));
    g->language = LANG_EN;
    audio_init(&g->audio);
    game_reset(g);
}

void game_init_shader_locs(GameState *g, Shader bg_shader, Shader echo_shader, Shader post_shader) {
    if (!g) return;
    g->shader_locs.bg_time_loc = GetShaderLocation(bg_shader, "u_time");
    g->shader_locs.bg_res_loc = GetShaderLocation(bg_shader, "u_resolution");
    g->shader_locs.echo_time_loc = GetShaderLocation(echo_shader, "u_time");
    g->shader_locs.echo_res_loc = GetShaderLocation(echo_shader, "u_resolution");
    g->shader_locs.echo_orig_loc = GetShaderLocation(echo_shader, "u_pulse_origin");
    g->shader_locs.echo_rad_loc = GetShaderLocation(echo_shader, "u_pulse_radius");
    g->shader_locs.echo_int_loc = GetShaderLocation(echo_shader, "u_pulse_intensity");
    g->shader_locs.post_time_loc = GetShaderLocation(post_shader, "u_time");
    g->shader_locs.post_res_loc = GetShaderLocation(post_shader, "u_resolution");
    g->shader_locs.post_dist_loc = GetShaderLocation(post_shader, "u_distortion");
}

static void spawn_projectile(GameState *g, Vec3 pos, Vec3 vel, float damage, bool is_fireball) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!g->projectiles[i].active) {
            g->projectiles[i].pos = pos;
            g->projectiles[i].vel = vel;
            g->projectiles[i].damage = damage;
            g->projectiles[i].life = 1.8f;
            g->projectiles[i].is_fireball = is_fireball;
            g->projectiles[i].active = true;
            break;
        }
    }
}

// ========== BUTTON LAYOUT CONSTANTS ==========
#define BTN_X 340
#define BTN_W 600
#define BTN_H 48
#define BTN_Y_START 310
#define BTN_Y_GUIDE 375
#define BTN_Y_SETT  440

// Settings language and fullscreen buttons
#define LANG_BTN_X 290
#define LANG_BTN_W 700
#define LANG_BTN_H 45
#define LANG_BTN_Y1 270
#define LANG_BTN_Y2 330
#define FS_BTN_Y   390

void game_update(GameState *g, float dt) {
    if (!g) return;
    g->total_game_time += dt;
    audio_update(&g->audio, dt);

    Vector2 mpos = GetMousePosition();

    if (g->state != STATE_PLAYING) {
        if (IsCursorHidden()) EnableCursor();
    }

    // ===== MENU =====
    if (g->state == STATE_MENU) {
        g->distortion_level = 0.0f;

        if (IsKeyPressed(KEY_G) || IsKeyPressed(KEY_H)) {
            g->state = STATE_GUIDE;
            audio_play_sfx(&g->audio, SFX_CRYSTAL_COLLECT, 0.8f);
            return;
        }
        if (IsKeyPressed(KEY_O)) {
            g->state = STATE_SETTINGS;
            audio_play_sfx(&g->audio, SFX_CRYSTAL_COLLECT, 0.8f);
            return;
        }

        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            g->world_stage = 1;
            game_reset(g);
            g->state = STATE_PLAYING;
            DisableCursor();
            audio_play_sfx(&g->audio, SFX_PULSE_BOOM, 0.8f);
            return;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (mpos.x >= BTN_X && mpos.x <= (BTN_X + BTN_W)) {
                if (mpos.y >= BTN_Y_START && mpos.y <= (BTN_Y_START + BTN_H)) {
                    g->world_stage = 1;
                    game_reset(g);
                    g->state = STATE_PLAYING;
                    DisableCursor();
                    audio_play_sfx(&g->audio, SFX_PULSE_BOOM, 0.8f);
                    return;
                }
                if (mpos.y >= BTN_Y_GUIDE && mpos.y <= (BTN_Y_GUIDE + BTN_H)) {
                    g->state = STATE_GUIDE;
                    audio_play_sfx(&g->audio, SFX_CRYSTAL_COLLECT, 0.8f);
                    return;
                }
                if (mpos.y >= BTN_Y_SETT && mpos.y <= (BTN_Y_SETT + BTN_H)) {
                    g->state = STATE_SETTINGS;
                    audio_play_sfx(&g->audio, SFX_CRYSTAL_COLLECT, 0.8f);
                    return;
                }
            }
        }

        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            g->state = STATE_PLAYING;
            DisableCursor();
            audio_play_sfx(&g->audio, SFX_PULSE_BOOM, 0.8f);
        }
        return;
    }

    // ===== SETTINGS (2 LANGUAGES: ENGLISH & RUSSIAN) =====
    if (g->state == STATE_SETTINGS) {
        g->distortion_level = 0.0f;

        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
            g->language = LANG_EN;
            audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.7f);
        }
        if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
            g->language = LANG_RU;
            audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.7f);
        }

        if (IsKeyPressed(KEY_F) || IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
            audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.8f);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (mpos.x >= LANG_BTN_X && mpos.x <= (LANG_BTN_X + LANG_BTN_W)) {
                if (mpos.y >= LANG_BTN_Y1 && mpos.y <= (LANG_BTN_Y1 + LANG_BTN_H)) {
                    g->language = LANG_EN;
                    audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.8f);
                } else if (mpos.y >= LANG_BTN_Y2 && mpos.y <= (LANG_BTN_Y2 + LANG_BTN_H)) {
                    g->language = LANG_RU;
                    audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.8f);
                } else if (mpos.y >= FS_BTN_Y && mpos.y <= (FS_BTN_Y + LANG_BTN_H)) {
                    ToggleFullscreen();
                    audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.8f);
                }
            }
        }

        if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL)) {
            g->master_volume = FAST_MIN(1.0f, g->master_volume + 0.1f);
            SetMasterVolume(g->master_volume);
            audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.5f);
        }
        if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS)) {
            g->master_volume = FAST_MAX(0.0f, g->master_volume - 0.1f);
            SetMasterVolume(g->master_volume);
            audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.5f);
        }

        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_SPACE)) {
            g->state = STATE_MENU;
            audio_play_sfx(&g->audio, SFX_CRYSTAL_COLLECT, 0.8f);
        }
        return;
    }

    // ===== GUIDE =====
    if (g->state == STATE_GUIDE) {
        g->distortion_level = 0.0f;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (mpos.y >= 130 && mpos.y <= 166) {
                if (mpos.x >= 100 && mpos.x <= 420) { g->guide_tab = 0; audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.5f); }
                if (mpos.x >= 480 && mpos.x <= 800) { g->guide_tab = 1; audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.5f); }
                if (mpos.x >= 860 && mpos.x <= 1180) { g->guide_tab = 2; audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.5f); }
            }
        }
        if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
            g->guide_tab = (g->guide_tab + 1) % 3;
            audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.5f);
        }
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
            g->guide_tab = (g->guide_tab + 2) % 3;
            audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.5f);
        }
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) g->guide_tab = 0;
        if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) g->guide_tab = 1;
        if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) g->guide_tab = 2;

        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_G) || IsKeyPressed(KEY_SPACE)) {
            g->state = STATE_MENU;
            audio_play_sfx(&g->audio, SFX_CRYSTAL_COLLECT, 0.8f);
        }
        return;
    }

    // ===== LEVEL UP MODAL =====
    if (g->state == STATE_LEVELUP) {
        int chosen_skill = -1;

        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) chosen_skill = g->levelup_cards[0];
        if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) chosen_skill = g->levelup_cards[1];
        if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) chosen_skill = g->levelup_cards[2];

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (int c = 0; c < 3; c++) {
                int card_x = 160 + c * 330;
                if (mpos.x >= card_x && mpos.x <= (card_x + 300) && mpos.y >= 175 && mpos.y <= 645) {
                    chosen_skill = g->levelup_cards[c];
                    break;
                }
            }
        }

        if (chosen_skill != -1) {
            g->player.skill_levels[chosen_skill]++;
            g->state = STATE_PLAYING;
            DisableCursor();
            audio_play_sfx(&g->audio, SFX_CRYSTAL_COLLECT, 1.0f);
        }
        return;
    }

    // ===== ARTIFACT MODAL =====
    if (g->state == STATE_ARTIFACT_MODAL) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            g->state = STATE_PLAYING;
            DisableCursor();
        }
        return;
    }

    // ===== WIN / GAMEOVER =====
    if (g->state == STATE_WIN || g->state == STATE_GAMEOVER) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            g->world_stage = 1;
            g->state = STATE_MENU;
            EnableCursor();
            audio_play_sfx(&g->audio, SFX_ECHO_REFLECT, 0.8f);
        }
        return;
    }

    // ===== 3D FPS PLAYING STATE =====
    if (!IsCursorHidden()) DisableCursor();

    g->survival_timer += dt;

    // Pulse Cooldown update
    if (g->player.pulse_cooldown > 0.0f) {
        g->player.pulse_cooldown -= dt;
        if (g->player.pulse_cooldown < 0.0f) g->player.pulse_cooldown = 0.0f;
    }

    // Gradual enemy ramp
    int max_active = INITIAL_ENEMIES + (int)(g->survival_timer / ENEMY_RAMP_INTERVAL) * ENEMY_RAMP_AMOUNT;
    if (max_active > MAX_ENEMIES) max_active = MAX_ENEMIES;
    int active_count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (g->enemies[i].active) active_count++;
    }
    float progress = (float)g->player.crystals_collected / (float)REQUIRED_CRYSTALS;
    if (active_count < max_active) {
        for (int i = 0; i < MAX_ENEMIES && active_count < max_active; i++) {
            if (!g->enemies[i].active) {
                spawn_enemy_instance(g, i, progress);
                active_count++;
            }
        }
    }

    if (g->player.artifacts_unlocked[4]) {
        g->player.health = FAST_MIN(g->player.max_health, g->player.health + dt * 2.5f);
    }

    float enemy_chase_speed = FAST_LERP(80.0f, 175.0f, progress) + (g->survival_timer * 0.15f);
    float enemy_damage_rate = FAST_LERP(14.0f, 36.0f, progress) + (g->survival_timer * 0.08f);

    float cd_mult = (1.0f - g->player.skill_levels[13] * 0.08f);
    if (g->player.artifacts_unlocked[2]) cd_mult *= 0.65f;

    float dmg_mult = (1.0f + g->player.skill_levels[12] * 0.15f + g->player.skill_levels[15] * 0.20f);
    if (g->player.artifacts_unlocked[3]) dmg_mult *= 1.35f;
    // Magic Circle [11]: ground power ring gives +25% ATK
    if (g->player.skill_levels[11] > 0) dmg_mult *= (1.0f + g->player.skill_levels[11] * 0.10f);

    // Mouse Look
    Vector2 mouse_delta = GetMouseDelta();
    g->player.yaw -= mouse_delta.x * 0.18f;
    g->player.pitch -= mouse_delta.y * 0.18f;
    g->player.pitch = FAST_CLAMP(g->player.pitch, -85.0f, 85.0f);

    float yaw_rad = g->player.yaw * DEG2RAD;
    float pitch_rad = g->player.pitch * DEG2RAD;

    Vec3 forward = (Vec3){sinf(yaw_rad), 0.0f, cosf(yaw_rad)};
    Vec3 right = (Vec3){cosf(yaw_rad), 0.0f, -sinf(yaw_rad)};
    Vec3 cam_dir = (Vec3){
        cosf(pitch_rad) * sinf(yaw_rad),
        sinf(pitch_rad),
        cosf(pitch_rad) * cosf(yaw_rad)
    };

    // WASD Movement
    Vec3 move = {0.0f, 0.0f, 0.0f};
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) move = vec3_add(move, forward);
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) move = vec3_sub(move, forward);
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) move = vec3_sub(move, right);
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) move = vec3_add(move, right);

    move = vec3_normalize(move);
    float move_multiplier = 1.0f + g->player.skill_levels[14] * 0.15f;
    if (g->player.artifacts_unlocked[0]) move_multiplier += 0.30f;
    float speed = 210.0f * move_multiplier;

    // ===== LEFT SHIFT: GHOST / PHANTOM MODE =====
    // Holding Shift lets the player phase straight through monsters like a ghost (no body drag!)
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && g->player.silence_charge > 0.0f) {
        g->player.in_silence_mode = true;
        g->player.silence_charge -= dt * 25.0f;
        if (g->player.silence_charge < 0.0f) g->player.silence_charge = 0.0f;
        speed *= 1.10f; // Ghost speed boost!
        g->distortion_level = FAST_LERP(g->distortion_level, 0.85f, dt * 6.0f);
        particles_spawn_3d(&g->particles, g->player.pos, (Vec3){rnd_range(&g->rng_state,-10,10), rnd_range(&g->rng_state,-10,10), rnd_range(&g->rng_state,-10,10)}, (Color){0, 255, 200, 180}, 1.8f, 0.4f);
    } else {
        g->player.in_silence_mode = false;
        g->distortion_level = FAST_LERP(g->distortion_level, 0.0f, dt * 6.0f);
        if (g->player.silence_charge < 100.0f) {
            g->player.silence_charge += dt * 6.0f;
            if (g->player.silence_charge > 100.0f) g->player.silence_charge = 100.0f;
        }
    }

    // ===== MONSTER BODY CONTACT (Only applies when NOT in Ghost Mode) =====
    float total_monster_drag = 1.0f;
    Vec3 monster_pushback = {0.0f, 0.0f, 0.0f};

    if (!g->player.in_silence_mode) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            Enemy *e = &g->enemies[i];
            if (!e->active) continue;
            float d_player = vec3_dist(e->pos, g->player.pos);
            float body_r = 16.0f + e->radius_scale * 0.5f;
            if (d_player < body_r) {
                Vec3 push_dir = vec3_normalize(vec3_sub(g->player.pos, e->pos));
                if (e->monster_level == 3) {
                    total_monster_drag *= 0.45f;
                    monster_pushback = vec3_add(monster_pushback, vec3_scale(push_dir, 30.0f));
                } else if (e->monster_level == 2) {
                    total_monster_drag *= 0.70f;
                    monster_pushback = vec3_add(monster_pushback, vec3_scale(push_dir, 25.0f));
                } else {
                    total_monster_drag *= 0.90f;
                    monster_pushback = vec3_add(monster_pushback, vec3_scale(push_dir, 10.0f));
                }
            }
        }
    }

    speed *= total_monster_drag;

    if (vec3_length_sq(move) > 0.01f && g->player.skill_levels[14] > 0) {
        particles_spawn_3d(&g->particles, g->player.pos, (Vec3){0,0,0}, (Color){0, 240, 255, 180}, 2.0f, 0.2f);
    }

    Vec3 move_vec = vec3_add(vec3_scale(move, speed * dt), vec3_scale(monster_pushback, dt));
    move_vec.y = 0.0f; // HORIZONTAL MOVEMENT ONLY — prevent Y accumulator from launching player into the sky!

    // Outer Edge Black Hole Boundary — Initiate portal transition when reaching the edge
    float stage_max_r = 2500.0f + g->world_stage * 1500.0f;
    float dist_from_center = sqrtf(g->player.pos.x * g->player.pos.x + g->player.pos.z * g->player.pos.z);
    if (!g->transitioning && dist_from_center >= stage_max_r) {
        g->transitioning = true;
        g->transition_progress = 0.0f;
        g->player_frozen = true;
        g->pull_timer = 0.0f;
        audio_play_sfx(&g->audio, SFX_PORTAL_WIN, 1.0f);
        particles_burst_3d(&g->particles, g->player.pos, (Color){0, 240, 255, 255}, 80, 100.0f, 400.0f, 2.0f, 5.0f, 1.0f);
    }

    if (g->transitioning) {
        // Update transition progress (0.8s smooth hyperspace warp)
        g->transition_progress += dt / 0.8f;
        g->pull_timer += dt;

        // Pull player OUTWARD smoothly into the portal event horizon (NOT backward across the map!)
        Vec3 warp_dir = vec3_normalize((Vec3){g->player.pos.x, 0.0f, g->player.pos.z});
        if (vec3_length_sq(warp_dir) < 0.001f) warp_dir = (Vec3){0.0f, 0.0f, 1.0f};
        g->player.pos = vec3_add(g->player.pos, vec3_scale(warp_dir, 320.0f * dt));
        g->distortion_level = FAST_LERP(g->distortion_level, 1.0f, dt * 10.0f);

        // Freeze regular keyboard movement during portal transition
        move_vec = (Vec3){0.0f, 0.0f, 0.0f};

        if (g->transition_progress >= 1.0f) {
            g->world_stage++;
            if (g->world_stage > g->max_stage) g->max_stage = g->world_stage;

            // Restore health on stage clear
            g->player.health = g->player.max_health;

            g->player.pos = (Vec3){0.0f, 10.0f, 0.0f};
            g->player.crystals_collected = 0;
            g->player.silence_charge = 100.0f;
            g->survival_timer = 0.0f;
            g->screen_shake = 1.5f;
            g->distortion_level = 1.0f;

            world_init(&g->world, 9999 + g->world_stage * 13, g->world_stage);

            particles_burst_3d(&g->particles, (Vec3){0, 10.0f, 0}, (Color){255, 0, 200, 255}, 250, 300.0f, 800.0f, 5.0f, 10.0f, 2.5f);
            particles_burst_3d(&g->particles, (Vec3){0, 10.0f, 0}, (Color){0, 240, 255, 255}, 150, 200.0f, 600.0f, 3.0f, 8.0f, 2.0f);

            g->transitioning = false;
            g->player_frozen = false;
            g->pull_timer = -1.0f;
        }
    }

    g->player.pos = vec3_add(g->player.pos, move_vec);
    g->player.pos.y = 10.0f; // Firmly lock player height at ground level!

    g->camera.position = (Vector3){g->player.pos.x, g->player.pos.y, g->player.pos.z};
    g->camera.target = (Vector3){
        g->player.pos.x + cam_dir.x,
        g->player.pos.y + cam_dir.y,
        g->player.pos.z + cam_dir.z
    };

    // --- SPELLS ---
    g->player.bolt_timer += dt;
    if (g->player.bolt_timer >= FAST_MAX(0.10f, 0.45f * cd_mult)) {
        g->player.bolt_timer = 0.0f;
        int target = -1; float min_d = 650.0f;
        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (g->enemies[e].active) {
                float d = vec3_dist(g->player.pos, g->enemies[e].pos);
                if (d < min_d) { min_d = d; target = e; }
            }
        }
        if (target != -1) {
            Vec3 target_dir = vec3_normalize(vec3_sub(g->enemies[target].pos, g->player.pos));
            spawn_projectile(g, g->player.pos, vec3_scale(target_dir, 650.0f), 25.0f * dmg_mult, false);
            audio_play_sfx_3d(&g->audio, SFX_ECHO_REFLECT, g->player.pos, g->player.pos, cam_dir, 0.3f);
        }
    }

    if (g->player.skill_levels[1] > 0) {
        g->player.fireball_timer += dt;
        if (g->player.fireball_timer >= FAST_MAX(0.40f, 1.8f * cd_mult)) {
            g->player.fireball_timer = 0.0f;
            int target = -1; float min_d = 650.0f;
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (g->enemies[e].active) {
                    float d = vec3_dist(g->player.pos, g->enemies[e].pos);
                    if (d < min_d) { min_d = d; target = e; }
                }
            }
            if (target != -1) {
                Vec3 target_dir = vec3_normalize(vec3_sub(g->enemies[target].pos, g->player.pos));
                spawn_projectile(g, g->player.pos, vec3_scale(target_dir, 480.0f), (50.0f + g->player.skill_levels[1] * 20.0f) * dmg_mult, true);
                audio_play_sfx_3d(&g->audio, SFX_PULSE_BOOM, g->player.pos, g->player.pos, cam_dir, 0.6f);
            }
        }
    }

    if (g->player.skill_levels[2] > 0) {
        g->player.thunder_timer += dt;
        if (g->player.thunder_timer >= FAST_MAX(0.30f, 2.2f * cd_mult)) {
            g->player.thunder_timer = 0.0f;
            int num_strikes = 1 + g->player.skill_levels[2];
            for (int s = 0; s < num_strikes; s++) {
                int target = rnd_int(&g->rng_state, 0, MAX_ENEMIES - 1);
                if (g->enemies[target].active) {
                    for (int th = 0; th < MAX_THUNDERS; th++) {
                        if (!g->thunders[th].active) {
                            g->thunders[th].pos = g->enemies[target].pos;
                            g->thunders[th].damage = (60.0f + g->player.skill_levels[2] * 25.0f) * dmg_mult;
                            g->thunders[th].life = 0.4f;
                            g->thunders[th].active = true;
                            g->enemies[target].health -= g->thunders[th].damage;
                            particles_burst_3d(&g->particles, g->enemies[target].pos, (Color){0, 240, 255, 255}, 20, 80.0f, 250.0f, 2.0f, 4.0f, 0.5f);
                            audio_play_sfx_3d(&g->audio, SFX_PULSE_BOOM, g->enemies[target].pos, g->player.pos, cam_dir, 0.8f);
                            break;
                        }
                    }
                }
            }
        }
    }

    if (g->player.skill_levels[3] > 0) {
        g->player.meteor_timer += dt;
        if (g->player.meteor_timer >= FAST_MAX(0.50f, 3.2f * cd_mult)) {
            g->player.meteor_timer = 0.0f;
            int target = rnd_int(&g->rng_state, 0, MAX_ENEMIES - 1);
            if (g->enemies[target].active) {
                for (int m = 0; m < MAX_METEORS; m++) {
                    if (!g->meteors[m].active) {
                        g->meteors[m].target_pos = g->enemies[target].pos;
                        g->meteors[m].pos = (Vec3){g->enemies[target].pos.x + 40.0f, 220.0f, g->enemies[target].pos.z + 40.0f};
                        g->meteors[m].height = 220.0f;
                        g->meteors[m].radius = 45.0f + g->player.skill_levels[3] * 10.0f;
                        g->meteors[m].damage = (110.0f + g->player.skill_levels[3] * 40.0f) * dmg_mult;
                        g->meteors[m].active = true;
                        break;
                    }
                }
            }
        }
    }

    for (int m = 0; m < MAX_METEORS; m++) {
        MeteorSpell *met = &g->meteors[m];
        if (!met->active) continue;
        met->pos = vec3_add(met->pos, (Vec3){-120.0f * dt, -320.0f * dt, -120.0f * dt});
        particles_spawn_3d(&g->particles, met->pos, (Vec3){0,0,0}, (Color){255, 0, 50, 255}, 5.0f, 0.4f);
        if (met->pos.y <= 4.0f) {
            met->active = false;
            g->distortion_level = 1.0f;
            audio_play_sfx_3d(&g->audio, SFX_PULSE_BOOM, met->target_pos, g->player.pos, cam_dir, 1.0f);
            particles_burst_3d(&g->particles, met->target_pos, (Color){255, 0, 50, 255}, 80, 120.0f, 350.0f, 3.0f, 6.0f, 1.0f);
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (g->enemies[e].active && vec3_dist(g->enemies[e].pos, met->target_pos) < met->radius)
                    g->enemies[e].health -= met->damage;
            }
        }
    }

    for (int th = 0; th < MAX_THUNDERS; th++) {
        if (g->thunders[th].active) {
            g->thunders[th].life -= dt;
            if (g->thunders[th].life <= 0.0f) g->thunders[th].active = false;
        }
    }

    if (g->player.skill_levels[5] > 0) {
        g->player.ray_timer += dt;
        if (g->player.ray_timer >= FAST_MAX(0.30f, 2.5f * cd_mult)) {
            g->player.ray_timer = 0.0f;
            g->player.ray_active = true;
            g->player.ray_duration = 0.6f;
            g->player.ray_start = g->player.pos;
            g->player.ray_end = vec3_add(g->player.pos, vec3_scale(cam_dir, 600.0f));
            audio_play_sfx_3d(&g->audio, SFX_SILENCE_SHIELD, g->player.pos, g->player.pos, cam_dir, 0.8f);
            float ray_dmg = (75.0f + g->player.skill_levels[5] * 30.0f) * dmg_mult;
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (g->enemies[e].active) {
                    Vec3 to_enemy = vec3_sub(g->enemies[e].pos, g->player.pos);
                    float proj_len = vec3_dot(to_enemy, cam_dir);
                    if (proj_len > 0.0f && proj_len < 600.0f) {
                        Vec3 closest = vec3_add(g->player.pos, vec3_scale(cam_dir, proj_len));
                        if (vec3_dist(g->enemies[e].pos, closest) < 35.0f) {
                            g->enemies[e].health -= ray_dmg;
                            particles_burst_3d(&g->particles, g->enemies[e].pos, (Color){0, 240, 255, 255}, 10, 40.0f, 120.0f, 1.5f, 3.0f, 0.3f);
                        }
                    }
                }
            }
        }
    }

    if (g->player.ray_active) {
        g->player.ray_duration -= dt;
        if (g->player.ray_duration <= 0.0f) g->player.ray_active = false;
    }

    if (g->player.skill_levels[6] > 0) {
        g->player.lava_timer += dt;
        if (g->player.lava_timer >= FAST_MAX(0.50f, 3.5f * cd_mult)) {
            g->player.lava_timer = 0.0f;
            for (int l = 0; l < MAX_LAVA_ZONES; l++) {
                if (!g->lavas[l].active) {
                    g->lavas[l].pos = g->player.pos;
                    g->lavas[l].radius = 40.0f + g->player.skill_levels[6] * 12.0f;
                    g->lavas[l].duration = 6.0f;
                    g->lavas[l].damage_timer = 0.0f;
                    g->lavas[l].active = true;
                    break;
                }
            }
        }
    }

    for (int l = 0; l < MAX_LAVA_ZONES; l++) {
        LavaZoneSpell *lava = &g->lavas[l];
        if (!lava->active) continue;
        lava->duration -= dt;
        if (lava->duration <= 0.0f) { lava->active = false; continue; }
        lava->damage_timer += dt;
        if (lava->damage_timer >= 0.25f) {
            lava->damage_timer = 0.0f;
            float lava_dmg = (18.0f + g->player.skill_levels[6] * 8.0f) * dmg_mult;
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (g->enemies[e].active && vec3_dist(g->enemies[e].pos, lava->pos) < lava->radius)
                    g->enemies[e].health -= lava_dmg;
            }
        }
    }

    if (g->player.skill_levels[7] > 0) {
        g->player.tsunami_timer += dt;
        if (g->player.tsunami_timer >= FAST_MAX(0.60f, 5.0f * cd_mult)) {
            g->player.tsunami_timer = 0.0f;
            for (int ts = 0; ts < MAX_TSUNAMIS; ts++) {
                if (!g->tsunamis[ts].active) {
                    g->tsunamis[ts].pos = g->player.pos;
                    g->tsunamis[ts].dir = cam_dir;
                    g->tsunamis[ts].width = 120.0f + g->player.skill_levels[7] * 25.0f;
                    g->tsunamis[ts].dist = 0.0f;
                    g->tsunamis[ts].max_dist = 700.0f;
                    g->tsunamis[ts].damage = (90.0f + g->player.skill_levels[7] * 35.0f) * dmg_mult;
                    g->tsunamis[ts].active = true;
                    audio_play_sfx_3d(&g->audio, SFX_PULSE_BOOM, g->player.pos, g->player.pos, cam_dir, 1.0f);
                    break;
                }
            }
        }
    }

    for (int ts = 0; ts < MAX_TSUNAMIS; ts++) {
        TsunamiSpell *tsu = &g->tsunamis[ts];
        if (!tsu->active) continue;
        tsu->dist += dt * 550.0f;
        tsu->pos = vec3_add(tsu->pos, vec3_scale(tsu->dir, 550.0f * dt));
        particles_burst_3d(&g->particles, tsu->pos, (Color){0, 240, 255, 255}, 15, 60.0f, 200.0f, 1.5f, 3.0f, 0.4f);
        if (tsu->dist >= tsu->max_dist) { tsu->active = false; continue; }
        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (g->enemies[e].active && vec3_dist(g->enemies[e].pos, tsu->pos) < tsu->width * 0.5f)
                g->enemies[e].health -= tsu->damage * dt * 4.0f;
        }
    }

    if (g->player.skill_levels[10] > 0) {
        g->player.armageddon_timer += dt;
        if (g->player.armageddon_timer >= FAST_MAX(2.0f, 15.0f * cd_mult)) {
            g->player.armageddon_timer = 0.0f;
            g->distortion_level = 1.0f;
            audio_play_sfx_3d(&g->audio, SFX_PORTAL_WIN, g->player.pos, g->player.pos, cam_dir, 1.0f);
            particles_burst_3d(&g->particles, g->player.pos, (Color){255, 215, 0, 255}, 180, 150.0f, 500.0f, 4.0f, 8.0f, 1.5f);
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (g->enemies[e].active) g->enemies[e].health -= 300.0f * dmg_mult;
            }
        }
    }

    // ===== SATELLITE [4]: Contact damage to enemies within orbit radius =====
    if (g->player.skill_levels[4] > 0) {
        int ns = 2 + g->player.skill_levels[4];
        float sat_dmg = (20.0f + g->player.skill_levels[4] * 15.0f) * dmg_mult;
        float sat_r = 32.0f; // satellite sphere radius
        float orbit_r = 32.0f;
        for (int k = 0; k < ns; k++) {
            float sa = (k / (float)ns) * TAU + (float)GetTime() * 3.5f;
            Vec3 sat_pos = (Vec3){
                g->player.pos.x + cosf(sa) * orbit_r,
                g->player.pos.y + 10.0f,
                g->player.pos.z + sinf(sa) * orbit_r
            };
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (g->enemies[e].active && vec3_dist(g->enemies[e].pos, sat_pos) < sat_r + g->enemies[e].radius_scale) {
                    g->enemies[e].health -= sat_dmg * dt;
                    particles_spawn_3d(&g->particles, sat_pos, (Vec3){0,0,0}, (Color){255, 215, 0, 200}, 2.5f, 0.15f);
                }
            }
        }
    }

    // ===== ELECTRIC ZONE [8]: Shock ring damages all enemies in radius =====
    if (g->player.skill_levels[8] > 0) {
        float ez_r = 40.0f + g->player.skill_levels[8] * 8.0f;
        float ez_dmg = (15.0f + g->player.skill_levels[8] * 10.0f) * dmg_mult;
        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (g->enemies[e].active && vec3_dist(g->enemies[e].pos, g->player.pos) < ez_r) {
                g->enemies[e].health -= ez_dmg * dt;
                if (rnd_float(&g->rng_state) < 0.05f)
                    particles_spawn_3d(&g->particles, g->enemies[e].pos, (Vec3){0, 20.0f, 0}, (Color){0, 240, 255, 220}, 2.0f, 0.2f);
            }
        }
    }

    // Projectiles
    for (int p = 0; p < MAX_PROJECTILES; p++) {
        MagicProjectile *proj = &g->projectiles[p];
        if (!proj->active) continue;
        proj->life -= dt;
        if (proj->life <= 0.0f) { proj->active = false; continue; }
        proj->pos = vec3_add(proj->pos, vec3_scale(proj->vel, dt));
        Color p_col = proj->is_fireball ? (Color){255, 0, 50, 255} : (Color){0, 240, 255, 255};
        particles_spawn_3d(&g->particles, proj->pos, (Vec3){0,0,0}, p_col, 3.5f, 0.2f);

        for (int e = 0; e < MAX_ENEMIES; e++) {
            Enemy *enemy = &g->enemies[e];
            if (!enemy->active) continue;
            if (vec3_dist(proj->pos, enemy->pos) < (14.0f + enemy->radius_scale * 0.5f)) {
                proj->active = false;
                enemy->health -= proj->damage;
                if (proj->is_fireball) {
                    particles_burst_3d(&g->particles, enemy->pos, (Color){255, 0, 50, 255}, 35, 80.0f, 220.0f, 2.5f, 5.0f, 0.6f);
                    for (int n = 0; n < MAX_ENEMIES; n++) {
                        if (g->enemies[n].active && vec3_dist(g->enemies[n].pos, enemy->pos) < 55.0f)
                            g->enemies[n].health -= proj->damage * 0.7f;
                    }
                } else {
                    particles_burst_3d(&g->particles, enemy->pos, (Color){0, 240, 255, 255}, 15, 60.0f, 180.0f, 2.0f, 4.0f, 0.4f);
                }
                if (enemy->health <= 0.0f) {
                    enemy->active = false;
                    g->total_kills++;
                    add_mana(g, 4.0f);
                    if (enemy->monster_level == 3) {
                        world_spawn_mana_orb(&g->world, enemy->pos, true);
                        world_spawn_mana_orb(&g->world, vec3_add(enemy->pos, (Vec3){10,0,10}), true);
                        world_spawn_mana_orb(&g->world, vec3_add(enemy->pos, (Vec3){-10,0,-10}), true);
                    } else {
                        world_spawn_mana_orb(&g->world, enemy->pos, (rnd_float(&g->rng_state) < 0.25f));
                        // ~20% chance to drop a heart on death
                        if (rnd_float(&g->rng_state) < 0.20f)
                            world_spawn_heart(&g->world, enemy->pos);
                    }
                    spawn_enemy_instance(g, e, progress);
                }
                break;
            }
        }
    }

    // Decoy
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !g->decoy.active) {
        g->decoy.active = true;
        g->decoy.pos = g->player.pos;
        g->decoy.vel = vec3_scale(cam_dir, 420.0f);
        g->decoy.timer = 1.0f;
        g->decoy.pulse_radius = 0.0f;
        g->decoy.pulse_intensity = 0.0f;
        particles_burst_3d(&g->particles, g->player.pos, (Color){255, 215, 0, 255}, 25, 80.0f, 180.0f, 2.0f, 4.0f, 0.5f);
    }

    if (g->decoy.active) {
        if (g->decoy.timer > 0.0f) {
            g->decoy.timer -= dt;
            g->decoy.pos = vec3_add(g->decoy.pos, vec3_scale(g->decoy.vel, dt));
            g->decoy.vel.y -= dt * 300.0f;
            particles_spawn_3d(&g->particles, g->decoy.pos, (Vec3){0,0,0}, (Color){255, 215, 0, 220}, 2.5f, 0.3f);
            if (g->decoy.timer <= 0.0f || g->decoy.pos.y <= 2.0f) {
                g->decoy.timer = 0.0f;
                g->decoy.pulse_radius = 0.0f;
                g->decoy.pulse_intensity = 1.0f;
                audio_play_sfx_3d(&g->audio, SFX_DECOY_LAND, g->decoy.pos, g->player.pos, cam_dir, 1.0f);
                particles_burst_3d(&g->particles, g->decoy.pos, (Color){255, 215, 0, 255}, 100, 120.0f, 320.0f, 3.0f, 6.0f, 0.9f);
                for (int e = 0; e < MAX_ENEMIES; e++) {
                    if (g->enemies[e].active && vec3_dist(g->enemies[e].pos, g->decoy.pos) < 450.0f) {
                        g->enemies[e].state = ENEMY_INVESTIGATE_DECOY;
                        g->enemies[e].target_pos = g->decoy.pos;
                    }
                }
            }
        } else {
            g->decoy.pulse_radius += dt * 600.0f;
            g->decoy.pulse_intensity = 1.0f - (g->decoy.pulse_radius / 500.0f);
            if (g->decoy.pulse_radius >= 500.0f) g->decoy.active = false;
        }
    }

    // ===== ECHOLOCATION PULSE KNOCKBACK (WITH COOLDOWN) =====
    if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) && g->player.pulse_cooldown <= 0.0f) {
        g->player.charging_pulse = true;
        g->player.pulse_charge_time += dt;
        if (g->player.pulse_charge_time > 2.0f) g->player.pulse_charge_time = 2.0f;
        particles_spawn_3d(&g->particles, g->player.pos, (Vec3){cosf(g->player.pulse_charge_time*20)*20, sinf(g->player.pulse_charge_time*20)*20, sinf(g->player.pulse_charge_time*15)*20}, (Color){0, 240, 255, 200}, 2.0f, 0.3f);
    } else if (g->player.charging_pulse) {
        g->player.charging_pulse = false;
        float ratio = g->player.pulse_charge_time / 2.0f;
        if (ratio < 0.1f) ratio = 0.1f;
        g->player.pulse_active = true;
        g->player.pulse_origin = g->player.pos;
        g->player.pulse_radius = 0.0f;
        g->player.pulse_max_radius = 450.0f + ratio * 750.0f;
        g->player.pulse_intensity = 1.0f;
        g->player.pulse_cooldown = 1.8f; // Set 1.8s Cooldown after pulse attack!

        audio_emit_noise(&g->audio, g->player.in_silence_mode ? 0.05f : (ratio * 0.9f));
        audio_play_sfx_3d(&g->audio, SFX_PULSE_BOOM, g->player.pos, g->player.pos, cam_dir, 0.9f + ratio * 0.3f);
        particles_burst_3d(&g->particles, g->player.pos, (Color){0, 240, 255, 255}, 90 + (int)(ratio * 120), 120.0f, 320.0f, 2.5f, 5.0f, 0.8f);
        g->player.pulse_charge_time = 0.0f;
    }

    if (g->player.pulse_active) {
        g->player.pulse_radius += dt * 680.0f;
        g->player.pulse_intensity = 1.0f - (g->player.pulse_radius / g->player.pulse_max_radius);

        // POWERFUL KNOCKBACK: Pulse wave pushes all monsters violently away!
        for (int e = 0; e < MAX_ENEMIES; e++) {
            Enemy *enemy = &g->enemies[e];
            if (!enemy->active) continue;
            float d_orig = vec3_dist(enemy->pos, g->player.pulse_origin);
            if (d_orig < g->player.pulse_radius && d_orig > (g->player.pulse_radius - 80.0f)) {
                Vec3 push_dir = vec3_normalize(vec3_sub(enemy->pos, g->player.pulse_origin));
                float knockback_force = 450.0f * dt * g->player.pulse_intensity;
                if (enemy->monster_level == 3) knockback_force *= 0.25f;
                enemy->pos = vec3_add(enemy->pos, vec3_scale(push_dir, knockback_force));
            }
        }

        if (g->player.pulse_radius >= g->player.pulse_max_radius) {
            g->player.pulse_active = false;
            g->player.pulse_intensity = 0.0f;
        }
        world_update_3d(&g->world, dt, g->player.pos, g->player.pulse_origin, g->player.pulse_radius, g->player.pulse_intensity);
    } else {
        world_update_3d(&g->world, dt, g->player.pos, (Vec3){0,0,0}, 0, 0);
    }

    // Enemy AI & Contact Damage
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &g->enemies[i];
        if (!e->active) continue;
        float dist_to_player = vec3_dist(e->pos, g->player.pos);
        if (dist_to_player > 750.0f) {
            spawn_enemy_instance(g, i, progress);
            dist_to_player = vec3_dist(e->pos, g->player.pos);
        }
        e->state = ENEMY_CHASE;
        e->target_pos = g->player.pos;
        float cur_speed = enemy_chase_speed;
        if (e->monster_level == 3) cur_speed *= 0.35f;
        else if (e->type_class == ENEMY_CLASS_STALKER) cur_speed *= 1.25f;
        Vec3 dir = vec3_normalize(vec3_sub(e->target_pos, e->pos));
        e->pos = vec3_add(e->pos, vec3_scale(dir, cur_speed * dt));

        float attack_radius = 16.0f + e->radius_scale * 0.5f;
        if (dist_to_player < attack_radius) {
            if (g->player.skill_levels[9] > 0 && rnd_float(&g->rng_state) < 0.45f) {
                particles_burst_3d(&g->particles, g->player.pos, (Color){0, 255, 200, 255}, 15, 40.0f, 100.0f, 1.5f, 3.0f, 0.3f);
            } else {
                g->player.health -= dt * enemy_damage_rate;
                g->distortion_level = 1.0f;
                audio_play_sfx_3d(&g->audio, SFX_PLAYER_HURT, e->pos, g->player.pos, cam_dir, 0.8f);
                particles_burst_3d(&g->particles, g->player.pos, (Color){255, 0, 170, 255}, 6, 60.0f, 150.0f, 2.0f, 4.0f, 0.4f);
                if (g->player.health <= 0.0f) {
                    g->player.health = 0.0f;
                    g->state = STATE_GAMEOVER;
                }
            }
        }
    }

    // Pickups
    Vec2 p_xz = (Vec2){g->player.pos.x, g->player.pos.z};
    float pickup_radius_boost = 1.0f + g->player.skill_levels[14] * 0.50f;
    for (int i = 0; i < g->world.count; i++) {
        WorldObject *obj = &g->world.objects[i];
        if (!obj->active) continue;
        Vec2 obj_xz = (Vec2){obj->pos.x, obj->pos.z};
        float dist_xz = vec2_dist(p_xz, obj_xz);
        if (dist_xz < ((22.0f + obj->radius) * pickup_radius_boost)) {
            if (obj->type == OBJ_CHEST) {
                obj->active = false;
                trigger_artifact_unlock(g);
                particles_burst_3d(&g->particles, obj->pos, (Color){255, 215, 0, 255}, 60, 100.0f, 300.0f, 3.0f, 6.0f, 1.0f);
            } else if (obj->type == OBJ_MANA_BLUE) {
                obj->active = false;
                add_mana(g, 15.0f);
                audio_play_sfx_3d(&g->audio, SFX_CRYSTAL_COLLECT, obj->pos, g->player.pos, cam_dir, 0.6f);
            } else if (obj->type == OBJ_MANA_YELLOW) {
                obj->active = false;
                add_mana(g, 50.0f);
                audio_play_sfx_3d(&g->audio, SFX_CRYSTAL_COLLECT, obj->pos, g->player.pos, cam_dir, 0.9f);
            } else if (obj->type == OBJ_HEART) {
                obj->active = false;
                float heal = 25.0f;
                g->player.health = FAST_MIN(g->player.health + heal, g->player.max_health);
                audio_play_sfx_3d(&g->audio, SFX_CRYSTAL_COLLECT, obj->pos, g->player.pos, cam_dir, 1.0f);
                particles_burst_3d(&g->particles, obj->pos, (Color){255, 60, 100, 255}, 30, 60.0f, 180.0f, 2.0f, 4.0f, 0.6f);
            } else if (obj->type == OBJ_CRYSTAL) {
                obj->active = false;
                g->player.crystals_collected++;
                audio_play_sfx_3d(&g->audio, SFX_CRYSTAL_COLLECT, obj->pos, g->player.pos, cam_dir, 1.0f);
                particles_burst_3d(&g->particles, obj->pos, (Color){255, 215, 0, 255}, 45, 90.0f, 240.0f, 3.0f, 6.0f, 0.7f);
            } else if (obj->type == OBJ_SILENCE_NODE) {
                obj->active = false;
                g->player.silence_charge = FAST_MIN(100.0f, g->player.silence_charge + 45.0f);
                audio_play_sfx_3d(&g->audio, SFX_SILENCE_SHIELD, obj->pos, g->player.pos, cam_dir, 0.9f);
                particles_burst_3d(&g->particles, obj->pos, (Color){0, 255, 200, 255}, 35, 70.0f, 200.0f, 2.5f, 5.0f, 0.6f);
            } else if (obj->type == OBJ_PORTAL && g->player.crystals_collected >= REQUIRED_CRYSTALS) {
                // Next Stage Transition!
                g->world_stage++;
                if (g->world_stage > g->max_stage) g->max_stage = g->world_stage;
                
                // Preserve Player HP/Level/Spells, but reset position & world for next stage
                g->player.pos = (Vec3){0.0f, 10.0f, 0.0f};
                g->player.crystals_collected = 0;
                g->player.silence_charge = 100.0f;
                g->survival_timer = 0.0f;
                g->distortion_level = 1.0f;
                
                world_init(&g->world, 9999 + g->world_stage * 13, g->world_stage);
                
                audio_play_sfx_3d(&g->audio, SFX_PORTAL_WIN, obj->pos, g->player.pos, cam_dir, 1.0f);
                particles_burst_3d(&g->particles, g->player.pos, (Color){255, 215, 0, 255}, 200, 200.0f, 600.0f, 4.0f, 9.0f, 2.0f);
            }
        }
    }
    particles_update_3d(&g->particles, dt);
}

// =============== DRAW ===============

void game_draw(GameState *g, Shader bg_shader, Shader echo_shader, Shader post_shader, RenderTexture2D target) {
    if (!g) return;

    float t = (float)GetTime();
    float res[2] = {1280.0f, 720.0f};
    Vector2 mpos = GetMousePosition();

    BeginDrawing();
    ClearBackground(BLACK);

    BeginTextureMode(target);
    ClearBackground((Color){5, 8, 20, 255});

    // Background shader (Pure Starry Night Sky)
    BeginShaderMode(bg_shader);
    SetShaderValue(bg_shader, g->shader_locs.bg_time_loc, &t, SHADER_UNIFORM_FLOAT);
    SetShaderValue(bg_shader, g->shader_locs.bg_res_loc, res, SHADER_UNIFORM_VEC2);
    DrawRectangle(0, 0, 1280, 720, WHITE);
    EndShaderMode();

    // ===== MENU =====
    if (g->state == STATE_MENU) {
        const char *title = "E C H O";
        draw_text_bitmap(title, center_x(title, 0, 1280, 6), 140, 6, (Color){0, 240, 255, 255});

        const char *sub = "COSMIC SURVIVAL 3D HORDE";
        draw_text_bitmap(sub, center_x(sub, 0, 1280, 2), 230, 2, (Color){255, 215, 0, 220});

        // START button
        bool h_start = (mpos.x >= BTN_X && mpos.x <= BTN_X + BTN_W && mpos.y >= BTN_Y_START && mpos.y <= BTN_Y_START + BTN_H);
        DrawRectangle(BTN_X, BTN_Y_START, BTN_W, BTN_H, h_start ? (Color){30, 70, 130, 245} : (Color){15, 30, 60, 230});
        DrawRectangleLines(BTN_X, BTN_Y_START, BTN_W, BTN_H, h_start ? (Color){255, 215, 0, 255} : (Color){0, 240, 255, 255});
        if (h_start) DrawRectangleLines(BTN_X - 2, BTN_Y_START - 2, BTN_W + 4, BTN_H + 4, (Color){0, 240, 255, 180});
        const char *s1 = T(TR_START_KEY);
        draw_text_bitmap(s1, center_x(s1, BTN_X, BTN_W, 2), center_y(BTN_Y_START, BTN_H, 2), 2, h_start ? (Color){255, 215, 0, 255} : WHITE);

        // GUIDE button
        bool h_guide = (mpos.x >= BTN_X && mpos.x <= BTN_X + BTN_W && mpos.y >= BTN_Y_GUIDE && mpos.y <= BTN_Y_GUIDE + BTN_H);
        DrawRectangle(BTN_X, BTN_Y_GUIDE, BTN_W, BTN_H, h_guide ? (Color){80, 25, 90, 245} : (Color){40, 15, 50, 230});
        DrawRectangleLines(BTN_X, BTN_Y_GUIDE, BTN_W, BTN_H, h_guide ? (Color){0, 240, 255, 255} : (Color){255, 0, 170, 255});
        if (h_guide) DrawRectangleLines(BTN_X - 2, BTN_Y_GUIDE - 2, BTN_W + 4, BTN_H + 4, (Color){255, 0, 170, 180});
        const char *s2 = T(TR_GUIDE_BTN);
        draw_text_bitmap(s2, center_x(s2, BTN_X, BTN_W, 2), center_y(BTN_Y_GUIDE, BTN_H, 2), 2, h_guide ? (Color){0, 240, 255, 255} : (Color){255, 215, 0, 255});

        // SETTINGS button
        bool h_sett = (mpos.x >= BTN_X && mpos.x <= BTN_X + BTN_W && mpos.y >= BTN_Y_SETT && mpos.y <= BTN_Y_SETT + BTN_H);
        DrawRectangle(BTN_X, BTN_Y_SETT, BTN_W, BTN_H, h_sett ? (Color){25, 85, 70, 245} : (Color){15, 45, 40, 230});
        DrawRectangleLines(BTN_X, BTN_Y_SETT, BTN_W, BTN_H, h_sett ? (Color){255, 215, 0, 255} : (Color){0, 255, 200, 255});
        if (h_sett) DrawRectangleLines(BTN_X - 2, BTN_Y_SETT - 2, BTN_W + 4, BTN_H + 4, (Color){0, 255, 200, 180});
        const char *s3 = T(TR_SETT_BTN);
        draw_text_bitmap(s3, center_x(s3, BTN_X, BTN_W, 2), center_y(BTN_Y_SETT, BTN_H, 2), 2, h_sett ? (Color){255, 215, 0, 255} : (Color){0, 255, 200, 255});

        const char *ch = T(TR_CONTROLS);
        draw_text_bitmap(ch, center_x(ch, 0, 1280, 1), 540, 1, (Color){180, 180, 220, 200});
        const char *mh = T(TR_MONSTERS);
        draw_text_bitmap(mh, center_x(mh, 0, 1280, 1), 570, 1, (Color){0, 255, 200, 220});

        // High Score / Max Stage Indicator on the Right Side of Main Menu Lobby
        DrawRectangle(950, 140, 300, 90, (Color){10, 14, 35, 235});
        DrawRectangleLines(950, 140, 300, 90, (Color){255, 215, 0, 255});
        draw_text_bitmap("BEST RECORD", center_x("BEST RECORD", 950, 300, 2), 155, 2, (Color){255, 215, 0, 255});
        char rec_str[64];
        snprintf(rec_str, sizeof(rec_str), "MAX STAGE: %d", g->max_stage > 0 ? g->max_stage : 1);
        draw_text_bitmap(rec_str, center_x(rec_str, 950, 300, 2), 190, 2, (Color){0, 240, 255, 255});

        draw_neon_cursor(mpos);

        EndTextureMode();
        float zero_dist = 0.0f;
        BeginShaderMode(post_shader);
        SetShaderValue(post_shader, g->shader_locs.post_time_loc, &t, SHADER_UNIFORM_FLOAT);
        SetShaderValue(post_shader, g->shader_locs.post_res_loc, res, SHADER_UNIFORM_VEC2);
        SetShaderValue(post_shader, g->shader_locs.post_dist_loc, &zero_dist, SHADER_UNIFORM_FLOAT);
        DrawTexturePro(target.texture, (Rectangle){0, 0, 1280, -720}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0.0f, WHITE);
        EndShaderMode();
        EndDrawing();
        return;
    }

    // ===== SETTINGS =====
    if (g->state == STATE_SETTINGS) {
        DrawRectangle(220, 90, 840, 540, (Color){12, 14, 35, 245});
        DrawRectangleLines(220, 90, 840, 540, (Color){0, 255, 200, 255});

        const char *st = T(TR_SETT_TITLE);
        draw_text_bitmap(st, center_x(st, 220, 840, 3), 120, 3, (Color){0, 255, 200, 255});

        const char *bh = T(TR_BACK_HINT);
        draw_text_bitmap(bh, center_x(bh, 220, 840, 1), 170, 1, (Color){180, 180, 220, 220});

        const char *sl = T(TR_SELECT_LNG);
        draw_text_bitmap(sl, center_x(sl, 220, 840, 2), 220, 2, (Color){255, 215, 0, 255});

        // Language 1: English
        bool h_en = (mpos.x >= LANG_BTN_X && mpos.x <= LANG_BTN_X + LANG_BTN_W && mpos.y >= LANG_BTN_Y1 && mpos.y <= LANG_BTN_Y1 + LANG_BTN_H);
        Color c_en = (g->language == LANG_EN) ? (Color){0, 240, 255, 255} : (h_en ? (Color){0, 200, 220, 255} : (Color){80, 80, 120, 255});
        DrawRectangle(LANG_BTN_X, LANG_BTN_Y1, LANG_BTN_W, LANG_BTN_H, (g->language == LANG_EN) ? (Color){25, 45, 75, 240} : (h_en ? (Color){25, 35, 65, 240} : (Color){20, 25, 55, 240}));
        DrawRectangleLines(LANG_BTN_X, LANG_BTN_Y1, LANG_BTN_W, LANG_BTN_H, c_en);
        const char *le = T(TR_ENGLISH);
        draw_text_bitmap(le, LANG_BTN_X + 20, center_y(LANG_BTN_Y1, LANG_BTN_H, 2), 2, c_en);
        if (g->language == LANG_EN) {
            const char *ac = T(TR_ACTIVE);
            draw_text_bitmap(ac, LANG_BTN_X + LANG_BTN_W - text_width(ac, 2) - 20, center_y(LANG_BTN_Y1, LANG_BTN_H, 2), 2, (Color){0, 240, 255, 255});
        }

        // Language 2: Russian
        bool h_ru = (mpos.x >= LANG_BTN_X && mpos.x <= LANG_BTN_X + LANG_BTN_W && mpos.y >= LANG_BTN_Y2 && mpos.y <= LANG_BTN_Y2 + LANG_BTN_H);
        Color c_ru = (g->language == LANG_RU) ? (Color){255, 0, 170, 255} : (h_ru ? (Color){220, 0, 150, 255} : (Color){80, 80, 120, 255});
        DrawRectangle(LANG_BTN_X, LANG_BTN_Y2, LANG_BTN_W, LANG_BTN_H, (g->language == LANG_RU) ? (Color){55, 20, 55, 240} : (h_ru ? (Color){45, 20, 45, 240} : (Color){20, 25, 55, 240}));
        DrawRectangleLines(LANG_BTN_X, LANG_BTN_Y2, LANG_BTN_W, LANG_BTN_H, c_ru);
        const char *lr = T(TR_RUSSIAN);
        draw_text_bitmap(lr, LANG_BTN_X + 20, center_y(LANG_BTN_Y2, LANG_BTN_H, 2), 2, c_ru);
        if (g->language == LANG_RU) {
            const char *ac = T(TR_ACTIVE);
            draw_text_bitmap(ac, LANG_BTN_X + LANG_BTN_W - text_width(ac, 2) - 20, center_y(LANG_BTN_Y2, LANG_BTN_H, 2), 2, (Color){255, 0, 170, 255});
        }

        // Fullscreen Toggle
        bool is_fs = IsWindowFullscreen();
        bool h_fs = (mpos.x >= LANG_BTN_X && mpos.x <= LANG_BTN_X + LANG_BTN_W && mpos.y >= FS_BTN_Y && mpos.y <= FS_BTN_Y + LANG_BTN_H);
        Color c_fs = is_fs ? (Color){0, 255, 200, 255} : (h_fs ? (Color){0, 220, 180, 255} : (Color){80, 80, 120, 255});
        DrawRectangle(LANG_BTN_X, FS_BTN_Y, LANG_BTN_W, LANG_BTN_H, is_fs ? (Color){15, 45, 40, 240} : (h_fs ? (Color){20, 35, 45, 240} : (Color){20, 25, 55, 240}));
        DrawRectangleLines(LANG_BTN_X, FS_BTN_Y, LANG_BTN_W, LANG_BTN_H, c_fs);
        const char *lfs = T(TR_FULLSCREEN);
        draw_text_bitmap(lfs, LANG_BTN_X + 20, center_y(FS_BTN_Y, LANG_BTN_H, 2), 2, c_fs);
        const char *fs_status = is_fs ? T(TR_FS_ON) : T(TR_FS_OFF);
        draw_text_bitmap(fs_status, LANG_BTN_X + LANG_BTN_W - text_width(fs_status, 2) - 20, center_y(FS_BTN_Y, LANG_BTN_H, 2), 2, is_fs ? (Color){0, 255, 200, 255} : (Color){180, 180, 200, 255});

        // Volume
        const char *vol = T(TR_VOLUME);
        draw_text_bitmap(vol, LANG_BTN_X + 20, 465, 2, (Color){0, 240, 255, 255});
        int vol_bar_x = LANG_BTN_X + LANG_BTN_W - 230;
        DrawRectangle(vol_bar_x, 465, 200, 20, (Color){40, 40, 60, 220});
        DrawRectangle(vol_bar_x, 465, (int)(200.0f * g->master_volume), 20, (Color){0, 240, 255, 255});
        DrawRectangleLines(vol_bar_x, 465, 200, 20, WHITE);
        char vol_str[16];
        snprintf(vol_str, sizeof(vol_str), "%d%%", (int)(g->master_volume * 100.0f));
        draw_text_bitmap(vol_str, vol_bar_x + 210, 465, 2, (Color){255, 215, 0, 255});

        const char *lh = "CLICK OR PRESS [1] [2] [F]";
        draw_text_bitmap(lh, center_x(lh, 220, 840, 1), 540, 1, (Color){0, 255, 200, 220});

        draw_neon_cursor(mpos);

        EndTextureMode();
        float zero_dist = 0.0f;
        BeginShaderMode(post_shader);
        SetShaderValue(post_shader, g->shader_locs.post_time_loc, &t, SHADER_UNIFORM_FLOAT);
        SetShaderValue(post_shader, g->shader_locs.post_res_loc, res, SHADER_UNIFORM_VEC2);
        SetShaderValue(post_shader, g->shader_locs.post_dist_loc, &zero_dist, SHADER_UNIFORM_FLOAT);
        DrawTexturePro(target.texture, (Rectangle){0, 0, 1280, -720}, (Rectangle){0, 0, 1280, 720}, (Vector2){0, 0}, 0.0f, WHITE);
        EndShaderMode();
        EndDrawing();
        return;
    }

    // ===== GUIDE =====
    if (g->state == STATE_GUIDE) {
        DrawRectangle(60, 40, 1160, 640, (Color){12, 14, 35, 245});
        DrawRectangleLines(60, 40, 1160, 640, (Color){0, 240, 255, 255});

        const char *gt = "ECHO ARCHIVES & GAME GUIDE";
        draw_text_bitmap(gt, center_x(gt, 60, 1160, 3), 65, 3, (Color){255, 215, 0, 255});
        const char *gh = "[1] [2] [3] OR TAB TO SWITCH  |  ESC TO EXIT";
        draw_text_bitmap(gh, center_x(gh, 60, 1160, 1), 105, 1, (Color){180, 180, 220, 220});

        Color tab0_c = (g->guide_tab == 0) ? (Color){0, 240, 255, 255} : (Color){80, 80, 120, 255};
        Color tab1_c = (g->guide_tab == 1) ? (Color){255, 0, 170, 255} : (Color){80, 80, 120, 255};
        Color tab2_c = (g->guide_tab == 2) ? (Color){255, 215, 0, 255} : (Color){80, 80, 120, 255};

        DrawRectangle(100, 130, 320, 36, (Color){20, 25, 55, 250});
        DrawRectangleLines(100, 130, 320, 36, tab0_c);
        const char *t0 = "[1] 16 COSMIC SPELLS";
        draw_text_bitmap(t0, center_x(t0, 100, 320, 2), center_y(130, 36, 2), 2, tab0_c);

        DrawRectangle(480, 130, 320, 36, (Color){20, 25, 55, 250});
        DrawRectangleLines(480, 130, 320, 36, tab1_c);
        const char *t1 = "[2] MONSTER HORDE";
        draw_text_bitmap(t1, center_x(t1, 480, 320, 2), center_y(130, 36, 2), 2, tab1_c);

        DrawRectangle(860, 130, 320, 36, (Color){20, 25, 55, 250});
        DrawRectangleLines(860, 130, 320, 36, tab2_c);
        const char *t2 = "[3] GOALS & LORE";
        draw_text_bitmap(t2, center_x(t2, 860, 320, 2), center_y(130, 36, 2), 2, tab2_c);

        if (g->guide_tab == 0) {
            for (int i = 0; i < NUM_SKILLS; i++) {
                int col = i % 4;
                int row = i / 4;
                int sx = 95 + col * 280;
                int sy = 190 + row * 115;
                const SkillCardInfo *sp = &SKILL_CARDS[i];
                const char *stitle = (g->language == LANG_RU) ? sp->title_ru : sp->title_en;
                const char *scat   = (g->language == LANG_RU) ? sp->category_ru : sp->category_en;
                const char *spros  = (g->language == LANG_RU) ? sp->pros_ru : sp->pros_en;
                const char *scons  = (g->language == LANG_RU) ? sp->cons_ru : sp->cons_en;

                DrawRectangle(sx, sy, 265, 105, (Color){18, 22, 48, 240});
                DrawRectangleLines(sx, sy, 265, 105, (Color){0, 240, 255, 180});
                draw_text_bitmap(stitle, sx + 10, sy + 12, 1, (Color){0, 240, 255, 255});
                draw_text_bitmap(scat, sx + 10, sy + 34, 1, (Color){255, 0, 170, 220});
                draw_text_bitmap(spros, sx + 10, sy + 54, 1, (Color){0, 255, 200, 255});
                draw_text_bitmap(scons, sx + 10, sy + 76, 1, (Color){255, 215, 0, 255});
            }
        } else if (g->guide_tab == 1) {
            DrawRectangle(100, 190, 1080, 130, (Color){22, 18, 40, 240});
            DrawRectangleLines(100, 190, 1080, 130, (Color){255, 0, 170, 255});
            draw_text_bitmap("LVL 1: SMALL BLACK TENTACLE BEASTS", 120, 205, 2, (Color){255, 0, 170, 255});
            draw_text_bitmap("Pitch-black void imps with 8 writhing 3D tentacles and ruby eyes.", 120, 240, 1, WHITE);
            draw_text_bitmap("Fast swarm chasers. Attack in large numbers.", 120, 265, 1, (Color){0, 255, 200, 255});
            draw_text_bitmap("Drops: Blue Mana Orbs (+15 XP).", 120, 290, 1, (Color){255, 215, 0, 255});

            DrawRectangle(100, 340, 1080, 130, (Color){35, 18, 25, 240});
            DrawRectangleLines(100, 340, 1080, 130, (Color){255, 50, 0, 255});
            draw_text_bitmap("LVL 2: RED TENTACLE MUTANTS", 120, 355, 2, (Color){255, 50, 0, 255});
            draw_text_bitmap("Crimson core entities with 10 heavy black thrashing tentacles.", 120, 390, 1, WHITE);
            draw_text_bitmap("High HP chasers. Heavy damage on contact.", 120, 415, 1, (Color){0, 255, 200, 255});
            draw_text_bitmap("Drops: Yellow Mana Orbs (+50 XP).", 120, 440, 1, (Color){255, 215, 0, 255});

            DrawRectangle(100, 490, 1080, 150, (Color){15, 15, 25, 240});
            DrawRectangleLines(100, 490, 1080, 150, (Color){0, 240, 255, 255});
            draw_text_bitmap("LVL 3: BLACK HOLE SINGULARITY BOSS", 120, 505, 2, (Color){0, 240, 255, 255});
            draw_text_bitmap("Titanic singularity with orbiting accretion rings and light beams.", 120, 540, 1, WHITE);
            draw_text_bitmap("Slow moving, huge HP pool. Gravitational force.", 120, 565, 1, (Color){0, 255, 200, 255});
            draw_text_bitmap("Drops: Multiple Yellow Mana Orbs & High XP.", 120, 590, 1, (Color){255, 215, 0, 255});
        } else {
            DrawRectangle(100, 190, 1080, 450, (Color){18, 22, 45, 240});
            DrawRectangleLines(100, 190, 1080, 450, (Color){255, 215, 0, 255});
            draw_text_bitmap("STORY & LORE:", 130, 210, 2, (Color){255, 215, 0, 255});
            draw_text_bitmap("Battle Mage trapped in infinite void overrun by tentacle hordes.", 130, 240, 1, WHITE);
            draw_text_bitmap("Use Echolocation Pulse to reveal hidden Mana Orbs and Treasure Chests.", 130, 265, 1, (Color){0, 240, 255, 255});
            draw_text_bitmap("GAME OBJECTIVES:", 130, 310, 2, (Color){0, 255, 200, 255});
            draw_text_bitmap("1. SURVIVE: Kill monsters for Blue (+15) and Yellow (+50) Mana.", 130, 340, 1, WHITE);
            draw_text_bitmap("2. LEVEL UP: Choose from 16 Cosmic Spells each level up.", 130, 365, 1, WHITE);
            draw_text_bitmap("3. CHESTS: Find chests via Echo Pulse -> unlock 5 Artifacts.", 130, 390, 1, (Color){255, 215, 0, 255});
            draw_text_bitmap("4. PORTAL: Collect 5 Golden Crystals to WIN!", 130, 415, 1, (Color){255, 0, 170, 255});
            draw_text_bitmap("CONTROLS:", 130, 460, 2, (Color){255, 0, 170, 255});
            draw_text_bitmap("WASD: Move  |  MOUSE: 3D Camera Look", 130, 490, 1, WHITE);
            draw_text_bitmap("LMB / SPACE: Charge Echolocation Pulse Knockback (Reveals Items)", 130, 515, 1, (Color){0, 240, 255, 255});
            draw_text_bitmap("RMB: Throw Decoy Grenade to attract monsters away", 130, 540, 1, (Color){255, 215, 0, 255});
            draw_text_bitmap("LEFT SHIFT: Silence Ghost Mode (Phases straight through monsters!)", 130, 565, 1, (Color){0, 255, 200, 255});
        }

        draw_neon_cursor(mpos);

        EndTextureMode();
        float zero_dist = 0.0f;
        BeginShaderMode(post_shader);
        SetShaderValue(post_shader, g->shader_locs.post_time_loc, &t, SHADER_UNIFORM_FLOAT);
        SetShaderValue(post_shader, g->shader_locs.post_res_loc, res, SHADER_UNIFORM_VEC2);
        SetShaderValue(post_shader, g->shader_locs.post_dist_loc, &zero_dist, SHADER_UNIFORM_FLOAT);
        DrawTexturePro(target.texture, (Rectangle){0, 0, 1280, -720}, (Rectangle){0, 0, 1280, 720}, (Vector2){0, 0}, 0.0f, WHITE);
        EndShaderMode();
        EndDrawing();
        return;
    }

    // ===== 3D World & Camera =====
    BeginMode3D(g->camera);
    world_draw_3d(&g->world, g->player.pos, g->world_stage);

    // 3D Spells & Cosmic Abilities
    // Projectiles (Fireball & Energy Bolt)
    for (int p = 0; p < MAX_PROJECTILES; p++) {
        if (g->projectiles[p].active) {
            Vector3 pp = (Vector3){g->projectiles[p].pos.x, g->projectiles[p].pos.y, g->projectiles[p].pos.z};
            if (g->projectiles[p].is_fireball) {
                DrawSphere(pp, 8.0f, (Color){255, 60, 0, 255});
                DrawSphere(pp, 4.0f, (Color){255, 220, 100, 255});
                DrawSphereWires(pp, 13.0f + sinf(t*15.0f)*2.0f, 10, 10, (Color){255, 0, 170, 255});
            } else {
                DrawSphere(pp, 4.0f, (Color){0, 240, 255, 255});
                DrawSphere(pp, 2.0f, WHITE);
                DrawSphereWires(pp, 7.0f, 6, 6, (Color){0, 255, 200, 220});
            }
        }
    }
    // Meteors (Skill 3)
    for (int m = 0; m < MAX_METEORS; m++) {
        if (g->meteors[m].active) {
            Vector3 mp = (Vector3){g->meteors[m].pos.x, g->meteors[m].pos.y, g->meteors[m].pos.z};
            DrawSphere(mp, 18.0f, (Color){255, 40, 0, 255});
            DrawSphere(mp, 10.0f, (Color){255, 220, 0, 255});
            DrawSphereWires(mp, 26.0f + sinf(t*12.0f)*3.0f, 12, 12, (Color){255, 0, 170, 255});
        }
    }
    // Thunderbolts (Skill 2)
    for (int th = 0; th < MAX_THUNDERS; th++) {
        if (g->thunders[th].active) {
            Vector3 gp = (Vector3){g->thunders[th].pos.x, 2.0f, g->thunders[th].pos.z};
            Vector3 sp = (Vector3){g->thunders[th].pos.x, 220.0f, g->thunders[th].pos.z};
            DrawCylinder(gp, 12.0f, 12.0f, 220.0f, 16, (Color){0, 240, 255, 160});
            DrawCylinderWires(gp, 28.0f, 28.0f, 4.0f, 24, (Color){255, 215, 0, 255});
            DrawLine3D(sp, gp, WHITE);
            DrawSphere(gp, 15.0f, (Color){0, 240, 255, 220});
        }
    }
    // Plasma Ray / Death Beam (Skill 5)
    if (g->player.ray_active) {
        Vector3 rs = (Vector3){g->player.ray_start.x, g->player.ray_start.y, g->player.ray_start.z};
        Vector3 re = (Vector3){g->player.ray_end.x, g->player.ray_end.y, g->player.ray_end.z};
        DrawLine3D(rs, re, WHITE);
        DrawCylinderWires(rs, 8.0f, 8.0f, 600.0f, 16, (Color){0, 240, 255, 240});
        DrawCylinderWires(rs, 14.0f, 14.0f, 600.0f, 12, (Color){255, 0, 170, 200});
        DrawSphere(re, 20.0f, (Color){0, 240, 255, 220});
    }
    // Lava Zone / Void Pool (Skill 6)
    for (int l = 0; l < MAX_LAVA_ZONES; l++) {
        if (g->lavas[l].active) {
            Vector3 lp = (Vector3){g->lavas[l].pos.x, 1.0f, g->lavas[l].pos.z};
            DrawCylinder(lp, g->lavas[l].radius, g->lavas[l].radius, 1.0f, 24, (Color){255, 30, 0, 200});
            DrawCylinderWires(lp, g->lavas[l].radius, g->lavas[l].radius, 1.5f, 24, (Color){255, 215, 0, 255});
            DrawCylinderWires(lp, g->lavas[l].radius * 0.6f, g->lavas[l].radius * 0.6f, 2.0f, 16, (Color){255, 0, 170, 220});
        }
    }
    // Cosmic Tsunami (Skill 7)
    for (int ts = 0; ts < MAX_TSUNAMIS; ts++) {
        if (g->tsunamis[ts].active) {
            Vector3 wp = (Vector3){g->tsunamis[ts].pos.x, 15.0f, g->tsunamis[ts].pos.z};
            float tw = g->tsunamis[ts].width * 0.5f;
            DrawCylinder(wp, tw, tw, 30.0f, 24, (Color){0, 240, 255, 140});
            DrawCylinderWires(wp, tw + 4.0f, tw + 4.0f, 32.0f, 24, (Color){255, 0, 170, 220});
        }
    }
    // Satellites (Skill 4)
    if (g->player.skill_levels[4] > 0) {
        int ns = 2 + g->player.skill_levels[4];
        for (int k = 0; k < ns; k++) {
            float sa = (k / (float)ns) * TAU + t * 3.5f;
            Vector3 sp = (Vector3){ g->player.pos.x + cosf(sa)*36.0f, 10.0f + sinf(t*5.0f+k)*3.0f, g->player.pos.z + sinf(sa)*36.0f };
            DrawSphere(sp, 5.0f, (Color){255, 215, 0, 255});
            DrawSphere(sp, 2.5f, WHITE);
            DrawSphereWires(sp, 9.0f + sinf(t*10.0f+k)*2.0f, 8, 8, (Color){0, 240, 255, 230});
        }
    }
    // Magic Circle / Rune Matrix (Skill 11)
    if (g->player.skill_levels[11] > 0) {
        Vector3 mc = (Vector3){g->player.pos.x, 0.5f, g->player.pos.z};
        DrawCylinder(mc, 55.0f, 55.0f, 0.6f, 32, (Color){255, 215, 0, 80});
        DrawCylinderWires(mc, 55.0f, 55.0f, 0.8f, 32, (Color){255, 215, 0, 240});
        DrawCylinderWires(mc, 35.0f, 35.0f, 0.8f, 24, (Color){0, 240, 255, 220});
        // 4 Vertical Plasma Pillars at circle edges!
        for (int p = 0; p < 4; p++) {
            float pa = (p / 4.0f) * TAU + t * 1.5f;
            Vector3 pp = (Vector3){g->player.pos.x + cosf(pa)*45.0f, 20.0f, g->player.pos.z + sinf(pa)*45.0f};
            DrawCylinderWires(pp, 4.0f, 4.0f, 40.0f, 12, (Color){255, 0, 170, 220});
        }
    }
    // Electric Zone (Skill 8) — Smooth Electric Plasma Field
    if (g->player.skill_levels[8] > 0) {
        Vector3 ez = (Vector3){g->player.pos.x, 10.0f, g->player.pos.z};
        float ez_r = 40.0f + g->player.skill_levels[8] * 8.0f + sinf(t * 12.0f) * 3.0f;
        Color ez_col = (Color){0, 240, 255, 120};
        DrawCylinder((Vector3){ez.x, 0.4f, ez.z}, ez_r, ez_r, 0.5f, 32, ez_col);
        DrawCylinderWires((Vector3){ez.x, 0.4f, ez.z}, ez_r + 2.0f, ez_r + 2.0f, 1.0f, 32, (Color){255, 0, 170, 220});
        // Smooth inner plasma field — NO wireframe mesh dome!
        DrawSphere(ez, ez_r * 0.70f, (Color){0, 240, 255, 30});
    }

    // Phase Shield / Ghost Mode (Left Shift & Skill 9)
    if (g->player.in_silence_mode) {
        Vector3 p_head = (Vector3){g->player.pos.x, 10.0f, g->player.pos.z};
        float shield_r = 18.0f + sinf(t * 6.0f) * 1.5f;
        // 3D Wireframe Protective Energy Field Grid Sphere surrounding camera!
        DrawSphereWires(p_head, shield_r, 12, 12, (Color){0, 255, 200, 240});
        DrawSphere(p_head, shield_r, (Color){0, 255, 200, 45});
    } else if (g->player.skill_levels[9] > 0) {
        Vector3 p_head = (Vector3){g->player.pos.x, 10.0f, g->player.pos.z};
        float shield_r = 18.0f + sinf(t * 6.0f) * 1.5f;
        DrawSphere(p_head, shield_r, (Color){0, 240, 255, 60});
        DrawCylinderWires((Vector3){p_head.x, 0.3f, p_head.z}, shield_r + 2.0f, shield_r + 2.0f, 0.5f, 24, (Color){0, 240, 255, 200});
    }

    // Decoy Grenade (Skill 10)
    if (g->decoy.active) {
        Vector3 dp = (Vector3){g->decoy.pos.x, g->decoy.pos.y, g->decoy.pos.z};
        DrawSphere(dp, 5.0f, (Color){0, 240, 255, 255});
        if (g->decoy.pulse_intensity > 0.05f) {
            Vector3 dp_f = (Vector3){dp.x, 0.3f, dp.z};
            Color d_col = (Color){0, 240, 255, (unsigned char)(220 * g->decoy.pulse_intensity)};
            DrawCylinder(dp_f, g->decoy.pulse_radius, g->decoy.pulse_radius, 0.5f, 28, d_col);
        }
    }

    // Echo Pulse 3D Wave (Space / Active Pulse) — Smooth Expanding Translucent Plasma Wave
    if (g->player.pulse_active && g->player.pulse_intensity > 0.05f) {
        Vector3 o3 = (Vector3){g->player.pulse_origin.x, 10.0f, g->player.pulse_origin.z};
        float pr = g->player.pulse_radius;
        unsigned char alpha_val = (unsigned char)(180 * g->player.pulse_intensity);
        Color wire_col = (Color){255, 0, 170, alpha_val};

        // Smooth expanding translucent neon energy dome — ZERO WIREFRAME MESH LINES!
        DrawSphere((Vector3){o3.x, 10.0f, o3.z}, pr, (Color){0, 240, 255, (unsigned char)(50 * g->player.pulse_intensity)});
        DrawCylinderWires((Vector3){o3.x, 0.3f, o3.z}, pr, pr, 0.5f, 32, wire_col);
    }

    particles_draw_3d(&g->particles);

    // 3D Enemy Models (Solid 3D entities — NO wireframe spheres enclosing the screen!)
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &g->enemies[i];
        if (!e->active) continue;
        Vector3 center = (Vector3){e->pos.x, e->pos.y + sinf(t*3.0f+i)*1.5f, e->pos.z};
        float r = e->radius_scale;

        if (e->monster_level == 3) {
            // Boss Monster: Solid dark core + glowing magenta/cyan aura
            DrawSphere(center, r, (Color){15, 5, 25, 255});
            DrawSphere(center, r * 0.7f, (Color){255, 0, 170, 240});
            DrawSphere(center, r * 0.4f, (Color){0, 240, 255, 255});
            // Orbiting plasma rings
            Vector3 pr1 = (Vector3){center.x, center.y + sinf(t*4.0f)*2.0f, center.z};
            DrawCylinder(pr1, r * 1.5f, r * 1.5f, 2.0f, 24, (Color){255, 215, 0, 200});
        } else if (e->monster_level == 2) {
            // Level 2 Mutant: Solid crimson body + tentacles
            DrawSphere(center, r, (Color){255, 0, 40, 255});
            DrawSphere(center, r * 0.5f, (Color){255, 120, 0, 255});
            for (int k = 0; k < 8; k++) {
                float ta = (k/8.0f)*TAU + t*4.0f;
                Vector3 tip = (Vector3){ center.x+cosf(ta)*(r+16.0f+sinf(t*10.0f+k)*5.0f), center.y+sinf(t*8.0f+k)*6.0f, center.z+sinf(ta)*(r+16.0f+sinf(t*10.0f+k)*5.0f) };
                DrawLine3D(center, tip, (Color){180, 0, 30, 255});
                DrawSphere(tip, 2.5f, (Color){255, 0, 170, 255});
            }
            Vector3 e1 = (Vector3){center.x-r*0.3f, center.y+r*0.3f, center.z+r*0.7f};
            Vector3 e2 = (Vector3){center.x+r*0.3f, center.y+r*0.3f, center.z+r*0.7f};
            DrawSphere(e1, 3.0f, (Color){0, 240, 255, 255}); DrawSphere(e2, 3.0f, (Color){0, 240, 255, 255});
        } else {
            // Level 1 Stalker: Pitch black void core + tentacles + red glowing eyes
            DrawSphere(center, r, (Color){10, 10, 15, 255});
            for (int k = 0; k < 6; k++) {
                float ta = (k/6.0f)*TAU + t*5.0f;
                Vector3 tip = (Vector3){ center.x+cosf(ta)*(r+10.0f+sinf(t*6.0f+k)*4.0f), center.y+sinf(t*6.0f+k)*4.0f, center.z+sinf(ta)*(r+12.0f+sinf(t*6.0f+k)*4.0f) };
                DrawLine3D(center, tip, (Color){20, 20, 30, 255});
                DrawSphere(tip, 2.0f, (Color){255, 0, 170, 255});
            }
            Vector3 e1 = (Vector3){center.x-r*0.3f, center.y+r*0.3f, center.z+r*0.7f};
            Vector3 e2 = (Vector3){center.x+r*0.3f, center.y+r*0.3f, center.z+r*0.7f};
            DrawSphere(e1, 2.0f, RED); DrawSphere(e2, 2.0f, RED);
        }
    }

    // Hand orb
    float yr = g->player.yaw * DEG2RAD;
    float pr = g->player.pitch * DEG2RAD;
    Vec3 fwd = (Vec3){cosf(pr)*sinf(yr), sinf(pr), cosf(pr)*cosf(yr)};
    Vec3 rgt = (Vec3){cosf(yr), 0.0f, -sinf(yr)};
    Vector3 orb = (Vector3){ g->player.pos.x+fwd.x*12+rgt.x*5, g->player.pos.y+fwd.y*12-4+sinf(t*4), g->player.pos.z+fwd.z*12+rgt.z*5 };
    DrawSphere(orb, 2.2f, (Color){0, 240, 255, 255});
    DrawSphereWires(orb, 3.8f+sinf(t*8)*0.8f, 6, 6, (Color){255, 215, 0, 255});

    EndMode3D();

    // Echo overlay
    if (g->player.pulse_active && g->player.pulse_intensity > 0.01f) {
        BeginShaderMode(echo_shader);
        Vector2 so = GetWorldToScreen((Vector3){g->player.pulse_origin.x, g->player.pulse_origin.y, g->player.pulse_origin.z}, g->camera);
        so.y = 720.0f - so.y;
        float nr = g->player.pulse_radius / 720.0f;
        SetShaderValue(echo_shader, g->shader_locs.echo_time_loc, &t, SHADER_UNIFORM_FLOAT);
        SetShaderValue(echo_shader, g->shader_locs.echo_res_loc, res, SHADER_UNIFORM_VEC2);
        SetShaderValue(echo_shader, g->shader_locs.echo_orig_loc, &so, SHADER_UNIFORM_VEC2);
        SetShaderValue(echo_shader, g->shader_locs.echo_rad_loc, &nr, SHADER_UNIFORM_FLOAT);
        SetShaderValue(echo_shader, g->shader_locs.echo_int_loc, &g->player.pulse_intensity, SHADER_UNIFORM_FLOAT);
        DrawRectangle(0, 0, 1280, 720, WHITE);
        EndShaderMode();
    }

    // ===== HUD & MODALS =====
    if (g->state == STATE_PLAYING || g->state == STATE_LEVELUP || g->state == STATE_ARTIFACT_MODAL) {
        if (g->state == STATE_PLAYING) {
            DrawCircle(640, 360, 3.0f, (Color){0, 240, 255, 255});
            DrawCircleLines(640, 360, 10.0f, (Color){255, 215, 0, 220});
            DrawLine(630, 360, 636, 360, (Color){0, 240, 255, 200});
            DrawLine(644, 360, 650, 360, (Color){0, 240, 255, 200});
        }

        // ===== TOP-LEFT STATS PANEL =====
        // Panel: 15,12 | w=380, h=114 — 4 rows: HP, Mana, Ghost, Pulse
        DrawRectangle(15, 12, 380, 114, (Color){8, 10, 28, 245});
        DrawRectangleLines(15, 12, 380, 114, (Color){0, 240, 255, 255});

        // Row 1: Health   label + bar
        //   label Y=20, bar Y=20, height=14
        draw_text_bitmap(T(TR_HEALTH), 23, 20, 1, (Color){0, 220, 255, 255});
        DrawRectangle(128, 20, 258, 14, (Color){30, 30, 55, 220});
        DrawRectangle(128, 20, (int)(258.0f * FAST_CLAMP(g->player.health / g->player.max_health, 0, 1)), 14, (Color){255, 60, 100, 255});
        DrawRectangleLines(128, 20, 258, 14, (Color){200, 200, 255, 200});
        char hp_str[32]; snprintf(hp_str, sizeof(hp_str), "%d/%d", (int)g->player.health, (int)g->player.max_health);
        draw_text_bitmap(hp_str, 128 + (258 - text_width(hp_str, 1)) / 2, 22, 1, WHITE);

        // Row 2: Mana XP  label + bar
        draw_text_bitmap(T(TR_MANA), 23, 40, 1, (Color){255, 215, 0, 255});
        DrawRectangle(128, 40, 258, 14, (Color){30, 30, 55, 220});
        float fr = FAST_CLAMP(g->player.mana / g->player.mana_to_next, 0, 1);
        DrawRectangle(128, 40, (int)(258 * fr), 14, (Color){255, 200, 0, 255});
        DrawRectangleLines(128, 40, 258, 14, (Color){200, 200, 255, 200});
        char xps[32]; snprintf(xps, sizeof(xps), "%d/%d", (int)g->player.mana, (int)g->player.mana_to_next);
        draw_text_bitmap(xps, 128 + (258 - text_width(xps, 1)) / 2, 42, 1, WHITE);

        // Row 3: Ghost (Silence)  label + bar
        draw_text_bitmap(T(TR_SILENCE), 23, 60, 1, (Color){0, 255, 180, 255});
        DrawRectangle(128, 60, 258, 14, (Color){30, 30, 55, 220});
        DrawRectangle(128, 60, (int)(258 * FAST_CLAMP(g->player.silence_charge / 100.0f, 0, 1)), 14, (Color){0, 220, 170, 255});
        DrawRectangleLines(128, 60, 258, 14, (Color){200, 200, 255, 200});

        // Row 4: Echo Pulse CD   label + bar
        draw_text_bitmap(T(TR_PULSE), 23, 80, 1, (Color){0, 200, 255, 255});
        DrawRectangle(128, 80, 258, 14, (Color){30, 30, 55, 220});
        float cd_ratio = FAST_CLAMP(1.0f - (g->player.pulse_cooldown / 1.8f), 0, 1);
        DrawRectangle(128, 80, (int)(258 * cd_ratio), 14, (Color){0, 210, 255, 255});
        DrawRectangleLines(128, 80, 258, 14, (Color){200, 200, 255, 200});
        if (g->player.pulse_cooldown <= 0.0f) {
            const char *rdy = T(TR_READY);
            draw_text_bitmap(rdy, 128 + (258 - text_width(rdy, 1)) / 2, 82, 1, (Color){255, 215, 0, 255});
        }

        // Row 5: HP value indicator line at bottom of panel
        draw_text_bitmap(hp_str, 23, 100, 1, (Color){255, 100, 130, 200});


        // Top-center timer & Stage indicator (Width=500px, X=405 to X=905, H=36px)
        DrawRectangle(405, 12, 500, 36, (Color){8, 10, 28, 245});
        DrawRectangleLines(405, 12, 500, 36, (Color){0, 240, 255, 255});
        int mins = (int)(g->survival_timer / 60.0f); int secs = (int)fmodf(g->survival_timer, 60.0f);
        char sb[96];
        snprintf(sb, sizeof(sb), "STAGE %d  |  TIME %02d:%02d  |  KILLS %d", g->world_stage, mins, secs, g->total_kills);
        draw_text_bitmap(sb, center_x(sb, 405, 500, 1), center_y(12, 36, 1), 1, (Color){0, 240, 255, 255});

        // Radar
        int cx = 1140, cy = 110; float mr = 75.0f, ms = mr / 450.0f;
        DrawCircle(cx, cy, mr, (Color){10, 12, 30, 245});
        DrawCircleLines(cx, cy, mr, (Color){0, 240, 255, 255});
        DrawCircleLines(cx, cy, mr*0.5f, (Color){0, 240, 255, 140});
        draw_text_bitmap("RADAR", cx-25, cy-92, 1, (Color){0, 240, 255, 255});
        float sa = t*2.5f;
        DrawLine(cx, cy, cx+(int)(cosf(sa)*mr), cy+(int)(sinf(sa)*mr), (Color){0, 255, 200, 200});
        DrawCircle(cx, cy, 3.5f, (Color){0, 255, 200, 255});

        for (int i = 0; i < g->world.count; i++) {
            WorldObject *obj = &g->world.objects[i];
            if (!obj->active || obj->pulse_visibility < 0.05f) continue;
            float dx = (obj->pos.x - g->player.pos.x)*ms, dz = (obj->pos.z - g->player.pos.z)*ms;
            if (dx*dx+dz*dz < mr*mr) {
                int mx = cx+(int)dx, my = cy+(int)dz;
                if (obj->type == OBJ_MANA_BLUE)   DrawCircle(mx, my, 2.0f, (Color){0, 240, 255, 255});
                else if (obj->type == OBJ_MANA_YELLOW) DrawCircle(mx, my, 3.0f, (Color){255, 215, 0, 255});
                else if (obj->type == OBJ_CHEST)   DrawRectangle(mx-2, my-2, 5, 5, (Color){255, 215, 0, 255});
                else if (obj->type == OBJ_PORTAL)  DrawCircle(mx, my, 4.0f, (Color){255, 0, 170, 255});
                else if (obj->type == OBJ_HEART)   DrawCircle(mx, my, 3.0f, (Color){255, 60, 100, 255});
            }
        }
        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (!g->enemies[e].active) continue;
            float dx = (g->enemies[e].pos.x - g->player.pos.x)*ms, dz = (g->enemies[e].pos.z - g->player.pos.z)*ms;
            if (dx*dx+dz*dz < mr*mr) {
                int mx = cx+(int)dx, my = cy+(int)dz;
                Color mc = (g->enemies[e].monster_level == 3) ? (Color){255, 0, 0, 255} : (Color){255, 0, 170, 220};
                DrawCircle(mx, my, (g->enemies[e].monster_level == 3) ? 4.0f : 2.0f, mc);
            }
        }

        // Artifact Modal
        if (g->state == STATE_ARTIFACT_MODAL) {
            DrawRectangle(0, 0, 1280, 720, (Color){10, 11, 26, 220});
            DrawRectangle(340, 200, 600, 320, (Color){25, 20, 5, 245});
            DrawRectangleLines(340, 200, 600, 320, (Color){255, 215, 0, 255});
            const char *at = "TREASURE CHEST OPENED!";
            draw_text_bitmap(at, center_x(at, 340, 600, 3), 240, 3, (Color){255, 215, 0, 255});
            draw_text_bitmap("RARE ARTIFACT UNLOCKED:", center_x("RARE ARTIFACT UNLOCKED:", 340, 600, 2), 300, 2, (Color){0, 240, 255, 255});
            const ArtifactInfo *art = &ARTIFACT_LIST[g->unlocked_artifact_idx];
            draw_text_bitmap(art->title, center_x(art->title, 340, 600, 3), 360, 3, (Color){255, 0, 170, 255});
            draw_text_bitmap(art->desc, center_x(art->desc, 340, 600, 2), 420, 2, (Color){0, 255, 200, 255});
            const char *cs = "SPACE / CLICK TO CONTINUE";
            draw_text_bitmap(cs, center_x(cs, 340, 600, 2), 480, 2, WHITE);
            draw_neon_cursor(mpos);
        }

        // ===== ULTRA-READABLE LEVEL UP CARDS WITH FULL NATIVE RUSSIAN TRANSLATIONS & CRISP LARGE TEXT =====
        if (g->state == STATE_LEVELUP) {
            DrawRectangle(0, 0, 1280, 720, (Color){4, 6, 16, 245});
            const char *lu = T(TR_LEVELUP);
            draw_text_bitmap(lu, center_x(lu, 0, 1280, 3), 70, 3, (Color){255, 215, 0, 255});
            const char *lh = (g->language == LANG_RU) ? "КЛИКНИ КАРТОЧКУ ИЛИ НАЖМИ [1], [2], [3]" : "CLICK CARD OR PRESS KEY [1], [2], [3]";
            draw_text_bitmap(lh, center_x(lh, 0, 1280, 2), 125, 2, (Color){0, 240, 255, 255});

            for (int c = 0; c < 3; c++) {
                int ci = g->levelup_cards[c];
                const SkillCardInfo *info = &SKILL_CARDS[ci];
                int cx2 = 160 + c * 330;
                bool h_card = (mpos.x >= cx2 && mpos.x <= cx2 + 300 && mpos.y >= 170 && mpos.y <= 650);

                const char *stitle = (g->language == LANG_RU) ? info->title_ru : info->title_en;
                const char *scat   = (g->language == LANG_RU) ? info->category_ru : info->category_en;
                const char *spros  = (g->language == LANG_RU) ? info->pros_ru : info->pros_en;
                const char *scons  = (g->language == LANG_RU) ? info->cons_ru : info->cons_en;

                // Solid Charcoal Panel (NO background blur, NO neon shadows!)
                DrawRectangle(cx2, 170, 300, 480, h_card ? (Color){28, 36, 70, 255} : (Color){14, 18, 40, 255});
                DrawRectangleLines(cx2, 170, 300, 480, h_card ? (Color){255, 215, 0, 255} : (Color){0, 240, 255, 255});
                if (h_card) DrawRectangleLines(cx2 - 2, 168, 304, 484, (Color){0, 255, 200, 255});

                // Key Tag
                char ks[32];
                snprintf(ks, sizeof(ks), (g->language == LANG_RU) ? "[ КЛАВИША %d ]" : "[ KEY %d ]", c+1);
                DrawRectangle(cx2 + 30, 190, 240, 32, (Color){25, 20, 50, 250});
                DrawRectangleLines(cx2 + 30, 190, 240, 32, h_card ? (Color){255, 215, 0, 255} : (Color){0, 240, 255, 255});
                draw_text_bitmap(ks, center_x(ks, cx2, 300, 2), 197, 2, h_card ? (Color){255, 215, 0, 255} : (Color){0, 240, 255, 255});

                // Category
                draw_text_bitmap(scat, center_x(scat, cx2, 300, 1), 235, 1, (Color){255, 0, 170, 255});

                // Title
                int tw2 = text_width(stitle, 2);
                if (tw2 <= 270) {
                    draw_text_bitmap(stitle, center_x(stitle, cx2, 300, 2), 265, 2, (Color){0, 240, 255, 255});
                } else {
                    draw_text_bitmap(stitle, center_x(stitle, cx2, 300, 1), 270, 1, (Color){0, 240, 255, 255});
                }

                DrawLine(cx2 + 20, 305, cx2 + 280, 305, (Color){0, 240, 255, 180});

                // PROS BOX - Solid dark green background + crisp bright green text
                DrawRectangle(cx2 + 15, 325, 270, 65, (Color){10, 32, 24, 250});
                DrawRectangleLines(cx2 + 15, 325, 270, 65, (Color){0, 255, 180, 255});
                draw_text_bitmap(spros, cx2 + 25, 348, 1, (Color){0, 255, 180, 255});

                // CONS / SECONDARY EFFECT BOX - Solid dark yellow background + crisp bright gold text
                DrawRectangle(cx2 + 15, 405, 270, 65, (Color){34, 30, 10, 250});
                DrawRectangleLines(cx2 + 15, 405, 270, 65, (Color){255, 220, 0, 255});
                draw_text_bitmap(scons, cx2 + 25, 428, 1, (Color){255, 220, 0, 255});

                // Level
                char ls[32];
                snprintf(ls, sizeof(ls), (g->language == LANG_RU) ? "ТЕКУЩИЙ УРОВЕНЬ: %d" : "CURRENT LVL: %d", g->player.skill_levels[ci]);
                draw_text_bitmap(ls, center_x(ls, cx2, 300, 1), 545, 1, WHITE);

                // Select Prompt
                const char *sel_txt = T(TR_CLICK_SEL);
                draw_text_bitmap(sel_txt, center_x(sel_txt, cx2, 300, 1), 595, 1, h_card ? (Color){0, 255, 200, 255} : (Color){160, 160, 200, 220});
            }

            draw_neon_cursor(mpos);
        }
    } else if (g->state == STATE_WIN) {
        const char *v1 = T(TR_VICTORY);
        draw_text_bitmap(v1, center_x(v1, 0, 1280, 5), 240, 5, (Color){255, 215, 0, 255});
        const char *v2 = T(TR_WIN_DESC);
        draw_text_bitmap(v2, center_x(v2, 0, 1280, 2), 320, 2, (Color){0, 255, 200, 255});

        char stg_info[64];
        snprintf(stg_info, sizeof(stg_info), "STAGES REACHED: %d  |  TOTAL KILLS: %d", g->world_stage, g->total_kills);
        draw_text_bitmap(stg_info, center_x(stg_info, 0, 1280, 2), 390, 2, (Color){0, 240, 255, 255});

        const char *v3 = T(TR_RESTART);
        draw_text_bitmap(v3, center_x(v3, 0, 1280, 2), 470, 2, WHITE);
        draw_neon_cursor(mpos);
    } else if (g->state == STATE_GAMEOVER) {
        const char *g1 = T(TR_GAMEOVER);
        draw_text_bitmap(g1, center_x(g1, 0, 1280, 4), 240, 4, (Color){255, 0, 170, 255});
        const char *g2 = T(TR_GO_DESC);
        draw_text_bitmap(g2, center_x(g2, 0, 1280, 2), 320, 2, (Color){180, 180, 220, 255});

        char stg_info[64];
        snprintf(stg_info, sizeof(stg_info), "REACHED STAGE: %d  |  TOTAL KILLS: %d", g->world_stage, g->total_kills);
        draw_text_bitmap(stg_info, center_x(stg_info, 0, 1280, 2), 390, 2, (Color){255, 215, 0, 255});

        const char *g3 = T(TR_RESTART);
        draw_text_bitmap(g3, center_x(g3, 0, 1280, 2), 470, 2, WHITE);
        draw_neon_cursor(mpos);
    }

    EndTextureMode();

    BeginShaderMode(post_shader);
    SetShaderValue(post_shader, g->shader_locs.post_time_loc, &t, SHADER_UNIFORM_FLOAT);
    SetShaderValue(post_shader, g->shader_locs.post_res_loc, res, SHADER_UNIFORM_VEC2);
    SetShaderValue(post_shader, g->shader_locs.post_dist_loc, &g->distortion_level, SHADER_UNIFORM_FLOAT);
    DrawTexturePro(target.texture, (Rectangle){0, 0, 1280, -720}, (Rectangle){0, 0, 1280, 720}, (Vector2){0, 0}, 0.0f, WHITE);
    EndShaderMode();

    EndDrawing();
}

void game_close(GameState *g) {
    if (!g) return;
    audio_close(&g->audio);
}
