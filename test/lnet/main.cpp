#include <hclc.hpp>
#include <iostream>
#include <map>

/** local network test (e.g. local nodes w/ tree + hclc) */

using Forest = std::map<std::string, Tree>;

struct Agent : public Node {
  std::map<std::string, Tree> f; 
  Agent(
      unsigned int port, 
      std::map<std::string, Tree> f
    ) : Node(port), f(f){
    this->Graph.Forest = &(this->f);
  }
};

int main(void) {
  Forest af, bf;
  std::string ttrip = gen::trip("seed");
  af[ttrip] = Tree("./achains");
  bf[ttrip] = Tree("./bchains");


  hclc ha(ttrip), hb(ttrip);
  Agent Alice(1337, std::move(af));
  Agent Bob(1338, std::move(bf));
  
  std::cout << "== Completed intialisation ==\n";

  Alice.Lazy(true, false);   
  Alice.R()->Open();
  std::cout << "Alice now waiting\n";
  

  std::cout << "Bob Contacting\n";
  Bob.Contact("127.0.0.1", 1337);

  std::cout << "Waiting to Ensure Connection\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  
  std::cout << "== Beginning HCLC Exchange ==\n";
  
  std::cout << "Alice Connection Count: " << Alice.Connections.size() << '\n';
  std::cout << "Bob Connection Count: " << Bob.Connections.size() << '\n';

  std::jthread at(&hclc::ConnHandle, ha, Alice.Connections.at(0).get());
  std::jthread bt(&hclc::ConnHandle, ha, Bob.Connections.at(0).get());
  
  at.detach();
  bt.detach();

  std::cout << "== Waiting for HCLC to complete ==\n";
  while(!Alice.Connections.at(0).get()->Flags.Get(Conn::CLOSE)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }

  std::cout << "Stopping Bob and Alice\n";
  Alice.Stop();
  Bob.Stop();

  std::cout << "All done\n";
};
