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

#include "rec_player.hpp"
#include <iostream>
using namespace std;

//==============================================================================
//                              Initiates Player
//==============================================================================
void Player::init()
{
    for(int p = 0; p <5 ; p++)
        info[p]=8;
    info[5]=0;
    info[6]=0;
    info[7]=0;
}

//==============================================================================
//                              Start-of-turn Computation
//==============================================================================

void Player::preturnUpdate()
{
}

//==============================================================================
//                              Play Command
//==============================================================================

void Player::act(int& action, int& card, int& hint_to, int& hint_type, int& hint_value)
{
    //-------------PLAY (if told to play and no one has played)-----------------
    if(info[mPlayerIndex] < 4 && info[5] < 1){
      action = ACT_PLAY;
      card = info[mPlayerIndex];
      return;
    }

    //----------PLAY (if told to play and not yet 2 plays and strikes left)-----
    if(info[mPlayerIndex] < 4 && info[5] < 2 && mGame->getNumStrikes()>0){
      action = ACT_PLAY;
      card = info[mPlayerIndex];
      return;
    }
    
    //------------HINT (if a play has not yet been made)-------------------------
    if (mGame->getTime() > 0){
        int sum = 0;
        for(int p = 0; p < 5; p++){
          if(mPlayerIndex != p)
          {
              sum += getNumber(p);
          }
        }
        sum %= 8;

        action = ACT_HINT;
        hint_to = (sum%4+mPlayerIndex+1)%5;
        if (sum < 4)
            hint_type = RANK;
        else
            hint_type = SUIT;

        //no info passed through hint_value
        if (hint_type == RANK)
            hint_value = mGame->getCard(hint_to,0).rank;
         if (hint_type == SUIT)
            hint_value = mGame->getCard(hint_to,0).suit;               
        return;
    }
     
     //-------------Discard (if told to discard)-------------------------------
    if(3 < info[mPlayerIndex] && info[mPlayerIndex] < 8){
      action = ACT_DISCARD;
      card = info[mPlayerIndex]-4;
      return;
    }
    
    //--------DISCARD RANDOM (if nothing better)----------------------------
    action = ACT_DISCARD;
    card = 3;
    return;
}
    
//==============================================================================
//                              DeclarationUpdate
//==============================================================================
void Player::declarationUpdate()
{
  int turnNumber = mGame->getTurnNumber();
  Turn turn = mGame->getTurnHistory(turnNumber);
  
  int p_turn = turn.player;
  int action = turn.action;
  int card = turn.card;
  int hint_to = turn.hint_to;
  int hint_type = turn.hint_type;
  int hint_value = turn.hint_value; 
  // This all the information we use, although players also have access to
  // the get methods in hansim (as well as previous turn history)
    
    if (action == OUT_HINT){
        
        int sumPassed = 0;
        sumPassed = ((25+hint_to - p_turn)%5)-1;
        if (hint_type == SUIT)
            sumPassed += 4;

        int obsSum = 0;
        for (int p=0; p<5; p++){
            if (p != p_turn && p != mPlayerIndex){
                info[p]=getNumber(p);
                obsSum += getNumber(p);
            }
        }
        
        obsSum %= 8;

        info[p_turn]=8;
        if (p_turn != mPlayerIndex)
        info[mPlayerIndex]=(64 + sumPassed - obsSum)%8;
    
        info[5]=0;
        info[6]=0;
    }
}

//==============================================================================
//                              ResolutionUpdate
//==============================================================================
void Player::resolutionUpdate()
{
  
  int turnNumber = mGame->getTurnNumber();
  Turn turn = mGame->getTurnHistory(turnNumber);

  int p_turn = turn.player;
  int action = turn.action;
  int card = turn.card;
  int hint_to = turn.hint_to;
  int hint_type = turn.hint_type;
  int hint_value = turn.hint_value; 
  int outcome = turn.outcome; 
  int hint_info[4];
  for (int i = 0; i<4; i++){
      hint_info[i] = turn.hint_info[i]; 
  }
  // This all the information we use, although players also have access to
  // the get methods in hansim (as well as previous turn history)
  
  if (outcome == OUT_PLAY ){
      info[5]+= 1;
  }
  
    if (outcome == OUT_DISCARD || outcome == OUT_STRIKE){
      info[6]+= 1;
      info[7]+= 1;
  }
}


//==============================================================================
//                  Prints the State
//==============================================================================
void Player::printState(ostream& out)
{
    out << ' ';
 
    for(int i=0; i<8; i++)
    {
        cerr << info[i] << "  ";
    }
}

//==============================================================================
//                  Auxiliary functions 
//==============================================================================
int Player::getNumber(int p)
{
    // index stores the hint directed towards player p, initialized to 9
    // to indicate we have not decided on the hint yet
    int index = 9;
 
    int lowest_playable_number = 26;
    for(int c = 0; c<4; c++)
    {   
        int rank = mGame->getCard(p,c).rank;
        int suit = mGame->getCard(p,c).suit;
        int number = (5*rank)+suit;
            
        if(mGame->getBoardPos(suit) == rank-1)
        {
            if (number < lowest_playable_number)
            {
                lowest_playable_number = number;
                index = c;
            }
            if (mGame->getCard(p,c).rank == 4)
            {
                lowest_playable_number = -1;
                index = c;
            }
        }
    }
    
    if (index < 4)
    {
        return index;
    }

    int highest_dead_number = -1;
    for(int c = 0; c<4; c++)
    {   
        int rank = mGame->getCard(p,c).rank;
        int suit = mGame->getCard(p,c).suit;
        int number = (5*rank)+suit;
            
        if(mGame->getBoardPos(suit) > rank-1)
        {
            if (number > highest_dead_number)
            {
                highest_dead_number = number;
                index = c;
            }
        }
    }
    if (index < 4)
    {
        index += 4;
        return (index);
    }
     
    int highest_noncrit_number = -1;
    for(int c = 0; c<4; c++)
    {   
        int rank = mGame->getCard(p,c).rank;
        int suit = mGame->getCard(p,c).suit;
            
        if(rank < 4)
        {
            int danger = 0;
            for(int i=0; i<mGame->getDiscardSize(); i++)
            {
                Card tmp = mGame->getDiscard(i);
                if (tmp.suit == suit && tmp.rank == rank)
                {
                   danger = 1;
                }   
            }
                
            if (danger == 0 && 5*rank+suit > highest_noncrit_number)
            {
                highest_noncrit_number = 5*rank+suit;
                index = c;
            }
        }
    }
    if (index < 4)
    {
        index += 4;
        return index;
    }
    return 7;
}
