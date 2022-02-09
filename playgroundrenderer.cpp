
#include <cmath>
#include <algorithm>
#include <time.h>

#include "playgroundrenderer.hpp"
#include "playground.hpp"


PlaygroundRenderer::PlaygroundRenderer(){
    renderer = NULL;
    bgTexture = NULL;
    playground = NULL;
    drawZone = {0, 0, 0, 0};
    tileSize = 0;
}

PlaygroundRenderer::~PlaygroundRenderer(){
    
    //detruire toutes les textures des différents tableaux de textures du serpent
    for(int i=0; i<sizeof(snakeHeadTextures) / sizeof(snakeHeadTextures[0]);i++){
        SDL_DestroyTexture(snakeHeadTextures[i]);
    }

    for(int i=0; i<sizeof(crazySnakeHeadTextures) / sizeof(crazySnakeHeadTextures[0]);i++){
        SDL_DestroyTexture(crazySnakeHeadTextures[i]);
    }

    for(int i=0; i<sizeof(snakeTailTextures) / sizeof(snakeTailTextures[0]);i++){
        SDL_DestroyTexture(snakeTailTextures[i]);
    }

    for(int i=0; i<sizeof(snakeBodyTextures) / sizeof(snakeBodyTextures[0]);i++){
        SDL_DestroyTexture(snakeBodyTextures[i]);
    }

    //detruire les textures des fruits
    for(int i=0; i<sizeof(fruitsTextures) / sizeof(fruitsTextures[0]);i++){
        SDL_DestroyTexture(fruitsTextures[i]);
    }

    //detruire les textures des tiles
    for(int i=0; i<sizeof(tilesTextures) / sizeof(tilesTextures[0]);i++){
        SDL_DestroyTexture(tilesTextures[i]);
    }

    if(bgTexture != NULL){
        SDL_DestroyTexture(bgTexture);
    }
}

//dessine le cadrillage, le fruit et le snake.
void PlaygroundRenderer::draw(){
    drawBackground();
    drawFruit(playground->GetFruit());
    drawSnake(playground->GetSnake());
}

//dessine le cadrillage
void PlaygroundRenderer::drawBackground(){
    SDL_RenderCopy(renderer, bgTexture, NULL, &drawZone);
}

void PlaygroundRenderer::drawFruit(Fruit* fruitToDraw){
    SDL_RenderSetViewport(renderer, &drawZone);
    SDL_Rect fruitRect = {fruitToDraw->GetCol() * tileSize, fruitToDraw->GetRow() * tileSize, tileSize, tileSize};

    //on utilise le fait que les valeurs des effets FruitEffect s'incrémentent à partir de 0 comme les index d'un tableau 
    SDL_RenderCopy(renderer, fruitsTextures[fruitToDraw->GetEffect()], NULL, &fruitRect);
}

void PlaygroundRenderer::drawSnake(Snake* snakeToDraw){
    SDL_RenderSetViewport(renderer, &drawZone);
    Segment* actual_segment = snakeToDraw->GetHead();

    drawSnakeHead(actual_segment, snakeToDraw->IsCrazy());
    actual_segment = actual_segment->GetNext();
    while(actual_segment->GetNext() != NULL){
        drawSnakeBodySegment(actual_segment);
        actual_segment = actual_segment->GetNext();
    }
    drawSnakeTail(actual_segment);
    
    SDL_RenderSetViewport(renderer, NULL);
}

void PlaygroundRenderer::drawSnakeHead(Segment* segment, bool snakeIsCrazy){
    SDL_Rect rectToDraw = {segment->GetCol()*tileSize, segment->GetRow()*tileSize, tileSize, tileSize};
    if(snakeIsCrazy){
        SDL_RenderCopy(renderer, crazySnakeHeadTextures[segment->GetDirection()], NULL, &rectToDraw);
    }
    else{
        SDL_RenderCopy(renderer, snakeHeadTextures[segment->GetDirection()], NULL, &rectToDraw);
    }
}

void PlaygroundRenderer::drawSnakeBodySegment(Segment* segment){
    SDL_Rect rectToDraw = {segment->GetCol()*tileSize, segment->GetRow()*tileSize, tileSize, tileSize};
    SDL_RenderCopy(renderer, snakeBodyTextures[segment->GetDirection()], NULL, &rectToDraw);
}

void PlaygroundRenderer::drawSnakeTail(Segment* segment){
    SDL_Rect rectToDraw = {segment->GetCol()*tileSize, segment->GetRow()*tileSize, tileSize, tileSize};
    SDL_RenderCopy(renderer, snakeTailTextures[segment->GetDirection()], NULL, &rectToDraw);
}

//initialise toutes les valeurs requises au fonctionnement du PlaygroundRenderer
int PlaygroundRenderer::Init(SDL_Renderer* renderer, SDL_Rect drawZone){
    this->renderer = renderer;
    this->maxDrawZone = drawZone;

    bool initSnakeTexturesFailed = InitSnakeTextures();
    if (initSnakeTexturesFailed){
        return EXIT_FAILURE;
    }

    bool initFruitsTexturesFailed = InitFruitsTextures();
    if(initFruitsTexturesFailed){
        return EXIT_FAILURE;
    }

    bool initTilesTexturesFailed = InitTilesTextures();
    if(initTilesTexturesFailed){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void PlaygroundRenderer::SetPlayground(Playground* playground)
{
    this->playground = playground;

    //calcule la tileSize en fonction du nombre de col/rows et de la width/height de la drawzone du playground
    int tileWidth = floor(maxDrawZone.w/playground->GetNbCols());
    int tileHeight = floor(maxDrawZone.h/playground->GetNbRows());
    this->tileSize = std::min(tileHeight,tileWidth);
    
    //change la drawzone pour qu'elle soit de la bonne taille et la place au milieu de l'écran
    this->drawZone.w = tileSize * playground->GetNbCols();
    this->drawZone.h = tileSize * playground->GetNbRows();
    this->drawZone.y = maxDrawZone.y + (maxDrawZone.h - this->drawZone.h) / 2;
    this->drawZone.x = maxDrawZone.x + (maxDrawZone.w - this->drawZone.w) / 2;

    GenerateBackground();
}

//load une image à partir d'un nom de fichier et renvoie la texture créée à partir de cette dernière
SDL_Texture* PlaygroundRenderer::LoadTexture(const std::string* filename){
    SDL_Surface* imageSurf = SDL_LoadBMP(filename->c_str());
    if(imageSurf == NULL){
        printf("Problem with loading image: %s\n", SDL_GetError());
    }
	SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurf);
    if(imageTexture == NULL){
        printf("Problem with creating texture: %s\n", SDL_GetError());
    }
	SDL_FreeSurface(imageSurf);
    return imageTexture;
}

//dessine le cadrillage sur une texture et enregistre cette dernière dans l'attribut bgTexture
void PlaygroundRenderer::GenerateBackground(){
    srand(time(NULL));
    SDL_Texture* randomTileTexture;
    if(bgTexture != NULL){
        SDL_DestroyTexture(bgTexture);
    }

    bgTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, drawZone.w, drawZone.h);

	SDL_SetRenderTarget(renderer, bgTexture);


    int row = 0;
    int col = 0;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    while(row < playground->GetNbRows()){
        SDL_Rect actual_tile = {col*tileSize, row*tileSize, tileSize, tileSize};
        //on prend une texture au hasard dans le tableau tilesTextures du playgroundRenderer

        //on prend un nombre au hasard entre 0 et la taille de tilesTexture-1 et on obtient la texture qui est à cet index
        int random_index = rand()%(sizeof(tilesTextures) / sizeof(tilesTextures[0]));
        randomTileTexture = tilesTextures[random_index];
        
        SDL_RenderCopy(renderer, randomTileTexture, NULL, &actual_tile);
        SDL_RenderDrawRect(renderer, &actual_tile);

        col ++;

        if(col >= playground->GetNbCols()){
            row ++;
            col = 0;
        }
    }
	
	SDL_SetRenderTarget(renderer, NULL);
}

//initialise les attributs snakeHeadTextures, snakeHeadTextures et snakeTailTextures qui sont utilisés pour dessiner le serpent
int PlaygroundRenderer::InitSnakeTextures(){
    //listes des noms  des images pour créer les textures du serpent
    const std::string headTextureNames[] = {"head_down.bmp", "head_up.bmp", "head_right.bmp", "head_left.bmp"};
    const std::string crazyHeadTextureNames[] = {"crazyhead_down.bmp", "crazyhead_up.bmp", "crazyhead_right.bmp", "crazyhead_left.bmp"};

    const std::string bodyTextureNames[] = {"body_down.bmp", "body_up.bmp", "body_right.bmp", "body_left.bmp",
                                            "body_downright.bmp", "body_downleft.bmp", "body_upright.bmp", "body_upleft.bmp",
                                            "body_rightdown.bmp", "body_rightup.bmp", "body_leftdown.bmp", "body_leftup.bmp"};
    const std::string tailTextureNames[] = {"tail_down.bmp", "tail_up.bmp", "tail_right.bmp", "tail_left.bmp",
                                            "tail_downright.bmp", "tail_downleft.bmp", "tail_upright.bmp", "tail_upleft.bmp",
                                            "tail_rightdown.bmp", "tail_rightup.bmp", "tail_leftdown.bmp", "tail_leftup.bmp"};
    

    //à chacun des éléments des tableaux de textures du PlaygroundRenderer, on associe une texture créée à 
    //partir d'un des noms d'images des tableaux au dessus
    for(int i=0; i<sizeof(headTextureNames) / sizeof(headTextureNames[0]);i++){
        std::string fullPath = std::string("images/") + headTextureNames[i];
        SDL_Texture* loadedTexture = LoadTexture(&fullPath);
        if(loadedTexture == NULL){
            return EXIT_FAILURE;
        }
        snakeHeadTextures[i] = loadedTexture;
    }

    for(int i=0; i<sizeof(crazyHeadTextureNames) / sizeof(crazyHeadTextureNames[0]);i++){
        std::string fullPath = std::string("images/") + crazyHeadTextureNames[i];
        SDL_Texture* loadedTexture = LoadTexture(&fullPath);
        if(loadedTexture == NULL){
            return EXIT_FAILURE;
        }
        crazySnakeHeadTextures[i] = loadedTexture;
    }

    for(int i=0; i<sizeof(bodyTextureNames) / sizeof(bodyTextureNames[0]);i++){
        std::string fullPath = std::string("images/") + bodyTextureNames[i];
        SDL_Texture* loadedTexture = LoadTexture(&fullPath);
        if(loadedTexture == NULL){
            return EXIT_FAILURE;
        }
        snakeBodyTextures[i] = loadedTexture;
    }

    for(int i=0; i<sizeof(tailTextureNames) / sizeof(tailTextureNames[0]);i++){
        std::string fullPath = std::string("images/") + tailTextureNames[i];
        SDL_Texture* loadedTexture = LoadTexture(&fullPath);
        if(loadedTexture == NULL){
            return EXIT_FAILURE;
        }
        snakeTailTextures[i] = loadedTexture;
    }

    return EXIT_SUCCESS;
}

//initialise l'attribut fruitsTextures du playgroundRenderer
int PlaygroundRenderer::InitFruitsTextures(){
    //textures correspondant dans l'ordre des différents effets dans l'enum FruitEffect
    const std::string effectsImagesName [4] = {"images/bonus.bmp", "images/poison.bmp", "images/speedup.bmp", "images/inverse.bmp"}; 

    for(int i=0; i<sizeof(effectsImagesName) / sizeof(effectsImagesName[0]);i++){
        SDL_Texture* loadedTexture = LoadTexture(&effectsImagesName[i]);
        if(loadedTexture == NULL){
            return EXIT_FAILURE;
        }
        fruitsTextures[i] = LoadTexture(&effectsImagesName[i]);
    }

    return EXIT_SUCCESS;
}

//initialise l'attribut tilesTextures du playgroundRenderer
int PlaygroundRenderer::InitTilesTextures(){
    const std::string tilesTexturesName[2] = {"images/tile1.bmp", "images/tile2.bmp"};

    for(int i=0; i<sizeof(tilesTexturesName) / sizeof(tilesTexturesName[0]);i++){
        SDL_Texture* loadedTexture = LoadTexture(&tilesTexturesName[i]);
        if(loadedTexture == NULL){
            return EXIT_FAILURE;
        }
        tilesTextures[i] = LoadTexture(&tilesTexturesName[i]);
    }

    return EXIT_SUCCESS;
}