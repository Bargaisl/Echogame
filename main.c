#include "raylib.h"
#include "game.h"
#include "shaders.h"
#include <stdio.h>

void log_debug(const char *msg) {
    FILE *f = fopen("debug.log", "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

int main(void) {
    remove("debug.log");
    log_debug("Starting main...");

    log_debug("Calling SetConfigFlags...");
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);

    log_debug("Calling InitWindow...");
    InitWindow(1280, 720, "ECHO 3D - Synthwave Neon Edition");

    if (!IsWindowReady()) {
        log_debug("InitWindow failed!");
        return 1;
    }
    log_debug("Window initialized successfully.");

    SetTargetFPS(60);

    log_debug("Compiling bg_shader...");
    Shader bg_shader = LoadShaderFromMemory(NULL, FS_BACKGROUND);
    log_debug("Compiling echo_shader...");
    Shader echo_shader = LoadShaderFromMemory(NULL, FS_ECHO);
    log_debug("Compiling post_shader...");
    Shader post_shader = LoadShaderFromMemory(NULL, FS_POST);
    log_debug("Shaders compiled successfully.");

    log_debug("Creating render target...");
    RenderTexture2D target = LoadRenderTexture(1280, 720);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    log_debug("Render target created.");

    log_debug("Initializing game engine...");
    static GameState game;
    game_init(&game);
    game_init_shader_locs(&game, bg_shader, echo_shader, post_shader);
    log_debug("Game engine initialized.");

    log_debug("Entering main loop...");
    int frame_count = 0;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;

        game_update(&game, dt);
        game_draw(&game, bg_shader, echo_shader, post_shader, target);

        if (frame_count < 10) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Main loop frame %d", frame_count);
            log_debug(buf);
            frame_count++;
        }
    }

    log_debug("Exiting main loop, cleaning up...");
    game_close(&game);
    UnloadRenderTexture(target);
    UnloadShader(bg_shader);
    UnloadShader(echo_shader);
    UnloadShader(post_shader);
    CloseWindow();

    log_debug("Clean exit.");
    return 0;
}
