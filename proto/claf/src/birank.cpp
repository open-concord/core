#include "../inc/birank.hpp"

void birank::orient_dir(bool dir) {
  // do nothing if in correct direction
  bool c = this->get_dir();
  if (c == dir) {return;}
  
  // positive gets ++ before being flipped
  // negative just gets flipped
  if (c) {
    this->irank++;
  }
  this->irank *= -1;
}

void birank::increment() {
  if (this->get_dir()) {
    this->irank++;
  } else {
    this->irank--;
  }
}

bool birank::get_dir() {
  /**
   * 0, the initial value, is considered positive.
   * This gives negatives preference (base negative, -1, has higher rank)
  */
  return std::abs(this->irank) >= 0;
}

bool operator== (birank a, birank b) {
  return (a.irank == b.irank);
}
