#pragma once

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <memory>
#include <vector>
#include <optional>
#include <shared_mutex>
#include <string>
#include "socket_data.hpp"
#include "logger.hpp"
#include <boost/noncopyable.hpp>

namespace db
{
    struct chat_room{};
    
    
    typedef std::string name_t;
    typedef std::string pass_t;
    typedef std::size_t id_type;
    typedef std::map<name_t, user_data> users_tree;
    typedef users_tree::const_iterator users_tree_it;
    typedef std::unordered_map<anet::socket_data_ptr, users_tree_it> activeSockets;

    struct server_database : public boost::noncopyable
    {
    private:
        users_tree userData_;
        activeSockets activeSockets_;

    private:
        mutable std::shared_mutex userDataMut, userNamesMut, activeSocketsMut;

    public:
        std::optional<users_tree_it> check_user_data(const name_t& name, const pass_t& pass) const
        {
            std::shared_lock lock(userDataMut);
            users_tree_it it = userData_.find(name);
            return ((it != userData_.end() && (it->second.pass_ == pass)) ? 
                            std::optional<users_tree_it>(it) : std::nullopt);
        }

        

    public:
        
        typedef std::pair<users_tree_it, bool> insert_user_return_v;
        insert_user_return_v insert_user(const user_data &user);
        
        void erase_user(const name_t &user);
        void insert_socket(const anet::socket_data_ptr& socketData, users_tree_it user = {})
        {
            std::lock_guard lock(activeSocketsMut);
            activeSockets_[socketData] = user;
        }
        void erase_socket(const anet::socket_data_ptr& socketData)
        {
            std::lock_guard lock(activeSocketsMut);
            activeSockets_.erase(socketData);
        }
    };

    
    // struct lobby_database : public boost::noncopyable
    // {
    //     typedef std::unordered_map<anet::socket_data_ptr, ip_type> active_ip_table;
    //     typedef std::unordered_map<anet::socket_data_ptr, name_t> active_names_table;
    //     typedef std::unordered_map<name_t, server::chat_room_ptr> room_table;

    // private:
    //     active_ip_table activeIp_{};
    //     active_names_table activeNames_{};
    //     room_table rooms{};

    // private:
    //     mutable std::shared_mutex activeIpMut, activeNamesMut, roomsMut;

    // public:
    //     bool is_contains_room_name(const name_t &name) const;
    //     server::chat_room_ptr get_chat_room_ptr(const name_t &name) const;
    //     ip_type get_active_ip(const anet::socket_data_ptr& socketData) const;
    //     name_t get_active_name(const anet::socket_data_ptr& socketData) const;

    // public:
    //     void shutdown_all_sockets();
    //     void insert_active_ip(const anet::socket_data_ptr& socketData, const ip_type& ip);
    //     void erase_active_ip(const anet::socket_data_ptr& socketData);
    //     void insert_active_name(const anet::socket_data_ptr& socketData, const name_t& name);
    //     void erase_active_name(const anet::socket_data_ptr& socketData);
    //     void insert_room(const server::chat_room_ptr& room, const name_t& name);

    // };
}