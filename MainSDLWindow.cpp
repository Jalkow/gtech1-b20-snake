#include "MainSDLWindow.hpp"
#include "scorerenderer.hpp"
#include "playgroundrenderer.hpp"
#include "playground.hpp"

#include <SDL2/SDL_ttf.h>

MainSDLWindow::MainSDLWindow(void){
    this->renderer = NULL;
    this->window = NULL;
    this->playground = NULL;
    this->mainSnake = NULL;
    this->score = NULL;
    this->outsideMainLoopFont = NULL;
    this->outsideMainLoopSmallFont = NULL;
    this->pixelizedFont = NULL;
    this->scoreGraphics = new ScoreRenderer();
    this->playgroundGraphics = new PlaygroundRenderer();
}

MainSDLWindow::~MainSDLWindow(void){
    SDL_DestroyRenderer(this->renderer); 
    SDL_DestroyWindow(this->window);
    delete playgroundGraphics;
    delete scoreGraphics;
    if(outsideMainLoopFont != NULL){
        TTF_CloseFont(outsideMainLoopFont);
    }
    if(outsideMainLoopSmallFont != NULL){
        TTF_CloseFont(outsideMainLoopSmallFont);
    }
    if(pixelizedFont != NULL){
        TTF_CloseFont(pixelizedFont);
    }
    TTF_Quit();
    SDL_Quit();
}

//initialise tous les attributs de la MainSDLWindow
int MainSDLWindow::Init(const char* window_name, int width, int height){
    scoreZone = {0, 0, width, ((height/10)*1)};
    playgroundZone = {0, ((height/10)*1), width, ((height/10)*9)};

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
       printf("Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
       return EXIT_FAILURE;
    }

    if(TTF_Init()==-1) {
        printf("Erreur TTF_Init: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Erreur lors de la creation d'une fenetre : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, -1 , SDL_RENDERER_PRESENTVSYNC);
    if(renderer == NULL) {
        printf("Erreur lors de la creation d'un renderer : %s\n",SDL_GetError());
        return EXIT_FAILURE;
    }

    bool failedScoreInit = scoreGraphics->Init(renderer, scoreZone);
    if(failedScoreInit){
        return EXIT_FAILURE;
    }

    bool failedPlaygroundInit = playgroundGraphics->Init(renderer, playgroundZone);
    if(failedPlaygroundInit){
        return EXIT_FAILURE;
    }

    outsideMainLoopFont = TTF_OpenFont("fonts/arial.ttf", 64);
    if(outsideMainLoopFont == NULL){
        printf("Erreur lors de la lecture d'une font : %s\n",SDL_GetError());
        return EXIT_FAILURE;
    }

    outsideMainLoopSmallFont = TTF_OpenFont("fonts/arial.ttf", 32);
    if(outsideMainLoopSmallFont == NULL){
        printf("Erreur lors de la lecture d'une font : %s\n",SDL_GetError());
        return EXIT_FAILURE;
    }

    pixelizedFont = TTF_OpenFont("fonts/pixel.ttf", 64);
    if(pixelizedFont == NULL){
        printf("Erreur lors de la lecture d'une font : %s\n",SDL_GetError());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void MainSDLWindow::Run(){
    int windowWidth = 800;
    int windowHeight = 800;
    int nbRow = 25;
    int nbCol = 25;
    
    ////////////////////
    // REPLAYING LOOP //
    ////////////////////

    bool replayingGame = true;
    while(replayingGame){
        int framerate = 100;
        
        mainSnake = new Snake(nbRow/4, nbCol/4, RIGHT, std::min(nbRow, nbCol)/6);
        
        playground = new Playground(nbRow, nbCol, mainSnake);
        playgroundGraphics->SetPlayground(playground);

        score = new Score();
        scoreGraphics->SetScore(score);

        if (!WaitForPlayerReady()){
            delete mainSnake;
            delete playground;
            delete score;
            return;
        }
            
 
        ///////////////////
        //    MAIN LOOP  //
        ///////////////////

       int timeSinceCrazy = 0;
       bool gameContinues = true;
        while (gameContinues){
            Uint32 frameTimeStart = SDL_GetTicks();

            if (!HandleMainLoopEvents()){
                delete mainSnake;
                delete playground;
                delete score;
                return;
            }

            if(mainSnake->IsCrazy()){
                timeSinceCrazy ++;
                if(timeSinceCrazy == 20){
                    mainSnake->SetCrazy(false);
                    timeSinceCrazy = 0;
                }
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            gameContinues = mainSnake->Move(playground, score, &framerate);
            if (playground->GetFruit() == NULL){
                playground->SpawnFruit();
            }
            
            playgroundGraphics->draw();
            scoreGraphics->draw();

            SDL_RenderPresent(renderer);
            
            // si la frame est allée plus vite que le framerate, on attend jusqu'à arriver à framerate, faisant en sorte que
            // chacune de nos frame ait la même durée
            int timeSinceFrameStart = SDL_GetTicks() - frameTimeStart;
            if (timeSinceFrameStart < framerate){
                SDL_Delay(framerate - timeSinceFrameStart);
            }
        }

        ///////////////////
        //ECRAN GAME OVER//
        ///////////////////
        replayingGame = ShowGameOver();

        //supression objets aloués  
        delete mainSnake;
        delete playground;
        delete score;
    }
}

bool MainSDLWindow::HandleMainLoopEvents(){
    bool uniqueInputUsed = false;
    
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
            return false;
        if (event.type == SDL_KEYDOWN){
            if (event.key.keysym.scancode == SDL_SCANCODE_DOWN && !uniqueInputUsed){
                if(mainSnake->IsCrazy()){
                    mainSnake->ChangeDirection(UP);
                    uniqueInputUsed = true;
                }
                else{
                    mainSnake->ChangeDirection(DOWN);
                    uniqueInputUsed = true; 
                }
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_UP && !uniqueInputUsed){
                if(mainSnake->IsCrazy()){
                    mainSnake->ChangeDirection(DOWN);
                    uniqueInputUsed = true;
                } 
                else{
                    mainSnake->ChangeDirection(UP);
                    uniqueInputUsed = true;
                } 
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT && !uniqueInputUsed){
                if(mainSnake->IsCrazy()){
                    mainSnake->ChangeDirection(LEFT);
                    uniqueInputUsed = true;
                }
                else{
                    mainSnake->ChangeDirection(RIGHT);
                    uniqueInputUsed = true;
                }
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT && !uniqueInputUsed){
                if(mainSnake->IsCrazy()){
                    mainSnake->ChangeDirection(RIGHT);
                    uniqueInputUsed = true;
                }
                else{
                    mainSnake->ChangeDirection(LEFT);
                    uniqueInputUsed = true;
                }
            }
        }
    }

    return true;
}


bool MainSDLWindow::WaitForPlayerReady(){
    int windowWidth;
    int windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    SDL_Surface* gameStartSurface = TTF_RenderText_Blended(outsideMainLoopFont, "Press any key to start !", {255, 255, 255, 255});
    SDL_Texture* gameStartText = SDL_CreateTextureFromSurface(renderer, gameStartSurface);
    SDL_FreeSurface(gameStartSurface);
    SDL_Rect gameStartPlacement;
    SDL_QueryTexture(gameStartText, NULL, NULL, &gameStartPlacement.w, &gameStartPlacement.h);
    gameStartPlacement.x = (windowWidth-gameStartPlacement.w)/2;
    gameStartPlacement.y = (windowHeight-gameStartPlacement.h)/2;

    
    SDL_Rect startButton = {gameStartPlacement.x - 30, gameStartPlacement.y - 20, gameStartPlacement.w + 60, gameStartPlacement.h + 40};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    playgroundGraphics->draw();
    scoreGraphics->draw();

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &startButton);

    SDL_RenderCopy(renderer, gameStartText, NULL, &gameStartPlacement);

    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool waitingForStart = true;
    while(waitingForStart)
    {
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
                return false;
            if (event.type == SDL_KEYDOWN)
                waitingForStart = false;
        }
    }

    return true;
}

bool MainSDLWindow::ShowGameOver(){
    int windowWidth;
    int windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // game over text
    SDL_Surface* gameOverSurface = TTF_RenderText_Blended(pixelizedFont, "Game Over !", {255, 0, 0, 255});
    SDL_Texture* gameOverText = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_FreeSurface(gameOverSurface);
    SDL_Rect gameOverPlacement;
    SDL_QueryTexture(gameOverText, NULL, NULL, &gameOverPlacement.w, &gameOverPlacement.h);
    gameOverPlacement.x = (windowWidth-gameOverPlacement.w)/2;
    gameOverPlacement.y = windowHeight/4;
    
    //score text
    std::string endScoreString = std::string("Your final score is : " + std::to_string(score->getScore()));
    SDL_Surface* endScoreSurface = TTF_RenderText_Blended(outsideMainLoopSmallFont, endScoreString.c_str(), {255, 255, 255, 255});
    SDL_Texture* endScoreText = SDL_CreateTextureFromSurface(renderer, endScoreSurface);
    SDL_FreeSurface(endScoreSurface);
    SDL_Rect endScorePlacement;
    SDL_QueryTexture(endScoreText, NULL, NULL, &endScorePlacement.w, &endScorePlacement.h);
    endScorePlacement.x = (windowWidth-endScorePlacement.w)/2;
    endScorePlacement.y = gameOverPlacement.y + endScorePlacement.h + windowHeight/10;

    //replay text & buttons
    SDL_Surface* endReplaySurface = TTF_RenderText_Blended(outsideMainLoopSmallFont, "Want to play again ?", {255, 255, 255, 255});
    SDL_Texture* endReplayText = SDL_CreateTextureFromSurface(renderer, endReplaySurface);
    SDL_FreeSurface(endReplaySurface);
    SDL_Rect endReplayPlacement;
    SDL_QueryTexture(endReplayText, NULL, NULL, &endReplayPlacement.w, &endReplayPlacement.h);
    endReplayPlacement.x = (windowWidth-endReplayPlacement.w)/2;
    endReplayPlacement.y = endScorePlacement.y + endReplayPlacement.h + windowHeight/8;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //couleur background des boutons
    SDL_Surface* endYesSurface = TTF_RenderText_Blended(pixelizedFont, "Yes", {0, 255, 0, 255});
    SDL_Texture* endYesText = SDL_CreateTextureFromSurface(renderer, endYesSurface);
    SDL_FreeSurface(endYesSurface);
    SDL_Rect endYesPlacement;
    SDL_QueryTexture(endYesText, NULL, NULL, &endYesPlacement.w, &endYesPlacement.h);
    endYesPlacement.x = windowWidth/2 - endYesPlacement.w - windowWidth/10;
    endYesPlacement.y = endReplayPlacement.y + endYesPlacement.h + windowHeight/10;

    SDL_Surface* endNoSurface = TTF_RenderText_Blended(pixelizedFont, "No", {255, 0, 0, 255});
    SDL_Texture* endNoText = SDL_CreateTextureFromSurface(renderer, endNoSurface);
    SDL_FreeSurface(endNoSurface);
    SDL_Rect endNoPlacement;
    SDL_QueryTexture(endNoText, NULL, NULL, &endNoPlacement.w, &endNoPlacement.h);
    endNoPlacement.x = windowWidth/2 + windowWidth/10;
    endNoPlacement.y = endYesPlacement.y;

    SDL_RenderCopy(renderer, gameOverText, NULL, &gameOverPlacement);
    SDL_RenderCopy(renderer, endScoreText, NULL, &endScorePlacement);
    SDL_RenderCopy(renderer, endReplayText, NULL, &endReplayPlacement);
    
    //SDL_RenderFillRect(renderer, &endYesPlacement);
    SDL_RenderCopy(renderer, endYesText, NULL, &endYesPlacement);

    //SDL_RenderFillRect(renderer, &endNoPlacement);
    SDL_RenderCopy(renderer, endNoText, NULL, &endNoPlacement);

    SDL_RenderPresent(renderer);

    SDL_Event event;
    while(true){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
                return false;
            if (event.type == SDL_MOUSEBUTTONDOWN){
                if(event.button.button == SDL_BUTTON_LEFT
                    && event.button.x >= endYesPlacement.x
                    && event.button.x <= endYesPlacement.x + endYesPlacement.w
                    && event.button.y >= endYesPlacement.y
                    && event.button.y <= endYesPlacement.y + endYesPlacement.h){
                        return true;
                }
                else if(event.button.button == SDL_BUTTON_LEFT
                        && event.button.x >= endNoPlacement.x
                        && event.button.x <= endNoPlacement.x + endNoPlacement.w
                        && event.button.y >= endNoPlacement.y
                        && event.button.y <= endNoPlacement.y + endNoPlacement.h){
                            return false;
                }
            }
        }
    }
}
