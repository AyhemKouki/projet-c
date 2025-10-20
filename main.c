#include "raylib.h"
#define MAX_PLATFORMS 8
#define MAX_DIAMONDS 10
#define MAX_SPIKEHEADS 3

typedef enum {
    PLAYER_IDLE,
    PLAYER_RUN,
    PLAYER_JUMP,
    PLAYER_FALL,
    PLAYER_HIT
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
    Image imgHit = LoadImage("Sprites/01-KingHuman/Hit.png");

    Image background = LoadImage("Sprites/Background/Blue.png");
    Image ground = LoadImage("Sprites/ground.png");
    Image platform = LoadImage("Sprites/platform.png");
    Image diamond = LoadImage("Sprites/Diamond.png");

    Image spikeHead = LoadImage("Sprites/enemy/idle.png");

    ImageResize(&imgIdle, imgIdle.width * 2, imgIdle.height * 2);
    ImageResize(&imgRun,  imgRun.width  * 2, imgRun.height  * 2);
    ImageResize(&imgJump, imgJump.width * 2, imgJump.height * 2);
    ImageResize(&imgFall, imgFall.width * 2, imgFall.height * 2);
    ImageResize(&imgHit, imgHit.width * 2, imgHit.height * 2);
    ImageResize(&ground , ground.width * 2 , ground.height * 2);
    ImageResize(&diamond , diamond.width * 2 , diamond.height * 2);
    ImageResize(&spikeHead , spikeHead.width * 1.5 , spikeHead.height * 1.5);

    Texture2D texIdle = LoadTextureFromImage(imgIdle);
    Texture2D texRun  = LoadTextureFromImage(imgRun);
    Texture2D texJump = LoadTextureFromImage(imgJump);
    Texture2D texFall = LoadTextureFromImage(imgFall);
    Texture2D texHit = LoadTextureFromImage(imgHit);
    Texture2D texBackground = LoadTextureFromImage(background);
    Texture2D texGround = LoadTextureFromImage(ground);
    Texture2D texPlatform = LoadTextureFromImage(platform);
    Texture2D texDiamond = LoadTextureFromImage(diamond);
    Texture2D texSpikeHead = LoadTextureFromImage(spikeHead);

    UnloadImage(imgIdle);
    UnloadImage(imgRun);
    UnloadImage(imgJump);
    UnloadImage(imgFall);
    UnloadImage(imgHit);
    UnloadImage(background);
    UnloadImage(ground);
    UnloadImage(platform);
    UnloadImage(diamond);
    UnloadImage(spikeHead);

    // Frame sizes
    const int FRAME_WIDTH = 78 * 2;
    const int FRAME_HEIGHT = texIdle.height;

    int idleFrameCount = texIdle.width / FRAME_WIDTH;
    int runFrameCount  = texRun.width  / FRAME_WIDTH;
    int jumpFrameCount = texJump.width / FRAME_WIDTH;
    int fallFrameCount = texFall.width / FRAME_WIDTH;
    int hitFrameCount  = texHit.width  / FRAME_WIDTH;

    // Player hitbox
    float hitboxOffsetX = 30;
    float hitboxOffsetY = 40;
    float hitboxWidth   = 60;
    float hitboxHeight  = 48;

    Rectangle player = {100 + hitboxOffsetX, 300 + hitboxOffsetY, hitboxWidth, hitboxHeight};
    float velocityY = 0;
    const float gravity = 0.5f;
    const float jumpForce = -12.0f;
    bool onGround = false;
    bool facingRight = true;

    int frame = 0;
    float frameSpeed = 0.15f;
    float frameTimer = 0;
    PlayerState state = PLAYER_IDLE;
    // Player hit animation state
    bool playerHit = false;
    int hitFrame = 0;
    float hitFrameTimer = 0.0f;
    float hitFrameSpeed = 0.2f;

    // Diamond animation properties
    const int DIAMOND_FRAME_WIDTH = 18 * 2; 
    const int DIAMOND_FRAME_HEIGHT = texDiamond.height;
    const int DIAMOND_FRAME_COUNT = 10;
    float diamondFrameTimer[MAX_DIAMONDS] = {0};
    float diamondFrameSpeed = 0.15f;
    int diamondFrame[MAX_DIAMONDS] = {0};

    // Spike Head properties
    Rectangle spikeHeads[MAX_SPIKEHEADS] = {
        {650, 320, 78, texSpikeHead.height},
        {1050, 520, 78, texSpikeHead.height},
        {1550, 320, 78, texSpikeHead.height}
    };
    float spikeMinY[MAX_SPIKEHEADS] = {320, 520, 320};
    float spikeAmplitude = 200; 
    float spikeSpeedDown = 6.0f;
    float spikeSpeedUp = 2.0f;
    bool spikeGoingDown[MAX_SPIKEHEADS] = {true, true, true};

    // Spikehead no longer plays its own hit animation; collisions trigger player hit animation

    // Camera
    Camera2D camera = {0};
    camera.target = (Vector2){ player.x + player.width/2, player.y + player.height/2 };
    camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Platforms - Enhanced level design
    Rectangle platforms[MAX_PLATFORMS] = {
        { 0, SCREEN_HEIGHT - 50, WORLD_WIDTH, 50 },     // Ground
        { 250, 550, 120, 20 },     // First jump
        { 500, 470, 96, 20 },      // Stair step 1
        { 700, 390, 96, 20 },      // Stair step 2
        { 900, 310, 150, 20 },     // Landing platform
        { 1150, 390, 96, 20 },     // Downward path
        { 1400, 470, 96, 20 },     // Continuing down
        { 1650, 550, 200, 20 }     // Final platform
    };

    // Diamonds - Strategic placement
    Rectangle diamonds[MAX_DIAMONDS] = {
        {280, 500, 25, 25},    // First platform
        {530, 420, 25, 25},    // Second platform  
        {730, 340, 25, 25},    // Third platform
        {950, 260, 25, 25},    // Fourth platform
        {1170, 340, 25, 25},   // Fifth platform
        {1420, 420, 25, 25},   // Sixth platform
        {1680, 500, 25, 25},   // Seventh platform
        {1800, 500, 25, 25},   // End of final platform
        {400, 300, 25, 25},    // Bonus: hard to reach
        {1600, 300, 25, 25}    // Bonus: hard to reach
    };

    int score = 0;

    while (!WindowShouldClose()) {
        // Apply gravity
        velocityY += gravity;
        player.y += velocityY;
        onGround = false;

        // Collision with platforms
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
        if (targetY > SCREEN_HEIGHT/2) targetY = SCREEN_HEIGHT/2;
        camera.target = (Vector2){ targetX, targetY };

    // Player state
    if (playerHit) state = PLAYER_HIT;
    else if (!onGround) state = (velocityY < 0) ? PLAYER_JUMP : PLAYER_FALL;
    else if (moving) state = PLAYER_RUN;
    else state = PLAYER_IDLE;

        // Animation update
        if (state != PLAYER_HIT) {
            frameTimer += frameSpeed;
            if (frameTimer >= 1.0f) { frameTimer = 0; frame++; }
        } else {
            // Update player hit frames independently; stop hit after one cycle
            hitFrameTimer += hitFrameSpeed;
            if (hitFrameTimer >= 1.0f) { hitFrameTimer = 0; hitFrame++; }
            if (hitFrame >= hitFrameCount) {
                hitFrame = 0;
                playerHit = false;
            }
        }

        int frameCount = 1;
        Texture2D currentTexture = texIdle;
        switch(state){
            case PLAYER_IDLE: frameCount = idleFrameCount; currentTexture = texIdle; break;
            case PLAYER_RUN:  frameCount = runFrameCount;  currentTexture = texRun;  break;
            case PLAYER_JUMP: frameCount = jumpFrameCount; currentTexture = texJump; break;
            case PLAYER_FALL: frameCount = fallFrameCount; currentTexture = texFall; break;
            case PLAYER_HIT:  frameCount = hitFrameCount;  currentTexture = texHit;  break;
        }
        if(state != PLAYER_HIT && frame >= frameCount) frame = 0;
        Rectangle sourceRec = { (state == PLAYER_HIT ? hitFrame : frame) * FRAME_WIDTH, 0, FRAME_WIDTH, FRAME_HEIGHT };
        if (!facingRight) sourceRec.width = -FRAME_WIDTH;

        // Diamond collision
        for (int i = 0; i < MAX_DIAMONDS; i++) {
            if (diamonds[i].width > 0 && CheckCollisionRecs(player, diamonds[i])) {
                score++;
                diamonds[i].width = 0;
                diamonds[i].height = 0;
            }
        }

        // Spike Head movement and collision
        for (int i = 0; i < MAX_SPIKEHEADS; i++) {
            if (spikeGoingDown[i]) {
                spikeHeads[i].y += spikeSpeedDown;
                if (spikeHeads[i].y >= spikeMinY[i] + spikeAmplitude)
                    spikeGoingDown[i] = false;
            } else {
                spikeHeads[i].y -= spikeSpeedUp;
                if (spikeHeads[i].y <= spikeMinY[i])
                    spikeGoingDown[i] = true;
            }

            // Player collision with spikehead triggers player hit animation
            if (CheckCollisionRecs(player, spikeHeads[i]) && !playerHit) {
                playerHit = true;
                hitFrame = 0;
                hitFrameTimer = 0.0f;
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
                    DrawTexture(texPlatform, platforms[i].x, platforms[i].y, WHITE);
                }

                // Draw ground along bottom
                for(int x = 0; x < WORLD_WIDTH; x += texGround.width) {
                    DrawTexture(texGround, x, platforms[0].y, WHITE);
                }

                // Draw spikeheads (no per-enemy hit animation)
                for (int i = 0; i < MAX_SPIKEHEADS; i++) {
                    DrawTexture(texSpikeHead, spikeHeads[i].x, spikeHeads[i].y, WHITE);
                }
                
                // Draw diamonds with animation
                for (int i = 0; i < MAX_DIAMONDS; i++) {
                    if (diamonds[i].width > 0) {
                        diamondFrameTimer[i] += diamondFrameSpeed;
                        if (diamondFrameTimer[i] >= 1.0f) {
                            diamondFrameTimer[i] = 0;
                            diamondFrame[i]++;
                            if (diamondFrame[i] >= DIAMOND_FRAME_COUNT) diamondFrame[i] = 0;
                        }

                        Rectangle sourceRecDiamond = {
                            diamondFrame[i] * DIAMOND_FRAME_WIDTH, 0, DIAMOND_FRAME_WIDTH, DIAMOND_FRAME_HEIGHT
                        };
                        DrawTextureRec(texDiamond, sourceRecDiamond, (Vector2){ diamonds[i].x -10, diamonds[i].y }, WHITE);
                    }
                }

                // Draw player
                Vector2 drawPos = { player.x - hitboxOffsetX, player.y - hitboxOffsetY };
                if (!facingRight) drawPos.x = player.x - hitboxOffsetX - player.width/2 - 5;
                DrawTextureRec(currentTexture, sourceRec, drawPos, WHITE);

            EndMode2D();

            // Draw UI
            DrawText(TextFormat("Score: %d/%d", score, MAX_DIAMONDS), 10, 40, 20, BLACK);
            DrawText("Press SPACE to jump", 10, 10, 20, BLACK);
            DrawText("Use A and D to move", 10, 70, 20, BLACK);

            // Win condition
            if (score == MAX_DIAMONDS) {
                DrawText("LEVEL COMPLETE!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 50, 40, GREEN);
                DrawText("All diamonds collected!", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2, 30, GREEN);
            }

        EndDrawing();
    }

    // Cleanup
    UnloadTexture(texIdle);
    UnloadTexture(texRun);
    UnloadTexture(texJump);
    UnloadTexture(texFall);
    UnloadTexture(texHit);
    UnloadTexture(texBackground);
    UnloadTexture(texGround);
    UnloadTexture(texPlatform);
    UnloadTexture(texDiamond);
    UnloadTexture(texSpikeHead);

    CloseWindow();
    return 0;
}