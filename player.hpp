#include <fstream>

class Player
{
public:
  virtual void init() = 0;
  virtual void preturnUpdate() = 0;
  virtual void act(int& action, int& card, int& hint_to, int& hint_type, int& hint_value) = 0;
  virtual void declarationUpdate() = 0;
  virtual void resolutionUpdate() = 0;
  virtual void printState(std::ostream& out) = 0;
};
