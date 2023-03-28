#pragma once
#include <deque>
#include <memory>
#include <unordered_map>
#include "socket_data.hpp"
#include "protocol.hpp"

namespace server
{
    struct IChannel
    {
        virtual void send_all(const common::msg_type&) = 0;
        virtual void enter_user(anet::socket_data_ptr &) = 0;
        virtual void leave_user(anet::socket_data_ptr &) = 0;
        virtual void leave_all() = 0;
        virtual ~IChannel() = default;
    };
    typedef std::shared_ptr<IChannel> IChannel_ptr;
    
    struct ILobby
    {
        typedef std::optional<server::IChannel_ptr> join_channel_v;
        
        virtual void enter_user(anet::socket_data_ptr &) = 0;
        virtual bool add_channel(const common::name_t& name) = 0;
        virtual join_channel_v join_channel(const common::name_t& name, anet::socket_data_ptr&) = 0;
        virtual void leave_user(anet::socket_data_ptr &) = 0;
        virtual void leave_all() = 0;
        virtual ~ILobby() = default;
    };
    typedef std::shared_ptr<ILobby> ILobby_ptr;

    struct IUser
    {
        common::name_t name_;
        common::pass_t pass_;
        IUser(const common::name_t& name, const common::pass_t& pass)
            : name_(name), pass_(pass) {}
        virtual void send_command(anet::socket_data_ptr&, common::command) = 0;
        virtual ~IUser() = default;
    };
    typedef std::shared_ptr<IUser> IUser_ptr;
}
