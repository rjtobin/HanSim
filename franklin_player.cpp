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

#include "player.hpp"
#include <iostream>
using namespace std;

//==============================================================================
//                              Initiates Player
//==============================================================================
void Player::init()
{
  for(int p = 0; p <5 ; p++)
    for (int c = 0; c < 4; c++)
      for (int r = 0; r < 5; r++)
        for (int s = 0; s < 5; s++)
          data[p][c][r][s] = UNKNOWN;
          
  for(int i=0; i<5; i++)
    for(int j=0; j<5; j++)
      seenCards[i][j] = 0;
}

//==============================================================================
//                              Start-of-turn Computation
//==============================================================================

void Player::preturnUpdate(int player)
{
  for(int i=0; i<5; i++)
    for(int j=0; j<5; j++)
      seenCards[i][j] = 0;
  
  Card tmp;
  for(int i=0; i<mGame->getDiscardSize(); i++)
  {
    tmp = mGame->getDiscard(i);
    seenCards[tmp.color][tmp.rank]++;
  }
  
  for(int i=0; i<mGame->getNumPlayers(); i++)
  {
    if(i == mPlayerIndex)
      continue;
    for(int j=0; j<mGame->getHandsize(i); j++)
    {
      tmp = mGame->getCard(i,j);
      if(tmp.color >= 0 && tmp.color < 5 && tmp.rank >= 0 && tmp.rank < 5)
      {
        seenCards[tmp.color][tmp.rank]++;
      }
    }
  }
  
  for(int i=0; i<mGame->getHandsize(mPlayerIndex); i++)
  {
    int rank = getRank(i);
    int suit = getSuit(i);
    
    if(rank != UNKNOWN && suit != UNKNOWN)
      seenCards[suit][rank]++;
  }
  //cout << "start seen " << seenCards[0][0] << ' ' << seenCards[0][1] << ' ' << seenCards[0][2] << ' ' << seenCards[0][3] << ' ' << seenCards[0][4] << endl;
}

//==============================================================================
//                              Play Command
//==============================================================================

void Player::play(int& action, int& arg1, int& arg2, int& arg3)
{

//-----------------------------TRY PLAY-----------------------------------------

  int lowest_playable_rank = UNKNOWN;
  int lowest_playable_card = UNKNOWN;

  // look through our cards, see if the player knows any card completely,
  // and that card is playable
  for(int c = 0; c<4; c++)
  {
    int rank=getRank(c);
    int suit=getSuit(c);

    if(rank!=UNKNOWN && suit!=UNKNOWN)
    {
      if(mGame->getBoardPos(suit) == rank-1)
      {
        if (rank < lowest_playable_rank)
        {
          lowest_playable_rank = rank;
          lowest_playable_card = c;
        }
      }
    }
    
    else if(rank!=UNKNOWN)
    {
      int num_seen = 0;
      int available = -1;
      for(int i=0; i<5; i++)
      {
        num_seen += seenCards[i][rank];
        if(rank == 0 && seenCards[i][rank] == 2)
          available = i;
        if(rank == 4 && seenCards[i][rank] == 0)
          available = i;
        if(rank > 0 && rank < 4 && seenCards[i][rank] == 1)
          available = i;
      }
      int total_count;
      if(rank == 0)
        total_count = 15;
      if(rank > 0 && rank < 4)
        total_count = 10;
      if(rank == 4)
        total_count = 5;
      if(num_seen == total_count - 1)
      {
        Card tmp;
        tmp = mGame->getCard(mPlayerIndex,c);
        //cerr << "DEDUCTION: " << suit << ' ' << available << " = " << tmp.color << ' ' << tmp.rank << endl;
        //cerr << "seen: " << ' ' << seenCards[suit][0] << ' ' << seenCards[suit][1] << ' ' << seenCards[suit][2] << ' ' << seenCards[suit][3] << ' ' << seenCards[suit][4] << endl;
        if(mGame->getBoardPos(available) == rank - 1)
        {
          //cout << "using deduction " << endl;
          action = ACT_PLAY;
          arg1 = c;
          return;
        }
      }
    }
    
    else if(suit!=UNKNOWN)
    {
      int num_seen = 0;
      int available = -1;
      for(int i=0; i<5; i++)
      {
        num_seen += seenCards[suit][i];
        if(i==0 && seenCards[suit][i] == 3-1)
          available = 0;
        if(i==4 && seenCards[suit][i] == 0)
          available = 4;
        else if(i>0 && i<4 && seenCards[suit][i] == 1)
          available = i;
      }
      if(num_seen == 9)
      {
        Card tmp;
        tmp = mGame->getCard(mPlayerIndex,c);
        //cerr << "DEDUCTION: " << suit << ' ' << available << " = " << tmp.color << ' ' << tmp.rank << endl;
        //cerr << "seen: " << ' ' << seenCards[suit][0] << ' ' << seenCards[suit][1] << ' ' << seenCards[suit][2] << ' ' << seenCards[suit][3] << ' ' << seenCards[suit][4] << endl;
        if(mGame->getBoardPos(suit) == available - 1)
        {
          action = ACT_PLAY;
          arg1 = c;
          return;
        }
      }
    }
  }

  if(lowest_playable_rank != UNKNOWN)
  { 
    action = ACT_PLAY;
    arg1 = lowest_playable_card;
    return;
  }

  if(/*goodCards(mPlayerIndex) != 1 &&*/ mGame->getDiscardSize() < 23 && mGame->getTurnNumber() > 5)
  {
    for(int c = 0; c < 4; c++)
    {
      if(isAlive(mPlayerIndex,c)==0)
      {
        action = ACT_DISCARD;
        arg1 = c;
        return;
      }
    }
  }

///////////////////////////////////////////////////////////////////////////////////////


  //-------------------------------HINT-------------------------------------------
  if(mGame->getTime() > 0)
  {

   // if no unknown, both unknown_card and unknown_type remain 0
    int unknown_card[5];
    int unknown_type[5];
   
    for(int i=0; i<5; i++)
      unknown_card[i] = unknown_type[i] = 0;

    for(int p=0; p<5; p++)
    {
      int card_found = 0;

      //rank known, color unknown, rank matches board (im excited)
      for(int c=0; c<4; c++) 
      {
        if(getRank(p,c)!=UNKNOWN && getSuit(p,c)==UNKNOWN && goodPlayOnRank(p,c))
        {
          card_found = 1;
          unknown_card[p]=c;
          unknown_type[p]=1;
          break;
        }
      }

      //rank unknown, suit know, posPlay on suit; don't want to hint at dead
      if(card_found == 0)
      {
        for(int c=0; c<4; c++) 
        {
          if(getRank(p,c)==UNKNOWN && getSuit(p,c)!=UNKNOWN && posPlayOnSuit(p,c) )
          {
            card_found = 1;
            unknown_card[p]=c;
            unknown_type[p]=0;
            break;
          }
        }
      }

      //rank known, color unknown, rank matches board in future
      if(card_found == 0)
      {
        for(int c=0; c<4; c++)
        {
          if(getRank(p,c)!=UNKNOWN && getSuit(p,c)==UNKNOWN && posPlayOnRank(p,c))
          {
            card_found=1;
            unknown_card[p]=c;
            unknown_type[p]=1;
            break;
          }
        }
      }
      
      //everything unknown, give rank hint
      if(card_found == 0)
      {
        for(int c=0; c<4; c++) 
        {
          if(getRank(p,c)==UNKNOWN && getSuit(p,c)==UNKNOWN)
          {
            card_found = 1;
            unknown_card[p]=c;
            unknown_type[p]=0;
            break;
          }
        }
      }
    }
 
    //Computes info_sum to pass to players
    int info_sum = 0;
    for(int p = 0; p < 5; p++){
      if(mPlayerIndex != p)
      {
        if(unknown_type[p] == 0)
          info_sum += mGame->getCard(p,unknown_card[p]).rank;
        if(unknown_type[p] == 1)
          info_sum += mGame->getCard(p,unknown_card[p]).color;
      }
    }
    info_sum = ((25 + info_sum) % 5);


    //Determines best hint to give
    int hint_possible = 0; //is a hint possible?
    int most_hints = 0; //what hint gives the most info
    int best_p = 5; 
    int best_t = 5;      

    //Checks suits
    for(int p_index=0; p_index < 5; p_index++)
    { 
      int p =((mPlayerIndex + p_index)%5);
      if(mPlayerIndex != p)
      {
        int hints = 0;
        for(int c = 0; c < 4; c++)
        {
          if(mGame->getCard(p,unknown_card[p]).color  == info_sum)
          {
            hint_possible = 1;
            //now checks to see if hint is new and not redundant
            
            if( getSuit(p,c) == UNKNOWN
             && (unknown_card[p] != c || unknown_type[p] != 1))
            {
              if(seeAlive(p,c))
                hints += 1;
            }
          }
        }

        if(hints > most_hints)
        {
          best_p = p;
          best_t = 1; 
          most_hints = hints;
        }

        if(hints == 0 && most_hints == 0 && hint_possible == 1)
        {
          best_p = p;
          best_t = 1; 
        }
      }
    }
    
    //Check Rank
    for(int p_index = 0; p_index<5; p_index++)
    { 
      int p =((mPlayerIndex + p_index)%5);
      if(mPlayerIndex != p)
      {
        int hints = 0;
        for(int c = 0; c < 4; c++)
        {
          if(mGame->getCard(p,c).rank == info_sum)
          {
            hint_possible = 1;
            
            // now checks to see if hint is new and not redundant
            if( getRank(p,c) == UNKNOWN
             &&(unknown_card[p] != c || unknown_type[p] != 0))
            {
              if(seeAlive(p,c)==1)
                hints += 1;
            }
          }
        }

        if(hints > most_hints)
        {
          best_p = p;
          best_t = 0; 
          most_hints = hints;
        }

        if(hints==0 && most_hints==0 && hint_possible==1)
        {
          best_p = p;
          best_t = 0; 
        }
      }
    }

    if(hint_possible == 1)
    {
      action = ACT_HINT;
      arg1 = best_p;
      if(best_t == 1)
        arg2 = HINT_COLOR;
      if(best_t == 0)
        arg2 = HINT_RANK;
      arg3 = info_sum;
      return;
    }
  }

  //--------------------Discard Dead Card-----------------------------------------
  
  for(int c=0; c<4; c++)
  {
    if(!isAlive(mPlayerIndex,c))
    {
      action = ACT_DISCARD;
      arg1 = c;
        return;
    }
  }

  //--------------------Discard Known Duplicate------------------------------------

  for(int c = 3; c >=0; c--)
  {
    int rank = getRank(c);
    int suit = getSuit(c);

    if(rank!=UNKNOWN && suit!=UNKNOWN)
    {
      for(int p=0; p<5; p++)
      {
        for(int p_card=0; p_card<4; p_card++)
        {
          if(p==mPlayerIndex && p_card == c)
            continue;
          if(getRank(p,p_card) == rank && getSuit(p,p_card) == suit)
          {
            action = ACT_DISCARD;
            arg1 = c;
            return;
          }
        }
      }
    }
  }


  //--------------------Discard Unknow Duplicate----------------------------------

  for(int c=3; c>=0; c--)
  {
    int rank = getRank(c);
    int suit = getSuit(c);

    if(rank!=UNKNOWN && suit!=UNKNOWN)
    {
      for(int p=0; p<5; p++)
      {
        if(p!=mPlayerIndex)
        {
          for(int p_card=0; p_card<4; p_card++)
          {
            if(mGame->getCard(p,p_card).rank == rank 
             && mGame->getCard(p,p_card).color == suit)
            {
              action = ACT_DISCARD;
              arg1 = c;
              return;
            }
          }
        }
      }
    }
  }

  //----------------Play random card-----------------------------------
  
  int potential = -1, p_card = 0;
  for(int c=0; c<4; c++)
  {
    int rank = getRank(c);
    int suit = getSuit(c);
  
    if(rank != UNKNOWN && suit == UNKNOWN)
    {
      if(rank == 4)
        continue;
      int maybe = 0;
      for(int i=0; i<5; i++)
        if(mGame->getBoardPos(i) == rank-1)
          maybe+=2;
      if(maybe > 0 && maybe > potential)
      {
        p_card = c;
        potential = maybe;
      }
    }
    if(suit != UNKNOWN && rank == UNKNOWN)
    {
      if(mGame->getBoardPos(suit) != 4)
        if(potential < 1)
        {
          potential = 1;
          p_card = c;
        }
    }
    
  }
  
  if(potential > 0 && mGame->getTime() > 0 && mGame->getTurnNumber() > 40)
  {
    action = ACT_PLAY;
    arg1 = p_card;
    return;
  }

//----------------Discard a Non Critical Card-----------------------------------

  int low_c = 5;
  int low_value = 5;

  for(int c = 0; c < 4 ; c++)
  {
    int rank = getRank(c);
    int suit = getSuit(c);

    bool is_c_critical = false;

    if(rank == 4)
      is_c_critical = true;

    if(rank>0 && rank<4 && suit != UNKNOWN)
    {
      for(int d = 0; d < mGame->getDiscardSize(); d++)
      {
        if (mGame->getDiscard(d).rank == rank
           && mGame->getDiscard(d).color == suit)
        {
          is_c_critical = true;
        }
      }
    }

    if (!is_c_critical && rank < low_value)
    {
      low_c = c;
      low_value = rank;
    }
  }

  if(low_value != 5)
  {
    action = ACT_DISCARD;
    arg1 = low_c;
    return;
  }

  //--------------------Discard Last Card-----------------------------------------

  action = ACT_DISCARD;
  arg1 = 3; //drand48() * 3;
}

//==============================================================================
//                              Updates Player (if 
//==============================================================================
void Player::update(int p_turn, int outcome, int arg1, int arg2, int arg3)
{
  if (outcome != OUT_HINT)
    return;

  int p_given = arg1;
  int t_given = 5;
  if(arg2==HINT_RANK)
    t_given=0;
  if(arg2==HINT_COLOR)
    t_given=1;
  int v_given = arg3;
  // This function is called after any player (including myself!)
  // makes an action.  'action' stores the action type
  // (OUT_PLAY, OUT_STRIKE, OUT_HINT, OUT_DISCARD).
  //
  // For OUT_PLAY, arg1 is set to the color of the played card.
  //
  // For OUT_STRIKE, arg1-3 do not store anything (I will
  // learn what card was played when the game calls 
  // updateHand() to tell me how that player's hand has changed).
  //
  // For OUT_HINT, arg1 is the player the hint was given to,
  // arg2 is the hint type (HINT_COLOR or HINT_RANK) and
  // arg3 is the value of the hint (ie. a color or rank).
  // 
  // In the case of discard, the arguments arg1-3 do _not_
  // store anything, because I will know what card was discarded
  // when the game calls updateHand().

  // If a player gave a hint to someone other than me, 
  // then update my memory:


  //-------------Updates implicit (i.e. mod) info given for color hint------------
  // Important to do implicit update BEFORE explicit update

  // if no unknown, both unknown_card and unknown_type remain 0
  int unknown_card[5];
  int unknown_type[5];
    
  for(int i=0; i<5; i++)
    unknown_card[i] = unknown_type[i] = 0;

  for(int p=0; p<5; p++)
  {
    int card_found = 0;
    
    //rank known, color unknown, rank matches board (im excited)
    for(int c=0; c<4; c++) 
    {
      if(getRank(p,c)!=UNKNOWN && getSuit(p,c)==UNKNOWN && goodPlayOnRank(p,c)==1)
      {
        card_found = 1;
        unknown_card[p]=c;
        unknown_type[p]=1;
        break;
      }
    }

    //rank unknown, suit know, posPlay on suit; don't want to hint at dead
    if(card_found == 0)
    {
      for(int c=0; c<4; c++)
      {
        if(getRank(p,c)==UNKNOWN && getSuit(p,c)!=UNKNOWN && posPlayOnSuit(p,c)==1)
        {
          card_found = 1;
          unknown_card[p]=c;
          unknown_type[p]=0;
          break;
        }
      }
    }

    if(card_found == 0)
    {
      for(int c=0; c<4; c++)
      {
        if(getRank(p,c)!=UNKNOWN && getSuit(p,c)==UNKNOWN && posPlayOnRank(p,c)==1)
        {
          card_found=1;
          unknown_card[p]=c;
          unknown_type[p]=1;
          break;
        }
      }
    }

    //everything unknown, give rank hint
    if(card_found == 0)
    {
      for(int c=0; c<4; c++) 
      {
        if(getRank(p,c)==UNKNOWN && getSuit(p,c)==UNKNOWN)
        {
          card_found = 1;
          unknown_card[p]=c;
          unknown_type[p]=0;
          break;
        }
      }
    }      
  }

  // Everyone except p_turn now knows their first unknown

  // First update everyone other than mPlayerIndex and p_turn
  for (int p=0; p<5; p++)
  {
    if(p != p_turn && p != mPlayerIndex)
    {
      if(unknown_type[p] == 0)
        setRank(p, unknown_card[p], mGame->getCard(p,unknown_card[p]).rank);
      if(unknown_type[p] == 1)
        setSuit(p, unknown_card[p], mGame->getCard(p,unknown_card[p]).color);
    }
  }

  //Second update everyone but p_turn about first unknown
  if(mPlayerIndex != p_turn)
  {
    for(int p=0; p<5; p++)
    {
      if(p != p_turn)
      {
        // compute visable sum
        int vis_sum = 0;
        for(int p=0; p<5; p++)
        {
          if(p != p_turn && p != mPlayerIndex)
          {
            if(unknown_type[p] == 0)
              vis_sum += mGame->getCard(p,unknown_card[p]).rank;
            if(unknown_type[p] == 1)
              vis_sum += mGame->getCard(p,unknown_card[p]).color;
          }
        }

        // deduces own card (we know myCard + vis_sum = v_given) 
        int v_for_me = 0;
        v_for_me = ((25 + v_given - vis_sum) % 5);
        if(unknown_type[mPlayerIndex] == 0)
          setRank(mPlayerIndex, unknown_card[mPlayerIndex], v_for_me);
        if(unknown_type[mPlayerIndex] == 1)
          setSuit( mPlayerIndex , unknown_card[mPlayerIndex] , v_for_me );
      }
    }
  }
  //------------Hint not to me, updates explicit (i.e. non mod) info given--------
  // If hint is to me, I will need more info to make the update,
  // this will be handled in the player hint methoid next
  if(outcome == OUT_HINT && p_given != mPlayerIndex)
  {
    if(t_given == 0) // The hint was a rank
    {
      for(int c=0; c<4; c++)
      {
        if(mGame->getCard(p_given,c).rank == v_given)
        {
          setRank(p_given,c,v_given);
        }
      }
    }
    if(t_given == 1) // The hint was a color
    {
      for(int c=0; c<4; c++)
      {
        if(mGame->getCard(p_given,c).color == v_given)
        {
          setSuit(p_given,c,v_given);
        }
      }
    }
  }
}

//==============================================================================
//                  Updates Player with Explicit info Given to Him
//==============================================================================
//  It was not possible for a player to update in the hint methoid since he did
//  not have enough information passed to him in the methoid

void Player::hint(int c_given, Card card)
{
  // The game gave me a hint of a card's color or
  // rank.  The card the hint is referring to is
  // card number 'index'.
  //
  // If the hint was a color hint, then
  // card.rank will be set to -1, and card.color
  // is the color of the card.
  //
  // If instead the hint is a rank hint, then
  // card.color is -1 and card.rank is the rank 
  // of the card.
  //
  // Note: the game calls hint() for each card
  // individually.  So if a player has given 
  // a hint about two or more of my cards,
  // this function will be called several
  // times in a single round!
  
  if(card.rank != -1)
    setRank(mPlayerIndex, c_given, card.rank);
  if(card.color != -1)
    setSuit(mPlayerIndex, c_given, card.color);
}

//==============================================================================
//                  Updates Hands after Discard
//==============================================================================
void Player::updateHand(int p_given, int c_given)
{
  // The game is telling me that the player 'player'
  // has dropped card 'index'.  All cards after
  // index have had their indices decreased,
  // and if the player was given a new card,
  // it was placed at the end of their hand.

  for(int c = c_given; c<3; c++) 
    for(int r=0; r<5; r++)
      for(int s=0; s<5; s++)
        data[p_given][c][r][s] = data[p_given][c+1][r][s];

  for(int r=0; r<5; r++)
    for(int s=0; s<5; s++)
      data[p_given][3][r][s] = UNKNOWN;
}

//==============================================================================
//                  Prints the State
//==============================================================================
void Player::printState(ostream& out)
{
  out << ' ';
  Card card;

  for(int c=0; c<4; c++)
  {
    int rank = getRank(c);
    int suit = getSuit(c);
    card = mGame->getCard(mPlayerIndex,c);
    if(suit != UNKNOWN && card.color != suit)
      out << "WRONG" << endl;
    if(rank != UNKNOWN && card.rank != rank)
      out << "WRONG" << endl;
    out << suit  << rank << ' ';
  }
}


//==============================================================================
//                  Auxiliary functions 
//==============================================================================
bool Player::goodCards(int p)
{
  for(int c=0; c<4; c++) 
  {
    if(getRank(p,c)!=UNKNOWN && getSuit(p,c)==UNKNOWN && goodPlayOnRank(p,c))
    {
      return true;
    }
  }

  for(int c=0; c<4; c++) 
  {
    if(getRank(p,c)!=UNKNOWN && getSuit(p,c)!=UNKNOWN)
    {
      return true;
    }
  }

  return false;
}

bool Player::posPlayOnSuit(int p, int c)
{
  if(mGame->getBoardPos(getSuit(p,c)) != 4)
    return true;
  else
    return false;
}

bool Player::posPlayOnRank(int p, int c)
{
  int min_boad_pos = 5;
  for (int s=0; s<5; s++)
  {
    if(mGame->getBoardPos(s) < min_boad_pos)
    {
      min_boad_pos = mGame->getBoardPos(s);
    }
  }
    
  if(getRank(p,c) != UNKNOWN && getRank(p,c) < min_boad_pos + 1)
    return false;
  else
    return true;
}

bool Player::goodPlayOnRank(int p, int c)
{
  if(getRank(p,c) == UNKNOWN)
    return false;
  for(int s=0; s<5; s++)
  {
    if(getRank(p,c) == mGame->getBoardPos(s) + 1)
    {
      return true;
    }
  }
  return false;
}

int Player::getRank(int p, int c)
{
  for(int rank = 0; rank<5; rank++)
  {
    int is_rank = 1;
    for(int r = 0; r<5; r++)
    {
      if(r == rank)
        continue;
      
      // check if we know for sure that the rank
      // cannot be any value except 'rank'
      for(int s = 0; s<5; s++)
      {
        if(data[p][c][r][s] != 0 )
          is_rank=0;
      }
    }
    if (is_rank == 1)
      return rank;
  }
  return UNKNOWN;
}

int Player::getRank(int c)
{
  return getRank(mPlayerIndex,c);
}

int Player::getSuit(int p, int c)
{
  for(int suit = 0; suit<5; suit++)
  {
    int is_suit = 1;
    for(int r = 0; r<5; r++)
    {
      for(int s = 0; s<5; s++)
      {
        if(s == suit)
          continue;
        if(data[p][c][r][s]!=0)
          is_suit=0;
      }
    }
    if(is_suit == 1)
      return suit;
  }
  return UNKNOWN;
}

int Player::getSuit(int c)
{
  return getSuit(mPlayerIndex,c);
}

void Player::setRank(int p, int c, int rank)
{
  for(int r = 0; r<5; r++)
  {
    for (int s = 0; s<5; s++)
    {
      if(r != rank)
        data[p][c][r][s] = 0;
    }
  }
}

void Player::setSuit(int p, int c, int suit)
{
  for(int r = 0; r<5; r++)
  {
    for(int s = 0; s<5; s++)
    {
      if(s != suit)
        data[p][c][r][s] = 0;
    }
  }
}

bool Player::isAlive(int p, int c)
{
  int rank = getRank(p,c);
  int suit = getSuit(p,c);
  if(rank != UNKNOWN && suit != UNKNOWN)
  {
    if(mGame->getBoardPos(suit) > rank-1)
    {
      return false; 
    }
  }

  if(rank != UNKNOWN)
  {
    int min_board_pos = 1000;
    for(int pos=0; pos<5; pos++)
    {
      if (min_board_pos > mGame->getBoardPos(pos))
        min_board_pos = mGame->getBoardPos(pos);
    }
    if(min_board_pos > rank-1)
    {
      return false;
    }
  }

  if(suit != UNKNOWN)
  {
    if(mGame->getBoardPos(suit) == 4)
    {
      return false;
    }
  }
  return true;
}

bool Player::seeAlive(int p, int c)
{
  int rank = mGame->getCard(p,c).rank;
  int suit = mGame->getCard(p,c).color;
  
  if(rank != UNKNOWN && suit != UNKNOWN)
  {
    if(mGame->getBoardPos(suit) > rank-1)
    {
      return false; 
    }
  }

  if(rank != UNKNOWN)
  {
    int min_board_pos = 1000;
    for(int pos=0; pos<5; pos++)
    {
      if (min_board_pos > mGame->getBoardPos(pos))
        min_board_pos = mGame->getBoardPos(pos);
    }
    if(min_board_pos > rank-1)
    {
      return false;
    }
  }

  if(suit != UNKNOWN)
  {
    if(mGame->getBoardPos(suit) == 4)
    {
      return false;
    }
  }
  return true;
}

