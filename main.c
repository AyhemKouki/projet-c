#include "raylib.h"
#define MAX_PLATFORMS 5

typedef enum {
    PLAYER_IDLE,
    PLAYER_RUN,
    PLAYER_JUMP,
    PLAYER_FALL
} PlayerState;

int main(void) {
    const int SCREEN_WIDTH = 1000;
    const int SCREEN_HEIGHT = 700;
    const int WORLD_WIDTH = 3000;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Platformer Game");
    SetTargetFPS(60);

    // Load and resize all sprite sheets
    Image imgIdle = LoadImage("Sprites/01-KingHuman/idle.png");
    Image imgRun  = LoadImage("Sprites/01-KingHuman/run.png");
    Image imgJump = LoadImage("Sprites/01-KingHuman/jump.png");
    Image imgFall = LoadImage("Sprites/01-KingHuman/fall.png");

    // Double their size
    ImageResize(&imgIdle, imgIdle.width * 2, imgIdle.height * 2);
    ImageResize(&imgRun,  imgRun.width  * 2, imgRun.height  * 2);
    ImageResize(&imgJump, imgJump.width * 2, imgJump.height * 2);
    ImageResize(&imgFall, imgFall.width * 2, imgFall.height * 2);

    // Convert back to textures
    Texture2D texIdle = LoadTextureFromImage(imgIdle);
    Texture2D texRun  = LoadTextureFromImage(imgRun);
    Texture2D texJump = LoadTextureFromImage(imgJump);
    Texture2D texFall = LoadTextureFromImage(imgFall);

    //  unload the images after converting
    UnloadImage(imgIdle);
    UnloadImage(imgRun);
    UnloadImage(imgJump);
    UnloadImage(imgFall);

    // Frame sizes (already doubled)
    const int FRAME_WIDTH = 78 * 2;
    const int FRAME_HEIGHT = texIdle.height;

    int idleFrameCount = texIdle.width / FRAME_WIDTH;
    int runFrameCount  = texRun.width  / FRAME_WIDTH;
    int jumpFrameCount = texJump.width / FRAME_WIDTH;
    int fallFrameCount = texFall.width / FRAME_WIDTH;

    // Adjust the player hitbox to match the visible character
    float hitboxOffsetX = 30;   // fine-tune horizontally
    float hitboxOffsetY = 40;   // fine-tune vertically
    float hitboxWidth   = 60;   // visible body width
    float hitboxHeight  = 50;   // visible body height

    // Player setup (same size as sprite frame)
    Rectangle player = {100+hitboxOffsetX, 300+hitboxOffsetY, hitboxWidth, hitboxHeight};
    float velocityY = 0;
    const float gravity = 0.5f;
    const float jumpForce = -10.0f;
    bool onGround = false;
    bool facingRight = true;

    int frame = 0;
    float frameSpeed = 0.15f;
    float frameTimer = 0;
    PlayerState state = PLAYER_IDLE;

    // Camera
    Camera2D camera = {0};
    camera.target = (Vector2){ player.x + player.width/2, player.y + player.height/2 };
    camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Platforms
    Rectangle platforms[MAX_PLATFORMS] = {
        { 200, 575, 150, 20 },
        { 450, 500, 150, 20 },
        { 700, 425, 150, 20 },
        { 900, 350, 150, 20 },
        { 0, SCREEN_HEIGHT - 50, WORLD_WIDTH, 50 }
    };

    while (!WindowShouldClose()) {
        // Apply gravity
        velocityY += gravity;
        player.y += velocityY;
        onGround = false;

        // Collision
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            if (CheckCollisionRecs(player, platforms[i])) {
                if (velocityY > 0 && player.y + player.height - velocityY <= platforms[i].y) {
                    player.y = platforms[i].y - player.height;
                    velocityY = 0;
                    onGround = true;
                }
            }
        }

        // Jump
        if (onGround && IsKeyPressed(KEY_SPACE)) {
            velocityY = jumpForce;
            onGround = false;
        }

        // Movement
        bool moving = false;
        if (IsKeyDown(KEY_D)) {
            player.x += 5;
            facingRight = true;
            moving = true;
        }
        if (IsKeyDown(KEY_A)) {
            player.x -= 5;
            facingRight = false;
            moving = true;
        }

        // World bounds
        if (player.x < 0) player.x = 0;
        if (player.x + player.width > WORLD_WIDTH) player.x = WORLD_WIDTH - player.width;

        // Camera bounds
        float targetX = player.x + player.width/2;
        float targetY = player.y + player.height/2;

        float minCameraX = SCREEN_WIDTH / 2.0f;
        float maxCameraX = WORLD_WIDTH - SCREEN_WIDTH / 2.0f;

        if (targetX < minCameraX) targetX = minCameraX;
        if (targetX > maxCameraX) targetX = maxCameraX;

        float screenCenterY = SCREEN_HEIGHT / 2.0f;
        float playerScreenY = player.y + player.height / 2;

        if (playerScreenY < screenCenterY)
            targetY = player.y + player.height / 2;
        else
            targetY = screenCenterY;

        camera.target = (Vector2){ targetX, targetY };

        // Player state
        if (!onGround) {
            state = (velocityY < 0) ? PLAYER_JUMP : PLAYER_FALL;
        } else if (moving) {
            state = PLAYER_RUN;
        } else {
            state = PLAYER_IDLE;
        }

        // Animation update
        frameTimer += frameSpeed;
        if (frameTimer >= 1.0f) {
            frameTimer = 0;
            frame++;
        }

        int frameCount = 1;
        Texture2D currentTexture = texIdle;
        switch (state) {
            case PLAYER_IDLE: frameCount = idleFrameCount; currentTexture = texIdle; break;
            case PLAYER_RUN:  frameCount = runFrameCount;  currentTexture = texRun;  break;
            case PLAYER_JUMP: frameCount = jumpFrameCount; currentTexture = texJump; break;
            case PLAYER_FALL: frameCount = fallFrameCount; currentTexture = texFall; break;
        }
        if (frame >= frameCount) frame = 0;

        Rectangle sourceRec = { frame * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT };
        if (!facingRight) sourceRec.width = -FRAME_WIDTH;

        BeginDrawing();
            ClearBackground(SKYBLUE);
            BeginMode2D(camera);

                for (int i = 0; i < MAX_PLATFORMS; i++) {
                    DrawRectangleRec(platforms[i], ORANGE);
                    DrawRectangleLinesEx(platforms[i], 2, BLACK);
                }

                // Draw sprite (perfectly aligned with hitbox)
                DrawTextureRec(currentTexture, sourceRec, (Vector2){ player.x - hitboxOffsetX, player.y - hitboxOffsetY }, WHITE);

                // Draw hitbox
                DrawRectangleLinesEx(player, 2, RED);

            EndMode2D();

            DrawText("Press SPACE to jump", 10, 10, 20, BLACK);
        EndDrawing();
    }

    // Cleanup
    UnloadTexture(texIdle);
    UnloadTexture(texRun);
    UnloadTexture(texJump);
    UnloadTexture(texFall);

    CloseWindow();
    return 0;
}
