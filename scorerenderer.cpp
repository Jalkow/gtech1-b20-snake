#include "scorerenderer.hpp"
#include "score.hpp"

#include <string>

ScoreRenderer::ScoreRenderer(){
    renderer = NULL;
}

ScoreRenderer::~ScoreRenderer(){
    for(int i=0; i<sizeof(iconsTextures) / sizeof(iconsTextures[0]);i++){
        SDL_DestroyTexture(iconsTextures[i]);
    }
}

int ScoreRenderer::Init(SDL_Renderer* renderer, SDL_Rect drawZone){
    this->renderer = renderer;
    this->drawZone = drawZone;

    police = TTF_OpenFont("fonts/pixel.ttf", 30);
    if(!police){
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    bool initIconsTexturesFailed = InitIconsTextures();
    if(initIconsTexturesFailed){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ScoreRenderer::InitIconsTextures(){
    const std::string iconsTexturesName [2] = {"images/bonus.bmp", "images/score.bmp"};

    for(int i=0; i<sizeof(iconsTexturesName) / sizeof(iconsTexturesName[0]);i++){
        const std::string imageName = iconsTexturesName[i];
        SDL_Surface* iconSurface = SDL_LoadBMP(imageName.c_str());
        if(iconSurface == NULL){
            printf("Problem with loading image: %s\n", SDL_GetError());
            return EXIT_FAILURE;
        }

        iconsTextures[i] = SDL_CreateTextureFromSurface(renderer, iconSurface);
        if(iconsTextures[i] == NULL){
            printf("Problem with creating texture: %s\n", SDL_GetError());
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

void ScoreRenderer::SetScore(Score* score){
    this->score = score;
}

//dessine toute la partie score de l'écran 
void ScoreRenderer::draw(){
    SDL_RenderSetViewport(renderer, &drawZone);
    SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
    SDL_RenderFillRect(renderer, &drawZone);

    //dessine le nombre de fruit mangé ainsi que une petite image du fruit bonus sur le coté gauche de ce nombre
    std::string numberEatenString = std::to_string(score->getNumberEaten());
    SDL_Surface* numbEatenSurface = TTF_RenderText_Blended(police, numberEatenString.c_str(), {255, 255, 255, 255});
    SDL_Texture* numbEatenTexture = SDL_CreateTextureFromSurface(renderer, numbEatenSurface);
    SDL_FreeSurface(numbEatenSurface);
    SDL_Rect numbEatenPlacement = {drawZone.w/15, 0, 0, 0};
    SDL_QueryTexture(numbEatenTexture, NULL, NULL, &(numbEatenPlacement.w), &(numbEatenPlacement.h));
    numbEatenPlacement.y = (drawZone.h - numbEatenPlacement.h)/2;
    SDL_RenderCopy(renderer, numbEatenTexture, NULL, &numbEatenPlacement);

    //petite image du fruit
    numbEatenPlacement.x -= drawZone.w/20;
    numbEatenPlacement.w = drawZone.w/25;
    SDL_RenderCopy(renderer, iconsTextures[0], NULL, &numbEatenPlacement);

    //dessine le score et une petite image de trophée coté gauche
    std::string scoreString = std::to_string(score->getScore());
    SDL_Surface* scoreSurface = TTF_RenderText_Blended(police, scoreString.c_str(), {255, 255, 255, 255});
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_FreeSurface(scoreSurface);
    int nbEatenDigits = log10(score->getNumberEaten());
    SDL_Rect scorePlacement = {numbEatenPlacement.x + drawZone.w/6 + (drawZone.w/35)*nbEatenDigits, 0, 0, 0};
    SDL_QueryTexture(scoreTexture, NULL, NULL, &(scorePlacement.w), &(scorePlacement.h));
    scorePlacement.y = (drawZone.h - numbEatenPlacement.h)/2;
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scorePlacement);

    //image trophée
    scorePlacement.x -= drawZone.w/20;
    scorePlacement.w = drawZone.w/25;
    SDL_RenderCopy(renderer, iconsTextures[1], NULL, &scorePlacement);

    SDL_DestroyTexture(numbEatenTexture);
    SDL_DestroyTexture(scoreTexture);
    SDL_RenderSetViewport(renderer, NULL);
}