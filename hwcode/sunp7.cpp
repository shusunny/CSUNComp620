/*
n: two dimensioal space size(n by n), s: initial sharks percentage, f: initial fish percentage, bs: sharks breeding age, 
bf: fish breeding age, g: generation periods. Initially sharks and fish are placed randomly; 
each cell has 8 neighbors and s+f ≤ 40 percent of n×n, i.e. for n = 50, initial number of sharks and fish ≤ 1000.

HWK-7: Due Date: before midnight 11/14, 100 Pts.
Given a two dimensional space of n × n, (n ≤ 50), write an efficient sequential c program to simulate g generations game 
of life for shark and fish using above rules. Please name your file xxxxxp7.c, where xxxxx is at most the first 5 characters 
of your last name and p7 is the program number. Please print input parameters as well population of sharks and fish after 
every 10 generations in a user-friendly form.

Input parameters are:
n: two dimensioal space size(n by n),
s: initial sharks percentage,
f: initial fish percentage,
bs: sharks breeding age,
bf: fish breeding age,
x: sharks death age,
y: fish death age,
g: generation periods.

Please PRINT your name at the end of OUTPUT.

To execute: a.out   n   s   f     bs   bf  x    y   g
Example:    a.out   50  20  15    5    3   10   4   50

*/
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
using namespace std;

class Game;
class Organism;
class Shark;
class Fish;

int WORLD_DIMENSION;

//the number of each critter we initialize the world with
int INIT_SHARK_COUNT;
int INIT_FISH_COUNT;

//the time steps it takes for each critter to breed
int SHARK_BREED_TIME;
int FISH_BREED_TIME;

//the time steps it takes to starve
int SHARK_STARVE_TIME;
int FISH_STARVE_TIME;

//number to indicate the type of each critter
const int SHARK = 1;
const int FISH = 2;

//valid moves for each organism
const int LEFT = 1;
const int RIGHT = 2;
const int DOWN = 3;
const int UP = 4;

typedef Organism* OrganismPtr;
typedef Game* GamePtr;

class Game {
  int WORLD_DIMENSION;
  friend class Organism;
  friend class Fish;
  friend class Shark;

private:
  OrganismPtr **world;
  int timeStepCount;
  int generateRandomNumber(int startRange, int endRange) const;

public:
  Game();
  void startGame(int WORLD_DIMENSION);
  void takeTimeStep();
  void printWorld() const;
};

class Organism {

protected:
  GamePtr currGame;
  int x;
  int y;
  int timeTillBreed;
  int timeStepCount;
  //given a coordinate of the cell (x,y), 
  //returns a list of valid moves to adjacent empty cells
  vector<int> getMovesToEmptyCells(int x, int y) const;
  bool isValidCoordinate(int x, int y) const;
  //given a valid move from grid[x][y],
  //updates x and y according to the move
  void getCoordinate(int& x, int& y, int move) const;

public:
  Organism(): currGame(nullptr), x(0), y(0), timeTillBreed(0), timeStepCount(0){}
  Organism(GamePtr currGame, int x, int y);
  virtual void breed() = 0;
  virtual void move();
  virtual int getType() = 0;
  virtual bool starves() { return false; }

};

class Shark: public Organism {

private:
  int timeTillStarve;
  //given a coordinate of the cell (x,y),
  //returns a list of valid moves to adjacent fishes
  vector<int> getMovesToFish(int x, int y) const;

public:
  Shark(): Organism(), timeTillStarve(0){}
  Shark(GamePtr currGame, int x, int y);
  void breed();
  void move();
  int getType(){ return SHARK; }
  bool starves(){ return timeTillStarve == 0; }

};

class Fish: public Organism {

public:
  Fish(): Organism(){}
  Fish(GamePtr currGame, int x, int y);
  void breed();
  int getType(){ return FISH; }

};

int Game::generateRandomNumber(int startRange, int endRange) const {
  return rand() % (endRange - startRange + 1) + startRange;
}


Game::Game(){
  srand(time(NULL));
  timeStepCount = 0;
  for (int x = 0; x < WORLD_DIMENSION; x++)
    for (int y = 0; y < WORLD_DIMENSION; y++)
      world[x][y] = nullptr;
}

void Game::startGame(int WORLD_DIMENSION){
  int x,y;
  int sharkCount = 0;
  int fishCount = 0;
  this->WORLD_DIMENSION = WORLD_DIMENSION;

  world = new OrganismPtr* [WORLD_DIMENSION];
  for (int i=0; i<WORLD_DIMENSION; i++) {
    world[i] = new OrganismPtr [WORLD_DIMENSION]; 
  }
  while (sharkCount < INIT_SHARK_COUNT){
    x = generateRandomNumber(0, WORLD_DIMENSION - 1);
    y = generateRandomNumber(0, WORLD_DIMENSION - 1);
    if (world[x][y] != nullptr) continue;
    world[x][y] = new Shark(this, x, y);
    sharkCount++;
  }

  while (fishCount < INIT_FISH_COUNT){
    x = generateRandomNumber(0, WORLD_DIMENSION - 1);
    y = generateRandomNumber(0, WORLD_DIMENSION - 1);
    if (world[x][y] != nullptr) continue;
    world[x][y] = new Fish(this, x, y);
    fishCount++;
  }
}

void Game::takeTimeStep(){
  timeStepCount++;

  for (int x = 0; x < WORLD_DIMENSION; x++){
    for (int y = 0; y < WORLD_DIMENSION; y++){
      if (world[x][y] == nullptr) continue;
      if (world[x][y]->getType() == SHARK)
        world[x][y]->move();
    }
  }

  for (int x = 0; x < WORLD_DIMENSION; x++){
    for (int y = 0; y < WORLD_DIMENSION; y++){
      if (world[x][y] == nullptr) continue;
      if (world[x][y]->getType() == FISH)
        world[x][y]->move();
    }
  }

  for (int x = 0; x < WORLD_DIMENSION; x++){
    for (int y = 0; y < WORLD_DIMENSION; y++){
      if (world[x][y] == nullptr) continue;
      world[x][y]->breed();
    }
  }

  for (int x = 0; x < WORLD_DIMENSION; x++){
    for (int y = 0; y < WORLD_DIMENSION; y++){
      if (world[x][y] == nullptr) continue;
      if (world[x][y]->starves()){
        delete world[x][y];
        world[x][y] = nullptr;
      }
    }
  }
}

void Game::printWorld() const {
  int sharkcount = 0;
  int fishcount = 0;
  for (int x = 0; x < WORLD_DIMENSION; x++){
    for (int y = 0; y < WORLD_DIMENSION; y++){
      if (world[x][y] == nullptr)
        cout << '*';
      else if (world[x][y]->getType() == FISH) {
        cout << 'F';
        fishcount++;
      }
      else{  //world[x][y]->getType() == SHARK
        cout << 'S';
        sharkcount++;
        }
    }
    cout << endl;
  }
  cout << "nshark = "<< sharkcount << "\tnfish = "<< fishcount << endl;
}

vector<int> Organism::getMovesToEmptyCells(int x, int y) const {
  vector<int> movesToEmptyCells;
  int tempX, tempY;
  for (int move = LEFT; move <= UP; move++){
    tempX = x;
    tempY = y;
    getCoordinate(tempX, tempY, move);
    if (!isValidCoordinate(tempX, tempY)) continue;
    if (currGame->world[tempX][tempY] == nullptr) 
      movesToEmptyCells.push_back(move);
  }
  return movesToEmptyCells;
}

bool Organism::isValidCoordinate(int x, int y) const {
  if (x < 0 || x >= WORLD_DIMENSION || y < 0 || y >= WORLD_DIMENSION)
    return false;
  return true;
}

void Organism::getCoordinate(int& x, int& y, int move) const {
  if (move == LEFT) x--;
  if (move == RIGHT) x++;
  if (move == DOWN) y--;
  if (move == UP) y++;
}

Organism::Organism(GamePtr currGame, int x, int y){
  this->currGame = currGame;
  this->x = x;
  this->y = y;
  timeTillBreed = 0;
  timeStepCount = currGame->timeStepCount;
}

void Organism::move(){
  if (timeStepCount == currGame->timeStepCount) return;
  timeStepCount++;
  timeTillBreed--;
  int randomMove = currGame->generateRandomNumber(LEFT, UP);
  int newX = x;
  int newY = y;
  getCoordinate(newX, newY, randomMove);
  if (isValidCoordinate(newX, newY)){
    if (currGame->world[newX][newY] != nullptr) return;
    currGame->world[x][y] = nullptr;
    currGame->world[newX][newY] = this;
    x = newX;
    y = newY;
  }
}

vector<int> Shark::getMovesToFish(int x, int y) const {
  vector<int> movesToFish;
  int tempX, tempY;
  for (int move = LEFT; move <= UP; move++){
    tempX = x;
    tempY = y;
    getCoordinate(tempX, tempY, move);
    if (!isValidCoordinate(tempX, tempY)) continue;
    if (currGame->world[tempX][tempY] == nullptr) continue;
    if (currGame->world[tempX][tempY]->getType() == FISH) 
      movesToFish.push_back(move);
  }
  return movesToFish;
}

Shark::Shark(GamePtr currGame, int x, int y): Organism(currGame, x, y){
  timeTillStarve = SHARK_STARVE_TIME;
  timeTillBreed = SHARK_BREED_TIME;
}

void Shark::breed(){
  if (timeTillBreed > 0) return;
  vector<int> validMoves = getMovesToEmptyCells(x, y);
  if (validMoves.size() == 0) return;
  int randomMove = validMoves[currGame->generateRandomNumber(0, validMoves.size() - 1)];
  int newX = x;
  int newY = y;
  getCoordinate(newX, newY, randomMove);
  currGame->world[newX][newY] = new Shark(currGame, newX, newY);
  timeTillBreed = SHARK_BREED_TIME;
}

void Shark::move(){
  if (timeStepCount == currGame->timeStepCount) return;
  vector<int> movesToFish = getMovesToFish(x, y);
  if (movesToFish.size() == 0){
    Organism::move();
    timeTillStarve--;
    return;
  }
  timeStepCount++;
  timeTillStarve = SHARK_STARVE_TIME;
  int randomMove = movesToFish[currGame->generateRandomNumber(0, movesToFish.size() - 1)];
  int fishX = x;
  int fishY = y;
  getCoordinate(fishX, fishY, randomMove);
  delete currGame->world[fishX][fishY];
  currGame->world[fishX][fishY] = this;
  currGame->world[x][y] = nullptr;
  x = fishX;
  y = fishY;
}

Fish::Fish(GamePtr currGame, int x, int y): Organism(currGame, x, y){
  timeTillBreed = FISH_BREED_TIME;
}

void Fish::breed(){
  if (timeTillBreed > 0) return;
  vector<int> validMoves = getMovesToEmptyCells(x, y);
  if (validMoves.size() == 0) return;
  int randomMove = validMoves[currGame->generateRandomNumber(0, validMoves.size() - 1)];
  int newX = x;
  int newY = y;
  getCoordinate(newX, newY, randomMove);
  currGame->world[newX][newY] = new Fish(currGame, newX, newY);
  timeTillBreed = FISH_BREED_TIME;
}

int main(int argc, char **argv){
  int GENERATION;
  // get parameters
  WORLD_DIMENSION = atoi(argv[1]); 
  INIT_SHARK_COUNT = atoi(argv[2]);
  INIT_FISH_COUNT = atoi(argv[3]);
  SHARK_BREED_TIME = atoi(argv[4]);
  FISH_BREED_TIME = atoi(argv[5]);
  SHARK_STARVE_TIME = atoi(argv[6]);
  FISH_STARVE_TIME = atoi(argv[7]);
  // get iteration time g
  GENERATION = atoi(argv[8]);

  Game g;
  // get the exact of init count
  INIT_SHARK_COUNT = INIT_SHARK_COUNT*WORLD_DIMENSION*WORLD_DIMENSION/100; 
  INIT_FISH_COUNT = INIT_FISH_COUNT*WORLD_DIMENSION*WORLD_DIMENSION/100; 
  cout << "WELCOME TO SHARK VS. FISH GAME" << endl;

  g.startGame(WORLD_DIMENSION);
  cout << "This is randomly generated board as Generation 0" << endl << endl;
  g.printWorld();
  cout << endl;
  
  for (int i=1; i< GENERATION; i++){
    g.takeTimeStep();
    if (i%10 == 0){
      cout << "Generation " << i << ".\n";
      g.printWorld();
      cout << endl;
    }
  }
  
  cout << "Generation " << GENERATION << ".\n";
  g.printWorld();
  cout << endl;

  cout << "by Zhen Sun for Comp620 homework-7" << endl;
  
  return 0;
  
}