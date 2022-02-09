#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <SDL2/SDL.h>
#include "MainSDLWindow.hpp"
#include "score.hpp"
#include "fruits.hpp"


typedef enum{
    DOWN, UP, RIGHT, LEFT, DOWNRIGHT, DOWNLEFT, UPRIGHT, UPLEFT, RIGHTDOWN, RIGHTUP, LEFTDOWN, LEFTUP
} Direction;

class Playground;

class Segment{
    public :
        Segment(int row, int col, Direction direction, Segment* next);

        ~Segment();

        int GetRow();
        int GetCol();
        Direction GetDirection();
        Segment* GetNext();
        
        void SetNext(Segment* nextSegment);
        void SetDirection(Direction newDirection);

    private :
        int row;
        int col;
        Direction direction;
        Segment* next;

};

class Snake{
    public:
        Snake(int starting_row, int starting_col, Direction starting_direction, int length);

        ~Snake();

        bool Move(Playground* playground, Score* score, int* framerate);

        void ChangeDirection(Direction newDirection);

        bool Eat(Fruit* fruitToEat, Score* score, int* framerate);

        bool occupiesTile(int row, int col);

        void SetCrazy(bool crazy);
        bool IsCrazy();

        Segment* GetHead();

    private:
        Segment* head;
        int length;
        Direction directionToMove;
        bool crazy;

};


#endif