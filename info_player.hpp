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

#define UNKNOWN -1

#define NOTPOSSIBLE 0
#define POSSIBLE 1

#define FALSE 0
#define TRUE 1

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
   * Called after an action executes
  */
  void resolutionUpdate();
  
  /*
   * called at the start of each turn for every player
  */
  void printState(std::ostream& out);
  
private:
  HanSim* mGame;
  int mPlayerIndex;
  
//==============================================================================
//                         groupData Management
//==============================================================================
  
  /*
   * 5 players; 4 cards; 5 ranks; 5 suits
   * entries set to POSSIBLE or NOTPOSSIBLE
   * will always be indexed by groupData[p][c][r][s]
   * p = player; c = card; r = rank; s = suit (color)
   * stores communal information
   * myData will look the same for all players
  */
  int groupData[5][4][5][5];
  
  /*
   * Prints out all of groupData
  */
  void printData();
  
  /*
   * Returns true if rank and suit are known in groupData
   * else returns false
  */
  int isKnown(int player, int card);
  
  /*
   * Looks at groupData
   * if rank of the card (player,index) is known to everyone, returns 0-4
   * else returns UNKNOWN
  */                        
  int getRank(int p, int c);
  
  /*
   * Looks at groupData
   * if suit of the card (player,index) is known to everyone, returns 0-4
   * else returns UNKNOWN
  */           
  int getSuit(int p, int c);
  
  /*
   * Updates groupData
  */                        
  void setRank(int p, int c, int rank);
  
  /*
   * Updates groupData
  */    
  void setRankIsNot(int p, int c, int rank);

  /*
   * Updates groupData
  */    
  void setSuitIsNot(int p, int c, int suit);
  
  /*
   * Updates groupData
  */         
  void setSuit(int p, int c, int suit);

  int getRelativeRank(int p, int c);
    
  void setRelativeRank(int p, int c, int relRank); 
  
  /*
   * Looks at groupData
   * returns TRUE if:
   * card will never be playable
  */
  int isDead(int p, int c);
  
  /*
   * Looks at groupData
   * returns TRUE if: card is currently playable
   * returns FALSE if: not currently playable
   * returns UNKNOWN if: card might be playable
   * NOTE: Card may be playable even if it's unknown
  */
  int isPlayable(int p, int c);
  
  /*
   * Looks at groupData
   * returns TRUE if card is held in another players hand
  */
  int isDuplicate(int p, int c);
 
  /*
   * Looks at groupData
   * Returns true if card is last card not discarded
   * of the same color and suit
  */
  int isCritical(int p, int c); 
  
  /*
   * Looks at groupData
   * After p,c is played, shifts to account for new card
   * new card is placed as card 3, others are shifted left
  */
  void updatePlay(int p, int c);
  
    
  //------------------------Data Inference--------------------------------------
  
  /*
   * updates data
   * if all cards of one type are visible (to groupData and used)
   * then eliminates this as a possibility for other cards
  */
  void updateData();
  
//==============================================================================
//                           myData Management
//==============================================================================
  
  /*
   * 5 players; 4 cards; 5 ranks; 5 suits
   * possibility is 5=unknown; 0=false; 1=true.
   * will always be indexed by groupData[p][c][r][s]
   * p = player; c = card; r = rank; s = suit (color)
   * stores (almost) all communal information
   * myData will look the same for all players
  */
  int myData[5][4][5][5];
  
  /*
   * prints out all of myData
  */
  void myPrintData();
  
  /*
   * Returns true if rank and suit are known
   * else returns false
  */
  int myIsKnown(int player, int card);
  
  /*
   * Looks at myData
   * if rank of the card (player,index) is know to everyone, returns 0-4
   * else returns UNKNOWN
  */                        
  int myGetRank(int p, int c);
  
  /*
   * Looks at myData
   * If suit of the card (player,index) is know to everyone, returns 0-4
   * Else returns UNKNOWN
  */           
  int myGetSuit(int p, int c);
  
  /*
   * Updates myData
  */                        
  void mySetRank(int p, int c, int rank);
  
  /*
   * Updates myData
  */         
  void mySetSuit(int p, int c, int suit);
  
  /*
   * Looks at myData
   * returns TRUE if:
   * card will never be playable
  */
  int myIsDead(int p, int c);
  
  /*
   * Looks at myData
   * returns TRUE if: card is currently playable
   * returns FALSE if: not currently playable
   * returns UNKNOWN if: card might be playable
   * NOTE: Card may be playable even if it's unknown
  */
  int myIsPlayable(int p, int c);
  
  //------------------------Data Inference--------------------------------------
  
  /*
   * updates myData
   * if all cards of one type are visible (to myData and used)
   * then eliminates this as a possibility for other cards
  */
  void myUpdateData();
  

//==============================================================================
//                        Encoding Schemes
//==============================================================================

  //--------------------------hint_to/hint_type---------------------------------

  /*
   * Takes a number 0-7 and encodes it as a hint
   * (using hint_to/hint_type)
  */
  void toTypeEncode(int number, int& hint_to, int& hint_type, int& hint_value);
  
  /*
   * Takes hint_to, hint_type, hint_value and returns a number 
   * (based on hint_to/hint_type)
  */
  int toTypeDecode(int hint_to, int hint_type, int hint_value);
  
//==============================================================================
//                        Card Targeting
//==============================================================================
  
  /*
   * Looks at groupData
   * returns the index of the first unknown card that is not dead
   * if every card is dead returns the index of the first unknown card
   * if every card is known, returns 0
  */
  int firstUnknownAliveCard(int p);
  
  /*
   * Looks at groupData
   * returns the card the hint should target
   * this will be the card with the highest probability of being playable
  */
  int highestProbCard(int p);
  
//==============================================================================
//                        Possibility Partitioning
//==============================================================================

//-----------8 Classes; none bigger than 8, dead class, then max singletons-----
  
  /*
   * Builds rankSuitPartition partition matrix
   * We partition the 1's of the pos. matrix into classes 1-8
   * 0 corresponds to non pos. places
   * NOTE: Hint will be a number 0-7 corresponding to 1-8 respectively
  */
  void printSmart8Partition(int p, int c);
  
    
  /*
   * Builds rankSuitPartition partition matrix
   * We partition the 1's of the pos. matrix into classes 1-8
   * 0 corresponds to non pos. places
   * NOTE: Hint will be a number 0-7 corresponding to 1-8 respectively
   * 
   * Then looks at rankSuitPartition partition matrix and card
   * Returns a number corresponding the the class the card is in
   * NOTE: Hint will be a number 0-7 corresponding to 1-8 respectively
   * Should only be called on other players since uses sight info
  */
  int smart8PartitionNumber(int p, int c);
  
  /*
   * Builds rankSuitPartition partition matrix
   * We partition the 1's of the pos. matrix into classes 1-8
   * 0 corresponds to non pos. places
   * NOTE: Hint will be a number 0-7 corresponding to 1-8 respectively
   * 
   * Then looks at rankSuitPartition partition matrix and given number
   * Eliminates possibilities not corresponding to the identified partition
  */
  void useSmart8PartitionNumber(int number, int p, int c);
  
//==============================================================================
//                       Auxiliary Methods
//==============================================================================
  
  /*
   * Determines possible cards (with multiplicity)
   * based upon possible values in my data and known cards.
   * If unknown cards were uniformly distributed (which they are not)
   * this would be the probability that the card is playable;
   * in practice, it should be close to correct
  */
  double probPlayable(int player, int card);
 
  /*
   * Determines possible cards (with multiplicity)
   * based upon possible values in my data and known cards.
   * If unknown cards were uniformly distributed (which they are not)
   * this would be the probability that the card is playable;
   * in practice, it should be close to correct
  */
  double myProbPlayable(int player, int card);
  
};

#endif
