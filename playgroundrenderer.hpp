#ifndef PLAYGROUNDRENDERER_HPP
#define PLAYGROUNDRENDERER_HPP

#include <string>
#include <SDL2/SDL.h>

#include "fruits.hpp"
#include "snake.hpp"

class PlaygroundRenderer{
    public:
        PlaygroundRenderer();
        ~PlaygroundRenderer();

        int Init(SDL_Renderer* renderer, SDL_Rect drawZone);
        
        void SetPlayground(Playground* playground);
        void draw();

    private:
        void GenerateBackground();
        int InitSnakeTextures();
        int InitFruitsTextures();
        int InitTilesTextures();

        SDL_Texture* LoadTexture(const std::string* filename);

        void drawBackground();
        void drawFruit(Fruit* fruitToDraw);
        void drawSnake(Snake* snakeToDraw);
        void drawSnakeHead(Segment* segment, bool snakeIsCrazy);
        void drawSnakeBodySegment(Segment* segment);
        void drawSnakeTail(Segment* segment);

    private:
        SDL_Rect maxDrawZone;
        SDL_Rect drawZone;
        Playground* playground;
        SDL_Renderer* renderer;
        SDL_Texture* bgTexture;
        SDL_Texture* snakeHeadTextures[4];
        SDL_Texture* crazySnakeHeadTextures[4];
        SDL_Texture* snakeBodyTextures[12];
        SDL_Texture* snakeTailTextures[12];
        SDL_Texture* fruitsTextures[4];
        SDL_Texture* tilesTextures[2];
        int tileSize;
};


#endif