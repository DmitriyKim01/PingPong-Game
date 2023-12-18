#include <iostream>
#include <raylib.h>

using namespace std;

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

class Ball {
    protected : 
        void LimitMovement () {
            if (y + radius >= GetScreenHeight() || y - radius <= 0) {
                acceleration_y *= -1;
            }
            if (x + radius >= GetScreenWidth() || x - radius <= 0) {
                acceleration_x *= -1;
                isDefeated = true;
                ResetBall();
            }
        }
    public:
        float x;
        float y;
        int acceleration_x;
        int acceleration_y;
        int radius;
        bool isDefeated;

        Ball(float x, float y, int radius, int acceleration){
            this->x = x;
            this->y = y;
            this->radius = radius;
            this->acceleration_x = acceleration;
            this->acceleration_y = acceleration;
        }

        void Draw(){
            DrawCircle(x, y, radius, Yellow);
        }

        void Move(){        
            x += acceleration_x;
            y += acceleration_y;
            LimitMovement();
        }

        void ResetBall() {
            x = GetScreenWidth()/2;
            y = GetScreenHeight()/2;

            int speed_choices[2] = {-1, 1};
            acceleration_y *= speed_choices[GetRandomValue(0,1)];
        }
};
class Paddle {
    public:
        float y;
        float x;
        int speed;
        float width;
        float height;
    
    Paddle(float x, float y, float width, float height, int speed){
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->speed = speed;
    }

    void Draw(){
        DrawRectangle(x, y, width, height, WHITE);
    }

    void Move(){
        if (IsKeyDown(KEY_W) && y > 0) {
            y -= speed;
        }
        if (IsKeyDown(KEY_S) && y + height < GetScreenHeight()){
            y += speed;
        }
    }
};

class AIPaddle: public Paddle{
    public:
        AIPaddle(float x, float y, float width, float height, int speed): Paddle(x, y, width, height, speed * 3){};
    
        void Move(float ball_y) {
            if (y + height/2 > ball_y && y > 0) {
                y -= speed;
            }
            if (y + height/2 <= ball_y && y + height < GetScreenHeight()) {
                y += speed;
            }
        }
};

int main () {
    // Screen settings
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 800;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ping Pong");
    SetTargetFPS(165);

    // Ball settings
    const int BALL_RADIUS = 15;
    const int BALL_ACCELERATION = 7;
    Ball ball = Ball(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, BALL_RADIUS, BALL_ACCELERATION);

    // Player settings
    const int PLAYER_HEIGHT = 120;
    const int PLAYER_WIDTH = 25;
    const int PLAYER_MARGIN = 10;
    const int PLAYER_SPEED = 4;
    Paddle player = Paddle(PLAYER_MARGIN,SCREEN_HEIGHT/2-(PLAYER_HEIGHT/2+PLAYER_MARGIN), PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_SPEED);

    // AI settings
    AIPaddle aiPlayer = AIPaddle(SCREEN_WIDTH-(PLAYER_WIDTH+PLAYER_MARGIN), SCREEN_HEIGHT/2-PLAYER_HEIGHT/2, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_SPEED);

    // UI settings
    int score = 0;
    ball.isDefeated = true;

    // Audio settings
    InitAudioDevice();
    Sound pong_sound = LoadSound("../resources/pong.wav");
    Music music = LoadMusicStream("../resources/BeatThee.mp3"); // Load music file
    PlayMusicStream(music); // Start playing music

    while (WindowShouldClose() == false){
        BeginDrawing();
        ClearBackground(Dark_Green);
        DrawRectangle(SCREEN_WIDTH/2, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Green);
        DrawCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 150, Light_Green);
        DrawLine(SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, WHITE);

        UpdateMusicStream(music);

        if (ball.isDefeated) {
            DrawText(TextFormat("%i", score), (SCREEN_WIDTH - MeasureText(TextFormat("%i", score), 80))/2, 10, 80, WHITE);
            DrawText("Press enter to restart?", (SCREEN_WIDTH - MeasureText("Press enter to restart?", 50))/2, SCREEN_HEIGHT/2, 50, WHITE);
            if (IsKeyDown(KEY_ENTER)) {
                score = 0;
                ball.isDefeated = false;
            }
        }
        else {
            if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height})) {
                ball.acceleration_x *= -1;
                ball.x = player.x + player.width + ball.radius; // Move the ball outside the player paddle
                score += 1;
                PlaySound(pong_sound);
            }
            if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{aiPlayer.x, aiPlayer.y, aiPlayer.width, aiPlayer.height})) {
                ball.acceleration_x *= -1;
                ball.x = aiPlayer.x - ball.radius; // Move the ball outside the AI paddle
                PlaySound(pong_sound);
            }
            ball.Move();
            player.Move();
            aiPlayer.Move(ball.y);

            player.Draw();
            ball.Draw();
            aiPlayer.Draw();
            DrawText(TextFormat("%i", score), (SCREEN_WIDTH - MeasureText(TextFormat("%i", score), 80))/2, 10, 80, WHITE);
        }
        EndDrawing();
    }
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
    
