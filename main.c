#include "raylib.h"
#define MAX_PLATFORMS 5

int main(void) {
    int SCREEN_WIDTH = 1000;
    int SCREEN_HEIGHT = 700;
    int WORLD_WIDTH = 3000;
    // Initialize window and OpenGL context
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shooter Game");
    // Set target FPS (maximum)
    SetTargetFPS(60);

    // Player setup
    Rectangle player = {100, 300, 40, 40};
    float velocityY = 0;
    const float gravity = 0.5f;
    const float jumpForce = -10.0f;
    bool onGround = false;

    // camera setup
    Camera2D camera = {0};
    camera.target = (Vector2){ player.x + player.width/2, player.y + player.height/2 };
    camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Platform setup
    Rectangle platforms[MAX_PLATFORMS] = {
        // Ground {pos x , pos y, width, height}
        { 200, 575, 150, 20 },
        { 450, 500, 150, 20 },
        { 700, 425, 150, 20 },
        { 900, 350, 150, 20 }
    };

    while (!WindowShouldClose() /*Detect window close button or ESC key */ ) {
        // Apply gravity
        velocityY += gravity;
        player.y += velocityY;

        // === Collision detection with platforms ===
        for (int i = 0; i < MAX_PLATFORMS; i++) {
            if (CheckCollisionRecs(player, platforms[i])) {
                // Player is falling onto the platform
                if (velocityY > 0 && player.y + player.height - velocityY <= platforms[i].y) {
                    player.y = platforms[i].y - player.height;
                    velocityY = 0;
                    onGround = true;
                }
            }
        }

        // Ground collision
        if (player.y + player.height >= SCREEN_HEIGHT - 50) {
            player.y = SCREEN_HEIGHT - 50 - player.height;
            velocityY = 0;
            onGround = true;
        }

        // Jump
        if (onGround && IsKeyPressed(KEY_SPACE)) {
            velocityY = jumpForce;
            onGround = false;
        }

        // Horizontal movement
        if (IsKeyDown(KEY_D)) player.x += 5;
        if (IsKeyDown(KEY_A)) player.x -= 5;

        // Keep player inside world bounds
        if (player.x < 0) player.x = 0;
        if (player.x + player.width > WORLD_WIDTH) player.x = WORLD_WIDTH - player.width;

        // === Update camera target to follow player WITH BOUNDS ===
        float targetX = player.x + player.width/2;
        float targetY = player.y + player.height/2;
        
        // Limit the camera to not exceed the world boundaries
        float minCameraX = SCREEN_WIDTH/2.0f;
        float maxCameraX = WORLD_WIDTH - SCREEN_WIDTH/2.0f;
        
        // Apply the limits to the camera position
        if (targetX < minCameraX) targetX = minCameraX;
        if (targetX > maxCameraX) targetX = maxCameraX;

        float screenCenterY = SCREEN_HEIGHT/2.0f;
        float playerScreenY = player.y + player.height/2; // Y position of the player's center
        
        // If the player is above the screen center, follow in Y
        // Otherwise, keep the Y position fixed (the player falls freely)
        if (playerScreenY < screenCenterY) {
            // The player is above the center, follow in Y
            targetY = player.y + player.height/2;
        } else {
            // The player is below the center, keep the camera fixed in Y
            targetY = screenCenterY;
        }
        
        camera.target = (Vector2){ targetX, targetY };

        // Setup canvas to start drawing
        BeginDrawing();
            // Set background color
            ClearBackground(SKYBLUE);

            BeginMode2D(camera);
                // Draw player
                DrawRectangleRec(player, RED);

                // Draw ground
                DrawRectangle(0, SCREEN_HEIGHT - 50, WORLD_WIDTH, 50, DARKGREEN);

                // Draw platforms
                for (int i = 0; i < MAX_PLATFORMS; i++) {
                    DrawRectangleRec(platforms[i], ORANGE);
                }
            EndMode2D();

            DrawText("Press SPACE to jump", 10, 10, 20, BLACK);
        // End canvas drawing
        EndDrawing();
    }

    // Close window and unload OpenGL context
    CloseWindow();
    return 0;
}