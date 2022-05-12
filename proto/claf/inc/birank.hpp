#pragma once

#include <cassert>
#include <cmath>

/**
 * rank that can express positive or negative values needs special properties
 */
struct birank {
  int irank = 0;
  operator bool();
  /** dir */
  void increment();
  void orient_dir(bool dir);
  bool get_dir();
};

bool operator== (birank a, birank b);
bool operator> (birank a, birank b);
bool operator< (birank a, birank b);
