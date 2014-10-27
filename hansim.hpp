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

#ifndef HANSIM_HPP
#define HANSIM_HPP

#include <cstdlib>
#include <iostream>
#include <fstream> 

#define ACT_DISCARD 0
#define ACT_PLAY 1
#define ACT_HINT 2

#define OUT_PLAY 0
#define OUT_STRIKE 1
#define OUT_HINT 2
#define OUT_DISCARD 3

#define HINT_COLOR 0
#define HINT_RANK 1

class HanSim;

/*
Card:
0 - 1
1 - 2
2 - 3
3 - 4
4 - 5

Color:
0 - blue
1 - green
2 - red
3 - white
4 - yellow
*/

class Card
{
public:
  Card() : color(5), rank(5) {;}
  Card(int c, int r) : color(c), rank(r) {;}
  int color, rank; 
};


#include "player.hpp"

class HanSim
{
public:
  HanSim();
  ~HanSim();
  
  
  ////
  //// GAME MANAGEMENT
  ////
  
  /*
   Reshuffle and redeal, then play the game.  
   Returns score of game.  The argument 'display'
   should be true if the game status should be 
   outputted to the screen.
  */
  int play(bool display, bool to_file);

  
  ////
  //// PLAYER INTERFACE
  ////
  
  /*
   Get card indexed card_index from player's hand.  If called
   when mCurPlayer is the same as player, will return a (-1,-1) 
   card.
   
   Returns the requested card. 
  */
  Card getCard(int player, int card_index);
  
  /*
   Get a card from the discard pile, numbered chronologically 
   by index.
   
   Returns the requested card.
  */
  Card getDiscard(int index);
  
  /*
   Returns the number of cards in the discard pile.
  */
  int getDiscardSize();
  
  /*
   Get the rank of the highest card of a given color that has been played.
   If no card of that color has been played, returns -1.
  */
  int getBoardPos(int color);

  /*
   Returns the number of players.
  */
  int getNumPlayers();

  /*
   Returns the number of hints available to players.
  */
  int getTime();
  
  /*
   Returns the handsize of the given player.
  */
  int getHandsize(int player);
    
  /*
    Returns the turn number.
  */  
  int getTurnNumber();
    
  /*
    Returns the number of strikes.
  */  
  int getNumStrikes();  
    
private:
  void output_board(std::ostream& out);
  void output_line(std::ostream& out);

  void initDeck();
  void shuffleDeck();

  /*
   Discard card indexed by card_index from player's hand, and
   gives a new card if possible.  Updates the players that 
   this has happened.
  */
  Card discard(int player, int card_index);


  Player* mPlayer[5];
  int mNumPlayers;
  
  Card mDeck[60];
  int mDeckTop;
  
  Card mDiscard[60];
  int mDiscardSize;
  
  Card mHands[5][4];
  int mHandSize[5];
  
  int mCurPlayer;

  int mHints;
  int mStrikes;
  
  int mBoardPos[5];

  int mTurnNumber;
};

#endif
