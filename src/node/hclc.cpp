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

void update_chain(Conn *conn) {
  auto CTX = (conn->ctx);
  auto TREE = (*(conn->parent_chains))[CTX.chain_trip];
  TREE.batch_push(CTX.new_blocks);
  std::unordered_set<std::string> new_block_hashes;
  for (const auto& new_block : CTX.new_blocks) {
    new_block_hashes.insert(new_block.hash);
  }
}

void load_new_blocks(exchange_context ctx, std::vector<json> prov_blocks) {
  for (const auto& new_block_json : prov_blocks) {
    ctx.new_blocks.push_back(json_to_block(new_block_json));
  }
}

void load_req_blocks(json& ret, std::vector<std::string> req_hashes, Tree tree) {
  std::vector<json> provided_blocks;

  for (const auto& req_hash : req_hashes) {
    provided_blocks.push_back(block_to_json(tree.get_chain()[req_hash]));
  }

  ret["CONTENT"]["provided_blocks"] = provided_blocks;
}

std::unordered_set<std::string> iterate_layer(std::unordered_set<std::string> layer, std::unordered_set<std::string> seen_hashes, Tree tree) {
  std::unordered_set<std::string> unseen_hashes;
  std::unordered_set<std::string> p_hash_union = tree.get_parent_hash_union(layer);
  std::set_difference(p_hash_union.begin(), p_hash_union.end(), seen_hashes.begin(), seen_hashes.end(), std::inserter(unseen_hashes, unseen_hashes.begin())); //populate unseen_hashes with current layer hashes not in seen_hashes
  return unseen_hashes;
}

// error handler
json error(int error_code) {
  json ret = {
      {"FLAG", "ERROR"},
      {"CONTENT", error_code}
  };
  return ret;
}

// - handle functions -
json send_blocks(Conn *conn, json cont) {
  try {
    auto CTX = (conn->ctx);
    auto TREE = (*(conn->parent_chains))[CTX.chain_trip];

    json ret = {
      {"FLAG", "HBLOCKS"}
    };

    /** send the blocks that the client requested */
    load_req_blocks(ret, cont["req_hashes"], TREE);

    /** retrieve blocks that the client gave us */
    load_new_blocks(CTX, cont["provided_blocks"]);

    /** send the next layers of hashes */
    std::vector<std::vector<std::string>> hash_layers;

    for (size_t i = 0; i < CTX.k; i++) {
      /** last round of hashes should go in seen hashes */
      for (const auto& prev_hash : CTX.last_layer) {
        CTX.seen_hashes.insert(prev_hash);
      }

      /** get the unseen hashes from the proceeding layer */
      CTX.last_layer = iterate_layer(CTX.last_layer, CTX.seen_hashes, TREE); //get unseen hashes from proceeding layer
      std::vector<std::string> v_unseen_hashes; // now to convert to vector from unordered set for json compatibility
      for (const auto& u_hash : CTX.last_layer) {
        v_unseen_hashes.push_back(u_hash);
      }
      hash_layers.push_back(v_unseen_hashes);
    }

    ret["CONTENT"]["hash_layers"] = hash_layers;

    return ret;
  } catch (int err) {
    return error(err);
  }
}

json begin_sending_blocks(Conn *conn, json cont) {
  try {
    auto CTX = (conn->ctx);
    auto TREE = (*(conn->parent_chains))[CTX.chain_trip];

    json ret = {
      {"FLAG", "HBLOCKS"}
    };

    CTX.chain_trip = cont["chain"];
    CTX.k = cont["k"];
    CTX.last_layer = TREE.get_qualifying_hashes(&Tree::is_childless);

    // set up childless hashes vector

    std::vector<std::string> v_childless_hashes;
    for (const auto& cl_hash : CTX.last_layer) {
      v_childless_hashes.push_back(cl_hash);
    }

    // send the first layers of hashes
    std::vector<std::vector<std::string>> hash_layers({v_childless_hashes});

    for (size_t i = 0; i < CTX.k - 1; i++) {
    // last round of hashes should go in seen hashes
      for (const auto& prev_hash : CTX.last_layer) {
        CTX.seen_hashes.insert(prev_hash);
      }

      // get the unseen hashes from the current layer

      CTX.last_layer = iterate_layer(CTX.last_layer, CTX.seen_hashes, TREE); //get unseen hashes from proceeding layer
      std::vector<std::string> v_unseen_hashes; //now to convert to vector from unordered set for json compatibility
      for (const auto& u_hash : CTX.last_layer) {
        v_unseen_hashes.push_back(u_hash);
      }
      hash_layers.push_back(v_unseen_hashes);
    }

    ret["CONTENT"]["hash_layers"] = hash_layers;
    ret["CONTENT"]["provided_blocks"] = std::vector<std::string>();

    return ret;
  } catch (int err) {
    return error(err);
  }
}

json end_sending_blocks(Conn *conn, json cont) {
  try {
    auto CTX = (conn->ctx);
    auto TREE = (*(conn->parent_chains))[CTX.chain_trip];

    json ret = {
      {"FLAG", "HEND"}
    };

    // send the last requested blocks

    load_req_blocks(ret, cont["req_hashes"], TREE);

    // receive the last of our blocks and update the chain

    load_new_blocks(CTX, cont["provided_blocks"]);

    update_chain(conn);

    return ret;
  } catch (int err) {
    return error(err);
  }
}

json evaluate_blocks(Conn *conn, json cont) {
    try {
        auto CTX = (conn->ctx);
        auto TREE = (*(conn->parent_chains))[CTX.chain_trip];
        // check each subsequent block, see contact.txt
        json ret = {
        {"FLAG", "CBLOCKS"}
        };

        // retrieve blocks that the host gave us

        load_new_blocks(CTX, cont["provided_blocks"]);

        // below: mimicking host behavior to compare contents at each layer with their respective trees

        std::vector<std::string> req_hashes;
        std::vector<json> provided_blocks;
        std::vector<std::vector<std::string>> hash_layers = cont["hash_layers"].get<std::vector<std::vector<std::string>>>();

        for (const auto& v_hash_layer : hash_layers) {
            std::unordered_set<std::string> hash_layer; //convert the vector hash layer to an unordered set
            for (const auto& h : v_hash_layer) {
                hash_layer.insert(h);
            }

            for (const auto& prev_hash : CTX.last_layer) {
                CTX.seen_hashes.insert(prev_hash);
            }


            //initialize CTX.last_layer if this is the first response
            if (CTX.first_layer) {
                CTX.last_layer = TREE.get_qualifying_hashes(&Tree::is_childless);
                CTX.first_layer = false;
            }
            else CTX.last_layer = iterate_layer(CTX.last_layer, CTX.seen_hashes, TREE);

            //if the sets match, we're done
            if (CTX.last_layer == hash_layer) {
                ret["FLAG"] = "CEND";
                break;
            }

            // request hashes = host hashes - client hashes
            std::set_difference(hash_layer.begin(), hash_layer.end(), CTX.last_layer.begin(), CTX.last_layer.end(), std::back_inserter(req_hashes));

            // provided hashes = client hashes - host hashes
            std::vector<std::string> provided_hashes;
            std::set_difference(CTX.last_layer.begin(), CTX.last_layer.end(), hash_layer.begin(), hash_layer.end(), std::back_inserter(provided_hashes));
            for (const auto& prov_hash : provided_hashes) {
                provided_blocks.push_back(block_to_json(TREE.get_chain()[prov_hash]));
            }
        }

        ret["CONTENT"]["provided_blocks"] = provided_blocks;
        ret["CONTENT"]["req_hashes"] = req_hashes;

        return ret;
    } catch (int err) {
        return error(err);
    }
}

json end_evaluating_blocks(Conn *conn, json cont) {
    try {
        auto CTX = (conn->ctx);
        auto TREE = (*(conn->parent_chains))[CTX.chain_trip];

        json ret = {
            {"FLAG", "DONE"}
        };

        // retrieve our last blocks

        load_new_blocks(CTX, cont);

        return ret;
    } catch (int err) {
        return error(err);
    }

}
// - end of C2C handle functions

// map of communication roadmap
std::map<std::string /*prev flag*/, json (*)(Conn*, json)> next {
    {"READY", &begin_sending_blocks},
    {"HBLOCKS", &evaluate_blocks},
    {"CBLOCKS", &send_blocks},
    {"CEND", &end_sending_blocks},
    {"HEND", &end_evaluating_blocks}
};

std::string hclc_logic(Conn* conn) {
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
