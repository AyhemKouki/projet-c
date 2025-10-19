#include "raylib.h"
#define MAX_PLATFORMS 5
#define MAX_DIAMONDS 5

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

    // Load and resize sprite sheets
    Image imgIdle = LoadImage("Sprites/01-KingHuman/idle.png");
    Image imgRun  = LoadImage("Sprites/01-KingHuman/run.png");
    Image imgJump = LoadImage("Sprites/01-KingHuman/jump.png");
    Image imgFall = LoadImage("Sprites/01-KingHuman/fall.png");

    Image background = LoadImage("Sprites/Background/Blue.png");
    Image ground = LoadImage("Sprites/ground.png");
    Image platform = LoadImage("Sprites/platform.png");
    Image diamond = LoadImage("Sprites/Diamond.png");

    ImageResize(&imgIdle, imgIdle.width * 2, imgIdle.height * 2);
    ImageResize(&imgRun,  imgRun.width  * 2, imgRun.height  * 2);
    ImageResize(&imgJump, imgJump.width * 2, imgJump.height * 2);
    ImageResize(&imgFall, imgFall.width * 2, imgFall.height * 2);
    ImageResize(&ground , ground.width * 2 , ground.height * 2);
    ImageResize(&diamond , diamond.width * 2 , diamond.height * 2);


    Texture2D texIdle = LoadTextureFromImage(imgIdle);
    Texture2D texRun  = LoadTextureFromImage(imgRun);
    Texture2D texJump = LoadTextureFromImage(imgJump);
    Texture2D texFall = LoadTextureFromImage(imgFall);
    Texture2D texBackground = LoadTextureFromImage(background);
    Texture2D texGround = LoadTextureFromImage(ground);
    Texture2D texPlatform = LoadTextureFromImage(platform);
    Texture2D texDiamond = LoadTextureFromImage(diamond);

    UnloadImage(imgIdle);
    UnloadImage(imgRun);
    UnloadImage(imgJump);
    UnloadImage(imgFall);
    UnloadImage(background);
    UnloadImage(ground);
    UnloadImage(platform);
    UnloadImage(diamond);

    // Frame sizes
    const int FRAME_WIDTH = 78 * 2;
    const int FRAME_HEIGHT = texIdle.height;

    int idleFrameCount = texIdle.width / FRAME_WIDTH;
    int runFrameCount  = texRun.width  / FRAME_WIDTH;
    int jumpFrameCount = texJump.width / FRAME_WIDTH;
    int fallFrameCount = texFall.width / FRAME_WIDTH;

    // Player hitbox
    float hitboxOffsetX = 30;
    float hitboxOffsetY = 40;
    float hitboxWidth   = 60;
    float hitboxHeight  = 48;

    Rectangle player = {100 + hitboxOffsetX, 300 + hitboxOffsetY, hitboxWidth, hitboxHeight};
    float velocityY = 0;
    const float gravity = 0.5f;
    const float jumpForce = -10.0f;
    bool onGround = false;
    bool facingRight = true;

    int frame = 0;
    float frameSpeed = 0.15f;
    float frameTimer = 0;
    PlayerState state = PLAYER_IDLE;

    //diamond animation properties
    const int DIAMOND_FRAME_WIDTH = 18 * 2; 
    const int DIAMOND_FRAME_HEIGHT = texDiamond.height;
    const int DIAMOND_FRAME_COUNT = 10; // sprite sheet has 10 frames
    float diamondFrameTimer[MAX_DIAMONDS] = {0}; // separate timer for each diamond
    float diamondFrameSpeed = 0.15f; // speed of animation
    int diamondFrame[MAX_DIAMONDS] = {0}; // current frame index for each diamond


    // Camera
    Camera2D camera = {0};
    camera.target = (Vector2){ player.x + player.width/2, player.y + player.height/2 };
    camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Platforms
    Rectangle platforms[MAX_PLATFORMS] = {
        { 200, 575, 96, 20 },
        { 450, 500, 96, 20 },
        { 700, 425, 96, 20 },
        { 900, 350, 96, 20 },
        { 0, SCREEN_HEIGHT - 50, WORLD_WIDTH, 50 }
    };

    // Diamonds
    Rectangle diamonds[MAX_DIAMONDS] = {
        {250, 520, 25, 25},
        {500, 450, 25, 25},
        {750, 370, 25, 25},
        {950, 300, 25, 25},
        {1200, 550, 25, 25}
    };

    int score = 0;

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
        if (IsKeyDown(KEY_D)) { player.x += 5; facingRight = true; moving = true; }
        if (IsKeyDown(KEY_A)) { player.x -= 5; facingRight = false; moving = true; }

        // World bounds
        if (player.x < 0) player.x = 0;
        if (player.x + player.width > WORLD_WIDTH) player.x = WORLD_WIDTH - player.width;

        // Camera bounds
        float targetX = player.x + player.width/2;
        float targetY = player.y + player.height/2;
        if (targetX < SCREEN_WIDTH/2) targetX = SCREEN_WIDTH/2;
        if (targetX > WORLD_WIDTH - SCREEN_WIDTH/2) targetX = WORLD_WIDTH - SCREEN_WIDTH/2;
        if (targetY < SCREEN_HEIGHT/2) targetY = SCREEN_HEIGHT/2;
        if (targetY > SCREEN_HEIGHT/2) targetY = SCREEN_HEIGHT/2; // optional vertical lock
        camera.target = (Vector2){ targetX, targetY };

        // Player state
        if (!onGround) state = (velocityY < 0) ? PLAYER_JUMP : PLAYER_FALL;
        else if (moving) state = PLAYER_RUN;
        else state = PLAYER_IDLE;

        // Animation update
        frameTimer += frameSpeed;
        if (frameTimer >= 1.0f) { frameTimer = 0; frame++; }

        int frameCount = 1;
        Texture2D currentTexture = texIdle;
        switch(state){
            case PLAYER_IDLE: frameCount = idleFrameCount; currentTexture = texIdle; break;
            case PLAYER_RUN:  frameCount = runFrameCount;  currentTexture = texRun;  break;
            case PLAYER_JUMP: frameCount = jumpFrameCount; currentTexture = texJump; break;
            case PLAYER_FALL: frameCount = fallFrameCount; currentTexture = texFall; break;
        }
        if(frame >= frameCount) frame = 0;

        Rectangle sourceRec = { frame * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT };
        if (!facingRight) sourceRec.width = -FRAME_WIDTH;

        // Diamond collision
        for (int i = 0; i < MAX_DIAMONDS; i++) {
            if (diamonds[i].width > 0 && CheckCollisionRecs(player, diamonds[i])) {
                score++;               // Increase score
                diamonds[i].width = 0; // Remove diamond (simple way)
                diamonds[i].height = 0;
            }
        }


        BeginDrawing();
            ClearBackground(SKYBLUE);
            BeginMode2D(camera);

                // Draw tiled background
                for (int x = 0; x < WORLD_WIDTH; x += texBackground.width) {
                    for (int y = 0; y < SCREEN_HEIGHT; y += texBackground.height) {
                        DrawTexture(texBackground, x, y, WHITE);
                    }
                }

                // Draw platforms
                for (int i = 0; i < MAX_PLATFORMS; i++) {
                    
                    DrawTexture(texPlatform ,platforms[i].x , platforms[i].y , WHITE );
                    DrawRectangleLinesEx(platforms[i], 2, BLACK);
                }

                // Draw ground along bottom of screen (or at platform level)
                for(int x = 0; x < WORLD_WIDTH; x += texGround.width) {
                    DrawTexture(texGround, x, platforms[MAX_PLATFORMS-1].y, WHITE);
                }
                
                // Draw diamonds with animation
                for (int i = 0; i < MAX_DIAMONDS; i++) {
                    if (diamonds[i].width > 0) {
                        // Update animation timer
                        diamondFrameTimer[i] += diamondFrameSpeed;
                        if (diamondFrameTimer[i] >= 1.0f) {
                            diamondFrameTimer[i] = 0;
                            diamondFrame[i]++;
                            if (diamondFrame[i] >= DIAMOND_FRAME_COUNT) diamondFrame[i] = 0;
                        }

                        // Draw diamond
                        Rectangle sourceRecDiamond = {
                            diamondFrame[i] * DIAMOND_FRAME_WIDTH, 0, DIAMOND_FRAME_WIDTH, DIAMOND_FRAME_HEIGHT
                        };
                        DrawTextureRec(texDiamond, sourceRecDiamond, (Vector2){ diamonds[i].x -10, diamonds[i].y }, WHITE);
                        // Draw hitbox
                        DrawRectangleLinesEx(diamonds[i], 2, RED);
                    }
                }


                // Draw player
                Vector2 drawPos = { player.x - hitboxOffsetX, player.y - hitboxOffsetY };
                if (!facingRight) drawPos.x = player.x - hitboxOffsetX - player.width/2 - 5;
                DrawTextureRec(currentTexture, sourceRec, drawPos, WHITE);

                // Draw hitbox
                DrawRectangleLinesEx(player, 2, RED);

            EndMode2D();

            // Draw score on screen
            DrawText(TextFormat("Score: %d", score), 10, 40, 20, BLACK);
            DrawText("Press SPACE to jump", 10, 10, 20, BLACK);

        EndDrawing();
    }

    // Cleanup
    UnloadTexture(texIdle);
    UnloadTexture(texRun);
    UnloadTexture(texJump);
    UnloadTexture(texFall);
    UnloadTexture(texBackground);
    UnloadTexture(texGround);
    UnloadTexture(texPlatform);
    UnloadTexture(texDiamond);

    CloseWindow();
    return 0;
}
