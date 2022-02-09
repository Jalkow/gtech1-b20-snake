#include <time.h>
#include "playground.hpp"

Playground::Playground(int nbRows, int nbCols, Snake* snake){
    this->nbCols = nbCols;
    this->nbRows = nbRows;
    this->snake = snake;
    
    SpawnFruit();
}

Playground::~Playground(){
    delete presentFruit;
}

int Playground::GetNbCols(){
    return nbCols;
}

int Playground::GetNbRows(){
    return nbRows;
}

Fruit* Playground::GetFruit(){
    return presentFruit;
}

Snake* Playground::GetSnake(){
    return snake;
}

void Playground::DeleteFruit(){
    delete presentFruit;
    presentFruit = NULL;
}

//crée un nouveau fruit avec des attributs aléatoire
void Playground::SpawnFruit(){
    srand(time(NULL));

    FruitEffect randomEffect;
    int randomEffectNumber = (rand()%10) + 1;

    //10% de chance d'obtenir un fruit special puisque on peut avoir les chiffres de 1 à 10
    if(randomEffectNumber == 10){
        randomEffect = static_cast<FruitEffect>((rand()% (COUNT-1)) + 1);
    }
    else{
        randomEffect = BONUS;
    }
    
    //vérifier que les coordonnées créées aléatoirement sont correcte (pas sur le serpent) et si ce n'est pas le cas
    //en générer de nouvelles
    int randomCol = rand()%nbCols;
    int randomRow = rand()%nbRows;
    while(snake->occupiesTile(randomRow, randomCol))
    {
        randomCol = rand()%nbCols;
        randomRow = rand()%nbRows;
    }
    
    //si on est sorti de la boucle while alors les coordonnées sont correctes donc on peut créer la pomme
    presentFruit = new Fruit(randomRow, randomCol, randomEffect);
}

