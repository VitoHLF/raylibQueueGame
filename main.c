#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
//#include <conio.h>
#include <time.h>
//#include <stdbool.h>

#define waitTime 3.0f
#define playingTime 10.0f

#define playingState 1
#define waitingState 2
#define overState 3

typedef struct No {
    char item;
    int posX;
    int posY;
    struct No *proximo;
} No;

typedef struct Timer{
    float Lifetime;
}Timer;

void enfileirar(No **fila, char item, int posDesloc);
No* desenfileirar(No **fila);
void geraItens(No** fila, int dificuldade);
bool comparaInput(No** fila, char input);
void printFila(No** fila);
void drawSushi(No* no, int radius);
void checkKeyPressed(No** fila);
void StartTimer(Timer* timer, float lifetime);
void UpdateTimer(Timer* timer);
bool TimerDone(Timer* timer);

int main() 
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    No* fila = NULL;
    int dificuldade = 4, gameState = waitingState, pontuacao = 0;
    Timer playingTimer = {0}, waitingTimer = {0};
    float displayTimer;
    
    //--------------------------------------------------------------------------------------
    // SETUP DO JOGO
    srand(time(NULL));
    
    StartTimer(&waitingTimer, waitTime);
    
    //--------------------------------------------------------------------------------------
    // INICIALIZANDO TELA
    InitWindow(screenWidth, screenHeight, "Jogo de Filas");   

    SetTargetFPS(60);               
    //--------------------------------------------------------------------------------------    
    // LOOP DE JOGO
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        //----------------------------------------------------------------------------------
        // ESPERA ENTRE NIVEIS
        if(gameState == waitingState){
            UpdateTimer(&waitingTimer);
            displayTimer = waitingTimer.Lifetime;
            if(TimerDone(&waitingTimer)){
                dificuldade++;
                geraItens(&fila, dificuldade);
                gameState = playingState;
                StartTimer(&playingTimer, playingTime);
            } 
            else{
                
            }
        }
        
        //----------------------------------------------------------------------------------
        // JOGANDO
        if(gameState == playingState){
            UpdateTimer(&playingTimer);
            displayTimer = playingTimer.Lifetime;
            
            if(TimerDone(&playingTimer)){
                gameState = overState;
            }
            else if(fila){
                checkKeyPressed(&fila);
            }else{
                gameState = waitingState;
                StartTimer(&waitingTimer, waitTime);
            }
        }
        
        //----------------------------------------------------------------------------------
        // DESENHANDO NA TELA
        BeginDrawing();
            
            ClearBackground(RAYWHITE);
            
            if(gameState == waitingState) DrawText(TextFormat("Proximos pedidos em %.2fs", displayTimer), 300, 100, 50, GREEN);
            else if(gameState == playingState) DrawText(TextFormat("Tempo: %.2fs", displayTimer), 500, 100, 50, RED);
            
            /*
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
            */
            //Items na Esteira
            if(gameState == playingState) printFila(&fila);
            if(gameState == overState) DrawText("GAME OVER!", 350, 320, 100, RED);
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void enfileirar(No **fila, char item, int posDesloc){
    No *aux, *novo = malloc(sizeof(No));
    if(novo){
        novo->item = item;
        novo->posX = 500 + posDesloc;
        novo->posY = 435;
        novo->proximo = NULL;
        if(*fila == NULL)
            *fila = novo;
        else{
            aux = *fila;
            while (aux->proximo)
                aux = aux->proximo;
            aux->proximo = novo;
        }
    }else
        printf("\nErro ao alocar memoria.\n");
}

No* desenfileirar(No **fila){
    No *remover= NULL;

    if(*fila){
        remover = *fila;
        *fila = remover->proximo;
    }
    return remover;
}

void geraItens(No** fila, int dificuldade){
    for(int i = 0; i<dificuldade; i++){
        switch(rand()%6){
            case 0:
                enfileirar(fila, 'A', i*50);
                break;
            case 1:
                enfileirar(fila, 'S', i*50);
                break;
            case 2:
                enfileirar(fila, 'D', i*50);
                break;
            case 3:
                enfileirar(fila, 'J', i*50);
                break;
            case 4:
                enfileirar(fila, 'K', i*50);
                break;
            case 5:
                enfileirar(fila, 'L', i*50);
                break;
        }
    }
}

bool comparaInput(No** fila, char input){
    if(*fila){
        No *remover, *aux = *fila;
        if(input == aux->item){
            remover = desenfileirar(fila);
            free(remover);
            return true;
        }
    }
    return false;
}

void printFila(No** fila){
    No* aux = *fila;
    if(*fila){
        drawSushi(aux, 18);
        while(aux->proximo){
            drawSushi(aux->proximo, 18);
            aux = aux->proximo;
        }
    }
}

void drawSushi(No* no, int radius){
    int fontCorrect = 5;
    
    switch(no->item){
        case 'A':
            DrawCircle(no->posX, no->posY, 18, GREEN);
            DrawText("A", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'S':
            DrawCircle(no->posX , no->posY , 18, GREEN);
            DrawText("S", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'D':
            DrawCircle(no->posX , no->posY , 18, GREEN);
            DrawText("D", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'J':
            DrawCircle(no->posX , no->posY , 18, GREEN);
            DrawText("J", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'K':
            DrawCircle(no->posX , no->posY , 18, GREEN);
            DrawText("K", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'L':
            DrawCircle(no->posX , no->posY , 18, GREEN);
            DrawText("L", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);        
    }
}

void checkKeyPressed(No** fila){
    if(IsKeyPressed(KEY_A)) comparaInput(fila, 'A');
    else if(IsKeyPressed(KEY_S)) comparaInput(fila, 'S');
    else if(IsKeyPressed(KEY_D)) comparaInput(fila, 'D');
    else if(IsKeyPressed(KEY_J)) comparaInput(fila, 'J');
    else if(IsKeyPressed(KEY_K)) comparaInput(fila, 'K');
    else if(IsKeyPressed(KEY_L)) comparaInput(fila, 'L');
    
}

// start or restart a timer with a specific lifetime
void StartTimer(Timer* timer, float lifetime){
    if (timer != NULL)
        timer->Lifetime = lifetime;
}

// update a timer with the current frame time
void UpdateTimer(Timer* timer){
    // subtract this frame from the timer if it's not allready expired
    if (timer != NULL && timer->Lifetime > 0)
        timer->Lifetime -= GetFrameTime();
}

// check if a timer is done.
bool TimerDone(Timer* timer){
    if (timer != NULL)
        return timer->Lifetime <= 0;

	return false;
}
