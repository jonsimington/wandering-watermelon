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
  int piece;
  string type;
  int toRank;
  string toFile;
  int fromRank; //
  string fromFile;//
  int target;
  bool isCapture;
  string promotionType;
  int history;
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
  //MoveList * legalMoves;
  //int numMoves;
  
  MyPiece(){ moved=false;}//legalMoves=NULL; numMoves=0;
  //void addMove(int p, int toRank, char toFile, bool capture, int e, string promotion);
};

class State
{
  public:
  MyPiece * playerPieces;
  MyPiece * enemyPieces;
  int numPlayerPieces;
  int numEnemyPieces;
  int forward;
  int StateMatValue;
  int boringPly;
  MoveList history [16];
  MoveList ** hTable;
  const int hTableSize=64;
  
  bool passant;
  int rankP;
  string fileP;
  int passantTarget;
  
  bool castleK;
  bool castleQ;
  bool castlek;
  bool castleq;
  
  MoveList * legalMoves;
  int numMoves;
  void addMove(int p, int toRank, char toFile, int fromRank, char fromFile, bool capture, int e, string promotion);
  
  int checkSpace(int checkRank, char checkFile, int & target);
  bool checkKing();
  void genMoves();
  void validMoveCheck(int p, int toRank, char toFile, bool capture, int e);
  void castleCheck(int k);
  void enPassant(int pawn);
  void fen(string fen);
  void matAdvEval();
  void matAdvEvalMIN();
  
  int hashValue(int rank, string file);
  int getHistory(MoveList * move);
  void setHistory(MoveList * move);
  
  MoveList* DLM(int depthLimit, int qLimit);
  int MAX(int depthLimit, int alpha, int beta, bool isQ, int qLimit);
  int MIN(int depthLimit, int alpha, int beta, bool isQ, int qLimit);
  
  void makeNextState(State & targetState);
  void updateState(MoveList* move);
  bool checkDraw();
};

}
}
#endif

