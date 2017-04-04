



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
  StateMatValue=-count;
}

/*
//Finds the material value for a state. Inverted due to how data is handled in MIN()
void State::matAdvEvalMIN()
{
  int count=0;
  for(int p=0; p<numPlayerPieces; p++)
  {
    if(playerPieces[p].captured!=true)
    {
      if( playerPieces[p].type=="Pawn")
      {
        count=count-1;
      }
      if( playerPieces[p].type=="Knight" || playerPieces[p].type=="Bishop")
      {
        count=count-3;
      }
      if( playerPieces[p].type=="Rook")
      {
        count=count-5;
      }
      if( playerPieces[p].type=="Queen")
      {
        count=count-9;
      }
    }
  }
  for(int p=0; p<numEnemyPieces; p++)
  {
    if(enemyPieces[p].captured!=true)
    {
      if( enemyPieces[p].type=="Pawn")
      {
        count=count+1;
      }
      if( enemyPieces[p].type=="Knight" || enemyPieces[p].type=="Bishop")
      {
        count=count+3;
      }
      if( enemyPieces[p].type=="Rook")
      {
        count=count+5;
      }
      if( enemyPieces[p].type=="Queen")
      {
        count=count+9;
      }
    }
  }
  StateMatValue=-count;
}
*/

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
      if(playerPieces[r].type=="Rook" && playerPieces[r].moved==false && playerPieces[k].rank==playerPieces[r].rank) //Rook hasn't moved and K and R on same rank
      {
        int end=false;
        int look;
        if(playerPieces[k].file>playerPieces[r].file && cQueen)//Rook is on left and Queenside castle not disabled
        {
          for(int i=1; end==false; i++) //left
          {
            spaceValue=checkSpace(playerPieces[k].rank, playerPieces[k].file[0]-i, look);
            if(spaceValue)
            {
              end=true;
              if(spaceValue==1 && look==r && ! checkKing())
              {
                playerPieces[k].file=fileOld[0]-1;
                if( ! checkKing()) //One over
                {
                  playerPieces[k].file=fileOld[0]-2;
                  if( ! checkKing()) //two over
                  {
                    addMove(k, playerPieces[k].rank, fileOld[0]-2, playerPieces[k].rank, fileOld[0], false, 0, "");
                    legalMoves->isCastle=+1;
                    legalMoves->rookIndex=r;
                  }
                }
                playerPieces[k].file=fileOld;
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
                if(spaceValue==1 && look==r && ! checkKing())
                {
                  playerPieces[k].file=fileOld[0]+1;
                  if( ! checkKing()) //One over
                  {
                    playerPieces[k].file=fileOld[0]+2;
                    if( ! checkKing()) //two over
                    {
                      addMove(k, playerPieces[k].rank, fileOld[0]+2, playerPieces[k].rank, fileOld[0], false, 0, "");
                      legalMoves->isCastle=-1;
                      legalMoves->rookIndex=r;
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


  
  
  



//Ensures King is not in check after move.
//Move is made for check, then reverted.
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
      addMove(p, toRank, toFile, rankStore, fileStore[0], capture, e, "Queen");
      addMove(p, toRank, toFile, rankStore, fileStore[0], capture, e, "Rook");
      addMove(p, toRank, toFile, rankStore, fileStore[0], capture, e, "Bishop");
      addMove(p, toRank, toFile, rankStore, fileStore[0], capture, e, "Knight");
    }
    else
    {
      addMove(p, toRank, toFile, rankStore, fileStore[0], capture, e, "");
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
void State::addMove(int p, int toRank, char toFile, int fromRank, char fromFile, bool capture, int e, string type)
{
  MoveList * tmp;
  tmp= new MoveList;
  tmp->toFile=toFile;
  tmp->toRank=toRank;
  tmp->fromFile=fromFile;
  tmp->fromRank=fromRank;
  tmp->target=e;
  tmp->piece=p;
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
    //throw "tantrum"; //If this throws, everything will die.
    //Fun Fact: I had a bug where the King was getting "promoted" in my internal datastructures
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
      if(spaceValue==2 && (enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Rook") )
      {
        return true;
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
      if(spaceValue==2 && (enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Rook") )
      {
        return true;
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
      if(spaceValue==2 && (enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Rook") )
      {
        return true;
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
      if(spaceValue==2 && (enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Rook") )
      {
        return true;
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
      if(spaceValue==2 && (enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Bishop") )
      {
        return true;
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
      if(spaceValue==2 && (enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Bishop") )
      {
        return true;
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
      if(spaceValue==2 && (enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Bishop") )
      {
        return true;
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
      if(spaceValue==2 && (enemyPieces[threat].type=="Queen" || enemyPieces[threat].type=="Bishop") )
      {
        return true;
      }
    }
  }
  
  //Check pawn locations
  spaceValue=checkSpace(playerPieces[k].rank+forward, playerPieces[k].file[0]+1, threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Pawn")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank+forward, playerPieces[k].file[0]-1, threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Pawn")
    {
      return true;
    }
  }
  
  //Check for knights
  spaceValue=checkSpace(playerPieces[k].rank+2, playerPieces[k].file[0]-1, threat);  //up up left
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Knight")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank+2, playerPieces[k].file[0]+1, threat); //up up right
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Knight")
    {
      return true;
    }
  }
  
  spaceValue=checkSpace(playerPieces[k].rank-2, playerPieces[k].file[0]-1, threat); //down down left
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Knight")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-2, playerPieces[k].file[0]+1, threat); //down down right
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Knight")
    {
      return true;
    }
  }
  
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0]-2, threat); //left left up
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Knight")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0]-2, threat); //left left down
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Knight")
    {
      return true;
    }
  }
  
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0]+2, threat); //right right up
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Knight")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0]+2, threat); //right right down
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="Knight")
    {
      return true;
    }
  }
  
  //Check for King
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0], threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="King")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0], threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="King")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank, playerPieces[k].file[0]+1, threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="King")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank, playerPieces[k].file[0]-1, threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="King")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0]+1, threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="King")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0]+1, threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="King")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank+1, playerPieces[k].file[0]-1, threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="King")
    {
      return true;
    }
  }
  spaceValue=checkSpace(playerPieces[k].rank-1, playerPieces[k].file[0]-1, threat);
  if(spaceValue)
  {
    if(spaceValue==2 && enemyPieces[threat].type=="King")
    {
      return true;
    }
  }
  
  
  return false;
}

//Recursive Depth-Limited Minimax
MoveList* State::DLM(int depthLimit)
{
  
  
  
  //int bestValue=-101; //replacing this with alpha
  int currentValue;
  int alpha=-1500;
  int beta=1500;
  MoveList* bestMove;
  
  State* nextState=new State[numMoves];
  MoveList * move=legalMoves;
  for(int i=0; i<numMoves; i++)
  {
    nextState[i].numPlayerPieces=0;
    makeNextState(nextState[i]);
    nextState[i].updateState(move);
    currentValue=nextState[i].MIN(depthLimit-1, alpha, beta);
    if(currentValue>alpha)
    {
      alpha=currentValue;
      //bestValue=currentValue;
      bestMove=move;
    }
    //Randomizer is no longer viable due to AB pruning
    move=move->next;
    
    if(currentValue>beta) //This should not actually be possible unless hueristic is changed.
    {
      cout<<"Something has gone very wrong in DLM. Beta exceeded"<<endl;
    }
  }
  
  for(int i=0; i<numMoves; i++)
  {
    delete[] nextState[i].enemyPieces;
    delete[] nextState[i].playerPieces;
    
    MoveList * tmp=legalMoves;
    while(nextState[i].legalMoves != NULL)
    {
      tmp=nextState[i].legalMoves;
      nextState[i].legalMoves=nextState[i].legalMoves->next;
      delete tmp;
    }
  }
  delete[] nextState;
  
  
  return bestMove;
}

int State::MAX(int depthLimit, int alpha, int beta)
{
  genMoves();
  
  //If max has no moves AND in check, Max lost.
  if(numMoves==0 && checkKing())
  {
    StateMatValue=-1000;
    return StateMatValue;
  }
  if(checkDraw())
  {
    StateMatValue=0;
    return StateMatValue;
  }
  if(depthLimit==0)
  {
    matAdvEval();
    return StateMatValue;
  }
  
  
  //int bestValue=-101;
  int currentValue;
  
  State* nextState=new State[numMoves];
  MoveList * move=legalMoves;
  for(int i=0; i<numMoves; i++)
  {
    makeNextState(nextState[i]);
    nextState[i].updateState(move);
    currentValue=nextState[i].MIN(depthLimit-1, alpha, beta);
    if(currentValue>alpha)
    {
      alpha=currentValue;
    }
    if(currentValue>=beta)
    {
      numMoves=i+1;
      break;
    }
    move=move->next;
  }
  
  
  
  for(int i=0; i<numMoves; i++)
  {
    delete[] nextState[i].enemyPieces;
    delete[] nextState[i].playerPieces;
    
    MoveList * tmp=legalMoves;
    while(nextState[i].legalMoves != NULL)
    {
      tmp=nextState[i].legalMoves;
      nextState[i].legalMoves=nextState[i].legalMoves->next;
      delete tmp;
    }
  }
  delete[] nextState;
  
  MoveList * temp=legalMoves;
  while(legalMoves != NULL)
  {
    temp=legalMoves;
    legalMoves=legalMoves->next;
    delete temp;
  }
  numMoves=0;
  
  return alpha;
}

int State::MIN(int depthLimit, int alpha, int beta)
{
  genMoves();
  
  //If min has no moves AND in check, Max wins.
  if(numMoves==0 && checkKing())
  {
    StateMatValue=1000;
    return StateMatValue;
  }
  if(checkDraw())
  {
    StateMatValue=0;
    return StateMatValue;
  }
  if(depthLimit==0)
  {
    matAdvEval();
    StateMatValue= -StateMatValue; //Condensed alternate Eval functions, this is currently needed to ensure correct value.
    return StateMatValue;
  }
  
  
  //int bestValue=101;
  int currentValue;
  
  State* nextState=new State[numMoves];
  MoveList * move=legalMoves;
  for(int i=0; i<numMoves; i++)
  {
    makeNextState(nextState[i]);
    nextState[i].updateState(move);
    currentValue=nextState[i].MAX(depthLimit-1, alpha, beta);
    if(currentValue<beta)
    {
      beta=currentValue;
    }
    if(currentValue<=alpha)
    {
      numMoves=i+1;
      break;
    }
    move=move->next;
  }
  
  
  
  for(int i=0; i<numMoves; i++)
  {
    delete[] nextState[i].enemyPieces;
    delete[] nextState[i].playerPieces;
    
    MoveList * tmp=legalMoves;
    while(nextState[i].legalMoves != NULL)
    {
      tmp=nextState[i].legalMoves;
      nextState[i].legalMoves=nextState[i].legalMoves->next;
      delete tmp;
    }
  }
  delete[] nextState;
  
  MoveList * temp=legalMoves;
  while(legalMoves != NULL)
  {
    temp=legalMoves;
    legalMoves=legalMoves->next;
    delete temp;
  }
  numMoves=0;
  
  return beta;
}

//Mostly a copy function. Swaps player and enemy pieces.
void State::makeNextState(State & targetState)
{
  targetState.numPlayerPieces=numEnemyPieces;
  targetState.numEnemyPieces=numPlayerPieces;
  targetState.forward=-forward;
  
  targetState.playerPieces= new MyPiece[targetState.numPlayerPieces];
  for(int i=0; i<targetState.numPlayerPieces; i++)
  {
    
    targetState.playerPieces[i].captured=enemyPieces[i].captured;
    targetState.playerPieces[i].color=enemyPieces[i].color;
    targetState.playerPieces[i].rank=enemyPieces[i].rank;
    targetState.playerPieces[i].file=enemyPieces[i].file;
    targetState.playerPieces[i].type=enemyPieces[i].type;
    targetState.playerPieces[i].moved=enemyPieces[i].moved;
    targetState.playerPieces[i].pieceRef=enemyPieces[i].pieceRef;
  }
  
  
  targetState.enemyPieces= new MyPiece[targetState.numEnemyPieces];
  for(int i=0; i<targetState.numEnemyPieces; i++)
  {
    targetState.enemyPieces[i].captured=playerPieces[i].captured;
    targetState.enemyPieces[i].color=playerPieces[i].color;
    targetState.enemyPieces[i].rank=playerPieces[i].rank;
    targetState.enemyPieces[i].file=playerPieces[i].file;
    targetState.enemyPieces[i].type=playerPieces[i].type;
    targetState.enemyPieces[i].moved=playerPieces[i].moved;
    targetState.enemyPieces[i].pieceRef=playerPieces[i].pieceRef;
  }
  
  
  targetState.passant=passant;
  targetState.rankP=rankP;
  targetState.fileP=fileP;
  targetState.passantTarget=passantTarget;
  
  targetState.castleK=castleK;
  targetState.castleQ=castleQ;
  targetState.castlek=castlek;
  targetState.castleq=castleq;
  
  targetState.legalMoves=NULL;
  targetState.numMoves=0;
  
  targetState.boringPly=boringPly;
  for(int i=0; i<15; i++)
  {
    targetState.history[i].toRank=history[i].toRank;
    targetState.history[i].fromRank=history[i].fromRank;
    targetState.history[i].toFile=history[i].toFile;
    targetState.history[i].fromFile=history[i].fromFile;
    targetState.history[i].piece=history[i].piece;
  }
}

//Applies a move to the state
void State::updateState(MoveList* move)
{
  passant=false;
  
  //Passant update
  if(playerPieces[move->piece].type=="Pawn" && move->toRank==playerPieces[move->piece].rank+forward*2)
  {
    passant=true;
    rankP=move->toRank-forward;
    fileP=move->toFile;
    passantTarget=move->piece;
  }
  
  if(playerPieces[move->piece].type=="Pawn" || move->isCapture)
  {
    boringPly=0;
  }
  else
  {
    boringPly++;
  }
  
  if(move->promotionType!="" && playerPieces[move->piece].type=="Pawn")
  {
    playerPieces[move->piece].type=move->promotionType;
  }
  
  playerPieces[move->piece].file=move->toFile;//Update own information
  playerPieces[move->piece].rank=move->toRank;
  playerPieces[move->piece].moved=true;
  if(move->isCastle!=0)
  {
    playerPieces[move->rookIndex].file=playerPieces[move->piece].file[0]+move->isCastle;
  }
  

  if(move->isCapture)
  {
    enemyPieces[move->target].captured=true;
  }
  
  for(int i=15; i>0; i--)
  {
    history[i].toRank  =history[i-1].toRank;
    history[i].fromRank=history[i-1].fromRank;
    history[i].toFile  =history[i-1].toFile;
    history[i].fromFile=history[i-1].fromFile;
    history[i].piece   =history[i-1].piece;
  }
  
  history[0].toRank  =move->toRank;
  history[0].fromRank=move->fromRank;
  history[0].toFile  =move->toFile;
  history[0].fromFile=move->fromFile;
  history[0].piece   =move->piece;
  
  
}

//Checks if a draw has occurred
bool State::checkDraw()
{
  //stalemate
  if(numMoves==0 && ! checkKing())
  {
    return true;
  }
  //Insufficient Materials
  int pBish, eBish, knight, pBColor, eBColor, other;
  pBish =0;
  eBish =0;
  knight=0;
  other =0;
  for(int i=0; i<numPlayerPieces; i++)
  {
    if(playerPieces[i].captured!=true)
    {
      if(playerPieces[i].type=="Pawn" && playerPieces[i].type=="Rook" && playerPieces[i].type=="Queen")
      {
        other++;
      }
      if(playerPieces[i].type=="Knight")
      {
        knight++;
      }
      if(playerPieces[i].type=="Bishop")
      {
        pBish++;
        pBColor= (playerPieces[i].rank + (playerPieces[i].file[0]-'a'))%2;
      }
    }
  }
  for(int i=0; i<numEnemyPieces; i++)
  {
    if(enemyPieces[i].captured!=true)
    {
      if(enemyPieces[i].type=="Pawn" && enemyPieces[i].type=="Rook" && enemyPieces[i].type=="Queen")
      {
        other++;
      }
      if(enemyPieces[i].type=="Knight")
      {
        knight++;
      }
      if(enemyPieces[i].type=="Bishop")
      {
        eBish++;
        eBColor= (enemyPieces[i].rank + (enemyPieces[i].file[0]-'a'))%2;
      }
    }
  }
  if(other==0)
  {
    if(knight==0 && eBish==0 && pBish==0)
    {
      return true;
    }
    if(knight==1 && eBish==0 && pBish==0)
    {
      return true;
    }
    if(knight==0 && eBish==1 && pBish==0)
    {
      return true;
    }
    if(knight==0 && eBish==0 && pBish==1)
    {
      return true;
    }
    if(knight==0 && eBish==1 && pBish==1 && eBColor==pBColor)
    {
      return true;
    }
  }
  //50 move rule
  if(boringPly>=100)
  {
    return true;
  }
  
  bool repeat=true;
  //Simple repetition
  if(boringPly>=8)
  {
    for( int i=0; i<4; i++)
    {
      if( enemyPieces[history[i].piece].type!=enemyPieces[history[i+4].piece].type|| history[i].toRank!=history[i+4].toRank || history[i].toFile!=history[i+4].toFile || history[i].fromRank!=history[i+4].fromRank || history[i].fromFile!=history[i+4].fromFile)
      {
        repeat=false;
      }
    }
    if(repeat)
    {
      return true;
    }
  }
  
  return false;
}



}
}