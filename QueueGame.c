#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
//#include <conio.h>
#include <time.h>
//#include <stdbool.h>

#define waitTime 3.0f
#define playingTime 10.0f

#define mainMenuState 0
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
void drawPopup(char eventType, Texture2D* popups);


int main() 
{
    const int screenWidth = 1366;
    const int screenHeight = 720;
    
    No* fila = NULL;
    int dificuldade, gameState = mainMenuState, pontuacao = 0, randomEventProc, pedidosCompletos = 0;
    Timer playingTimer = {0}, waitingTimer = {0}, randomEventTimer = {0}, createEventTimer = {0};
    float displayTimer;
    Event eventoPopup;
    Texture2D sushis[6], popups[2];
    Vector2 pontoMouse;
    bool exitWindow = false;
    
    //--------------------------------------------------------------------------------------
    // SETUP DO JOGO
    srand(time(NULL));
    
    
    
    //--------------------------------------------------------------------------------------
    // INICIALIZANDO TELA
    InitWindow(screenWidth, screenHeight, "Jogo de Filas"); 

    //--------------------------------------------------------------------------------------
    // TEXTURAS    
    sushis[0] = LoadTexture("resources/sushis/a06.png");
    sushis[1] = LoadTexture("resources/sushis/a05.png");
    sushis[2] = LoadTexture("resources/sushis/a04.png");
    sushis[3] = LoadTexture("resources/sushis/a03.png");
    sushis[4] = LoadTexture("resources/sushis/a02.png");
    sushis[5] = LoadTexture("resources/sushis/a01.png");
    
    popups[0] = LoadTexture("resources/baloes/balao1.png");
    popups[1] = LoadTexture("resources/baloes/balao3.png");
        
    Texture2D background = LoadTexture("resources/elementosDeTela/fundoGeral.png");
    Texture2D backgroundImage = LoadTexture("resources/elementosDeTela/jogandoFundo.png");
    Texture2D backgroundEspera = LoadTexture("resources/elementosDeTela/esperaFundo.png");
    Texture2D guiaInputs = LoadTexture("resources/elementosDeTela/guiaInputs.png");
    
    Texture2D bckgMenu = LoadTexture("resources/elementosDeTela/menuPrincipal.png");
    Texture2D botaoIniciar = LoadTexture("resources/botoes/iniciarStandard.png");
    Texture2D botaoIniciarMouseOver = LoadTexture("resources/botoes/iniciarMouseOver.png");
    Texture2D botaoSair = LoadTexture("resources/botoes/sairStandard.png");
    Texture2D botaoSairMouseOver = LoadTexture("resources/botoes/sairMouseOver.png");
    Texture2D botaoReiniciar = LoadTexture("resources/botoes/reiniciarStandard.png");
    Texture2D botaoReiniciarMouseOver = LoadTexture("resources/botoes/reiniciarMouseOver.png");

    SetTargetFPS(60);               
    //--------------------------------------------------------------------------------------    
    // LOOP DE JOGO
    while (!exitWindow)    // Detect window close button or ESC key
    {
        if(IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()) exitWindow = true;
        //----------------------------------------------------------------------------------
        // MENU PRINCIPAL
        if(gameState == mainMenuState){
            pontoMouse = GetMousePosition();
            
            if(CheckCollisionPointRec(pontoMouse, (Rectangle){533, 453, 300, 81}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { //BOTÃO INICIAR
                gameState = waitingState;
                StartTimer(&waitingTimer, waitTime);
                pontuacao = 0;
                pedidosCompletos = 0;
                eventoPopup.isActive = false;
                eventoPopup.eventType = 'E';
                dificuldade = 4;
                
            }
            
            if(CheckCollisionPointRec(pontoMouse, (Rectangle){533, 552, 300, 81}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { // BOTÃO SAIR
                exitWindow = true;
            }
        }
        
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
                pedidosCompletos++;
                StartTimer(&waitingTimer, waitTime);
            }
        }
        
        //----------------------------------------------------------------------------------
        // GAME OVER
        
        if(gameState == overState){
            pontoMouse = GetMousePosition();
            if(CheckCollisionPointRec(pontoMouse, (Rectangle){633, 400, 300, 81}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                gameState = mainMenuState;
            }
            
            if(CheckCollisionPointRec(pontoMouse, (Rectangle){633, 500, 300, 81}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                exitWindow = true;
            }
        }
        
        //----------------------------------------------------------------------------------
        // DESENHANDO NA TELA
        BeginDrawing();
            
            ClearBackground(RAYWHITE);
            
            //----------------------------------------------------------------------------------
            // MENU PRINCIPAL
            if(gameState == mainMenuState){
                DrawTexture(bckgMenu, -4, 0, WHITE);
                
                if(CheckCollisionPointRec(pontoMouse, (Rectangle){533, 453, 300, 81})) DrawTexture(botaoIniciarMouseOver, 533, 453, WHITE);
                else DrawTexture(botaoIniciar, 533, 453, WHITE);
                
                if(CheckCollisionPointRec(pontoMouse, (Rectangle){533, 552, 300, 81})) DrawTexture(botaoSairMouseOver, 533, 552, WHITE);
                else DrawTexture(botaoSair, 533, 552, WHITE);
            }
            
            //----------------------------------------------------------------------------------
            // ESPERA ENTRE NIVEIS
            if(gameState == waitingState){
                DrawTexture(background, -4, 0, WHITE);
                DrawTexture(backgroundEspera, 322, 12, WHITE);
                
                DrawText(TextFormat("%.2fs", displayTimer), 1034, 51, 50, GREEN);
                DrawText(TextFormat("%d", pontuacao),60, 295, 50, BLACK);
                DrawText(TextFormat("%d", pedidosCompletos),60, 561, 50, BLACK);
                DrawTexture(guiaInputs, 1219, 0, WHITE);
            }
            
            
            //----------------------------------------------------------------------------------
            // JOGANDO
            if(gameState == playingState){
                DrawTexture(background, -4, 0, WHITE);
                DrawTexture(backgroundImage, 327, 16, WHITE);
                
                DrawText(TextFormat("%.2fs", displayTimer), 1034, 51, 50, RED);
                DrawText(TextFormat("%d", pontuacao),60, 295, 50, BLACK);
                DrawText(TextFormat("%d", pedidosCompletos),60, 561, 50, BLACK);
                
                if(eventoPopup.isActive) {
                    drawPopup(eventoPopup.eventType, popups);
                }
                if(gameState == playingState) printFila(&fila, sushis);
                
                DrawTexture(guiaInputs, 1219, 0, WHITE);
                
                
            }
            
            //----------------------------------------------------------------------------------
            // GAME OVER
            if(gameState == overState) {
                DrawTexture(background, -4, 0, WHITE);
                DrawTexture(backgroundEspera, 322, 12, WHITE);
                
                DrawText(TextFormat("%d", pontuacao),60, 295, 50, BLACK);
                DrawText(TextFormat("%d", pedidosCompletos),60, 561, 50, BLACK);
                                
                DrawText("GAME OVER!", 450, 150, 100, RED);
                
                if(CheckCollisionPointRec(pontoMouse, (Rectangle){633, 400, 300, 81})) DrawTexture(botaoReiniciarMouseOver, 633, 400, WHITE);
                else DrawTexture(botaoReiniciar, 633, 400, WHITE);
                
                if(CheckCollisionPointRec(pontoMouse, (Rectangle){633, 500, 300, 81})) DrawTexture(botaoSairMouseOver, 633, 500, WHITE);
                else DrawTexture(botaoSair, 633, 500, WHITE);
            }
            
            
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
    }
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
            break;
        case 'S':
            DrawTextureEx(sushis[1], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            break;
        case 'D':
            DrawTextureEx(sushis[2], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            break;
        case 'J':
            DrawTextureEx(sushis[3], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            break;
        case 'K':
            DrawTextureEx(sushis[4], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
            break;
        case 'L':
            DrawTextureEx(sushis[5], (Vector2){no->posX, no->posY}, 0, 1, WHITE);
    }
}

// Inicia um timer com uma duração especificada
void StartTimer(Timer* timer, float lifetime){
    if (timer != NULL)
        timer->Lifetime = lifetime;
}

// Atualiza o timer com o tempo de frame
void UpdateTimer(Timer* timer){
    if (timer != NULL && timer->Lifetime > 0)
        timer->Lifetime -= GetFrameTime();
}

// Verifica se um timer terminou
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

void drawPopup(char eventType, Texture2D* popups){
    if(eventType == 'E'){
        DrawTexture(popups[0], 683, 45, WHITE);
    }
    if(eventType == 'U'){
        DrawTexture(popups[1], 683, 45, WHITE);
    }
}