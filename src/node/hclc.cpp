#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <functional>

#include "../../inc/node.hpp"
#include "../../inc/strops.hpp"
#include "../../inc/crypt++.hpp"
#include "../../inc/tree.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// error handler
json error(int error_code) {
  json ret = {
      {"FLAG", "ERROR"},
      {"CONTENT", error_code}
  };
  return ret;
}

// add blocks in context to chain
json update_chain(Conn *conn, json cont = {}) {
  auto CTX = (conn->ctx);
  auto TREE = (*(conn->parent_chains))[CTX.chain_trip];
  TREE.batch_push(CTX.new_blocks);
  std::unordered_set<std::string> new_block_hashes;
  for (const auto& new_block : CTX.new_blocks) {
    new_block_hashes.insert(new_block.hash);
  }
  return {{"FLAG", "DONE"}};
}

// the actual HCLC process starts here

// open contact - COPEN
json client_open(Conn *conn, std::string chain_trip) {
    try {
        auto CTX = (conn->ctx);
        auto TREE = (*(conn->parent_chains))[chain_trip];

        CTX.chain_trip = chain_trip;

        std::unordered_set<std::string> valence_hashes = TREE.get_qualifying_hashes(&Tree::is_childless);

        std::vector<std::string> c_valence_hashes;

        for (auto valence_hash : valence_hashes) {
          c_valence_hashes.push_back(valence_hash);
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
json host_open(Conn *conn, json cont) {
    try {
        auto CTX = (conn->ctx);

        CTX.chain_trip = cont["chain"];

        auto TREE = (*(conn->parent_chains))[CTX.chain_trip];

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
json transfer_blocks(Conn *conn, json cont) {
    try {
        auto CTX = (conn->ctx);
        auto TREE = (*(conn->parent_chains))[CTX.chain_trip];

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
            CTX.new_blocks.push_back(new_block);
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
            update_chain(conn);
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
void _Key_Exchange(Conn *c) {
  json _j;
  _j["FLAG"] = "KE";
  _j["CONT"] =  (c->net)->sec.Public();

  (c->net)->Raw_Write(_j.dump(), c->timeout);
  json _ij = json::parse((c->net)->Raw_Read(c->timeout));

  if (_ij["FLAG"] == "KE") {
    (c->net)->sec.Peer(_ij["CONT"]);
  } else {
    std::cout << "First FC wasn't Key Exchange\n";
  }
}

// communication roadmap
std::map<std::string /*prev flag*/, json (*)(Conn*, json)> next {
    {"COPEN", &host_open},
    {"HOPEN", &transfer_blocks},
    {"BLOCKS", &transfer_blocks},
    {"END", &update_chain}
};

// apply communication roadmap in a function that gets exported
std::string hclc_logic(Conn* conn) {
  if ((conn->net.get())->sec.Shared().empty() && (conn->net.get())->Host()) {
    _Key_Exchange(conn);
  }
  // make sure to;
  // add the socket's info to khosts
  json parsed = json::parse(conn->msg_buffer);
  std::cout << "NEW MSG: " << parsed << "\n";

  // message parsing
  std::string cmd = parsed["FLAG"];
  json cont = parsed["CONTENT"];

  // temp return var
  std::string rmsg;

  // client and server roles can both be stored in func map; communication flags ensure proper order of execution
  try {
    // still don't want to connect over loopback. TODO: add a nicer fix
    //if (!conn->net->Local()) {
    rmsg = ((*next[cmd])(conn, cont)).dump();
    //} //else {rmsg = handle_request(conn, cont).dump();}
  } catch (int err) {
    rmsg = error(err).dump();
  }

  // change 'done' to true to end the communication (make sure to return a <close> message)
  // conn_obj->done = true;
  // return response
  return rmsg;
}