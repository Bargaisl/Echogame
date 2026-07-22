#include "world.h"
#include "rlgl.h"
#include <string.h>

void world_init(World *w, uint32_t seed, int stage) {
    if (!w) return;
    memset(w->objects, 0, sizeof(w->objects));
    w->count = 0;
    w->seed = seed ? seed : 424242;
    w->total_crystals = 0;

    uint32_t rng = w->seed + (uint32_t)(stage * 777);

    // Stage scaling: Arena boundary grows significantly per stage
    float max_dist = 2500.0f + stage * 1500.0f;

    int num_crystals = 30;
    for (int i = 0; i < num_crystals && w->count < MAX_WORLD_OBJECTS; i++) {
        float angle = rnd_float(&rng) * TAU;
        float dist = rnd_range(&rng, 250.0f, max_dist * 0.95f);
        float y = rnd_range(&rng, 6.0f, 10.0f);
        Vec3 pos = (Vec3){cosf(angle) * dist, y, sinf(angle) * dist};
        WorldObject *obj = &w->objects[w->count++];
        obj->type = OBJ_CRYSTAL; obj->pos = pos; obj->radius = 14.0f; obj->height = 24.0f;
        obj->active = true; obj->pulse_visibility = 0.0f; w->total_crystals++;
    }

    int num_silence = 20;
    for (int i = 0; i < num_silence && w->count < MAX_WORLD_OBJECTS; i++) {
        float angle = rnd_float(&rng) * TAU;
        float dist = rnd_range(&rng, 300.0f, max_dist * 0.95f);
        float y = rnd_range(&rng, 6.0f, 10.0f);
        Vec3 pos = (Vec3){cosf(angle) * dist, y, sinf(angle) * dist};
        WorldObject *obj = &w->objects[w->count++];
        obj->type = OBJ_SILENCE_NODE; obj->pos = pos; obj->radius = 12.0f; obj->height = 12.0f;
        obj->active = true; obj->pulse_visibility = 0.0f;
    }

    int num_chests = 30;
    for (int i = 0; i < num_chests && w->count < MAX_WORLD_OBJECTS; i++) {
        float angle = rnd_float(&rng) * TAU;
        float dist = rnd_range(&rng, 180.0f, max_dist * 0.90f);
        Vec3 pos = (Vec3){cosf(angle) * dist, 6.0f, sinf(angle) * dist};
        WorldObject *obj = &w->objects[w->count++];
        obj->type = OBJ_CHEST; obj->pos = pos; obj->radius = 12.0f; obj->height = 12.0f;
        obj->active = true; obj->pulse_visibility = 0.0f;
    }

    int num_orbs = 120;
    for (int i = 0; i < num_orbs && w->count < MAX_WORLD_OBJECTS; i++) {
        float angle = rnd_float(&rng) * TAU;
        float dist = rnd_range(&rng, 100.0f, max_dist * 0.90f);
        Vec3 pos = (Vec3){cosf(angle) * dist, 6.0f, sinf(angle) * dist};
        bool is_yellow = (rnd_float(&rng) < 0.20f);
        WorldObject *obj = &w->objects[w->count++];
        obj->type = is_yellow ? OBJ_MANA_YELLOW : OBJ_MANA_BLUE;
        obj->pos = pos; obj->radius = is_yellow ? 9.0f : 6.0f; obj->height = obj->radius;
        obj->active = true; obj->pulse_visibility = 0.0f;
    }

    // Portal position: at the boundary of the stage radius
    float portal_angle = rnd_float(&rng) * TAU;
    w->portal_pos = (Vec3){cosf(portal_angle) * max_dist, 15.0f, sinf(portal_angle) * max_dist};
    WorldObject *portal_obj = &w->objects[w->count++];
    portal_obj->type = OBJ_PORTAL; portal_obj->pos = w->portal_pos;
    portal_obj->radius = 50.0f; portal_obj->height = 90.0f;
    portal_obj->active = true; portal_obj->pulse_visibility = 0.0f;

    // Spawn initial heart pickups (same frequency as blue mana orbs)
    int num_hearts = 40;
    for (int i = 0; i < num_hearts && w->count < MAX_WORLD_OBJECTS; i++) {
        float angle = rnd_float(&rng) * TAU;
        float dist = rnd_range(&rng, 100.0f, 3500.0f);
        Vec3 pos = (Vec3){cosf(angle) * dist, 6.0f, sinf(angle) * dist};
        WorldObject *obj = &w->objects[w->count++];
        obj->type = OBJ_HEART; obj->pos = pos; obj->radius = 7.0f; obj->height = 7.0f;
        obj->active = true; obj->pulse_visibility = 0.0f;
    }
}

void world_spawn_mana_orb(World *w, Vec3 pos, bool is_yellow) {
    if (!w) return;
    int slot = -1;
    for (int i = 0; i < MAX_WORLD_OBJECTS; i++) { if (!w->objects[i].active) { slot = i; break; } }
    if (slot == -1) return;
    WorldObject *obj = &w->objects[slot];
    obj->type = is_yellow ? OBJ_MANA_YELLOW : OBJ_MANA_BLUE;
    obj->pos = (Vec3){pos.x, 6.0f, pos.z};
    obj->radius = is_yellow ? 9.0f : 6.0f; obj->height = obj->radius;
    obj->active = true; obj->pulse_visibility = 1.0f;
}

void world_spawn_chest(World *w, Vec3 pos) {
    if (!w) return;
    int slot = -1;
    for (int i = 0; i < MAX_WORLD_OBJECTS; i++) { if (!w->objects[i].active) { slot = i; break; } }
    if (slot == -1) return;
    WorldObject *obj = &w->objects[slot];
    obj->type = OBJ_CHEST; obj->pos = (Vec3){pos.x, 6.0f, pos.z};
    obj->radius = 12.0f; obj->height = 12.0f; obj->active = true; obj->pulse_visibility = 1.0f;
}

void world_spawn_heart(World *w, Vec3 pos) {
    if (!w) return;
    int slot = -1;
    for (int i = 0; i < MAX_WORLD_OBJECTS; i++) { if (!w->objects[i].active) { slot = i; break; } }
    if (slot == -1) return;
    WorldObject *obj = &w->objects[slot];
    obj->type = OBJ_HEART; obj->pos = (Vec3){pos.x, 6.0f, pos.z};
    obj->radius = 7.0f; obj->height = 7.0f;
    obj->active = true; obj->pulse_visibility = 1.0f;
}

void world_update_3d(World *w, float dt, Vec3 player_pos, Vec3 pulse_origin, float pulse_radius, float pulse_intensity) {
    if (!w) return;
    for (int i = 0; i < w->count; i++) {
        WorldObject *obj = &w->objects[i];
        if (!obj->active) continue;
        if (obj->pulse_visibility > 0.0f) {
            obj->pulse_visibility -= dt * 0.25f;
            if (obj->pulse_visibility < 0.0f) obj->pulse_visibility = 0.0f;
        }
        if (pulse_intensity > 0.05f) {
            float dist = vec3_dist(obj->pos, pulse_origin);
            if (fabsf(dist - pulse_radius) < (obj->radius + 75.0f)) obj->pulse_visibility = 1.0f;
        }
        float dist_to_player = vec3_dist(obj->pos, player_pos);
        if (dist_to_player > 3800.0f && obj->type != OBJ_PORTAL) {
            float angle = rnd_float(&w->seed) * TAU;
            float spawn_dist = rnd_range(&w->seed, 1500.0f, 3200.0f);
            obj->pos = (Vec3){player_pos.x + cosf(angle) * spawn_dist, obj->pos.y, player_pos.z + sinf(angle) * spawn_dist};
            obj->pulse_visibility = 0.0f;
        }
    }
}

void world_draw_3d(World *w, Vec3 player_pos, int stage) {
    if (!w) return;
    float t = (float)GetTime();

    // Stage theme colors (1=Cyan/Purple, 2=Gold/Red, 3=Emerald/Green, 4=Crimson/Magenta, 5+=Electric Blue)
    Color main_grid_col;
    Color sub_grid_col;
    Color bh_photon_col1, bh_photon_col2;
    Color bh_disc_cols[4];

    int theme = (stage - 1) % 4;
    if (theme == 0) { // Stage 1, 5, 9... Cyber Cyan / Violet
        main_grid_col = (Color){0, 220, 255, 200}; sub_grid_col = (Color){140, 40, 200, 100};
        bh_photon_col1 = (Color){180, 80, 255, 255}; bh_photon_col2 = (Color){220, 60, 255, 250};
        bh_disc_cols[0] = (Color){160, 0, 255, 255}; bh_disc_cols[1] = (Color){200, 60, 255, 255};
        bh_disc_cols[2] = (Color){140, 120, 255, 255}; bh_disc_cols[3] = (Color){200, 180, 255, 255};
    } else if (theme == 1) { // Stage 2, 6... Neon Amber / Fire Red
        main_grid_col = (Color){255, 170, 0, 200}; sub_grid_col = (Color){220, 40, 0, 100};
        bh_photon_col1 = (Color){255, 120, 0, 255}; bh_photon_col2 = (Color){255, 40, 0, 250};
        bh_disc_cols[0] = (Color){255, 60, 0, 255}; bh_disc_cols[1] = (Color){255, 160, 0, 255};
        bh_disc_cols[2] = (Color){255, 220, 0, 255}; bh_disc_cols[3] = (Color){255, 240, 150, 255};
    } else if (theme == 2) { // Stage 3, 7... Bio Emerald / Toxic Teal
        main_grid_col = (Color){0, 255, 150, 200}; sub_grid_col = (Color){0, 140, 200, 100};
        bh_photon_col1 = (Color){0, 240, 180, 255}; bh_photon_col2 = (Color){0, 255, 100, 250};
        bh_disc_cols[0] = (Color){0, 200, 100, 255}; bh_disc_cols[1] = (Color){0, 255, 200, 255};
        bh_disc_cols[2] = (Color){100, 255, 180, 255}; bh_disc_cols[3] = (Color){200, 255, 240, 255};
    } else { // Stage 4, 8... Blood Crimson / Deep Pink
        main_grid_col = (Color){255, 0, 120, 200}; sub_grid_col = (Color){150, 0, 200, 100};
        bh_photon_col1 = (Color){255, 0, 160, 255}; bh_photon_col2 = (Color){255, 0, 80, 250};
        bh_disc_cols[0] = (Color){200, 0, 100, 255}; bh_disc_cols[1] = (Color){255, 0, 150, 255};
        bh_disc_cols[2] = (Color){255, 100, 200, 255}; bh_disc_cols[3] = (Color){255, 200, 230, 255};
    }

    // ========== 1. BLACK HOLE SINGULARITY (follows player, always visible below) ==========
    Vector3 bh = (Vector3){player_pos.x, -150.0f, player_pos.z};
    float bhr = 70.0f;

    // Event horizon core (pitch black sphere)
    DrawSphere(bh, bhr, BLACK);

    // Photon ring
    DrawCylinderWires(bh, bhr + 5.0f, bhr + 5.0f, 3.0f, 36, bh_photon_col1);
    DrawCylinderWires(bh, bhr + 10.0f + sinf(t * 4.5f) * 3.0f, bhr + 10.0f + sinf(t * 4.5f) * 3.0f, 4.0f, 36, bh_photon_col2);

    // Accretion disc
    for (int r = 0; r < 20; r++) {
        float ring_r = (bhr + 18.0f) + r * 60.0f;
        float height = 6.0f + r * 1.0f;
        float wobble = sinf(t * 2.0f + r * 0.7f) * 5.0f;

        Color rc = bh_disc_cols[r % 4];
        rc.a = (unsigned char)(245 - r * 8);

        Vector3 rp = (Vector3){bh.x, bh.y + wobble, bh.z};
        DrawCylinderWires(rp, ring_r, ring_r, height, 40, rc);
    }

    // Curved spiral wisps
    for (int i = 0; i < 56; i++) {
        float ba = (i / 56.0f) * TAU;
        float sa = ba + t * 0.3f;
        float ir = bhr + 12.0f;
        float or2 = 380.0f + sinf(t * 2.5f + i) * 140.0f;
        float ca = sa + 0.35f;

        Vector3 ws = (Vector3){
            bh.x + cosf(sa) * ir,
            bh.y + sinf(t * 3.0f + i * 0.4f) * 4.0f,
            bh.z + sinf(sa) * ir
        };
        Vector3 we = (Vector3){
            bh.x + cosf(ca) * or2,
            bh.y + sinf(t * 1.5f + i) * 14.0f,
            bh.z + sinf(ca) * or2
        };

        Color wc = bh_disc_cols[i % 4];
        wc.a = (unsigned char)(160 - (i % 4) * 20);
        DrawLine3D(ws, we, wc);
    }

    // ========== 2. FLOOR SYSTEM ==========
    float step = 60.0f;
    float cx = floorf(player_pos.x / step) * step;
    float cz = floorf(player_pos.z / step) * step;
    int render_range = 24; // High-detail view distance around player

    // SOLID DARK GLASS PANELS WITH CHAMFERED EDGES
    for (int ti = -render_range; ti <= render_range; ti++) {
        for (int tj = -render_range; tj <= render_range; tj++) {
            float tx = cx + ti * step;
            float tz = cz + tj * step;
            float dist_sq = (tx - player_pos.x) * (tx - player_pos.x) + (tz - player_pos.z) * (tz - player_pos.z);
            if (dist_sq > 1800.0f * 1800.0f) continue; // Distance cull

            float margin = 2.0f; // Gap between tiles for glowing energy seams
            float x1 = tx + margin;
            float x2 = tx + step - margin;
            float z1 = tz + margin;
            float z2 = tz + step - margin;

            // Subtle procedural panel color variance based on tile coordinates
            int pattern = (int)(fabsf(sinf(ti * 12.9898f + tj * 78.233f)) * 4.0f);
            Color panel_col;
            if (pattern == 0)      panel_col = (Color){ 8, 10, 24, 245 };
            else if (pattern == 1) panel_col = (Color){ 4, 14, 28, 245 };
            else if (pattern == 2) panel_col = (Color){12,  8, 30, 245 };
            else                   panel_col = (Color){ 6, 12, 20, 245 };

            Vector3 q1 = {x1, -0.10f, z1};
            Vector3 q2 = {x2, -0.10f, z1};
            Vector3 q3 = {x2, -0.10f, z2};
            Vector3 q4 = {x1, -0.10f, z2};
            DrawTriangle3D(q1, q2, q3, panel_col);
            DrawTriangle3D(q1, q3, q4, panel_col);

            // Inner tech circuit trace inside alternate tiles
            if (pattern == 1 || pattern == 3) {
                float ix1 = tx + step * 0.25f;
                float ix2 = tx + step * 0.75f;
                float iz1 = tz + step * 0.25f;
                float iz2 = tz + step * 0.75f;
                Color trace_col = sub_grid_col; trace_col.a = 60;
                DrawLine3D((Vector3){ix1, 0.05f, iz1}, (Vector3){ix2, 0.05f, iz1}, trace_col);
                DrawLine3D((Vector3){ix2, 0.05f, iz1}, (Vector3){ix2, 0.05f, iz2}, trace_col);
            }

            // Glowing Tile Seams (Double-layered neon bloom)
            Color seam_col = ((ti + tj) % 4 == 0) ? main_grid_col : sub_grid_col;
            seam_col.a = 180;
            DrawLine3D((Vector3){x1, 0.1f, z1}, (Vector3){x2, 0.1f, z1}, seam_col);
            DrawLine3D((Vector3){x1, 0.1f, z1}, (Vector3){x1, 0.1f, z2}, seam_col);

            // Glowing Cyber-Nodes at Tile Intersections
            if ((ti + tj) % 2 == 0) {
                Vector3 node_pos = (Vector3){tx, 0.2f, tz};
                float pulse = 0.5f + 0.5f * sinf(t * 3.0f + (ti + tj) * 0.5f);
                Color node_col = main_grid_col;
                node_col.a = (unsigned char)(140 + 115 * pulse);
                DrawSphereWires(node_pos, 2.5f + pulse * 1.5f, 6, 6, node_col);
            }
        }
    }

    // Outer Edge Black Hole Event Boundary (Giant vortex wall at stage boundary)
    float stage_max_r = 2500.0f + stage * 1500.0f;
    for (int h = 0; h < 6; h++) {
        Vector3 hp = {0.0f, h * 12.0f, 0.0f};
        DrawCylinderWires(hp, stage_max_r, stage_max_r, 10.0f, 64, (h % 2 == 0) ? (Color){255, 0, 150, 240} : (Color){0, 240, 255, 220});
    }
    // Vertical boundary pillars
    for (int b = 0; b < 32; b++) {
        float ba = (b / 32.0f) * TAU + t * 0.2f;
        Vector3 bp1 = {cosf(ba) * stage_max_r, 0.0f, sinf(ba) * stage_max_r};
        Vector3 bp2 = {cosf(ba) * stage_max_r, 120.0f, sinf(ba) * stage_max_r};
        DrawLine3D(bp1, bp2, (Color){255, 215, 0, 230});
    }

    // Dynamic floor energy pulse rings expanding from player
    for (int r = 0; r < 5; r++) {
        float pr = fmodf(t * 45.0f + r * 90.0f, 450.0f);
        unsigned char pa = (unsigned char)(190.0f * (1.0f - pr / 450.0f));
        Vector3 fp = (Vector3){player_pos.x, 0.25f, player_pos.z};
        Color pr_col = main_grid_col; pr_col.a = pa;
        DrawCylinderWires(fp, pr, pr, 0.4f, 28, pr_col);
    }

    // ========== 3. WORLD OBJECTS ==========
    for (int i = 0; i < w->count; i++) {
        WorldObject *obj = &w->objects[i];
        if (!obj->active) continue;

        float dtp = vec3_dist(obj->pos, player_pos);
        float prox = FAST_CLAMP(1.0f - (dtp / 90.0f), 0.0f, 1.0f);
        float alpha = FAST_MAX(obj->pulse_visibility, prox);
        if (alpha < 0.05f) continue;

        Vector3 p3 = (Vector3){obj->pos.x, obj->pos.y, obj->pos.z};

        switch (obj->type) {
            case OBJ_CHEST: {
                Vector3 cc = (Vector3){p3.x, p3.y + sinf(t*3+i)*1.0f, p3.z};
                unsigned char a = (unsigned char)(255*alpha);
                DrawCubeV(cc, (Vector3){14, 10, 10}, (Color){25, 20, 5, (unsigned char)(220*alpha)});
                DrawCubeWiresV(cc, (Vector3){14, 10, 10}, (Color){255, 215, 0, a});
                DrawSphereWires(cc, 9.0f + sinf(t*8)*2, 8, 8, (Color){0, 240, 255, (unsigned char)(200*alpha)});
                break;
            }
            case OBJ_MANA_BLUE: {
                Vector3 mp = (Vector3){p3.x, p3.y + sinf(t*5+i)*1.5f, p3.z};
                DrawSphere(mp, obj->radius, (Color){0, 240, 255, (unsigned char)(255*alpha)});
                DrawSphereWires(mp, obj->radius + 2.0f, 6, 6, WHITE);
                break;
            }
            case OBJ_MANA_YELLOW: {
                Vector3 mp = (Vector3){p3.x, p3.y + sinf(t*6+i)*2.0f, p3.z};
                unsigned char a = (unsigned char)(255*alpha);
                DrawSphere(mp, obj->radius, (Color){255, 215, 0, a});
                DrawSphereWires(mp, obj->radius + 3, 8, 8, WHITE);
                DrawSphereWires(mp, obj->radius + 5, 6, 6, (Color){255, 0, 170, (unsigned char)(180*alpha)});
                break;
            }
            case OBJ_CRYSTAL: {
                Vector3 cp = (Vector3){p3.x, p3.y + sinf(t*3.5f+i)*2, p3.z};
                unsigned char a = (unsigned char)(255*alpha);
                DrawSphereWires(cp, obj->radius, 6, 6, (Color){255, 215, 0, a});
                DrawSphere(cp, obj->radius * 0.45f, (Color){255, 255, 255, (unsigned char)(220*alpha)});
                DrawCylinderWires(cp, obj->radius*1.5f, obj->radius*1.5f, 1, 12, (Color){0, 240, 255, (unsigned char)(200*alpha)});
                break;
            }
            case OBJ_SILENCE_NODE: {
                Vector3 sp = (Vector3){p3.x, p3.y + cosf(t*4.5f+i)*2, p3.z};
                unsigned char a = (unsigned char)(255*alpha);
                DrawSphere(sp, obj->radius, (Color){0, 255, 200, a});
                DrawSphereWires(sp, obj->radius + 4, 8, 8, (Color){255, 255, 255, (unsigned char)(180*alpha)});
                DrawSphereWires(sp, obj->radius + 7, 6, 6, (Color){0, 240, 255, (unsigned char)(120*alpha)});
                break;
            }
            case OBJ_PORTAL: {
                Vector3 pp = (Vector3){p3.x, p3.y + sinf(t*2.5f)*4, p3.z};
                unsigned char a = (unsigned char)(255*alpha);
                DrawCylinderWires(pp, obj->radius, obj->radius, 10, 20, (Color){255, 215, 0, a});
                DrawCylinderWires(pp, obj->radius*0.7f, obj->radius*0.7f, 15, 16, (Color){255, 0, 170, a});
                DrawSphereWires(pp, obj->radius*0.5f + sinf(t*6)*4, 12, 12, (Color){255, 215, 0, a});
                DrawSphere(pp, obj->radius*0.25f, (Color){255, 255, 255, (unsigned char)(240*alpha)});
                break;
            }
            case OBJ_HEART: {
                // Floating glowing heart — red sphere with pink pulse ring
                float bob = sinf(t * 4.5f + i) * 2.0f;
                Vector3 hp = (Vector3){p3.x, p3.y + bob, p3.z};
                unsigned char a = (unsigned char)(255 * alpha);
                DrawSphere(hp, obj->radius, (Color){255, 40, 80, a});
                DrawSphereWires(hp, obj->radius + 2.5f + sinf(t * 8.0f + i) * 1.5f, 8, 8, (Color){255, 120, 160, a});
                DrawSphereWires(hp, obj->radius + 5.0f, 6, 6, (Color){255, 200, 220, (unsigned char)(140 * alpha)});
                break;
            }
            default: break;
        }
    }
}

bool world_check_wall_collision_3d(World *w, Vec3 pos, float radius) {
    return false;
}
