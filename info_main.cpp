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

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "info_player.cpp"
#include "hansim.cpp"

using namespace std;

#ifndef SEED
#define SEED 100
#endif

int main()
{
  int num_games = 1000;
  
  srand48(SEED);
  
  int scores[26];
  for(int i=0; i<26; i++)
    scores[i] = 0;
  
  double total = 0;
  int tmp;
  for(int i=0; i<num_games; i++)
  {
    HanSim h;
    tmp = h.play(false,false);
    total += tmp;
    scores[tmp]++;
  }
  total /= num_games;

  for(int i=0; i<=25; i++)
    cerr << i << ' ' << scores[i] << endl;
  
  cout << total << endl;
  
  return 0;
}

