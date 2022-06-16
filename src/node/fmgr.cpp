#include "../../inc/node.hpp"

void FlagManager::SetFlag(unsigned int index, bool state) {
  try { 
    if (this->flags.size() < index) {throw "Out of Bounds Set";} 
    this->flags.at(index) = state; 
  } catch(const char* cem) {
    std::cout << "[!] " << cem << '\n';
  } catch(std::exception& e) {
    std::cout << "[!!] " << e.what() << '\n';
  } 
}

bool FlagManager::GetFlag(unsigned int index) {
  try {
    return this->flags.at(index);
  } catch(std::exception& e) {
    std::cout << "[!!] " << e.what() << '\n';
  }
}

FlagManager::FlagManager(unsigned int size) {
  this->flags.resize(size); 
}
