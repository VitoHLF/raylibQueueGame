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

#define pontosPorAcerto 50
#define penalidadePorErro 0.5f

#define limiteEsteira 395
#define velocidadeEsteira 2
#define espacamentoMinimoSushis 80
#define espacamentoInicialSushis 110

typedef struct No {
    char item;
    int posX;
    int posY;
    struct No *proximo;
} No;

typedef struct Event{
    bool isActive;
    char eventType;
} Event;

typedef struct Timer{
    float Lifetime;
}Timer;

void enfileirar(No** fila, char item, int posDesloc);
No* desenfileirar(No** fila);
void geraItens(No** fila, int dificuldade);
void checkKeyPressed(No** fila, int *pontuacao, Timer* playingTimer, Event* eventoPopup);
bool comparaInput(No** fila, char input, Event* eventoPopup);
void printFila(No** fila, Texture2D* sushis);
void drawSushi(No* no, int radius, Texture2D* sushis);
void StartTimer(Timer* timer, float lifetime);
void UpdateTimer(Timer* timer);
bool TimerDone(Timer* timer);
void updateSushiPositions(No** fila, int velocidade);
void createRandomEvent(Event* evento);
void drawLabels(Texture2D* labels);


int main() 
{
    const int screenWidth = 1366;
    const int screenHeight = 720;
    
    No* fila = NULL;
    int dificuldade = 4, gameState = waitingState, pontuacao = 0, randomEventProc;
    Timer playingTimer = {0}, waitingTimer = {0}, randomEventTimer = {0}, createEventTimer = {0};
    float displayTimer;
    Event eventoPopup;
    Texture2D sushis[6], labels[6];
    
    
    //--------------------------------------------------------------------------------------
    // SETUP DO JOGO
    srand(time(NULL));
    
    StartTimer(&waitingTimer, waitTime);
    eventoPopup.isActive = false;
    eventoPopup.eventType = 'E';
    
    //--------------------------------------------------------------------------------------
    // INICIALIZANDO TELA
    InitWindow(screenWidth, screenHeight, "Jogo de Filas"); 

    //--------------------------------------------------------------------------------------
    // TEXTURAS    
    sushis[0] = LoadTexture("resources/a06.png");
    sushis[1] = LoadTexture("resources/a05.png");
    sushis[2] = LoadTexture("resources/a04.png");
    sushis[3] = LoadTexture("resources/a03.png");
    sushis[4] = LoadTexture("resources/a02.png");
    sushis[5] = LoadTexture("resources/a01.png");
    
    labels[0] = LoadTexture("resources/labelA.png");
    labels[1] = LoadTexture("resources/labelS.png");
    labels[2] = LoadTexture("resources/labelD.png");
    labels[3] = LoadTexture("resources/labelJ.png");
    labels[4] = LoadTexture("resources/labelK.png");
    labels[5] = LoadTexture("resources/labelL.png");
    
    Texture2D textoPontos = LoadTexture("resources/Pontuacao.png");
    Texture2D textoPedido = LoadTexture("resources/PedidosCompletos.png");
    
    Texture2D backgroundDark = LoadTexture("resources/fundo.png");
    Texture2D backgroundLight = LoadTexture("resources/fundo2.png");
    Texture2D backgroundImage = LoadTexture("resources/backgroundImage1.png");
    Texture2D textBackground = LoadTexture("resources/backgroundPontos.png");

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
                StartTimer(&createEventTimer, 1.0f);
            } 
        }
        
        //----------------------------------------------------------------------------------
        // JOGANDO
        if(gameState == playingState){
            UpdateTimer(&playingTimer);
            UpdateTimer(&createEventTimer);
            UpdateTimer(&randomEventTimer);            
            
            displayTimer = playingTimer.Lifetime;
            
            if(TimerDone(&createEventTimer) && !eventoPopup.isActive){
                randomEventProc = rand()%100;
                if(randomEventProc<10) {
                    StartTimer(&randomEventTimer, 2.0f);
                    createRandomEvent(&eventoPopup);
                }
                else{
                    StartTimer(&createEventTimer, 1.0f);
                }
            }
            
            if(TimerDone(&randomEventTimer)){
                eventoPopup.isActive = false;
            }
            
            if(TimerDone(&playingTimer)){
                gameState = overState;
            }
            else if(fila){
                updateSushiPositions(&fila, dificuldade-3);
                checkKeyPressed(&fila, &pontuacao, &playingTimer, &eventoPopup);
            }else{
                gameState = waitingState;
                StartTimer(&waitingTimer, waitTime);
            }
        }
        
        //----------------------------------------------------------------------------------
        // DESENHANDO NA TELA
        BeginDrawing();
            
            ClearBackground(RAYWHITE);
            
            //----------------------------------------------------------------------------------
            // ESPERA ENTRE NIVEIS
            if(gameState == waitingState){
                DrawTexture(backgroundDark, 0, 0, WHITE);
                DrawTexture(backgroundLight, 20, 20, WHITE);
                DrawTexture(backgroundImage, 327, 16, WHITE);
                DrawTexture(textBackground, 45, 199, WHITE);
                
                DrawText(TextFormat("%.2fs", displayTimer), 1034, 51, 50, GREEN);
                DrawTexture(textoPontos, 100, 245, WHITE);
                DrawText(TextFormat("%d", pontuacao),60, 295, 50, LIGHTGRAY);
                DrawTexture(textoPontos, 100, 245, WHITE);
                drawLabels(&labels);
            }
            
            
            //----------------------------------------------------------------------------------
            // JOGANDO
            if(gameState == playingState){
                DrawTexture(backgroundDark, 0, 0, WHITE);
                DrawTexture(backgroundLight, 20, 20, WHITE);
                DrawTexture(backgroundImage, 327, 16, WHITE);
                DrawTexture(textBackground, 45, 199, WHITE);
                
                DrawText(TextFormat("%.2fs", displayTimer), 1034, 51, 50, RED);
                DrawTexture(textoPontos, 100, 245, WHITE);
                DrawText(TextFormat("%d", pontuacao),60, 295, 50, LIGHTGRAY);
                drawLabels(&labels);
                
                if(eventoPopup.isActive) {
                    DrawCircle(640, 300, 18, YELLOW);  
                    DrawText(TextFormat("%c", eventoPopup.eventType),637, 297, 20, LIGHTGRAY);
                }
                if(gameState == playingState) printFila(&fila, &sushis);
                
                
            }
            
            //----------------------------------------------------------------------------------
            // GAME OVER
            if(gameState == overState) DrawText("GAME OVER!", 350, 320, 100, RED);
            
            
            //Texto
            
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
            DrawText("L", 760, 570, 20, LIGHTGRAY); */
            
            //Popups
            
            /*
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
            
            
        EndDrawing();
    }
    
    
    UnloadTexture(sushis[0]);
    UnloadTexture(sushis[1]);
    UnloadTexture(sushis[2]);
    UnloadTexture(sushis[3]);
    UnloadTexture(sushis[4]);
    UnloadTexture(sushis[5]);
    
    UnloadTexture(labels[0]);
    UnloadTexture(labels[1]);
    UnloadTexture(labels[2]);
    UnloadTexture(labels[3]);
    UnloadTexture(labels[4]);
    UnloadTexture(labels[5]);
    
    UnloadTexture(textoPontos);
    UnloadTexture(textoPedido);
    
    UnloadTexture(backgroundDark);
    UnloadTexture(backgroundLight);
    UnloadTexture(backgroundImage);
    UnloadTexture(textBackground);
    
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
        novo->posX = 1220 + posDesloc;
        novo->posY = 505;
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

void checkKeyPressed(No** fila, int *pontuacao, Timer* playingTimer, Event* eventoPopup){
    int input = GetKeyPressed();
    
    if(eventoPopup->isActive && input == KEY_E && comparaInput(fila, 'E', eventoPopup)){
        playingTimer->Lifetime += 3;
        eventoPopup->isActive = false;
        *pontuacao += 3 * pontosPorAcerto;
    }
    else if(eventoPopup->isActive && input == KEY_U && comparaInput(fila, 'U', eventoPopup)) {
        playingTimer->Lifetime += 3;
        eventoPopup->isActive = false;
        *pontuacao += 3 * pontosPorAcerto;
    }
    else if(input == KEY_A && comparaInput(fila, 'A', eventoPopup)) *pontuacao += pontosPorAcerto;
    else if(input == KEY_S && comparaInput(fila, 'S', eventoPopup)) *pontuacao += pontosPorAcerto;
    else if(input == KEY_D && comparaInput(fila, 'D', eventoPopup)) *pontuacao += pontosPorAcerto;
    else if(input == KEY_J && comparaInput(fila, 'J', eventoPopup)) *pontuacao += pontosPorAcerto;
    else if(input == KEY_K && comparaInput(fila, 'K', eventoPopup)) *pontuacao += pontosPorAcerto;
    else if(input == KEY_L && comparaInput(fila, 'L', eventoPopup)) *pontuacao += pontosPorAcerto;
    
    else if(input != 0) playingTimer->Lifetime -= penalidadePorErro;
}

bool comparaInput(No** fila, char input, Event* eventoPopup){
    
    if(eventoPopup->isActive && eventoPopup->eventType == input) return true;    
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

void printFila(No** fila, Texture2D* sushis){
    No* aux = *fila;
    if(*fila){
        drawSushi(aux, 18, sushis);
        while(aux->proximo){
            drawSushi(aux->proximo, 18, sushis);
            aux = aux->proximo;
        }
    }
}

void drawSushi(No* no, int radius, Texture2D* sushis){
    
    switch(no->item){
        case 'A':
            DrawTextureEx(sushis[0], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            //DrawCircle(no->posX, no->posY, 18, GREEN);
            //DrawText("A", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'S':
            DrawTextureEx(sushis[1], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            //DrawCircle(no->posX , no->posY , 18, GREEN);
            //DrawText("S", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'D':
            DrawTextureEx(sushis[2], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            //DrawCircle(no->posX , no->posY , 18, GREEN);
            //DrawText("D", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'J':
            DrawTextureEx(sushis[3], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            //DrawCircle(no->posX , no->posY , 18, GREEN);
            //DrawText("J", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'K':
            DrawTextureEx(sushis[4], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            //DrawCircle(no->posX , no->posY , 18, GREEN);
            //DrawText("K", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);
            break;
        case 'L':
            DrawTextureEx(sushis[5], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            //DrawCircle(no->posX , no->posY , 18, GREEN);
            //DrawText("L", no->posX - fontCorrect, no->posY - fontCorrect, 18, BLACK);        
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

void createRandomEvent(Event* evento){
    int eventType = rand()%2;
    
    switch(eventType){
        case 0:
            evento->isActive = true;
            evento->eventType = 'E';
            break;
        case 1:
            evento->isActive = true;
            evento->eventType = 'U';
            break;
    }
}

void drawLabels(Texture2D* labels){
    DrawTexture(labels[0], 1234, 43, WHITE);
    DrawTexture(labels[1], 1234, 156, WHITE);
    DrawTexture(labels[2], 1234, 269, WHITE);
    DrawTexture(labels[3], 1234, 382, WHITE);
    DrawTexture(labels[4], 1234, 495, WHITE);
    DrawTexture(labels[5], 1234, 608, WHITE);
}