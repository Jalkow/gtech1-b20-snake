#ifndef PLAYGROUND_HPP
#define PLAYGROUND_HPP

#include "fruits.hpp"
#include "snake.hpp"

class Playground{
    public:
        Playground(int nbCols, int nbRows, Snake* snake);
        ~Playground();

        int GetNbCols();
        int GetNbRows();
        Fruit* GetFruit();
        Snake* GetSnake();

        void SetFruit(Fruit* newFruit);

        void SpawnFruit();

    private:
        int nbCols;
        int nbRows;
        Fruit* presentFruit;
        Snake* snake;
};

#endif