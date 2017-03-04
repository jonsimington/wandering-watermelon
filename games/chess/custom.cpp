

#include "ai.hpp"

namespace cpp_client
{

namespace chess
{

//Checks if any uncaptured pieces occupy a given space.
//Output: 1-space occupied by player's piece.
//        2-space occupied by opponent's piece.
//        0-space is open.
//        3-space is out of bounds.
//If the space is occupied, target is set to the index of the piece in that space.
int State::checkSpace(int checkRank, string checkFile, int & target)
{
  if(checkRank<1 || checkRank>8 || checkFile.compare("a")>0 || checkFile.compare("h")<0) //Double check this logic!!!!
  {
    return 3;
  }
  
  for(int i=0; i<numPlayerPieces; i++)
  {
    if(playerPieces[i]->captured==false && playerPieces[i]->rank==checkRank && playerPieces[i]->file==checkFile)
    {
      target=i;
      return 1;
    }
  }
  
  for(int i=0; i<numEnemyPieces; i++)
  {
    if(enemyPieces[i]->captured==false && enemyPieces[i]->rank==checkRank && enemyPieces[i]->file==checkFile)
    {
      target=i;
      return 2;
    }
  }
  
  return 0;
}

