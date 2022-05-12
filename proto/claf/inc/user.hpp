#pragma once

#include "claf.hpp"

struct keypair {
  std::string DSA;
  std::string RSA;

  keypair() {}
  keypair(std::string dsak, std::string rsak) : DSA(dsak), RSA(rsak) {}
};

struct user {
  std::string trip;
  keypair pubkeys, prikeys;
  // bool empty = false;

  user() {
    std::array<std::string, 2> DSA = cDSA::keygen();
    std::array<std::string, 2> RSA = cRSA::keygen();

    pubkeys = keypair(b64::encode(DSA[1]), b64::encode(RSA[1]));
    prikeys = keypair(b64::encode(DSA[0]), b64::encode(RSA[0]));

    this->trip = gen::trip(pubkeys.DSA + pubkeys.RSA, 24);
  }

  user(keypair pubset) : pubkeys(pubset) {
    this->trip = gen::trip(pubset.DSA + pubset.RSA, 24);
  }

  user(keypair pubset, keypair priset) : pubkeys(pubset), prikeys(priset) {
    this->trip = gen::trip(pubset.DSA + pubset.RSA, 24);
  }

  user(std::string trip, keypair priset) : trip(trip), prikeys(priset) {}
};

