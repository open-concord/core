#include "../inc/hclc.hpp"

// add blocks in context to chain
json hclc::update_chain(json cont = {}) {
  /** aliasing */
  auto eCTX = (c->ExchangeCtx);
  auto gCTX = (c->GraphCtx);
  auto TREE = (*(gCTX.ParentMap))[eCTX.ChainTrip];

  TREE.batch_push(eCTX.NewBlocks);
  std::unordered_set<std::string> new_block_hashes;
  for (const auto& block : eCTX.NewBlocks) {
    new_block_hashes.insert(block.hash);
  }
  return {{"FLAG", "DONE"}};
}

/** the actual HCLC process starts here */

// open contact - COPEN
json hclc::client_open(std::string chain_trip) {
    try {
      /** aliasing */  
      auto eCTX = (c->ExchangeCtx);
      auto gCTX = (c->GraphCtx);      
      auto TREE = (*(gCTX.ParentMap))[eCTX.ChainTrip];

      eCTX.ChainTrip = chain_trip;

      std::unordered_set<std::string> valence_hashes = TREE.get_qualifying_hashes(&Tree::is_childless);

      std::vector<std::string> c_valence_hashes;

      for (auto v_hash : valence_hashes) {
        c_valence_hashes.push_back(v_hash);
      }

      json ret = {
          {"FLAG", "COPEN"},
          {"CONTENT", {
            {"chain", chain_trip},
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
    /** aliasing */
    auto eCTX = ((this->c)->ExchangeCtx);
    auto gCTX = ((this->c)->GraphCtx);

    eCTX.ChainTrip = cont["chain"];
    auto TREE = (*(gCTX.ParentMap))[eCTX.ChainTrip];

    std::unordered_set<std::string> valence_hashes = TREE.get_qualifying_hashes(&Tree::is_childless);

    std::vector<std::string> h_valence_hashes;
    std::vector<std::string> c_valence_hashes = cont["val"];
    std::vector<std::string> req_hashes;
                //
    for (auto valence_hash : valence_hashes) {
      h_valence_hashes.push_back(valence_hash);
    }

    auto chain_saved = TREE.get_chain();
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

//send requests for parents of the latest layer of blcoks and fulfill the latest layer of such requests - BLOCKS
json hclc::transfer_blocks(json cont) {
    try {
        auto eCTX = ((this->c)->ExchangeCtx);
        auto gCTX = ((this->c)->GraphCtx);
        auto TREE = (*(gCTX.ParentMap))[eCTX.ChainTrip];

        auto chain_saved = TREE.get_chain();

        std::vector<std::string> prompt_req_hashes = cont["req"];
        std::vector<json> prompt_blocks_packet = cont["packet"];
        std::vector<std::string> req_hashes;
        std::vector<json> blocks_packet;

        //fetch blocks requested
        for (auto prompt_req : prompt_req_hashes) {
          blocks_packet.push_back(block_to_json(chain_saved[prompt_req]));
        }

        //add blocks received and request missing parents
        std::unordered_set<std::string> provided_p_hashes;
        for (auto prompt_block : prompt_blocks_packet) {
            block new_block = json_to_block(prompt_block);
            eCTX.NewBlocks.push_back(new_block);
            for (auto p_hash : new_block.p_hashes) {
                provided_p_hashes.insert(p_hash);
            }
        }

        //collect parents that are not in the chain and need to be requested
        for (auto p_hash : provided_p_hashes) {
            if (chain_saved.find(p_hash) == chain_saved.end()) {
                req_hashes.push_back(p_hash);
            }
        }

        //see if this concludes the exchange
        if (req_hashes.size() == 0 && blocks_packet.size() == 0) {
            this->update_chain();
            return {{"FLAG", "END"}};
        }

        //if it doesn't and we have content to send, go ahead and do that
        json ret = {
          {"FLAG", "BLOCKS"},
          {"CONTENT", {
            {"req", req_hashes},
            {"pack", blocks_packet}
          }}
        };

        return ret;
    } catch (int err) {
        return error(err);
    }
}

// the actual HCLC process ends here

// Keyex
void hclc::Key_Exchange() {
  
  auto NET = ((this->c)->Networking);
  
  json _j;
  _j["FLAG"] = "KE";
  _j["CONT"] = NET.sec.Public();


  NET.Raw_Write(_j.dump());
  json _ij = json::parse(NET.Raw_Read());

  if (_ij["FLAG"] == "KE") {
    NET.sec.Peer(_ij["CONT"]);
  } else {
    std::cout << "First FC wasn't Key Exchange\n";
  }
}

// apply communication roadmap in a function that gets exported
void hclc::ConnH(
    ConnCtx* _c,
    ...
  ) {
  // (possible args)
  std::string chain_trip = "";
  int k = -1;
/**
    std::string chain_trip = "",
    int k = -1
  ) {
*/
  if (this->c == nullptr) {_c = c;}
  auto NET = (c->Networking);
  if (NET.sec.Shared().empty()) {
    this->Key_Exchange();
  }

  /** prompt */
  if (
      c->ExchangeCtx.MessageCtx.empty() 
      && !(NET.host)
      && !chain_trip.empty()
      && k > -1
  ) {
    json ready_message;
    ready_message["FLAG"] = "READY";
    ready_message["CONTENT"] = {
      {"chain", chain_trip},
      {"k", k}
    };
    
    NET.Write(ready_message);
  }

  std::string mb = NET.Read();
  while (mb.empty()) {
    /** wait ig (this is a placeholder until lazy read on peer) */
    std::this_thread::sleep_for(std::chrono::seconds(2));
    mb = NET.Read();
  }
  json parsed = json::parse(mb);
  std::cout << "NEW MSG: " << parsed << "\n";
  
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
  NET.Write(rmsg);
  if (!(c->ExchangeCtx).close) {
    this->ConnH(c);
  } else {
    /** immediately cease contact */
    json j = {
      {"FLAG", "ERROR"},
      /** using 0 for now (eg. no error), please update w/ HCLC legal*/
      {"CONTENT", 0}
    };
    std::string s(j.dump());

    /** send claf compliant stop */
    NET.Write(s);
    NET.Close();
    return;
  }
}
