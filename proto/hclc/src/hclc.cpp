#include "../inc/hclc.hpp"

// add blocks in context to chain
json hclc::update_chain(json cont = {}) {
  (this->c)->GraphCtx.Forest[
    (this->c)->ExchangeCtx.ChainTrip
    ]->batch_push((this->c)->ExchangeCtx.NewBlocks);
  std::unordered_set<std::string> new_block_hashes;
  for (const auto& block : (this->c)->ExchangeCtx.NewBlocks) {
    new_block_hashes.insert(block.hash);
  }
  return {
    {"FLAG", "END"}
  };
}

/** the actual HCLC process starts here */

// open contact - COPEN
json hclc::client_open() {
    try {
      (this->c)->ExchangeCtx.ChainTrip = this->chain_trip;

      std::unordered_set<std::string> valence_hashes = 
        (this->c)->GraphCtx.Forest[
          (this->c)->ExchangeCtx.ChainTrip
          ]->get_qualifying_hashes(&Tree::is_childless);

      std::vector<std::string> c_valence_hashes;

      for (auto v_hash : valence_hashes) {
        c_valence_hashes.push_back(v_hash);
      }

      json ret = {
          {"FLAG", "COPEN"},
          {"CONTENT", {
            {"chain", this->chain_trip},
            {"val", c_valence_hashes}
          }}
      };
      return ret;
    } catch (int err) {
      return error(err);
    }
}

// receive COPEN and send host valence - HOPEN
json hclc::host_open(json cont) {
  try { 
    (this->c)->ExchangeCtx.ChainTrip = cont["chain"]; 

    std::unordered_set<std::string> valence_hashes = 
      (this->c)->GraphCtx.Forest[
        (this->c)->ExchangeCtx.ChainTrip
        ]->get_qualifying_hashes(&Tree::is_childless);

    std::vector<std::string> h_valence_hashes;
    std::vector<std::string> c_valence_hashes = cont["val"];
    std::vector<std::string> req_hashes;

    for (auto valence_hash : valence_hashes) {
      h_valence_hashes.push_back(valence_hash);
    }

    auto chain_saved = (this->c)->GraphCtx.Forest[
      (this->c)->ExchangeCtx.ChainTrip
      ]->get_chain();
    for (auto c_val_hash : c_valence_hashes) {
      if (chain_saved.find(c_val_hash) == chain_saved.end()) {
        req_hashes.push_back(c_val_hash);
      }
    }

    json ret = {
        {"FLAG", "HOPEN"},
        {"CONTENT", {
           {"val", h_valence_hashes},
          {"req", req_hashes}
        }}
    };

    return ret;
  } catch (int err) {
    return error(err);
  }
}

/** 
 * send requests for parents of the latest layer of blocks
 * and fulfill the latest layer of such requests - BLOCKS 
 */
json hclc::transfer_blocks(json cont) {
    try {
        auto chain_saved = (this->c)->GraphCtx.Forest[
          (this->c)->ExchangeCtx.ChainTrip
        ]->get_chain();
        /** pulling from inc content */
        std::vector<std::string> prompt_req_hashes = cont["req"];
        /** frameworking */
        std::vector<std::string> req_hashes;
        std::vector<json> packet;
       
        /** fetch blocks requested */
        for (auto prompt_req : prompt_req_hashes) {
          packet.push_back(chain_saved[prompt_req].jdump());
        }
        
        /** determine if this is a clean pull */
        if (!cont.contains(std::string{"pack"})) {
          std::cout << "clean pull\n";
          /** check the valance layer of peer */
          if (cont["val"].empty()) {
            /** dump the tree */
            for (const auto& [trip, block] : chain_saved) {
               packet.push_back(block.jdump());
            }
          } else {
            /** bring peer up to speed */
            for (const auto& val_trip : cont["val"]) {
              /** check that valance layer is legit */

              /** check for highest block */

              /** dump tree beyond valence layer */
          }
        } else {
          std::vector<json> prompt_packet = cont["pack"];
          /** add blocks received and request missing parents */
          std::unordered_set<std::string> provided_p_hashes;
          for (auto prompt_block : prompt_packet) {
            block new_block(prompt_block);
            (this->c)->ExchangeCtx.NewBlocks.push_back(new_block);
            for (auto p_hash : new_block.p_hashes) {
                provided_p_hashes.insert(p_hash);
            }
          }
          /** collect parents that are not in the chain and need to be requested */
          for (auto p_hash : provided_p_hashes) {
            if (chain_saved.find(p_hash) == chain_saved.end()) {
                req_hashes.push_back(p_hash);
            }
          }
        } 

        /** see if this concludes the exchange */
        if (req_hashes.size() == 0 && packet.size() == 0) {
            return this->update_chain();
        }
        /** if it doesn't and we have content to send, go ahead and do that */
        return {
          {"FLAG", "BLOCKS"},
          {"CONTENT", {{"req", req_hashes}, {"pack", packet}}}
        };
    } catch (std::exception& e) {
      std::cout << "[!] " << e.what() << '\n';
    } catch (int err) {
      return error(err);
    }
}

/** the actual HCLC process ends here */

// Keyex
void hclc::Key_Exchange() {  
  json _j{
    {"FLAG", "KE"},
    {"CONTENT", c->P()->sec.Public()}
  };
  
  c->P()->RawWrite(_j.dump());
  json _ij = json::parse(c->P()->AwaitRawRead());

  if (_ij["FLAG"] == "KE") {
    c->P()->sec.Peer(_ij["CONTENT"]);
    c->P()->sec.Gen(); 
  } else {
    std::cout << "First FC wasn't Key Exchange\n";
  }
}

// apply communication roadmap
void hclc::ConnHandle(Conn* _c) { 
  if (this->c == nullptr) {this->c = _c;}

  if (c->P()->sec.Zero(c->P()->sec.Shared())) { 
    this->Key_Exchange();
  }
  std::cout << "SHARED: " << c->P()->sec.Shared() << '\n'; // DEBUG
  
  /** prompt */
  if (
      !(c->P()->Flags.Get(Peer::HOST))
      && !this->chain_trip.empty()
  ) {
    std::cout << "NOT THE HOST\n"; // DEBUG
    c->P()->Write(client_open().dump()); 
  }

  json parsed = json::parse(c->P()->AwaitRead());
  std::cout << "NEW MSG: " << parsed << "\n"; // DEBUG
  
  // message parsing
  std::string cmd = parsed["FLAG"];
  json cont = parsed["CONTENT"];

  // temp return var
  std::string rmsg;

  try { 
    rmsg = ((this->next[cmd])(this, cont)).dump();
  } catch (int err) {
    rmsg = error(err).dump();
  }
  c->P()->Write(rmsg);
  if (!(c->Flags.Get(Conn::CLOSE))) {
    this->ConnHandle(c);
  } else {
    /** immediately cease contact */
    json j = {
      {"FLAG", "ERROR"},
      /** using 0 for now (eg. no error), please update w/ HCLC legal*/
      {"CONTENT", 0}
    };
    std::string s(j.dump());

    /** send claf compliant stop */
    c->P()->Write(s);
    c->P()->Close();
    return;
  }
}

// TODO: Add a flag in protocol template to show what is handled?
void hclc::NodeHandle(Node*) {
  std::cout << "[!] HCLC has no NodeHandle\n";
  return;
}
