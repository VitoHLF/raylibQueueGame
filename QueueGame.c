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

#define animationLapse 2
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
void flushFila(No** fila);

int main() 
{
    const int screenWidth = 1366;
    const int screenHeight = 720;
    
    No* fila = NULL;
    int dificuldade, gameState = mainMenuState, pontuacao = 0, randomEventProc, pedidosCompletos = 0;
    Timer playingTimer = {0}, waitingTimer = {0}, randomEventTimer = {0}, createEventTimer = {0}, animationTimer = {0};
    float displayTimer;
    Event eventoPopup;
    Texture2D sushis[6], popups[2], playingAnimFrames[4];
    Vector2 pontoMouse;
    bool exitWindow = false;
    unsigned long currentFrame = 0;
    
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
    
    playingAnimFrames[0] = LoadTexture("resources/personagens/personagensFrame1.png");
    playingAnimFrames[1] = LoadTexture("resources/personagens/personagensFrame2.png");
    playingAnimFrames[2] = LoadTexture("resources/personagens/personagensFrame3.png");
    playingAnimFrames[3] = LoadTexture("resources/personagens/personagensFrame4.png");
    
        
    Texture2D background = LoadTexture("resources/elementosDeTela/fundoGeral.png");
    Texture2D backgroundImage = LoadTexture("resources/elementosDeTela/jogandoFundo.png");
    Texture2D balcao = LoadTexture("resources/elementosDeTela/balcao.png");
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
                flushFila(&fila);
                gameState = waitingState;
                StartTimer(&waitingTimer, waitTime);
                StartTimer(&animationTimer, animationLapse);
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
            if(TimerDone(&waitingTimer)){ // verifica se o tempo de interludio acabou e muda o estado do jogo para jogando
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
            UpdateTimer(&animationTimer);
            
            displayTimer = playingTimer.Lifetime;
            
            if(TimerDone(&createEventTimer) && !eventoPopup.isActive){ // gera um evento aleatorio de popup 
                randomEventProc = rand()%100;
                if(randomEventProc<10) {
                    StartTimer(&randomEventTimer, 2.0f);
                    createRandomEvent(&eventoPopup);
                }
                else{
                    StartTimer(&createEventTimer, 1.0f);
                }
            }
            
            if(TimerDone(&animationTimer)){
                currentFrame++;
                StartTimer(&animationTimer, animationLapse);
            }
            
            if(TimerDone(&randomEventTimer)){ // determina se um evento esta ativo
                eventoPopup.isActive = false;
            }
            
            if(TimerDone(&playingTimer)){ // muda o jogo para a tela de fim de jogo caso o tempo zerar
                gameState = overState;
            }
            else if(fila){ // gerenciador de inputs e atualizações de posição dos nós 
                updateSushiPositions(&fila, dificuldade-3);
                checkKeyPressed(&fila, &pontuacao, &playingTimer, &eventoPopup);
            }else{ // se a fila estiver vazia o jogo sobe de nivel
                gameState = waitingState;
                pedidosCompletos++;
                StartTimer(&waitingTimer, waitTime);
            }
        }
        
        //----------------------------------------------------------------------------------
        // GAME OVER
        
        if(gameState == overState){
            pontoMouse = GetMousePosition();
            if(CheckCollisionPointRec(pontoMouse, (Rectangle){633, 400, 300, 81}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {  // botão de retorno ao menu principal
                gameState = mainMenuState;
            }
            
            if(CheckCollisionPointRec(pontoMouse, (Rectangle){633, 500, 300, 81}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { // botão de encerrar jogo
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
                
                //botão iniciar
                if(CheckCollisionPointRec(pontoMouse, (Rectangle){533, 453, 300, 81})) DrawTexture(botaoIniciarMouseOver, 533, 453, WHITE);
                else DrawTexture(botaoIniciar, 533, 453, WHITE);
                //botão sair
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
                
                switch(currentFrame%4){
                    case 0:
                        DrawTextureEx(playingAnimFrames[currentFrame%4], (Vector2){400, 110}, 0, 1.4, WHITE);
                        break;
                    case 1:
                        DrawTextureEx(playingAnimFrames[currentFrame%4], (Vector2){400, 110}, 0, 1.2, WHITE);
                        break;
                    case 2:
                        DrawTextureEx(playingAnimFrames[currentFrame%4], (Vector2){400, 100}, 0, 1.2, WHITE);
                        break;
                    case 3:
                        DrawTextureEx(playingAnimFrames[currentFrame%4], (Vector2){400, 120}, 0, 1.2, WHITE);
                        break;
                    
                }
                
                DrawTexture(balcao, 331, 450, WHITE);
                
                DrawText(TextFormat("%.2fs", displayTimer), 1034, 51, 50, RED);
                DrawText(TextFormat("%d", pontuacao),60, 295, 50, BLACK);
                DrawText(TextFormat("%d", pedidosCompletos),60, 561, 50, BLACK);
                
                if(eventoPopup.isActive) {
                    drawPopup(eventoPopup.eventType, popups);
                }
                if(gameState == playingState) printFila(&fila, sushis); // função de print da fila (sushis) na tela
                
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
                
                if(CheckCollisionPointRec(pontoMouse, (Rectangle){633, 400, 300, 81})) DrawTexture(botaoReiniciarMouseOver, 633, 400, WHITE); //botão de reiniciar
                else DrawTexture(botaoReiniciar, 633, 400, WHITE);
                
                if(CheckCollisionPointRec(pontoMouse, (Rectangle){633, 500, 300, 81})) DrawTexture(botaoSairMouseOver, 633, 500, WHITE); //botão de sair
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
        novo->item = item; // "ID" do item
        novo->posX = 1220 + posDesloc; //coordenada x de posição na tela
        novo->posY = 505; //coordenada y de posição na tela
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

void geraItens(No** fila, int dificuldade){ // função que gera os items com IDs aleatórios e os enfileira em sequencia
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

void checkKeyPressed(No** fila, int *pontuacao, Timer* playingTimer, Event* eventoPopup){ // função de controle de inputs, envia os parametros relevantes para a função de controle de fila
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
    
    if(eventoPopup->isActive && eventoPopup->eventType == input) return true; //verifica se evento ativo e input correto
    
    // verifica se input equivale a primeiro elemento da fila
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

void printFila(No** fila, Texture2D* sushis){ // print dos elementos da fila em sequencia, chama a função drawSushi que faz o controle de texturas por ID
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

void updateSushiPositions(No** fila, int velocidade){ //itera pela fila e atualiza suas posições no eixo X caso o movimento seja valido
    No* aux = *fila;
    if(*fila){        
        if(aux->posX >= limiteEsteira) aux->posX -= velocidadeEsteira + velocidade/4;
        while(aux->proximo){        
            if(aux->proximo->posX >= (aux->posX + espacamentoMinimoSushis)) aux->proximo->posX -= velocidadeEsteira + velocidade/4;
            aux = aux->proximo;
        }
    }
}

void createRandomEvent(Event* evento){ // criação do evento de popup com ID aleatório
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

void drawPopup(char eventType, Texture2D* popups){ // draws dos elementos visuais de popup
    if(eventType == 'E'){
        DrawTexture(popups[0], 683, 45, WHITE);
        DrawText("E", 743, 95, 50, BLACK);
    }
    if(eventType == 'U'){
        DrawTexture(popups[1], 683, 45, WHITE);
        DrawText("U", 743, 95, 50, BLACK);
    }
}

void flushFila(No** fila){
    No* aux = *fila;
    while(aux){
        desenfileirar(fila);
        aux = aux->proximo;
    }
}