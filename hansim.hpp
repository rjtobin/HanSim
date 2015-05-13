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

#ifndef HANSIM_HPP
#define HANSIM_HPP

#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

// Keywords to represent each player
#define PLAYER_0 0
#define PLAYER_1 1
#define PLAYER_2 2
#define PLAYER_3 3
#define PLAYER_4 4

// Keywords to represent each position
#define TURN_BEGIN 0
#define TURN_MIDDLE 1
#define TURN_END 2

// Keywords to represent each action
#define ACT_PLAY 0
#define ACT_HINT 1
#define ACT_DISCARD 2

// Keywords to represent each card
#define CARD_0 0
#define CARD_1 1
#define CARD_2 2
#define CARD_3 3

// Keywords to represent each hint_type
#define RANK 0
#define SUIT 1

// Keywords to represent each hint_value
#define RANK_0 0
#define RANK_1 1
#define RANK_2 2
#define RANK_3 3
#define RANK_4 4

// Keywords to represent each hint_value
#define SUIT_0 0
#define SUIT_1 1
#define SUIT_2 2
#define SUIT_3 3
#define SUIT_4 4

// Keywords to represent each outcome
#define OUT_PLAY 0
#define OUT_HINT 1
#define OUT_DISCARD 2
#define OUT_STRIKE 3

class HanSim;

class Card
{
public:
  Card() : suit(5), rank(5) {;}
  Card(int s, int r) : suit(s), rank(r) {;}
  int suit, rank; 
};

struct Turn
{
  int player, position;
  int action;
  int card; //identifies the card played or discarded
  int hint_to, hint_type, hint_value; //identify the hint given
  int outcome;
  int hint_info[4] ; //identifies cards that match the hint
};

class HanSim
{
public:

  //============================================================
  //                 Plays the game
  //============================================================
    
  /*
   Creates an instance of the game.
   That is, creates player objects and a shuffled deck of cards
  */
  HanSim();
    
  /*
   Plays a full instance of the game.
   Returns the score of game.
   The argument 'display' should be true if the game status 
   should be outputted to the screen or to a file.
   The argument 'to_file' should be true if the game 
   should be outputted to the file 'hansim_out.txt',
   otherwise the output will be the screen.
  */
  int play(bool display, bool to_file);

  /*
   Deletes the instance of the game.
  */
  ~HanSim();
  

  //============================================================
  //                 Player Interface
  //============================================================
  
  
  //---------------Look at cards in hands-----------------------
 
  /*
   Returns card card_index from player's hand.  If called
   when mCurPlayer is the same as player, will return a (-1,-1) 
   card since request is illegal.
  */
  Card getCard(int player, int card_index);


  //------Look at discarded, played, and used cards-------------
  
  /*
    We have three different types of discard decks:
     (1) Discard <- the cards that were discarded or
         were striked
     (2) Played <- cards that were successfully
         played by a player.
     (3) Used <- any card that was played, discarded
         or striked.
  */
  
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
   Get a card from the played pile, numbered chronologically 
   by index.
   
   Returns the requested card.
  */
  Card getPlayedCard(int index);
  
  /*
   Returns the number of cards in the played pile.
  */
  int getPlayedSize();

  /*
   Get a card from the used pile, numbered chronologically 
   by index.
   
   Returns the requested card.
  */
  Card getUsedCard(int index);
  
  /*
   Returns the number of cards in the discard pile.
  */
  int getUsedSize();
  
  
  //-----------Look at the board position---------------------
  
  /*
   Get the rank of the highest card of a given suitthat has been played.
   If no card of that suit has been played, returns -1.
  */
  int getBoardPos(int suit);
  
  
  //-----------Current game information---------------------

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
    Returns whether we are in the start, middle                                 
    or end of turn.
  // for mTurnPosition, TURN_BEGIN is start of turn (before anything
  // has happened), TURN_MIDDLE is mid-turn (before the end) and
  // TURN_END is the end of the turn (we are about to display
  // stuff and then start the next turn).
  */
  int getTurnPosition();
  
  /*
    Returns the number of strikes.
  */  
  int getNumStrikes();  
  
   /*
     Returns the current player.
   */  
  int getCurPlayer();  
  
  
  //-----------Recall past information---------------------

  /*
    Returns what happened on a given turn;                                       
  */
  Turn getTurnHistory(int turn);
  
  
    
private:
  void output_board(std::ostream& out);
  void output_line(std::ostream& out);

  void initDeck();
  void shuffleDeck();

  /*
   Discard card indexed by card_index from player's hand, and
   gives a new card if possible.  Updates the players that 
   this has happened.  'played' determines whether the card
   was successfully played or discarded, which controls what
   discard pile it should be put in.
  */
  Card useCard(int player, int card_index, bool played);


  Player* mPlayer[5];
  int mNumPlayers;
  
  Card mDeck[60];
  int mDeckTop;

  /*
    We have three different types of discard decks:
     (1) Discard <- the cards that were discarded or
         were striked
     (2) Played <- cards that were intentionally
         played by a player.
     (3) Used <- any card that was played, discarded
         or striked.
  */
  Card mDiscard[60];
  int mDiscardSize;

  Card mPlayed[60];
  int mPlayedSize;

  Card mUsed[60];
  int mUsedSize;
  
  Card mHands[5][4];
  int mHandSize[5];
  
  int mCurPlayer;
  
  int mHints;

  int mStrikes;
  
  int mBoardPos[5];

  int mTurnNumber;
  // for mTurnPosition, TURN_BEGIN is start of turn (before anything
  // has happened), TURN_MIDDLE is mid-turn (before the end) and
  // TURN_END is the end of the turn (we are about to display
  // stuff and then start the next turn).
  int mTurnPosition;

  Turn mTurnHistory[100];
  
};

#endif
