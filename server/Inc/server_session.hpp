#pragma once
#include <server_module.hpp>

namespace server
{
    struct access_user
    {
    public:
        std::string name_, pass_;
        std::atomic<bool> status;

    public:
        access_user(std::string name, std::string pass) : name_(name), pass_(pass) {}
        bool is_name(std::string &rhs)
        {
            return name_ == rhs;
        }
        bool operator==(access_user &rhs)
        {
            return (rhs.name_ == name_) == (rhs.pass_ == pass_);
        }
    };

    class user_database : public boost::noncopyable
    {
        using ip_type = std::string;

        using users_config_tree = std::map<ip_type, access_user>;
        using users_names_tree = std::set<std::string>;
        using room_config_table = std::unordered_map<std::string, chat_room>;

    private:
        users_config_tree usersConfig;
        users_names_tree usersNames;
        room_config_table activeRooms;

    public: // find, set and get methods
    };

    class server_control_block : public user_database
    {
        const anet::end_point_wrapper endPoint_;
        io__::io_context &ios_;
    
    public:
        server_control_block(io__::io_context &ios, unsigned short port)
            : ios_(ios), endPoint_(port, io__::ip::tcp::v4()) {}
    
        void start_accepting_connections() //timeout
        {
            accept_connections(utility::safe_make_unique<anet::tcp_listener>(ios_, endPoint_));
        }
    
    private:
        void accept_connections(anet::tcp_listener_ptr &&listener) noexcept
        {
            anet::listen::accepting_connection(std::move(listener), {&accepted_connection_handler, this});
        }

        void accepted_connection_handler(anet::tcp_listener_ptr &&listener, const boost::system::error_code &error) noexcept
        {
            
            accept_connections(std::move(listener));
        }
    };

    class server_session : public boost::noncopyable
    {
    private:
        io__::io_context &ios_;
        const anet::end_point_wrapper endPoint_;
        chat_room room_;

    public:
        server_session(io__::io_context &ios, unsigned short port);
        void start();

    private:
        void accepted_connection_handler(anet::tcp_listener_ptr &&listener, const boost::system::error_code &error) noexcept;
        void accept_connections(anet::tcp_listener_ptr &&listener) noexcept;
        void authorization_handler(anet::socket_data_ptr socketData, const boost::system::error_code &error) noexcept;
    };
}