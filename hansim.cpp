/* 
 This file is part of HanSim (copyright GRWC Han Group 2014).

  HanSim is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  HanSim is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with HanSim.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hansim.hpp"

using namespace std;

// Called when an instance of the game is created.
// Creates the player objects.
// Initiates a deck, which includes shuffling.
HanSim::HanSim()
: mNumPlayers(5), mCurPlayer(0), mHints(8), mStrikes(3)
{
  for(int i=0; i<mNumPlayers; i++)
    mPlayer[i] = new Player(this,i);
  
  initDeck();
}

// Called when an instance of the game is ended.
HanSim::~HanSim()
{
  for(int i=0; i<mNumPlayers; i++)
    delete mPlayer[i];
}

void HanSim::initDeck()
{
  mDeckTop = 0;
  mDiscardSize = 0;
  mPlayedSize = 0;
  mUsedSize = 0;
  
  for(int i=0; i<5; i++)
   mBoardPos[i] = -1;
  
  //Adds all cards to the deck
  Card t_card;
  int cur_card = 0;
  int mult;
  for(int s = 0; s < 5; s++)
  {
    for(int c = 0; c < 5; c++)
    {
      t_card.suit = s;
      t_card.rank = c;
      
      if(c == 0)
        mult = 3;
      else if(c < 4)
        mult = 2;
      else
        mult = 1;
    
      for(int i=0; i<mult; i++)
        mDeck[cur_card++] = t_card;
    }
  }
 
  //Shuffles the deck we just made
  shuffleDeck();
  
 //Deals cards to players
  for(int i=0; i<mNumPlayers; i++)
  {
    for(int j=0; j<4; j++)
    {
      mHands[i][j] = mDeck[mDeckTop];
      mDeckTop++;
    }
    mHandSize[i] = 4;
  }
}

void HanSim::shuffleDeck()
{
  Card tmp_card;
  int tmp_index;

  for(int j=49; j>=0; j--)
  {
    tmp_index = drand48() * (j+1);
    tmp_card = mDeck[tmp_index];
    mDeck[tmp_index] = mDeck[j];
    mDeck[j] = tmp_card;
  }
}


// Plays the full instance of a game
int HanSim::play(bool display, bool to_file)
{

  //------------Setup for the output---------------------
  
  // Use "out" as an abstract way to allow
  // us to output to files and the screen 
  // through the one variable
  
  ostream* out;
  
  // fout stores the file output object
  ofstream fout;
 
  // If the user wants to output to a file, set out = fout
  if(display && to_file)
  {
    fout.open("hansim_out.txt");
    out = &fout;
  }
  // Otherwise, the output will go to 
  // the screen and out = cout.
  else
  {
    out = &cout;
  }
  
  //============================================================
  //            Plays the full instance of the game
  //============================================================
  
  //====================Start of game setup=====================
    
  int final_turns = 4; // After the last card in the deck has been picked up, we want 5 more turns to run, so final turns will decreased once each turn
  mHints = 8;
  mCurPlayer = 0;
  mTurnNumber = 0;
  
  //initializes players-----------------------
  for(int i=0; i<mNumPlayers; i++)
    mPlayer[i]->init();
  
  
  
  //Plays the full instance of the game
  while(1)
  {

    // output board
    if(display)
    {
      output_line(*out);
      *out << "Turn: " << mTurnNumber << "\t\t Time: " << mHints;
      *out << "\t\t Strikes: " << mStrikes << endl << endl;
      output_board(*out);
    }
    mTurnNumber++;
    
    // ends the game if all strikes have been used of final turns are depleted
    if(final_turns == -1 || mStrikes < 0)
      break;
    else if(mDeckTop >= 50)
      final_turns--;
    
    // beginning of the turn (players can now think based upon what has happened up to this point)
    mTurnPosition = TURN_BEGIN;
    mTurnHistory[mTurnNumber].position = TURN_BEGIN;
    mTurnHistory[mTurnNumber].player = mCurPlayer;
    
    for(int i=0; i<mNumPlayers; i++)
      mPlayer[i]->preturnUpdate();
      
      
    // middle of the turn (active player is called upon to make a decision)
    mTurnPosition = TURN_MIDDLE;
    mTurnHistory[mTurnNumber].position = mTurnPosition;
   
    int action, card, hint_to, hint_type, hint_value, outcome;
    action = card = hint_to = hint_type = hint_value = outcome = UNKNOWN; 
    
    mPlayer[mCurPlayer]->act(action, card, hint_to, hint_type, hint_value);
    
    // check if player picked one of the three possible actions
    if(action != ACT_PLAY && action != ACT_DISCARD && action != ACT_HINT)
    {
      cerr << "ILLEGAL ACTION CODE" << endl; 
      return 0;                                                          
    }
    
    // check if player picked a legal index
    if( (action == ACT_PLAY || action == ACT_DISCARD) 
            && (card >= mHandSize[mCurPlayer] || card<0) )
    {
      cerr << "ILLEGAL CARD INDEX" << endl; 
      return 0;                                                          
    }
    
    //-------checks if hint is legal and creates hint vector------------
    if(action == ACT_HINT)
    {
      int legal_hint = false;
      // looks to see if at least one card matches the hint
      for(int c = 0; c < mHandSize[hint_to] ; c++)
      {
        if(hint_type == RANK)
        {
          if(mHands[hint_to][c].rank == hint_value)
          {
            mTurnHistory[mTurnNumber].hint_info[c]=1;
            legal_hint = true;
          }
          else
            mTurnHistory[mTurnNumber].hint_info[c]=0;
        }
        if(hint_type == SUIT)
        {
          if(mHands[hint_to][c].suit == hint_value)
          {
            mTurnHistory[mTurnNumber].hint_info[c]=1;
            legal_hint = true;
          }
          else
          {
            mTurnHistory[mTurnNumber].hint_info[c]=0;
          }
        }
      }                                                          
      if (legal_hint == false)
      {
        cerr << "ILLEGAL HINT, NOT MATCHING CARDS" << endl; 
        return 0;                                                          
      }
      
      if(hint_to ==  mCurPlayer)
      {
        cerr << "ILLEGAL HINT, TARGETED SELF" << endl; 
        return 0;       
      }
     
      if (mHints < 1)
      {
        cerr << "ILLEGAL HINT, NOT TIME" << endl; 
        return 0;                                                          
      }
    }

    //-------Resolves the action picked by the player-----------
    
    mTurnHistory[mTurnNumber].action = action;
    mTurnHistory[mTurnNumber].card = card;
    mTurnHistory[mTurnNumber].hint_to = hint_to;
    mTurnHistory[mTurnNumber].hint_type = hint_type;
    mTurnHistory[mTurnNumber].hint_value = hint_value;
    
    // Game now decides what outcome will be
    // Does not implement action yet and
    // does not check to verify that it is valid
    if(action == ACT_DISCARD)
      outcome = OUT_DISCARD;
    
    if(action == ACT_PLAY)
    {
      Card tmp_card;
      tmp_card = mHands[mCurPlayer][card];
      
      if(mBoardPos[tmp_card.suit] != tmp_card.rank-1)
      {
        outcome = OUT_STRIKE;
      }
      else
        outcome = OUT_PLAY;
    }
    
    if(action == ACT_HINT)
        outcome = OUT_HINT;
    
    mTurnHistory[mTurnNumber].outcome = outcome;
    
    
    //--------------Declaration Update------------------
    for(int i=0; i<mNumPlayers; i++)
    {
      mPlayer[i]->declarationUpdate();
    }

    
    // Implements Action
    mTurnPosition = TURN_END;
    mTurnHistory[mTurnNumber].position = mTurnPosition;

    if(outcome == OUT_DISCARD)
    {
      // discard card and reorder hand:
      useCard(mCurPlayer, card, false);
      mHints++;
      if(mHints > 8)
        mHints = 8;
    }
    
    if(outcome == OUT_STRIKE)
    {
      mStrikes--;
      useCard(mCurPlayer,card,false);
    }

    if(outcome == ACT_PLAY)
    {
      Card tmp_card;
      tmp_card = mHands[mCurPlayer][card];
      
      mBoardPos[tmp_card.suit]++;
      if(mBoardPos[tmp_card.suit] == 4)
        if(mHints < 8)
          mHints++;
      useCard(mCurPlayer,card,true);
    }

    if(outcome == ACT_HINT)
      mHints--;
            
    //--------------Resolution Update------------------
    for(int i=0; i<mNumPlayers; i++)
    {
      mPlayer[i]->resolutionUpdate();
    }    
    
    // Outputs what happened this turn
    if(display)
    {
      *out << endl;
      *out << endl;
      *out << "Player " << mCurPlayer << ": ";
      if(mTurnHistory[mTurnNumber].outcome == OUT_PLAY)
      {
        *out << "Successfully played suit" << hint_value << endl;
      }
      else if(mTurnHistory[mTurnNumber].outcome == OUT_STRIKE)
      {
        *out << "Strike" << endl;
      }
      else if(mTurnHistory[mTurnNumber].outcome == OUT_HINT)
      {
        *out << "Gave hint (";
        if(hint_type == SUIT)
          *out << "*," << hint_value << ")";
        else
          *out << hint_value << ",*)";
        *out << " to player " << hint_to << endl;
      }
      else if(mTurnHistory[mTurnNumber].outcome == OUT_DISCARD)
      {
        *out << "Discarded a card" << endl;
      }
      else
      {
        *out << "Something else happened?" << endl;
      }
      *out << "--------------------------------------";
      *out << endl;
    }
        
    // increment:
    mCurPlayer  = (mCurPlayer+1)%mNumPlayers;
  }
  
  
  // game finished:
  int score = 0;
  for(int i=0; i<5; i++)
    score += mBoardPos[i]+1;
  if(display)
  {
    *out << endl;
    *out << "Score: " << score << endl;
    *out << "====================================";
    *out << "====================================";
    *out << "====================================";
    *out << "====================================";
    *out << "=============" << endl << endl << endl;
  }
  if(display && to_file)
  {
    fout.close();
  }
  return score;
}

Card HanSim::useCard(int player, int card, bool played)
{
  Card new_card;
  
  mUsed[mUsedSize++] = mHands[player][card];
  if(played)
    mPlayed[mPlayedSize++] = mHands[player][card];
  else
    mDiscard[mDiscardSize++] = mHands[player][card];
  
  for(int i=card; i<4-1; i++)
  {
    mHands[player][i] = mHands[player][i+1];
  }
  if(mDeckTop < 50)
  {
    mHands[player][3] = mDeck[mDeckTop++];
    new_card =  mHands[player][3];
  }
  else 
  {
    mHandSize[player]--;
    new_card = Card(-1,-1);
  }

  return new_card;
}

Card HanSim::getCard(int player, int card)
{
  // The next two lines prevent cheating,
  // if you comment them out, you can cheat.
  
  if(player == mCurPlayer) 
    return Card(-1,-1);

  return mHands[player][card];
}

Card HanSim::getDiscard(int card)
{
  if(card >= mDiscardSize)
    return Card(-1,-1);

  return mDiscard[card];
}

int HanSim::getDiscardSize()
{
  return mDiscardSize;
}

Card HanSim::getPlayedCard(int card)
{
  if(card >= mPlayedSize)
    return Card(-1,-1);

  return mPlayed[card];
}

int HanSim::getPlayedSize()
{
  return mPlayedSize;
}

Card HanSim::getUsedCard(int card)
{
  if(card >= mUsedSize)
    return Card(-1,-1);

  return mUsed[card];
}

int HanSim::getUsedSize()
{
  return mUsedSize;
}

int HanSim::getBoardPos(int suit)
{
  return mBoardPos[suit];
}

int HanSim::getNumPlayers()
{
  return mNumPlayers;
}

void HanSim::output_board(ostream& out)
{
  out << "Turn:" << mTurnNumber << "   Hints:" << mHints << "   Strikes:" << mStrikes << "   Discards:";
  out << getDiscardSize();
  out << endl;
    
           
  out << "     ";
  for(int i=0; i<5; i++)
  {
    if(mBoardPos[i] >= 0)
      out << mBoardPos[i];
    else 
      out << "-";
    out << i << "   ";
  }
  out << endl << endl;
  

  for(int i=0; i<mNumPlayers; i++)
  {
    out << "Player " << i << "'s hand:  ";
    for(int j=0; j<mHandSize[i]; j++)
      out << mHands[i][j].rank << mHands[i][j].suit << ' ';
    out << endl;
    out << "Player " << i << "'s state: ";
    mPlayer[i]->printState(out);
    out << endl;
  }
  
  for(int i=0; i<mDiscardSize; i++)
    out << mDiscard[i].rank << mDiscard[i].suit << ',';
}

void HanSim::output_line(ostream& out)
{
  for(int i=0; i<50; i++)
    out << '=';
  out << endl;
}

int HanSim::getTime()
{
  return mHints;
}

int HanSim::getHandsize(int player)
{
  return mHandSize[player];
}

int HanSim::getTurnNumber()
{
  return mTurnNumber;
}

int HanSim::getTurnPosition()
{
  return mTurnPosition;
}

int HanSim::getNumStrikes()
{
  return mStrikes;
}

int HanSim::getCurPlayer()
{
  return mCurPlayer;
}

Turn HanSim::getTurnHistory(int turn)
{
  return mTurnHistory[turn];
}
