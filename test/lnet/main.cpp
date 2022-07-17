#include <hclc.hpp>
#include <iostream>
#include <map>

/** local network test (e.g. local nodes w/ tree + hclc) */

using Forest = std::map<std::string, Tree*>;

struct Agent : public Node {
  Agent(
      unsigned int port, 
      std::map<std::string, Tree*> f
    ) : Node(port) {
      this->Graph.Forest = f;
  }
};

void doExchange(Agent* Alice, Agent* Bob, std::string ttrip) {
  hclc ha(ttrip), hb(ttrip);
  Alice->Lazy(true, false);
  Alice->R()->Open();
  std::cout << "Alice now waiting\n";
  

  std::cout << "Bob Contacting\n";
  Bob->Contact("127.0.0.1", 1337);

  std::cout << "Waiting to Ensure Connection\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
 
  std::cout << "== Beginning HCLC Exchange ==\n";
  
  std::cout << "Alice Connection Count: " << Alice->Connections.size() << '\n';
  std::cout << "Bob Connection Count: " << Bob->Connections.size() << '\n';

  std::jthread at(&hclc::ConnHandle, ha, Alice->Connections.back().get());
  std::jthread bt(&hclc::ConnHandle, hb, Bob->Connections.back().get());
  
  at.detach();
  bt.detach();

  std::cout << "== Waiting for HCLC to complete ==\n";
  while(!Alice->Connections.back().get()->Flags.Get(Conn::CLOSE)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
  /** prune connections */
  Alice->Connections.pop_back();
  Bob->Connections.pop_back();
};


int main(void) {
  Forest af, bf;
  std::string ttrip{gen::trip("seed")};
 
  af[ttrip] = new Tree("./achains");
  bf[ttrip] = new Tree("./bchains");
  
  Agent Alice(1337, std::move(af));
  Agent Bob(1338, std::move(bf));
  
  std::cout << "== Completed intialisation ==\n";
  std::cout << "== Adding new blocks to Alice ==\n";
  // TODO 
  block origin(
      "hello",
      {},
      3,
      gen::trip("lain")
  );
  block first(
      "what's up",
      {origin.hash},
      3,
      gen::trip("lain")
  );
  Alice.Graph.Forest[ttrip]->batch_push({origin, first});
  
  doExchange(&Alice, &Bob, ttrip);
  
  std::cout << "== Adding new blocks to Bob ==\n";
  block wasd(
      "i'm new here",
      {origin.hash, first.hash},
      3,
      gen::trip("lain")
  );
  Bob.Graph.Forest[ttrip]->chain_push(wasd);

  doExchange(&Alice, &Bob, ttrip);

  std::cout << "Stopping Bob and Alice\n";
  
  Alice.Stop();
  Bob.Stop();
  
  for (const auto& [t, b] : Alice.Graph.Forest[ttrip]->get_chain()) {
    std::cout << t << b.dump() << '\n';
  }
  std::cout << "All done\n";
};
