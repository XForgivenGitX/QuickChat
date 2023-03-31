#include "acceptor.hpp"

namespace server
{
    acceptor::acceptor(anet::port_t port, IConnection_ptr connectionManager)
        : pool_(boost::thread::hardware_concurrency()), connectionManager_(connectionManager)
    {
        start_accepting_connections(port);
    }

    void acceptor::start_accepting_connections(anet::port_t port)
    {
        auto tcpListener = std::make_unique<anet::tcp_listener>(pool_.get_executor(), anet::end_point_wrapper{port, io__::ip::tcp::v4()});
        auto &listenerSocket = tcpListener->socketData_;
        accept_connections(std::move(tcpListener));

#ifdef SERVER_ENABLE_HANDLER_TRACKING
       BOOST_LOG_TRIVIAL(info) 
            << lg::build_log(lg::address_cat("server=", this), 
                "start accepting conections", 
                "port: " + std::to_string(port));
#endif
        start_signal_set(listenerSocket);
    }

    void acceptor::start_signal_set(anet::socket_data_ptr &listenerSocket)
    {
        io__::post(pool_.get_executor(), [&listenerSocket, this]
        {
            io__::signal_set set(signalIos_.get_executor(), SIGINT, SIGTERM, SIGABRT);
            set.async_wait(std::bind(&acceptor::signal_handler,
                this, listenerSocket, std::placeholders::_1,
                std::placeholders::_2));
            signalIos_.run();
        });
    }

    void acceptor::accept_connections(anet::tcp_listener_ptr &&listener)
    {
        anet::listen::accepting_connection(std::move(listener),
                                           {&acceptor::accepted_connection_handler, this});
    }

    void acceptor::accepted_connection_handler(anet::tcp_listener_ptr &&listener, const anet::err_c &error_c) noexcept
    {
#ifdef SERVER_ENABLE_HANDLER_TRACKING
        BOOST_LOG_TRIVIAL(info) 
            << lg::build_log(lg::address_cat("server=", this), 
                "called accepted connection handler", 
                "endp: " + listener->socketData_->get_ip(), 
                "sock: " + std::to_string(listener->socketData_->get_handle()),
                "status: " + error_c.message());
#endif
        if (error_c)
        {
            listener->socketData_->shutdown();
        }
        else
        {
            auto newSocket = std::make_shared<anet::socket_data>(pool_.get_executor());
            std::swap(newSocket, listener->socketData_);
            io__::post(pool_.get_executor(), std::bind(&IConnection::enter_user, connectionManager_, newSocket));
            accept_connections(std::move(listener));
        }
    }

    void acceptor::join()
    {
        pool_.join();
    }
    
    void acceptor::signal_handler(anet::socket_data_ptr listenerSocket, [[maybe_unused]] const anet::err_c &error_c, int)
    {
#ifdef SERVER_ENABLE_HANDLER_TRACKING
        BOOST_LOG_TRIVIAL(warning) 
            << lg::build_log(lg::address_cat("server=", this), 
                "called signal handler", 
                "status: " + error_c.message());
#endif
        pool_.stop();
        listenerSocket->shutdown();
        connectionManager_->leave_all();
    }
}