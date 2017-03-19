// AI
// This is where you build your AI

#include "ai.hpp"

// You can add #includes here for your AI.

namespace cpp_client
{

namespace chess
{

/// <summary>
/// This returns your AI's name to the game server.
/// Replace the string name.
/// </summary>
/// <returns>The name of your AI.</returns>
std::string AI::get_name() const
{
  // REPLACE WITH YOUR TEAM NAME!
  return "chess_john_brna";
}

/// <summary>
/// This is automatically called when the game first starts, once the game objects are created
/// </summary>
void AI::start()
{
  // This is a good place to initialize any variables
  srand(time(NULL));
  
  currentState= new State();
  currentState->numPlayerPieces=player->pieces.size();
  currentState->playerPieces= new MyPiece[currentState->numPlayerPieces];
  for(int i=0; i<currentState->numPlayerPieces; i++)
  {
    
    currentState->playerPieces[i].captured=player->pieces[i]->captured;
    currentState->playerPieces[i].color=player->pieces[i]->owner->color;
    currentState->playerPieces[i].rank=player->pieces[i]->rank;
    currentState->playerPieces[i].file=player->pieces[i]->file;
    currentState->playerPieces[i].type=player->pieces[i]->type;
    currentState->playerPieces[i].moved=player->pieces[i]->has_moved;
    currentState->playerPieces[i].pieceRef=player->pieces[i];
  }
  
  currentState->numEnemyPieces=player->opponent->pieces.size();
  currentState->enemyPieces= new MyPiece[currentState->numEnemyPieces];
  for(int i=0; i<currentState->numEnemyPieces; i++)
  {
    
    currentState->enemyPieces[i].captured=player->opponent->pieces[i]->captured;
    currentState->enemyPieces[i].color=player->opponent->pieces[i]->owner->color;
    currentState->enemyPieces[i].rank=player->opponent->pieces[i]->rank;
    currentState->enemyPieces[i].file=player->opponent->pieces[i]->file;
    currentState->enemyPieces[i].type=player->opponent->pieces[i]->type;
    currentState->enemyPieces[i].moved=player->opponent->pieces[i]->has_moved;
    currentState->enemyPieces[i].pieceRef=player->opponent->pieces[i];
  }
  
  if(player->color=="White")
  {
    currentState->forward=1;
  }
  else
  {
    currentState->forward=-1;
  }
  
  currentState->passant=false;
  currentState->fen(game->fen);
}

/// <summary>
/// This is automatically called the game (or anything in it) updates
/// </summary>
void AI::game_updated()
{
  // If a function you call triggers an update this will be called before it returns.
}

/// <summary>
/// This is automatically called when the game ends.
/// </summary>
/// <param name="won">true if you won, false otherwise</param>
/// <param name="reason">An explanation for why you either won or lost</param>
void AI::ended(bool won, const std::string& reason)
{
  delete[] currentState->enemyPieces;
  delete[] currentState->playerPieces;
  delete currentState;
  // You can do any cleanup of your AI here.  The program ends when this function returns.
}

/// <summary>
/// This is called every time it is this AI.player's turn.
/// </summary>
/// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
bool AI::run_turn()
{
  
  if(game->moves.size() > 0) //Get updates based on last move
  {
    //  game->moves[game->moves.size() - 1]->san ;
    int e=0;
    bool found=false;
    while(e<currentState->numEnemyPieces && ! found)
    {
      if(currentState->enemyPieces[e].captured==false && currentState->enemyPieces[e].rank==game->moves[game->moves.size() - 1]->from_rank && currentState->enemyPieces[e].file==game->moves[game->moves.size() - 1]->from_file)
      {
        found=true;
      }
      else
      {
        e++;
      }
    }
    
    int cap;
    if(1==currentState->checkSpace(game->moves[game->moves.size() - 1]->to_rank, game->moves[game->moves.size() - 1]->to_file[0], cap))
    {
      currentState->playerPieces[cap].captured=true;
    }
    
    if(currentState->passant && game->moves[game->moves.size() - 1]->to_rank==currentState->rankP && game->moves[game->moves.size() - 1]->to_file==currentState->fileP)
    {
      currentState->playerPieces[currentState->passantTarget].captured=true;
    }
    
    currentState->passant=false;
    
    //En Passant availability detection.
    int rankDif=game->moves[game->moves.size() - 1]->to_rank-game->moves[game->moves.size() - 1]->from_rank;
    if(game->moves[game->moves.size() - 1]->piece->type=="Pawn" && (rankDif==-2 || rankDif==2))
    {
      currentState->passant=true;
      currentState->rankP=game->moves[game->moves.size() - 1]->to_rank+currentState->forward;
      currentState->fileP=game->moves[game->moves.size() - 1]->to_file;
      currentState->passantTarget=e;
    }
    
    //Castle detection
    int fileDif=game->moves[game->moves.size() - 1]->to_file[0]-game->moves[game->moves.size() - 1]->from_file[0];
    if(game->moves[game->moves.size() - 1]->piece->type=="King" && (fileDif==-2 || fileDif==2))
    {
      bool rookFound=false;
      int r=0;
      while(!rookFound && r<currentState->numEnemyPieces)
      {
        bool direction;
        if(fileDif>0) //If King moved right, need a rook to the right.
        {
          direction=(currentState->enemyPieces[r].file[0]> game->moves[game->moves.size() - 1]->from_file[0]);
        }
        else //If King moved left, need a rook to the left.
        {
          direction=(currentState->enemyPieces[r].file[0]< game->moves[game->moves.size() - 1]->from_file[0]);
        }
        if(currentState->enemyPieces[r].type=="Rook" && currentState->enemyPieces[r].moved==false && direction && currentState->enemyPieces[r].rank==currentState->enemyPieces[e].rank)
        {
          rookFound=true;
        }
        else
        {
          r++;
        }
      }
      currentState->enemyPieces[r].moved=true;
      currentState->enemyPieces[r].file=game->moves[game->moves.size() - 1]->to_file[0]-(fileDif/2);
    }
    
    currentState->enemyPieces[e].rank=game->moves[game->moves.size() - 1]->to_rank;
    currentState->enemyPieces[e].file=game->moves[game->moves.size() - 1]->to_file;
    currentState->enemyPieces[e].moved=true;
    if(game->moves[game->moves.size() - 1]->promotion!="")
    {
      currentState->enemyPieces[e].type=game->moves[game->moves.size() - 1]->promotion;
    }
    
    
    
    
  }

  
  currentState->genMoves(); //Generate all moves
  
  /*
  //Random Move Selection
  int randPiece=rand()%currentState->numPlayerPieces; //Select a piece
  while(currentState->playerPieces[randPiece].numMoves==0 || currentState->playerPieces[randPiece].captured==true)
  {
    randPiece=rand()%currentState->numPlayerPieces;
  }
  int randMove=rand()%currentState->playerPieces[randPiece].numMoves; //Select a move
  //cout<<"Selected: "<<randPiece<<" "<<randMove<<" of "<<currentState->playerPieces[randPiece].numMoves<<endl;
  MoveList * selectedMove= currentState->playerPieces[randPiece].legalMoves; //Find selected move
  for( int i=0; i<randMove; i++)
  {

    selectedMove=selectedMove->next;
  }
  */
  
  /*
  //Prints all moves for selected Piece. OUTDATED.
  MoveList * printMove= currentState->playerPieces[randPiece].legalMoves;
  cout<<currentState->playerPieces[randPiece].type<<" at "<<currentState->playerPieces[randPiece].rank<<" "<<currentState->playerPieces[randPiece].file<<" could move to ";
  while(printMove != NULL)
  {
    if(printMove->promotionType!="")
    {
      cout<<printMove->toRank<<" "<<printMove->toFile<<" (promotes to "<<printMove->promotionType<<"), ";
    }
    else
    {
      cout<<printMove->toRank<<" "<<printMove->toFile<<", ";
    }
    
    printMove=printMove->next;
  }
  cout<<endl;
  */
  
  /*
  //Outputs piece locations
  for( int i=0; i<currentState->numPlayerPieces; i++)
  {
    if(currentState->playerPieces[i].captured!=true)
    {
      cout<<"My "<<currentState->playerPieces[i].type<<" at "<<currentState->playerPieces[i].rank<<" "<<currentState->playerPieces[i].file<<endl;
    }
  }
  
  for( int i=0; i<currentState->numEnemyPieces; i++)
  {
    if(currentState->enemyPieces[i].captured!=true)
    {
      cout<<"Enemy "<<currentState->enemyPieces[i].type<<" at "<<currentState->enemyPieces[i].rank<<" "<<currentState->enemyPieces[i].file<<endl;
    }
  }*/
  
  currentState->passant=false;
  /*
  //Send move to game
  if(selectedMove->promotionType!="")
  {
    currentState->playerPieces[randPiece].pieceRef->move(selectedMove->toFile, selectedMove->toRank, selectedMove->promotionType);
    currentState->playerPieces[randPiece].type=selectedMove->promotionType;
  }
  else
  {
    currentState->playerPieces[randPiece].pieceRef->move(selectedMove->toFile, selectedMove->toRank);
  }
  */
  
  /*
  //Passant update
  if(currentState->playerPieces[randPiece].type=="Pawn" && selectedMove->toRank==currentState->playerPieces[randPiece].rank+currentState->forward*2)
  {
    currentState->passant=true;
    currentState->rankP=selectedMove->toRank-currentState->forward;
    currentState->fileP=selectedMove->toFile;
    currentState->passantTarget=randPiece;
  }
  
  currentState->playerPieces[randPiece].file=selectedMove->toFile;//Update own information
  currentState->playerPieces[randPiece].rank=selectedMove->toRank;
  currentState->playerPieces[randPiece].moved=true;
  if(selectedMove->isCastle!=0)
  {
    currentState->playerPieces[selectedMove->rookIndex].file=currentState->playerPieces[randPiece].file[0]+selectedMove->isCastle;
  }
  

  if(selectedMove->isCapture)
  {
    currentState->enemyPieces[selectedMove->target].captured=true;
  }
  
  */
  
  /*
  MoveList * temp;
  
  for( int i=0; i<currentState->numPlayerPieces; i++)
  {
    while(currentState->playerPieces[i].legalMoves != NULL)
    {
      temp=currentState->playerPieces[i].legalMoves;
      currentState->playerPieces[i].legalMoves=currentState->playerPieces[i].legalMoves->next;
      delete temp;
    }
    currentState->playerPieces[i].numMoves=0;
  }*/
  
  
  return true; // to signify we are done with our turn.
  
}

/// <summary>
///  Prints the current board using pretty ASCII art
/// </summary>
/// <remarks>
/// Note: you can delete this function if you wish
/// </remarks>
void AI::print_current_board()
{
  for(int rank = 9; rank >= -1; rank--)
  {
    std::string str = "";
    if(rank == 9 || rank == 0) // then the top or bottom of the board
    {
      str = "   +------------------------+";
    }
    else if(rank == -1) // then show the ranks
    {
      str = "     a  b  c  d  e  f  g  h";
    }
    else // board
    {
      str += " ";
      str += std::to_string(rank);
      str += " |";
      // fill in all the files with pieces at the current rank
      for(int file_offset = 0; file_offset < 8; file_offset++)
      {
        std::string file(1, 'a' + file_offset); // start at a, with with file offset increasing the char;
        chess::Piece current_piece = nullptr;
        for(const auto& piece : game->pieces)
        {
          if(piece->file == file && piece->rank == rank) // then we found the piece at (file, rank)
          {
            current_piece = piece;
            break;
          }
        }

        char code = '.'; // default "no piece";
        if(current_piece != nullptr)
        {
          code = current_piece->type[0];

          if(current_piece->type == "Knight") // 'K' is for "King", we use 'N' for "Knights"
          {
            code = 'N';
          }

          if(current_piece->owner->id == "1") // the second player (black) is lower case. Otherwise it's upppercase already
          {
            code = tolower(code);
          }
        }

        str += " ";
        str += code;
        str += " ";
      }

      str += "|";
    }

    std::cout << str << std::endl;
  }
}

// You can add additional methods here for your AI to call

} // chess

} // cpp_client
