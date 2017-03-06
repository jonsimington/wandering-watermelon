#ifndef GAMES_CHESS_CUSTOM_HPP
#define GAMES_CHESS_CUSTOM_HPP

#include <string>
#include "piece.hpp"
using namespace std;

namespace cpp_client
{

namespace chess
{
  
class MoveList
{
  public:
  int toRank;
  string toFile;
  int target;
  bool isCapture;
  string promotionType;
  MoveList * next;
  
  int isCastle;
  int rookIndex;
};

class MyPiece
{
  public:
  bool captured;
  string color; 
  int rank;
  string file;
  string type;
  bool moved;
  Piece pieceRef;
  MoveList * legalMoves;
  int numMoves;
  
  MyPiece(){legalMoves=NULL; numMoves=0; moved=false;}
  void addMove(int toRank, char toFile, bool capture, int e, string promotion);
};

class State
{
  public:
  MyPiece * playerPieces;
  MyPiece * enemyPieces;
  int numPlayerPieces;
  int numEnemyPieces;
  int forward;
  
  bool passant;
  int rankP;
  string fileP;
  int passantTarget;
  
  bool castleK;
  bool castleQ;
  bool castlek;
  bool castleq;
  
  int checkSpace(int checkRank, char checkFile, int & target);
  bool checkKing();
  void genMoves();
  void validMoveCheck(int p, int toRank, char toFile, bool capture, int e);
  void castleCheck(int k);
  void enPassant(int pawn);
  void fen(string fen);
};

}
}
#endif

