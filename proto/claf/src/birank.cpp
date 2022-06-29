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

birank::operator bool() {
    return get_dir();
}

bool operator== (birank a, birank b) {
  return (a.irank == b.irank);
}

bool operator> (birank b1, birank b2) {
    int abs_r1 = std::abs(b1.irank);
    int abs_r2 = std::abs(b2.irank);
    if (abs_r1 > abs_r2) {
        return true;
    } else if (abs_r1 == abs_r2) {
        return ((abs_r1 >= 0) && (abs_r2 < 0));
        //positive gets priority because 0 is treated as such.
        //>= 0 is used here, but they'll never be equal and opposite in that case.
    } else {
        return false;
    }
}

bool operator< (birank b1, birank b2) {
    return (b2 > b1);
}
