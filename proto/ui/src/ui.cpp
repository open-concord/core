#include "../inc/ui.hpp"

void ui::NodeHandle(Node* n_) override {
  this->c = (this->c == nullptr) ? c_ : this->c;
  
}

void ui::ConnHandle(ConnCtx* c_) override {
  this->n = (this->n == nullptr) ? n_ : this->n;
  
}
