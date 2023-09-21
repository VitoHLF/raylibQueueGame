#include "raylib.h"

int main() 
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "raylib");   

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
            
            ClearBackground(RAYWHITE);
            //Texto
            DrawText("Pontos: 1550",20, 20, 20, LIGHTGRAY);
            DrawText("Tempo: 10s",550, 20, 20, LIGHTGRAY);
            DrawText("Completos: 5 pedidos",1000, 20, 20, LIGHTGRAY);
            
            //Personagens
            DrawRectangle(410, 200, 100, 200, BLACK);
            DrawRectangle(780, 200, 100, 200, BLACK);
            
            //Esteira
            DrawRectangle(0, 400, 1280, 70, LIGHTGRAY);
            
            //Guia de inputs
            DrawCircle(510, 550, 18, RED);
            DrawCircle(560, 550, 18, RED);
            DrawCircle(610, 550, 18, RED);
            DrawCircle(660, 550, 18, RED);
            DrawCircle(710, 550, 18, RED);
            DrawCircle(760, 550, 18, RED);
            
            DrawText("A", 510, 570, 20, LIGHTGRAY);
            DrawText("S", 560, 570, 20, LIGHTGRAY);
            DrawText("D", 610, 570, 20, LIGHTGRAY);
            DrawText("J", 660, 570, 20, LIGHTGRAY);
            DrawText("K", 710, 570, 20, LIGHTGRAY);
            DrawText("L", 760, 570, 20, LIGHTGRAY);
            
            //Popups
            DrawCircle(410, 150, 18, YELLOW);
            DrawCircle(460, 150, 18, YELLOW);
            DrawCircle(510, 150, 18, YELLOW);            
            DrawCircle(780, 150, 18, YELLOW);
            DrawCircle(830, 150, 18, YELLOW);
            DrawCircle(880, 150, 18, YELLOW);
            
            DrawText("Q", 410, 110, 20, LIGHTGRAY);
            DrawText("W", 460, 110, 20, LIGHTGRAY);
            DrawText("E", 510, 110, 20, LIGHTGRAY);
            DrawText("U", 780, 110, 20, LIGHTGRAY);
            DrawText("I", 830, 110, 20, LIGHTGRAY);
            DrawText("O", 880, 110, 20, LIGHTGRAY);
            
            //Items na Esteira
            DrawCircle(500,435,18,GREEN);
            DrawCircle(600,435,18,GREEN);
            DrawCircle(700,435,18,GREEN);
            DrawCircle(800,435,18,GREEN);
            
        
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
