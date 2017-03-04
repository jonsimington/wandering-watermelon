#ifndef GAMES_CHESS_CUSTOM_HPP
#define GAMES_CHESS_CUSTOM_HPP

#include "piece.hpp"

class MoveList
{
  int toRank;
  string toFile;
  MoveList * next;
}

class MyPiece
{
  public:
  bool captured;
  string color; 
  int rank;
  string file;
  string type;
  bool moved;
  Piece* pieceRef;
  MoveList * legalMoves;
};

class State
{
  public:
  MyPiece * playerPieces;
  MyPiece * enemyPieces;
  int numPlayerPieces;
  int numEnemyPieces;
  
  int checkSpace(int checkRank, string checkFile, int & target);
}
#endif

