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
#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player;

#include "hansim.hpp"

#define UNKNOWN 5

class Player
{
public:
  Player(HanSim* game, int index) : mGame(game), mPlayerIndex(index) {;}
  
  void init();
  void preturnUpdate(int player);
  void play(int& action, int& arg1, int& arg2, int& arg3);
  void update(int player, int action, int arg1, int arg2, int arg3);
  void hint(int index, Card card);
  void updateHand(int player, int index);
  void printState(std::ostream& out);

private:
  HanSim* mGame;
  int mPlayerIndex;
  int data[5][4][5][5]; //5 players; 4 cards; 5 ranks; 5 suits
                        // possibility is 5=unknown; 0=false; 1=true.
                        // will always be indexed by data[p][c][r][s]
                        // p = player; c = card; r = rank; s = suit (color)

  int myData[5][4][5][5]; //data is always same to all players
                          //this has extra info that I see
                          
  
  /*
   Using 'data', if we know the rank of the card with a given index, 
   belonging to a specified player.
   
   Returns the rank if we know it, otherwise UNKNOWN.
  */                        
  int getRank(int player, int index);
  
  /*
   Using 'data', check if we know the rank of the car with given index, 
   belonging to me.
   
   Returns the rank if we know it, otherwise UNKNOWN.
  */
  int getRank(int index);
  
  /*
   Using 'data', check if we know the suit of the card with a given index, 
   belonging to a specified player.
   
   Returns the rank if we know it, otherwise UNKNOWN.
  */
  int getSuit(int player, int index);

  /*
   Using 'data', check if we know the suit of the car with given index, 
   belonging to me.
   
   Returns the rank if we know it, otherwise UNKNOWN.
  */
  int getSuit(int index);
  
  /*
   Set all of the values of data[p][c][.][s] to be zero, for each rank
   value not equal to the value passed to the function.
  */
  void setRank(int p, int c, int rank);

  /*
   Set all of the values of data[p][c][r][.] to be zero, for each suit
   value not equal to the value passed to the function.
  */    
  void setSuit(int p, int c, int suit);
  
  /*
   Check if the information in 'data' allows us to conclude that the
   card indexed 'index' in the hand of player 'player' is alive. 
   
   Returns true if this card is alive, false if dead.
  */
  bool isAlive(int player, int index);
  
  /*
   By actually looking at the hand of player 'player', check if the card 
   in place 'index' is  alive.  This function should be called on other
   players only, as it relies on getCard() calls.
   
   Returns true if this card is alive, false if dead.
  */
  bool seeAlive(int player, int index); 
  
  /*
    Check, using the information in 'data', only taking ranks into 
    consideration, whether the card at index 'index' in player 'player's
    hand is possibly alive.
    
    Returns true if the above is true, false otherwise (in particular, if
    the rank is not known to 'data', returns true).
  */
  bool posPlayOnRank(int player, int index); 
  
  /*
    Check, using the information in 'data', only taking suits into 
    consideration, whether the card at index 'index' in player 'player's
    hand is possibly alive.
    
    Returns true if the above is true, false otherwise (in particular, if
    the suit is not known to 'data', returns true).
  */  
  bool posPlayOnSuit(int p, int c);
  
  /*
    Check, using the information in 'data', whether the rank of the 
    card (indexed 'index' in player 'player's hand) is exactly the rank
    of some card which we could play immediately (but note that this 
    doesn't mean that we can actually play this card, as the color might not
    be the right color).
    
    Returns true if the above is true, and false otherwise.  Note
    that if the rank of the card is unknown to 'data', returns false.
  */
  bool goodPlayOnRank(int p, int c); 
  
  /*
   Checks if player number 'player' knows either of the two following 
   things about her cards:
    (1) she knows some card's rank but not its color, and this 
        rank is exactly equal to the rank of some card that could be
        played immediately.
    (2) she knows the exact rank and color of some card in her hand.
  
   Returns true if either of the above conditions hold, false otherwise.
  */
  bool goodCards(int player);

};

#endif
