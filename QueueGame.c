#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
//#include <conio.h>
#include <time.h>
//#include <stdbool.h>

#define waitTime 4.0f
#define playingTime 10.0f

#define playingState 1
#define waitingState 2
#define overState 3

#define pontosPorAcerto 50
#define penalidadePorErro 0.5f

#define limiteEsteira 200
#define velocidadeEsteira 1
#define espacamentoMinimoSushis 40
#define espacamentoInicialSushis 70

typedef struct No {
    char item;
    int posX;
    int posY;
    struct No *proximo;
} No;

typedef struct Timer{
    float Lifetime;
}Timer;

void enfileirar(No** fila, char item, int posDesloc);
No* desenfileirar(No** fila);
void geraItens(No** fila, int dificuldade);
void checkKeyPressed(No** fila, int *pontuacao, Timer* playingTimer);
bool comparaInput(No** fila, char input);
void printFila(No** fila);
void drawSushi(No* no, int radius);
void StartTimer(Timer* timer, float lifetime);
void UpdateTimer(Timer* timer);
bool TimerDone(Timer* timer);
void updateSushiPositions(No** fila, int velocidade);

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
                updateSushiPositions(&fila, dificuldade-3);
                checkKeyPressed(&fila, &pontuacao, &playingTimer);
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
            
            
            //Texto
            DrawText(TextFormat("Pontos: %d", pontuacao),20, 20, 20, LIGHTGRAY);
            /*DrawText("Tempo: 10s",550, 20, 20, LIGHTGRAY);
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
                enfileirar(fila, 'A', i*espacamentoInicialSushis);
                break;
            case 1:
                enfileirar(fila, 'S', i*espacamentoInicialSushis);
                break;
            case 2:
                enfileirar(fila, 'D', i*espacamentoInicialSushis);
                break;
            case 3:
                enfileirar(fila, 'J', i*espacamentoInicialSushis);
                break;
            case 4:
                enfileirar(fila, 'K', i*espacamentoInicialSushis);
                break;
            case 5:
                enfileirar(fila, 'L', i*espacamentoInicialSushis);
                break;
        }
    }
}

void checkKeyPressed(No** fila, int *pontuacao, Timer* playingTimer){
    int input = GetKeyPressed();
    
    if(input == KEY_A && comparaInput(fila, 'A')) *pontuacao += pontosPorAcerto;
    else if(input == KEY_S && comparaInput(fila, 'S')) *pontuacao+= pontosPorAcerto;
    else if(input == KEY_D && comparaInput(fila, 'D')) *pontuacao+= pontosPorAcerto;
    else if(input == KEY_J && comparaInput(fila, 'J')) *pontuacao+= pontosPorAcerto;
    else if(input == KEY_K && comparaInput(fila, 'K')) *pontuacao+= pontosPorAcerto;
    else if(input == KEY_L && comparaInput(fila, 'L')) *pontuacao+= pontosPorAcerto;
    else if(input != 0) playingTimer->Lifetime -= penalidadePorErro;
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

void updateSushiPositions(No** fila, int velocidade){
    No* aux = *fila;
    if(*fila){        
        if(aux->posX >= limiteEsteira) aux->posX -= velocidadeEsteira + velocidade/4;
        while(aux->proximo){        
            if(aux->proximo->posX >= (aux->posX + espacamentoMinimoSushis)) aux->proximo->posX -= velocidadeEsteira + velocidade/4;
            aux = aux->proximo;
        }
    }
}
