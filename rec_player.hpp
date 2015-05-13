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

#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player;

#include "hansim.hpp"

#define UNKNOWN 5

class Player 
{
public:
  Player(HanSim* game, int index) : mGame(game), mPlayerIndex(index) {;}

  /*
   * Called once at start of game
  */  
  void init();
  
  /*
   * Called for each player at the start of every turn
  */  
  void preturnUpdate();
  
  /*
   * Called on the player whose turn it is
   * action: ACT_PLAY, ACT_HINT, ACT_DISCARD;
   * card: 0,1,2,3; The card in the hand that is to be played or discarded
   * hint_to: 0,1,2,3,4; The player to whom the him will be given (not self)
   * hint_type: RANK, SUIT; The type of hint to be given
   * hint_value: 0,1,2,3,4; Corresponds to suit or rank to be hinted
   *                         must be a card of the specified type in target hand
  */
  void act(int& action, int& card, int& hint_to, int& hint_type, int& hint_value);
  
  /*
   * Called after an action has been declared before the action executes
  */  
  void declarationUpdate();
  
  /*
   * Called after at the end of a turn after the action occurs
  */  
  void resolutionUpdate();
  
  /*
   * Called at the start of each turn for every player to allow printing
  */  
  void printState(std::ostream& out);
  
private:
  HanSim* mGame;
  int mPlayerIndex;
  int info[8]; //p0, p1, p2, p3, p4, plays since hint, discards since hint, total discards
  
  /* Return a number corresponding to the action the player should take
   * 
   * Cards ordered by rank, cards of same rank then ordered by suit
   * 
   * If a card is playable returns the index of the smallest playable card
   * i.e. 0-3 says play 0-3 respectively
   * 
   * Else if no playable cards, returns the index of the largest dead card + 4
   * i.e. 4-7 says discard 0-3 respectively
   * 
   * Else returns the index of the largest non-critical card + 4
   * i.e. 4-7 says discard 0-3 respectively
   * 
   * Else returns 4
  */ 
  int getNumber(int p);

};

#endif
