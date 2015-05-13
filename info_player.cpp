#include "info_player.hpp"
#include <iostream>
using namespace std;

//==============================================================================
//                              Initiates Player
//==============================================================================
void Player::init()
{
  for(int p = 0; p < 5; p++)
   for (int c = 0; c < 4; c++)
    for (int r = 0; r < 5; r++)
     for (int s = 0; s < 5; s++)
         groupData[p][c][r][s] = POSSIBLE;

  for(int p = 0; p < 5; p++)
   for (int c = 0; c < 4; c++)
    for (int r = 0; r < 5; r++)
     for (int s = 0; s < 5; s++)
         myData[p][c][r][s] = POSSIBLE;
}

//==============================================================================
//                              Start-of-turn Computation
//==============================================================================

void Player::preturnUpdate()
{
  updateData();

  myUpdateData();
}

//==============================================================================
//                              Action Command
//==============================================================================

void Player::act(int& action, int& card, int& hint_to, int& hint_type, int& hint_value)
{

    //------------------------Play if Possible----------------------------------
    for (int c = 0; c< mGame->getHandsize(mPlayerIndex); c++){
        if (myIsPlayable(mPlayerIndex,c) == TRUE) {
            action = ACT_PLAY;
            card = c;
            return;
        }           
    }    
      
    //-----------------------------Discard Dead---------------------------------
    if (mGame->getDiscardSize() < 5){
        for (int c = 0; c<mGame->getHandsize(mPlayerIndex); c++){
            if (isDead(mPlayerIndex,c) == TRUE) {
                action = ACT_DISCARD;
                card = c;
                return;
            }
        }
    }   
 
    //--------------------------Hint if Time------------------------------------
    if (mGame->getTime() > 0){
        
        action = ACT_HINT;
        
        int sum_to_pass = 0;
          
        for(int p = 0; p < 5; p++){
          if(mPlayerIndex != p)
          {          
              sum_to_pass += smart8PartitionNumber(p,highestProbCard(p));
          }
        }
        
        sum_to_pass %= 8;
        
        //cerr << "  Sum:" << sum_to_pass;
        
        toTypeEncode(sum_to_pass, hint_to, hint_type, hint_value);
        
        //Finds more optimal hint_value (not used in encoding)  
        for (int c = 0; c < mGame->getHandsize(hint_to); c++){
            if (hint_type == RANK){
                if ( getRank(hint_to,c)==UNKNOWN )
                    hint_value = mGame->getCard(hint_to,c).rank;
            }
            if (hint_type == SUIT){
                if ( getSuit(hint_to,c)==UNKNOWN )
                    hint_value = mGame->getCard(hint_to,c).suit;
            }
        }
        
        return;
    }
    
    //-----------------------------Discard Dead---------------------------------
    for (int c = 0; c<4; c++){
        if (isDead(mPlayerIndex,c) == TRUE) {
            action = ACT_DISCARD;
            card = c;
            return;
        }
    }    

    //-----------------------------Discard Duplicate----------------------------
    for (int c = 0; c<4; c++){
        if (isDuplicate(mPlayerIndex,c) == TRUE) {
            action = ACT_DISCARD;
            card = c;
            return;
        }
    }    

    //---------------------------Discard Non-Critical----------------------------
    for (int c = 0; c<4; c++){
        if (isCritical(mPlayerIndex,c) == FALSE) {
            action = ACT_DISCARD;
            card = c;
            return;
        }
    }   
    for (int c = 0; c<4; c++){
        if (isCritical(mPlayerIndex,c) != TRUE) {
            action = ACT_DISCARD;
            card = c;
            return;
        }
    } 
        
     //---------------------------Discard First---------------------------------
    action = ACT_DISCARD;
    card = 0;
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
  int outcome = turn.outcome; 
  int hint_info[4]; // 1 if match, 0 if no match on card in hand
  for (int i = 0; i<4; i++)
      hint_info[i] = turn.hint_info[i]; 
  // This all the information we use, although players also have access to
  // the get methods in hansim (as well as previous turn history)
  
    if (outcome == OUT_HINT){                                                  // XXX: we changed this   
        
        // Fist finds card targeted for each player and
        // the number sent to all player. Must do this before updating
        // any information (updating info changes the prob. calculations
        // and the wrong card would be identified)
        
        int targetCards[5];
        int targetNumbers[5];
        
         for(int p = 0; p < 5; p++){
            targetCards[p] = highestProbCard(p);
          }
        
        // uses encoded info
        int number = toTypeDecode(hint_to, hint_type, hint_value);
        int obs_number = 0;
        
        for(int p = 0; p < 5; p++){
          if(mPlayerIndex != p && mGame->getCurPlayer() != p)
          {
             targetNumbers[p] = smart8PartitionNumber(p,targetCards[p]);
             obs_number += targetNumbers[p];
          }}

        targetNumbers[mPlayerIndex]=(64+number - obs_number) % 8;
                
        for(int p = 0; p < 5; p++){
          if(mGame->getCurPlayer() != p)
          {
              useSmart8PartitionNumber(targetNumbers[p], p, targetCards[p]);
          }
        }

        //direct info
        for(int c = 0 ; c< 4; c++)
        {
            if(hint_info[c] == 1)
            {
                if (hint_type == RANK)
                    setRank(hint_to,c,hint_value);
                if (hint_type == SUIT)
                    setSuit(hint_to,c,hint_value);
            }
            if(hint_info[c] == 0)
            {
                if (hint_type == RANK)
                    setRankIsNot(hint_to,c,hint_value);
                if (hint_type == SUIT)
                    setSuitIsNot(hint_to,c,hint_value);
            }
        }
    }
  
    //shuffles information about known cards
    if (action == ACT_PLAY || action == ACT_DISCARD)
      updatePlay(p_turn, card);
}

//==============================================================================
//                              DeclarationUpdate
//==============================================================================
void Player::resolutionUpdate()
{
}

//==============================================================================
//                  Prints the Status
//==============================================================================

void Player::printState(ostream& out)
{
    for (int c = 0; c<4; c++){
        if (getRank(mPlayerIndex,c) == UNKNOWN)
            out << "8";
        else
            out << getRank(mPlayerIndex,c);
        if (getSuit(mPlayerIndex,c) == UNKNOWN)
            out << "8";
        else
            out << getSuit(mPlayerIndex,c);
        out << " ";
    }
}

//==============================================================================
//                         groupData Management
//==============================================================================

void Player::printData()
{   
    cerr << endl;
    for (int r = 0; r < 5; r++){
     for(int p = 0; p <5 ; p++){
      for (int c = 0; c < 4; c++){
        for (int s = 0; s < 5; s++){
            cerr << groupData[p][c][r][s] << "";
        }
        cerr << "   " ;
       }
       cerr << "     " ;
      }
      cerr << endl;
    }
}

int Player::isKnown(int p, int c)
{
    int numPos = 0;
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
         if(groupData[p][c][r][s] == POSSIBLE){
              numPos += 1; 
         }
    }}
    if (numPos == 1)
        return TRUE;
    return FALSE;
}

int Player::getRank(int p, int c)
{
    int rank = UNKNOWN;
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
         if(groupData[p][c][r][s] == POSSIBLE){
             if (rank != UNKNOWN && rank != r){
                 return UNKNOWN;
             }
             if (rank == UNKNOWN){
                 rank = r;
             }
         }
    }}
    return rank;
}

int Player::getSuit(int p, int c)
{
    int suit = UNKNOWN;
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
         if(groupData[p][c][r][s] == POSSIBLE){
             if (suit != UNKNOWN && suit != s){
                 return UNKNOWN;
             }
             if (suit == UNKNOWN){
                 suit = s;
             }
          }
    }}
    return suit;
}

void Player::setRank(int p, int c, int rank)
{
  for(int r = 0; r<5; r++){
   for (int s = 0; s<5; s++){
      if(r != rank)
        groupData[p][c][r][s] = NOTPOSSIBLE;
  }}
}

void Player::setSuit(int p, int c, int suit)
{
  for(int r = 0; r<5; r++){
    for(int s = 0; s<5; s++){
      if(s != suit)
        groupData[p][c][r][s] = NOTPOSSIBLE;
  }}
}

void Player::setRankIsNot(int p, int c, int rank)
{
    for (int s = 0; s<5; s++)
    {
        groupData[p][c][rank][s] = NOTPOSSIBLE;
    }
}

void Player::setSuitIsNot(int p, int c, int suit)
{
    for (int r = 0; r<5; r++)
    {
        groupData[p][c][r][suit] = NOTPOSSIBLE;
    }
}

int Player::isDead(int p, int c)
{
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
           if (groupData[p][c][r][s] == POSSIBLE){
               if (r > mGame->getBoardPos(s))
                   return FALSE;
           }
    }}
    return TRUE;
}

int Player::isPlayable(int p, int c)
{
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
           if (groupData[p][c][r][s] == POSSIBLE){
               if (r != mGame->getBoardPos(s)+1)
                   return FALSE;
           }
    }}
    return TRUE;
}

int Player::isDuplicate(int player, int c)
{
    if (isKnown(player,c) != TRUE)
        return UNKNOWN;
 
    int observedCards[5][5];
    for(int r = 0; r <5 ; r++){
     for (int s = 0; s < 5; s++){
         observedCards[r][s]=0;
    }}

    for(int p = 0; p <5 ; p++){
     for (int c = 0; c < 4; c++){
         if(isKnown(p,c) == TRUE){
             int rank = getRank(p,c);
             int suit = getSuit(p,c);
             observedCards[rank][suit] += 1;
         }      
    }}
       
    if(observedCards[getRank(player,c)][getSuit(player,c)] > 1)
         return TRUE;
    return FALSE;
}

int Player::isCritical(int player, int c)
{
    if (isKnown(player,c) != TRUE)
        return UNKNOWN;
        
    int observedCards[5][5];
    for(int r = 0; r <5 ; r++){
     for (int s = 0; s < 5; s++){
         observedCards[r][s]=0;
    }}

    for (int i=0; i < mGame->getUsedSize(); i++){
        Card tmp = mGame->getUsedCard(i);
        observedCards[tmp.rank][tmp.suit] += 1;
    }

    if(getRank(player,c) == 0 &&
        observedCards[getRank(player,c)][getSuit(player,c)] == 2)
       {
          return TRUE;
       }
       
    if(0 < getRank(player,c) && getRank(player,c) < 4 &&
        observedCards[getRank(player,c)][getSuit(player,c)] == 1)
       {
          return TRUE;
       }
       
    if(getRank(player,c)==4)
        return TRUE;
       
    return FALSE;
}

void Player::updatePlay(int p, int c)
{
    for (int i = c; i < 3; i++){
        for (int r = 0; r < 5; r++){
         for (int s = 0; s < 5; s++){
             groupData[p][i][r][s] = groupData[p][i+1][r][s];
        }}
    }
    
    for (int r = 0; r < 5; r++)
     for (int s = 0; s < 5; s++)
         groupData[p][3][r][s] = POSSIBLE;
}

//------------------------Data Inference--------------------------------------

void Player::updateData()
{
  //loops to make updates until no further deductions can be made
  
  int update_made = TRUE; 

  while (update_made == TRUE){
    
    update_made = FALSE;

       //Looks at known cards in hands and used cards
       //Counts the number of each type
       int observedCards[5][5];
       for(int r = 0; r <5 ; r++){
        for (int s = 0; s < 5; s++){ 
            observedCards[r][s]=0;
        }}

       for (int i=0; i < mGame->getUsedSize(); i++){
           Card tmp = mGame->getUsedCard(i);
           observedCards[tmp.rank][tmp.suit] += 1;
       }

       for(int p = 0; p <5 ; p++){
        for (int c = 0; c < mGame->getHandsize(p); c++){
           if(isKnown(p,c) == TRUE){ 
               observedCards[getRank(p,c)][getSuit(p,c)] += 1;
           }      
       }}
       
       //rank == 0
       for (int s = 0; s < 5; s++){
           if (observedCards[0][s]==3){
               for(int p = 0; p <5 ; p++){
                for (int c = 0; c < mGame->getHandsize(p); c++){   
                   if (groupData[p][c][0][s] == POSSIBLE && isKnown(p,c)==FALSE){
                       update_made = TRUE;
                       groupData[p][c][0][s] = NOTPOSSIBLE;
                   }
                }}  
           }
       }

       //rank == 1-3
       for (int r = 1; r < 4; r++){
        for (int s = 0; s < 5; s++){
           if (observedCards[r][s]==2){
               for(int p = 0; p <5 ; p++){
                for (int c = 0; c < mGame->getHandsize(p); c++){   
                   if (groupData[p][c][r][s] == POSSIBLE && isKnown(p,c)==FALSE){
                       update_made = TRUE;
                       groupData[p][c][r][s] = NOTPOSSIBLE;
                   }
                }}  
           }
       }}

       //rank == 4
        for (int s = 0; s < 5; s++){
           if (observedCards[4][s]==1){
               for(int p = 0; p <5 ; p++){
                for (int c = 0; c < mGame->getHandsize(p); c++){   
                   if (groupData[p][c][4][s] == POSSIBLE && isKnown(p,c)==FALSE ){
                       update_made = TRUE;
                       groupData[p][c][4][s] = NOTPOSSIBLE;
                   }
                }}  
           }
       }
    }
}

//==============================================================================
//                         myData Management
//==============================================================================

//NOTE: Only updated at start of turn, where it copies groupData then
//      adds additional info from other cards and makes deductions

void Player::myPrintData()
{   
    cerr << endl;
    for (int r = 0; r < 5; r++){
     for(int p = 0; p <5 ; p++){
      for (int c = 0; c < 4; c++){
       for (int s = 0; s < 5; s++){
           cerr << myData[p][c][r][s] << "";
       }
      cerr << "   " ;
     }
     cerr << "     " ;
    }
    cerr << endl;
    }
}

int Player::myIsKnown(int p, int c)
{
    int numPos = 0;
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
         if(myData[p][c][r][s] == POSSIBLE){
              numPos += 1; 
         }
    }}
    if (numPos == 1)
        return TRUE;
    return FALSE;
}

int Player::myGetRank(int p, int c)
{
    int rank = UNKNOWN;
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
         if(myData[p][c][r][s] == POSSIBLE){
             if (rank != UNKNOWN && rank != r){
                 return UNKNOWN;
             }
             if (rank == UNKNOWN){
                rank = r;
             }
         }
    }}
    return rank;
}

int Player::myGetSuit(int p, int c)
{
    int suit = UNKNOWN;
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
         if(myData[p][c][r][s] == POSSIBLE){
             if (suit != UNKNOWN && suit != s){
                 return UNKNOWN;
             }
             if (suit == UNKNOWN){
                 suit = s;
             }
         }
    }}
    return suit;
}

void Player::mySetRank(int p, int c, int rank)
{
  for(int r = 0; r<5; r++){
   for (int s = 0; s<5; s++){
      if(r != rank)
        myData[p][c][r][s] = NOTPOSSIBLE;
  }}
}

void Player::mySetSuit(int p, int c, int suit)
{
  for(int r = 0; r<5; r++){
    for(int s = 0; s<5; s++){
      if(s != suit)
        myData[p][c][r][s] = NOTPOSSIBLE;
  }}
}

int Player::myIsDead(int p, int c)
{
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
           if (myData[p][c][r][s] == POSSIBLE){
               if (r > mGame->getBoardPos(s))
                   return FALSE;
           }
    }}
    return TRUE;
}

int Player::myIsPlayable(int p, int c)
{
    for (int r = 0; r < 5; r++){
     for (int s = 0; s < 5; s++){
         if (myData[p][c][r][s] == POSSIBLE){
             if (r != mGame->getBoardPos(s)+1)
                 return FALSE;
         }
    }}
    return TRUE;
}

//------------------------myData Inference--------------------------------------

void Player::myUpdateData()
{
    
  for(int p = 0; p <5 ; p++)
    for (int c = 0; c < 4; c++)
      for (int r = 0; r < 5; r++)
        for (int s = 0; s < 5; s++)
          myData[p][c][r][s] = groupData[p][c][r][s];
    
  for(int p = 0; p <5 ; p++){
      if ( p != mPlayerIndex){
        for (int c = 0; c < mGame->getHandsize(p); c++){
            mySetRank(p,c,mGame->getCard(p,c).rank);
            mySetSuit(p,c,mGame->getCard(p,c).suit);
        }
      }
  }
      
    int update_made = TRUE; 

    while (update_made == TRUE){
    
    update_made = FALSE;

       //Looks at known cards in hands and used cards
       //Counts the number of each type
       int observedCards[5][5];
       for(int r = 0; r <5 ; r++){
        for (int s = 0; s < 5; s++){
            observedCards[r][s]=0;
        }}

       for (int i=0; i < mGame->getUsedSize(); i++){
           Card tmp = mGame->getUsedCard(i);
           observedCards[tmp.rank][tmp.suit] += 1;
       }

       for(int p = 0; p <5 ; p++){
        for (int c = 0; c < mGame->getHandsize(p); c++){
           if(myIsKnown(p,c) == TRUE){ 
               observedCards[myGetRank(p,c)][myGetSuit(p,c)] += 1;
           }      
       }}

       //rank == 0
       for (int s = 0; s < 5; s++){
           if (observedCards[0][s]==3){
               for(int p = 0; p <5 ; p++){
                for (int c = 0; c < mGame->getHandsize(p); c++){   
                   if (myData[p][c][0][s] == POSSIBLE && myIsKnown(p,c)==FALSE){
                       update_made = TRUE;
                       myData[p][c][0][s] = NOTPOSSIBLE;
                   }
                }}  
           }
       }

       //rank == 1-3
       for (int r = 1; r < 4; r++){
        for (int s = 0; s < 5; s++){
           if (observedCards[r][s]==2){
               for(int p = 0; p <5 ; p++){
                for (int c = 0; c < mGame->getHandsize(p); c++){   
                   if (myData[p][c][r][s] == POSSIBLE && myIsKnown(p,c)==FALSE){
                       update_made = TRUE;
                       myData[p][c][r][s] = NOTPOSSIBLE;
                   }
                }}  
           }
       }}

       //rank == 4
        for (int s = 0; s < 5; s++){
           if (observedCards[4][s]==1){
               for(int p = 0; p <5 ; p++){
                for (int c = 0; c < mGame->getHandsize(p); c++){   
                   if (myData[p][c][4][s] == POSSIBLE && myIsKnown(p,c)==FALSE ){
                       update_made = TRUE;
                       myData[p][c][4][s] = NOTPOSSIBLE;
                   }
                }}  
           }
       }
    }
}

//==============================================================================
//                          Encoding Schemes
//==============================================================================

//--------------------------hint_to/hint_type-----------------------------------

void Player::toTypeEncode(int number, int& hint_to, int& hint_type, int& hint_value){
    
        hint_to = (number%4+mGame->getCurPlayer()+1)%5;
        if (number < 4)
            hint_type = RANK;
        else
            hint_type = SUIT;
            
        //no info passed through hint_value
        if (hint_type == RANK)
            hint_value = mGame->getCard(hint_to,0).rank;
        if (hint_type == SUIT)
            hint_value = mGame->getCard(hint_to,0).suit;  
}

int Player::toTypeDecode(int hint_to, int hint_type, int hint_value){
        int number = ((25+hint_to - mGame->getCurPlayer())%5)-1;
        if (hint_type == SUIT)
            number += 4;
        return number;
}

//==============================================================================
//                        Card Targeting Schemes
//==============================================================================

int Player::firstUnknownAliveCard(int p){
    for (int c = 0; c<mGame->getHandsize(p); c++){
        if (isKnown(p,c) !=TRUE && isDead(p,c) != TRUE)
            return c;
    }
    
    for (int c = 0; c<mGame->getHandsize(p); c++){
        if (isKnown(p,c) !=TRUE)
           return c;
    }
    
    return 0;  
}

int Player::highestProbCard(int p){
    
    double higestProb = 0;
    int bestCard = 0;
    for (int c = 0; c<mGame->getHandsize(p); c++){
        if (isKnown(p,c) !=TRUE && probPlayable(p,c)> higestProb){
            higestProb = probPlayable(p,c);
            bestCard = c;
        }
    }
   
    return bestCard;  
}

//==============================================================================
//                       Possibility Partitioning
//==============================================================================

//-----------8 Classes; none bigger than 8, dead class, then max singletons-----

void Player::printSmart8Partition(int p, int c){
    
    //builds partition matrix
    int pMatrix[5][5];
     
    int elements = 0;
    int are_dead_elements = 0;
    for (int r = 0; r<5; r++){
     for (int s = 0; s<5; s++){
         if (groupData[p][c][r][s] == NOTPOSSIBLE){
             pMatrix[r][s] = 0;
         }
         if (groupData[p][c][r][s] == POSSIBLE && r <= mGame->getBoardPos(s)){
             pMatrix[r][s] = 1;
             if (are_dead_elements == 0){
                 are_dead_elements = 1;
                 elements++;
             }
         }
         if (groupData[p][c][r][s] == POSSIBLE && r > mGame->getBoardPos(s)){
             pMatrix[r][s] = 2;
             elements++;
         }
    }}
    
    int singletons = 0;
    
    for (int i =0 ; i<9; i++){
        if (i + 8*(8-i) >= elements){
            singletons = i;
        }
    }
    
    int current_partition = 1;

    if (are_dead_elements == 1){
        current_partition = 2;
    }
   
    int elts_in_current_partition = 0;
    
    for (int r = 0; r<5; r++){
    for (int s = 0; s<5; s++){
        if (pMatrix[r][s] == 2 && current_partition <= singletons){
            pMatrix[r][s] = current_partition;
            current_partition ++;
        }
        else if (pMatrix[r][s] == 2 && current_partition > singletons){
            pMatrix[r][s] = current_partition;
            elts_in_current_partition ++;
            if (elts_in_current_partition == 8){
                elts_in_current_partition = 0;
                current_partition ++;
            }
        }
    }}       
    //Finished building partition matrix
    
    //prints
    cerr << endl;
      for (int r = 0; r < 5; r++){
         for (int s = 0; s < 5; s++){
             cerr << pMatrix[r][s] << "";
         }
         cerr << endl;
       }
       cerr << endl;
       cerr << endl;
}

int Player::smart8PartitionNumber(int p, int c){
   
    //builds partition matrix
    int pMatrix[5][5];
     
    int elements = 0;
    int are_dead_elements = 0;
    for (int r = 0; r<5; r++){
     for (int s = 0; s<5; s++){
         if (groupData[p][c][r][s] == NOTPOSSIBLE){
             pMatrix[r][s] = 0;
         }
         if (groupData[p][c][r][s] == POSSIBLE && r <= mGame->getBoardPos(s)){
             pMatrix[r][s] = 1;
             if (are_dead_elements == 0){
                 are_dead_elements = 1;
                 elements++;
             }
         }
         if (groupData[p][c][r][s] == POSSIBLE && r > mGame->getBoardPos(s)){
             pMatrix[r][s] = 2;
             elements++;
         }
    }}
    
    int singletons = 0;
    
    for (int i =0 ; i<9; i++){
        if (i + 8*(8-i) >= elements){
            singletons = i;
        }
    }
    
    int current_partition = 1;

    if (are_dead_elements == 1){
        current_partition = 2;
    }
   
    int elts_in_current_partition = 0;
    
    for (int r = 0; r<5; r++){
    for (int s = 0; s<5; s++){
        if (pMatrix[r][s] == 2 && current_partition <= singletons){
            pMatrix[r][s] = current_partition;
            current_partition ++;
        }
        else if (pMatrix[r][s] == 2 && current_partition > singletons){
            pMatrix[r][s] = current_partition;
            elts_in_current_partition ++;
            if (elts_in_current_partition == 8){
                elts_in_current_partition = 0;
                current_partition ++;
            }
        }
    }}       
    //Finished building partition matrix
    
    // Returns number corresponding to the partition the card is in
    // -1 translates 1-8 to 0-7
    return (pMatrix[mGame->getCard(p,c).rank][mGame->getCard(p,c).suit]-1);
}

void Player::useSmart8PartitionNumber(int number, int p, int c){
    
    //translates 0-7 to 1-8
    number += 1;
    
    //builds partition matrix
    int pMatrix[5][5];
     
    int elements = 0;
    int are_dead_elements = 0;
    for (int r = 0; r<5; r++){
     for (int s = 0; s<5; s++){
         if (groupData[p][c][r][s] == NOTPOSSIBLE){
             pMatrix[r][s] = 0;
         }
         if (groupData[p][c][r][s] == POSSIBLE && r <= mGame->getBoardPos(s)){
             pMatrix[r][s] = 1;
             if (are_dead_elements == 0){
                 are_dead_elements = 1;
                 elements++;
             }
         }
         if (groupData[p][c][r][s] == POSSIBLE && r > mGame->getBoardPos(s)){
             pMatrix[r][s] = 2;
             elements++;
         }
    }}
    
    int singletons = 0;
    
    for (int i =0 ; i<9; i++){
        if (i + 8*(8-i) >= elements){
            singletons = i;
        }
    }
    
    int current_partition = 1;

    if (are_dead_elements == 1){
        current_partition = 2;
    }
   
    int elts_in_current_partition = 0;
    
    for (int r = 0; r<5; r++){
    for (int s = 0; s<5; s++){
        if (pMatrix[r][s] == 2 && current_partition <= singletons){
            pMatrix[r][s] = current_partition;
            current_partition ++;
        }
        else if (pMatrix[r][s] == 2 && current_partition > singletons){
            pMatrix[r][s] = current_partition;
            elts_in_current_partition ++;
            if (elts_in_current_partition == 8){
                elts_in_current_partition = 0;
                current_partition ++;
            }
        }
    }}       
    //Finished building partition matrix
    
    //Eliminates possibilities that do not correspond to the card's partition
    for (int r = 0; r<5; r++){
     for (int s = 0; s<5; s++){
         if (pMatrix[r][s] != number){
             groupData[p][c][r][s] = NOTPOSSIBLE;
         }
    }}
}

//==============================================================================
//                       Auxiliary Methods
//==============================================================================

double Player::probPlayable(int player, int card)
{
    if (isKnown(player,card) == TRUE)
    {
        int suit = getSuit(player,card);
        int rank = getRank(player,card);
        if( mGame->getBoardPos(suit) == rank-1)
            return 1;
        else
            return 0;
    }
    
    //Looks at known cards in hands and used cards
    //Counts the number of each type
    int observedCards[5][5];
    for(int r = 0; r <5 ; r++){
     for (int s = 0; s < 5; s++){
         observedCards[r][s]=0;
    }}

    for (int i=0; i < mGame->getUsedSize(); i++){
        Card tmp = mGame->getUsedCard(i);
        observedCards[tmp.rank][tmp.suit] += 1;
    }
    
    for(int p = 0; p <5 ; p++){
     for (int c = 0; c < 4; c++){
         if(isKnown(p,c) == TRUE){
             int rank = getRank(p,c);
             int suit = getSuit(p,c);
             observedCards[rank][suit] += 1;
         }      
    }}
       
    // possible cards with multiplicity
    double totalPosiblities  = 0;
    double playablePosibilities = 0;
       
    for(int r = 0; r <5 ; r++){
     for (int s = 0; s < 5; s++){
            
         if (groupData[player][card][r][s] == POSSIBLE){
             int inDeck = 2;
             if (r == 0)
                 inDeck = 3;
             if (r == 4)
                 inDeck = 1;
             totalPosiblities += (inDeck - observedCards[r][s]);
             if (r == mGame->getBoardPos(s) + 1){
                 playablePosibilities += (inDeck - observedCards[r][s]);
             }
        }
    }}
    // int prob = (playablePosibilities / totalPosiblities) / .01;
    // cerr  << "  ProbOf" << player << card << "=" << prob;
    return (playablePosibilities / totalPosiblities) / .01;
}

double Player::myProbPlayable(int player, int card)
{
    if (myIsKnown(player,card) == TRUE)
    {
        int suit = myGetSuit(player,card);
        int rank = myGetRank(player,card);
        if( mGame->getBoardPos(suit) == rank-1)
            return 1;
        else
            return 0;
    }
    
     //Looks at known cards in hands and used cards
     //Counts the number of each type
    int observedCards[5][5];
    for(int r = 0; r <5 ; r++){
     for (int s = 0; s < 5; s++){
         observedCards[r][s]=0;
    }}

    for (int i=0; i < mGame->getUsedSize(); i++){
        Card tmp = mGame->getUsedCard(i);
        observedCards[tmp.rank][tmp.suit] += 1;
    }
    
    for(int p = 0; p <5 ; p++){
     for (int c = 0; c < 4; c++){
         if(myIsKnown(p,c) == TRUE){
             int rank = myGetRank(p,c);
             int suit = myGetSuit(p,c);
             observedCards[rank][suit] += 1;
         }      
     }}
       
    //posable cards with multiplicity
    double totalPosiblities  = 0;
    double playablePosibilities = 0;
       
    for(int r = 0; r <5 ; r++){
     for (int s = 0; s < 5; s++){
            
         if (myData[player][card][r][s] == POSSIBLE){
             int inDeck = 2;
             if (r == 0)
                 inDeck = 3;
             if(r == 4)
                 inDeck = 1;
                 totalPosiblities += (inDeck - observedCards[r][s]);
             if(r == mGame->getBoardPos(s) + 1){
                 playablePosibilities += (inDeck - observedCards[r][s]);
             }
         }   
    }}
      // int prob = (playablePosibilities / totalPosiblities) / .01;
      // cerr  << "  ProbOf" << player << card << "=" << prob;
    return (playablePosibilities / totalPosiblities) / .01;
}
