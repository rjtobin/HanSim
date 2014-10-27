/* 
  This file is part of HanSim (copyright GRWC Han Group 2014).

  Foobar is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Foobar is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with HanSim.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hansim.hpp"

using namespace std;

HanSim::HanSim()
: mNumPlayers(5), mCurPlayer(0), mHints(8), mStrikes(3)
{
  for(int i=0; i<mNumPlayers; i++)
    mPlayer[i] = new Player(this, i);
  
  initDeck();
}

HanSim::~HanSim()
{
  for(int i=0; i<mNumPlayers; i++)
    delete mPlayer[i];
}

void HanSim::initDeck()
{
  Card t_card;
  
  int cur_card = 0;
  int mult;
  for(int col = 0; col < 5; col++)
  {
    for(int card = 0; card < 5; card++)
    {
      t_card.color = col;
      t_card.rank = card;
      
      if(card == 0)
        mult = 3;
      else if(card  < 4)
        mult = 2;
      else
        mult = 1;
    
      for(int i=0; i<mult; i++)
        mDeck[cur_card++] = t_card;
    }
  }
  
  mDeckTop = 0;
  mDiscardSize = 0;
  
  shuffleDeck();
  
  for(int i=0; i<mNumPlayers; i++)
  {
    for(int j=0; j<4; j++)
    {
      mHands[i][j] = mDeck[mDeckTop];
      mDeckTop++;
    }
    mHandSize[i] = 4;
  }
  
  for(int i=0; i<5; i++)
    mBoardPos[i] = -1;
}

void HanSim::shuffleDeck()
{
  Card tmp_card;
  int tmp_index;
  for(int i=0; i<1000; i++)
  {
    for(int j=0; j<50; j++)
    {
      tmp_index = drand48() * 50;
      tmp_card = mDeck[tmp_index];
      mDeck[tmp_index] = mDeck[j];
      mDeck[j] = tmp_card;
    }
  }
}

//void HanSim::shuffleDeck()
//{
//  Card newDeck[60];
//
//  int numbers [60];
// for (int i = 0; i < 60; i++)
//  {
//      numbers[i]=drand48();
//  }
//
//  for (int i = 0; i < 60; i++)
//  {
//    int min_value = 10;
//    int min_index = 0;
//   for (int j = 0; j<60; j++)
//   {
//     if (numbers[j]<min_value)
//      {
//            min_value = numbers[j];
//            min_index = j;
//      }
//    }
//    newDeck[i]=mDeck[min_index];
//    numbers[min_index]=100;
//  }
//  for (int i = 0; i<60; i++)
//   {
//    mDeck[i]=newDeck[i];
//    }
//}


/*
 This function needs tidying.
*/
int HanSim::play(bool display, bool to_file)
{
  // Use "out" as an abstract way to allow
  // us to output to files and the screen 
  // through the one variable... it's 
  // pretty horrible code though :(
  
  ostream* out;
  ofstream fout;
  if(display && to_file)
  {
    //ofstream fout;
    fout.open("hansim_out.txt");
    out = &fout;
  }
  else
  {
    out = &cout;
  }

  initDeck();
  mCurPlayer = 0;
  mTurnNumber = 0;
  int action, arg1, arg2, arg3;
  
  int outcome, rarg1, rarg2, rarg3;
  
  Card tmp_card, new_card;
  
  int final_turns = 4;
  
  for(int i=0; i<mNumPlayers; i++)
    mPlayer[i]->init();
  
  while(1)
  {
    // output board:
    if(display)
    {
      output_line(*out);
      *out << "Turn: " << mTurnNumber++ << "\t\t Time: " << mHints << "\t\t Strikes: " << mStrikes << endl << endl;
      output_board(*out);
    }
    
    // init:
    if(final_turns == 0 || mStrikes < 0)
      break;
    else if(mDeckTop >= 50)
      final_turns--;
    outcome = -1;
  
    // preturn update:
    for(int i=0; i<mNumPlayers; i++)
      mPlayer[i]->preturnUpdate(mCurPlayer);
  
    // play:
    
    mPlayer[mCurPlayer]->play(action, arg1, arg2, arg3); // pass data
    
    // process:
    
    if(action == ACT_DISCARD)
    {
      // discard card and reorder hand:
      
      new_card = discard(mCurPlayer, arg1);
    
      outcome = 3;
      mHints++;
      if(mHints > 8)
        mHints = 8;
    }
    else if(action == ACT_PLAY)
    {
      tmp_card = mHands[mCurPlayer][arg1];
      
      // check if player picked a card index too big 
      if(arg1 >= mHandSize[mCurPlayer]) 
      {
        // eg. if the player wants to play card four but they
        //     only have 3 cards in their hand, play card 0 instead
        arg1 = 0;
      }
      
      if(mBoardPos[tmp_card.color] != tmp_card.rank-1)
      {
        outcome = 1;
        mStrikes--;
        new_card = discard(mCurPlayer,arg1);
        cerr << "ILLEGAL MOVE" << endl;
        //mPlayer[mCurPlayer]->updateHand(arg1, new_card);
      }
      else
      {
        outcome = 0;
        rarg1 = tmp_card.color;
        mBoardPos[tmp_card.color]++;
        new_card = discard(mCurPlayer,arg1);
        //mPlayer[mCurPlayer]->updateHand(arg1, new_card);
      }
    }
    else 
    {
      if(action != ACT_HINT)
      {
        cerr << "ILLEGAL ACTION CODE: assuming 'hint' instead" << endl; 
        action = ACT_HINT;
      }

      if(mHints < 0)
      {
        cerr << "ILLEGAL MOVE: hint with no time" << endl;
        return 0; 
      }
      
      else
      {
        mHints--;
      
        outcome = 2;
       
        //int gcolor=-1, gcard=-1;
        
        //if(arg2 == 0)
        //  gcolor = arg3;
        //else
        //  gcard = arg3;
      
        //for(int i=0; i<mHandSize[arg1]; i++)
        //{        
        // if(mHands[arg1][i].color == gcolor 
        //      || mHands[arg1][i].rank == gcard)
        //  {
        //    mPlayer[arg1]->hint(i,Card(gcolor,gcard));
        //  }
        //}
        // do rargs here
        rarg1 = arg1;
        rarg2 = arg2;
        rarg3 = arg3;
      }
    }
    
    // update players:

    for(int i=0; i<mNumPlayers; i++)
    {
      mPlayer[i]->update(mCurPlayer, outcome, rarg1, rarg2, rarg3);
    }

    // pass the hint to the relevant player:
    if(action == ACT_HINT)
    {
      int gcolor=-1, gcard=-1;
        
      if(arg2 == 0)
        gcolor = arg3;
      else
        gcard = arg3;
      
      for(int i=0; i<mHandSize[arg1]; i++)
      {        
       if(mHands[arg1][i].color == gcolor 
              || mHands[arg1][i].rank == gcard)
          {
            mPlayer[arg1]->hint(i,Card(gcolor,gcard));
          }
        }
    }

    // output actions stuff:
    if(display)
    {
      *out << "Player " << mCurPlayer << ": ";
      if(outcome == 0)
      {
        *out << "Successfully played color " << rarg1 << endl;
      }
      else if(outcome == 1)
      {
        *out << "Strike" << endl;
      }
      else if(outcome == 2)
      {
        *out << "Gave hint (";
        if(arg2 == 0)
          *out << arg3 << ",*)";
        else
          *out << "*," << arg3 << ")";
        *out << " to player " << arg1 << endl;
        *out << arg1 << endl;
      }
      else if(outcome == 3)
      {
        *out << "Discarded a card" << endl;
      }
      else
      {
        *out << "Something else happened?" << endl;
      }
    }
        
    // increment:
    
    mCurPlayer  = (mCurPlayer+1)%mNumPlayers;
  }
  
  // game finished:
  int score = 0;
  for(int i=0; i<5; i++)
    score += mBoardPos[i]+1;
  if(display)
    *out << "Score: " << score << endl;
  if(display && to_file)
  {
    fout.close();
  }
  return score;
}

Card HanSim::discard(int player, int card)
{
  Card new_card;
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
  
  for(int i=0; i<mNumPlayers; i++)
  {
    mPlayer[i]->updateHand(player, card);
  }
  return new_card;
}

Card HanSim::getCard(int player, int card)
{
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

int HanSim::getBoardPos(int color)
{
  return mBoardPos[color];
}

int HanSim::getNumPlayers()
{
  return mNumPlayers;
}

void HanSim::output_board(ostream& out)
{
  for(int i=0; i<5; i++)
  {
    out << i;
    if(mBoardPos[i] >= 0)
      out << mBoardPos[i] << ' ';
    else 
      out << "- ";
  }
  out << endl << endl;
  
  for(int i=0; i<mNumPlayers; i++)
  {
    out << "Player " << i << "'s hand: ";
    for(int j=0; j<mHandSize[i]; j++)
      out << mHands[i][j].color << mHands[i][j].rank << ' ';
    out << endl;
    out << "Player " << i << " state: ";
    mPlayer[i]->printState(out);
    out << endl;
  }
  
  for(int i=0; i<mDiscardSize; i++)
    out << mDiscard[i].color << mDiscard[i].rank << ',';
  out << endl;
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

int HanSim::getNumStrikes()
{
  return mStrikes;
}
