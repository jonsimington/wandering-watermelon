



#include "ai.hpp"

namespace cpp_client
{

namespace chess
{

//Finds the material value for a state.
void State::matAdvEval()
{
  int count=0;
  for(int p=0; p<numPlayerPieces; p++)
  {
    if(playerPieces[p].captured!=true)
    {
      if( playerPieces[p].type=="Pawn")
      {
        count=count+1;
      }
      if( playerPieces[p].type=="Knight" || playerPieces[p].type=="Bishop")
      {
        count=count+3;
      }
      if( playerPieces[p].type=="Rook")
      {
        count=count+5;
      }
      if( playerPieces[p].type=="Queen")
      {
        count=count+9;
      }
    }
  }
  for(int p=0; p<numEnemyPieces; p++)
  {
    if(enemyPieces[p].captured!=true)
    {
      if( enemyPieces[p].type=="Pawn")
      {
        count=count-1;
      }
      if( enemyPieces[p].type=="Knight" || enemyPieces[p].type=="Bishop")
      {
        count=count-3;
      }
      if( enemyPieces[p].type=="Rook")
      {
        count=count-5;
      }
      if( enemyPieces[p].type=="Queen")
      {
        count=count-9;
      }
    }
  }
  StateMatValue=count;
}


//Handles reading FEN string
void State::fen(string fen)
{
  size_t space1, space2, space3, space4;
  space1=fen.find(" ");
  space2=fen.find(" ", space1+1);
  space3=fen.find(" ", space2+1);
  space4=fen.find(" ", space3+1);
  
  string fenCastle=fen.substr(space2+1, (space3)-(space2+1));
  string fenPassant=fen.substr(space3+1, (space4)-(space3+1));
  //cout<<"fen strings:"<<fenCastle<<" "<<fenPassant<<endl;
  
  castleK=false;
  castleQ=false;
  castlek=false;
  castleq=false;
  
  for(int i=0; i<(space3)-(space2+1); i++)
  {
    if(fenCastle[i]=='K')
    {
      castleK=true;
      
    }
    if(fenCastle[i]=='Q')
    {
      castleQ=true;
      
    }
    if(fenCastle[i]=='k')
    {
      castlek=true;
      
    }
    if(fenCastle[i]=='q')
    {
      castleq=true;
      
    }
  }
  
  
  if(fenPassant!="-")
  {
    
    passant=true;
    rankP=fenPassant[1]-48;
    fileP=fenPassant[0];
    for(int i=0; i<numPlayerPieces; i++)
    {
      if(playerPieces[i].captured==false && playerPieces[i].rank+1==rankP && playerPieces[i].file==fileP && playerPieces[i].type=="Pawn")
      {
        passantTarget=i;
      }
      if(playerPieces[i].captured==false && playerPieces[i].rank-1==rankP && playerPieces[i].file==fileP && playerPieces[i].type=="Pawn")
      {
        passantTarget=i;
      }
    }
    for(int i=0; i<numEnemyPieces; i++)
    {
      if(enemyPieces[i].captured==false && enemyPieces[i].rank+1==rankP && enemyPieces[i].file==fileP && enemyPieces[i].type=="Pawn")
      {
        passantTarget=i;
      }
      if(enemyPieces[i].captured==false && enemyPieces[i].rank-1==rankP && enemyPieces[i].file==fileP && enemyPieces[i].type=="Pawn")
      {
        passantTarget=i;
      }
    }
    
    
  }
}

//Checks if any uncaptured pieces occupy a given space.
//Output: 1-space occupied by player's piece.
//        2-space occupied by opponent's piece.
//        0-space is open.
//        3-space is out of bounds.
//If the space is occupied, target is set to the index of the piece in that space.
int State::checkSpace(int checkRank, char checkFile, int & target)
{
  
  if(checkRank<1 || checkRank>8 || checkFile<'a' || checkFile>'h') 
  {
    return 3;
  }
  
  for(int i=0; i<numPlayerPieces; i++)
  {
    if(playerPieces[i].captured==false && playerPieces[i].rank==checkRank && playerPieces[i].file[0]==checkFile)
    {
      target=i;
      return 1;
    }
  }
  
  for(int i=0; i<numEnemyPieces; i++)
  {
    if(enemyPieces[i].captured==false && enemyPieces[i].rank==checkRank && enemyPieces[i].file[0]==checkFile)
    {
      target=i;
      return 2;
    }
  }
  
  
  return 0;
}

//Generates a valid MoveList for each piece
void State::genMoves()
{
  int target;
  int spaceValue;
  bool end;
  for(int p=0; p<numPlayerPieces; p++)
  {
    if(playerPieces[p].captured!=true)
    {
      //Pawn
      if( playerPieces[p].type=="Pawn")
      {
        //Move
        spaceValue=checkSpace(playerPieces[p].rank+forward, playerPieces[p].file[0], target);
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank+forward, playerPieces[p].file[0], false, target);
          int homeRow;
          if(forward==1)
          {
            homeRow=2;
          }
          else
          {
            homeRow=7;
          }
          if(playerPieces[p].moved==0 && playerPieces[p].rank==homeRow)//Can only move two spaces if could already move one space
          {
            spaceValue=checkSpace(playerPieces[p].rank+(forward*2), playerPieces[p].file[0], target);
            if(spaceValue==0)
            {
              validMoveCheck(p, playerPieces[p].rank+(forward*2), playerPieces[p].file[0], false, target);
            }
          }
        }
        
        
        
        //Attacks
        spaceValue=checkSpace(playerPieces[p].rank+forward, playerPieces[p].file[0]+1, target);
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank+forward, playerPieces[p].file[0]+1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank+forward, playerPieces[p].file[0]-1, target);
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank+forward, playerPieces[p].file[0]-1, true, target);
        }
        enPassant(p);
      }
      
      //Rook
      if( playerPieces[p].type=="Rook" || playerPieces[p].type=="Queen") 
      {
        end=false;
        for(int i=1; end==false; i++) //up
        {
          spaceValue=checkSpace(playerPieces[p].rank+i, playerPieces[p].file[0], target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0], false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0], true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //down
        {
          spaceValue=checkSpace(playerPieces[p].rank-i, playerPieces[p].file[0], target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0], false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0], true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //left
        {
          spaceValue=checkSpace(playerPieces[p].rank, playerPieces[p].file[0]-i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]-i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]-i, true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //right
        {
          spaceValue=checkSpace(playerPieces[p].rank, playerPieces[p].file[0]+i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]+i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]+i, true, target);
            }
          }
        }
        
        
      }
      
      //Bishop
      if( playerPieces[p].type=="Bishop" || playerPieces[p].type=="Queen")
      {
        end=false;
        for(int i=1; end==false; i++) //up right
        {
          spaceValue=checkSpace(playerPieces[p].rank+i, playerPieces[p].file[0]+i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0]+i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0]+i, true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //down right
        {
          spaceValue=checkSpace(playerPieces[p].rank-i, playerPieces[p].file[0]+i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0]+i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0]+i, true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //up left
        {
          spaceValue=checkSpace(playerPieces[p].rank+i, playerPieces[p].file[0]-i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0]-i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0]-i, true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) // down left
        {
          spaceValue=checkSpace(playerPieces[p].rank-i, playerPieces[p].file[0]-i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0]-i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0]-i, true, target);
            }
          }
        }
      }
      
      //Knight
      if( playerPieces[p].type=="Knight")
      {
        spaceValue=checkSpace(playerPieces[p].rank+2, playerPieces[p].file[0]-1, target); //uul
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank+2, playerPieces[p].file[0]-1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank+2, playerPieces[p].file[0]-1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank+2, playerPieces[p].file[0]+1, target); //uur
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank+2, playerPieces[p].file[0]+1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank+2, playerPieces[p].file[0]+1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank-2, playerPieces[p].file[0]-1, target); //ddl
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank-2, playerPieces[p].file[0]-1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank-2, playerPieces[p].file[0]-1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank-2, playerPieces[p].file[0]+1, target); //ddr
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank-2, playerPieces[p].file[0]+1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank-2, playerPieces[p].file[0]+1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank+1, playerPieces[p].file[0]-2, target); //llu
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0]-2, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0]-2, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank-1, playerPieces[p].file[0]-2, target); //lld
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0]-2, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0]-2, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank+1, playerPieces[p].file[0]+2, target); //rru
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0]+2, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0]+2, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank-1, playerPieces[p].file[0]+2, target); //rrd
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0]+2, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0]+2, true, target);
        }
        
        
      }
      
      /*
      //Queen
      if( playerPieces[p].type=="Queen")
      {
        end=false;
        for(int i=1; end==false; i++) //up
        {
          spaceValue=checkSpace(playerPieces[p].rank+i, playerPieces[p].file[0], target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0], false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0], true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //down
        {
          spaceValue=checkSpace(playerPieces[p].rank-i, playerPieces[p].file[0], target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0], false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0], true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //left
        {
          spaceValue=checkSpace(playerPieces[p].rank, playerPieces[p].file[0]-i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]-i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]-i, true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //right
        {
          spaceValue=checkSpace(playerPieces[p].rank, playerPieces[p].file[0]+i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]+i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]+i, true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //up right
        {
          spaceValue=checkSpace(playerPieces[p].rank+i, playerPieces[p].file[0]+i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0]+i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0]+i, true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //down right
        {
          spaceValue=checkSpace(playerPieces[p].rank-i, playerPieces[p].file[0]+i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0]+i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0]+i, true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) //up left
        {
          spaceValue=checkSpace(playerPieces[p].rank+i, playerPieces[p].file[0]-i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0]-i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank+i, playerPieces[p].file[0]-i, true, target);
            }
          }
        }
        end=false;
        for(int i=1; end==false; i++) // down left
        {
          spaceValue=checkSpace(playerPieces[p].rank-i, playerPieces[p].file[0]-i, target);
          if(spaceValue==0)
          {
            validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0]-i, false, target);
          }
          if(spaceValue) //1, 2, or 3
          {
            end=true;
            if(spaceValue==2)
            {
              validMoveCheck(p, playerPieces[p].rank-i, playerPieces[p].file[0]-i, true, target);
            }
          }
        }
      }*/
      
      //King
      if( playerPieces[p].type=="King")
      {
        castleCheck(p);
        spaceValue=checkSpace(playerPieces[p].rank+1, playerPieces[p].file[0], target); //up
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0], false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0], true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank-1, playerPieces[p].file[0], target);//down
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0], false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0], true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank, playerPieces[p].file[0]+1, target);//left
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]+1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]+1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank, playerPieces[p].file[0]-1, target);//right
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]-1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank, playerPieces[p].file[0]-1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank+1, playerPieces[p].file[0]+1, target);//up right
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0]+1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0]+1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank-1, playerPieces[p].file[0]+1, target);//down right
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0]+1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0]+1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank+1, playerPieces[p].file[0]-1, target);//up left
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0]-1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank+1, playerPieces[p].file[0]-1, true, target);
        }
        
        spaceValue=checkSpace(playerPieces[p].rank-1, playerPieces[p].file[0]-1, target);//down left
        if(spaceValue==0)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0]-1, false, target);
        }
        if(spaceValue==2)
        {
          validMoveCheck(p, playerPieces[p].rank-1, playerPieces[p].file[0]-1, true, target);
        }
        
        
      }
    }
  }
}

//Handles en passant movements
void State::enPassant(int pawn)
{
  if(passant)
  {
    if(playerPieces[pawn].rank+forward==rankP && playerPieces[pawn].file[0]+1==fileP[0])
    {
      validMoveCheck(pawn, playerPieces[pawn].rank+forward, playerPieces[pawn].file[0]+1, true, passantTarget);
    }
    if(playerPieces[pawn].rank+forward==rankP && playerPieces[pawn].file[0]-1==fileP[0])
    {
      validMoveCheck(pawn, playerPieces[pawn].rank+forward, playerPieces[pawn].file[0]-1, true, passantTarget);
    }
  }
}



//handles castling
void State::castleCheck(int k)
{
  bool cKing, cQueen;
  if(forward==1) //White
  {
    cKing=castleK;
    cQueen=castleQ;
  }
  else
  {
    cKing=castlek;
    cQueen=castleq;
  }
  if(playerPieces[k].moved==false) //King hasn't moved
  {
    string fileOld= playerPieces[k].file;
    int spaceValue;
    for( int r=0; r<numPlayerPieces; r++)
    {
      if(playerPieces[r].type=="Rook" && playerPieces[r].moved==false) //Rook that hasn't moved
      {
        if(playerPieces[k].rank==playerPieces[r].rank) //K and R on same rank
        {
          int end=false;
          int look;
          if(playerPieces[k].file>playerPieces[r].file)//Rook is on left
          {
            if(cQueen)//Queenside castle not disabled
            {
              for(int i=1; end==false; i++) //left
              {
                spaceValue=checkSpace(playerPieces[k].rank, playerPieces[k].file[0]-i, look);
                if(spaceValue)
                {
                  end=true;
                  if(spaceValue==1)
                  {
                    if(look==r) //Space between is empty;
                    {
                      if( ! checkKing()) //make sure king is never in check
                      {
                        playerPieces[k].file=fileOld[0]-1;
                        if( ! checkKing()) //One over
                        {
                          playerPieces[k].file=fileOld[0]-2;
                          if( ! checkKing()) //two over
                          {
                            playerPieces[k].addMove(playerPieces[k].rank, fileOld[0]-2, false, 0, "");
                            playerPieces[k].legalMoves->isCastle=+1;
                            playerPieces[k].legalMoves->rookIndex=r;
                          }
                        }
                        playerPieces[k].file=fileOld;
                      }
                    }
                  }
                }
              }
            }
          }
          else //Rook is on right
          {
            if(cKing)
            {
              for(int i=1; end==false; i++) //right
              {
                spaceValue=checkSpace(playerPieces[k].rank, playerPieces[k].file[0]+i, look);
                if(spaceValue)
                {
                  end=true;
                  if(spaceValue==1)
                  {
                    if(look==r)
                    {
                      if( ! checkKing()) //make sure king is never in check
                      {
                        playerPieces[k].file=fileOld[0]+1;
                        if( ! checkKing()) //One over
                        {
                          playerPieces[k].file=fileOld[0]+2;
                          if( ! checkKing()) //two over
                          {
                            playerPieces[k].addMove(playerPieces[k].rank, fileOld[0]+2, false, 0, "");
                            playerPieces[k].legalMoves->isCastle=-1;
                            playerPieces[k].legalMoves->rookIndex=r;
                          }
                        }
                        playerPieces[k].file=fileOld;
                      }
                    }
                  }
                }
              }
            }
            
          }
        }
      }
    }
    
  }
}


  
  
  


//FOR STAGE ONE ONLY
//Ensures King is not in check after move.
//Move is made for check, then reverted, since generating more states is not yet needed.
void State::validMoveCheck(int p, int toRank, char toFile, bool capture, int e)
{
  int rankStore=playerPieces[p].rank;
  string fileStore=playerPieces[p].file;
  if(capture)
  {
    enemyPieces[e].captured=true;
  }
  playerPieces[p].rank=toRank;
  playerPieces[p].file=toFile;
  
  if( ! checkKing())
  {
    if(playerPieces[p].type=="Pawn" && (toRank==1 || toRank==8))
    {
      playerPieces[p].addMove(toRank, toFile, capture, e, "Queen");
      playerPieces[p].addMove(toRank, toFile, capture, e, "Rook");
      playerPieces[p].addMove(toRank, toFile, capture, e, "Bishop");
      playerPieces[p].addMove(toRank, toFile, capture, e, "Knight");
    }
    else
    {
      playerPieces[p].addMove(toRank, toFile, capture, e, "");
    }
    
  }
  
  
  playerPieces[p].rank=rankStore;
  playerPieces[p].file=fileStore;
  if(capture)
  {
    enemyPieces[e].captured=false;
  }
}

//Adds a move to a pieces move list.
void MyPiece::addMove(int toRank, char toFile, bool capture, int e, string type)
{
  MoveList * tmp;
  tmp= new MoveList;
  tmp->toFile=toFile;
  tmp->toRank=toRank;
  tmp->target=e;
  tmp->isCapture=capture;
  tmp->isCastle=false;
  tmp->promotionType=type;
  tmp->next=legalMoves;
  legalMoves=tmp;
  numMoves++;
}


//Returns true if king is in check.
bool State::checkKing()
{
  bool end=false;
  bool kingFound=false;
  int k=0;
  int threat;
  while(!kingFound && k<numPlayerPieces)
  {
    if(playerPieces[k].type=="King")
    {
      kingFound=true;
    }
    else
    {
      k++;
    }
  }
  
  
  
  if(!kingFound)
  {
    cout<<"KING NOT FOUND!"<<endl;
    return false;
  }
  
  //Checks line of sight for enemy Queens, Rooks, and Bishops.
  int spaceValue;
  for(int i=1; end==false; i++) //up
  {
    spaceValue=checkSpace(playerPieces[k].rank+i, playerPieces[k].file[0], threat);
    if(spaceValue)
    {
      end=true;
      if(spaceValue==2)
      {
        if(enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Rook")
        {
          return true;
        }
      }
    }
  }
  end=false;
  for(int i=1; end==false; i++) //down
  {
    spaceValue=checkSpace(playerPieces[k].rank-i, playerPieces[k].file[0], threat);
    if(spaceValue)
    {
      end=true;
      if(spaceValue==2)
      {
        if(enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Rook")
        {
          return true;
        }
      }
    }
  }
  end=false;
  for(int i=1; end==false; i++) //left
  {
    spaceValue=checkSpace(playerPieces[k].rank, playerPieces[k].file[0]-i, threat);
    if(spaceValue)
    {
      end=true;
      if(spaceValue==2)
      {
        if(enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Rook")
        {
          return true;
        }
      }
    }
  }
  end=false;
  for(int i=1; end==false; i++) //right
  {
    spaceValue=checkSpace(playerPieces[k].rank, playerPieces[k].file[0]+i, threat);
    if(spaceValue)
    {
      end=true;
      if(spaceValue==2)
      {
        if(enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Rook")
        {
          return true;
        }
      }
    }
  }
  end=false;
  for(int i=1; end==false; i++) //up left
  {
    spaceValue=checkSpace(playerPieces[k].rank+i, playerPieces[k].file[0]-i, threat);
    if(spaceValue)
    {
      end=true;
      if(spaceValue==2)
      {
        if(enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Bishop")
        {
          return true;
        }
      }
    }
  }
  end=false;
  for(int i=1; end==false; i++) //up right
  {
    spaceValue=checkSpace(playerPieces[k].rank+i, playerPieces[k].file[0]+i, threat);
    if(spaceValue)
    {
      end=true;
      if(spaceValue==2)
      {
        if(enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Bishop")
        {
          return true;
        }
      }
    }
  }
  end=false;
  for(int i=1; end==false; i++) //down left
  {
    spaceValue=checkSpace(playerPieces[k].rank-i, playerPieces[k].file[0]-i, threat);
    if(spaceValue)
    {
      end=true;
      if(spaceValue==2)
      {
        if(enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Bishop")
        {
          return true;
        }
      }
    }
  }
  end=false;
  for(int i=1; end==false; i++) //down right
  {
    spaceValue=checkSpace(playerPieces[k].rank-i, playerPieces[k].file[0]+i, threat);
    if(spaceValue)
    {
      end=true;
      if(spaceValue==2)
      {
        if(enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Bishop")
        {
          return true;
        }
      }
    }
  }
  
  //Check pawn locations
  spaceValue=checkSpace(playerPieces[k].rank+forward, playerPieces[k].file[0]+1, threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Pawn")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank+forward, playerPieces[k].file[0]-1, threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Pawn")
      {
        return true;
      }
    }
  }
  
  //Check for knights
  spaceValue=checkSpace(playerPieces[k].rank+2, playerPieces[k].file[0]-1, threat);  //up up left
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Knight")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank+2, playerPieces[k].file[0]+1, threat); //up up right
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Knight")
      {
        return true;
      }
    }
  }
  
  spaceValue=checkSpace(playerPieces[k].rank-2, playerPieces[k].file[0]-1, threat); //down down left
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Knight")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-2, playerPieces[k].file[0]+1, threat); //down down right
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Knight")
      {
        return true;
      }
    }
  }
  
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0]-2, threat); //left left up
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Knight")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0]-2, threat); //left left down
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Knight")
      {
        return true;
      }
    }
  }
  
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0]+2, threat); //right right up
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Knight")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0]+2, threat); //right right down
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="Knight")
      {
        return true;
      }
    }
  }
  
  //Check for King
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0], threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="King")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0], threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="King")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank, playerPieces[k].file[0]+1, threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="King")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank, playerPieces[k].file[0]-1, threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="King")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0]+1, threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="King")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0]+1, threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="King")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0]-1, threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="King")
      {
        return true;
      }
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0]-1, threat);
  if(spaceValue)
  {
    if(spaceValue==2)
    {
      if(enemyPieces[threat].type=="King")
      {
        return true;
      }
    }
  }
  
  
  return false;
}



}
}