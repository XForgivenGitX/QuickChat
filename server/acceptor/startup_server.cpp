#include "acceptor.hpp"
#include "connections_manager.hpp"
using namespace server;

int main(int argc, char** argv) 
{ 
    lg::init_logger();
    system("chcp 1251");
    // if(argc != 2) 
    // {
    //     std::cerr << "Incorrect arguments. Usage: <port> \n";
    //     return 1;
    // }
    auto manager = std::make_shared<connections_manager>();
    auto acceptor_ = std::make_shared<acceptor>(9001, manager);
    acceptor_->join();
}

