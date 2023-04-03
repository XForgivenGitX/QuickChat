#pragma once
#include "socket_data.hpp"

namespace anet
{
    constexpr uint16_t FRAME_MAX_LENGHT = 128;
    constexpr uint16_t FRAME_MIN_LENGHT = 1;
    
    struct send_receive
    {
        typedef utility::task_wrapped<void(socket_data_ptr&, 
                                const err_c &)> callback_func_t;
    
    public:
        static void send(const socket_data_ptr& socketData, callback_func_t &&handler);
        static void receive(const socket_data_ptr& socketData, callback_func_t &&handler,
                            std::size_t atMostBytes = FRAME_MAX_LENGHT, 
                            std::size_t atLeastBytes = FRAME_MIN_LENGHT);

    private:
        struct callback_function_wrapper;
        typedef std::function<void(const err_c &, 
                                    std::size_t, callback_function_wrapper*)> called_function;
        
        static void send_called_function(const err_c &error_c, 
                                                    std::size_t bytesTransferred, 
                                                        callback_function_wrapper* wrapper);
        
        static void receive_called_function(const err_c &error_c, 
                                                    std::size_t bytesTransferred, 
                                                        callback_function_wrapper* wrapper);
    private:
        send_receive() = delete;
    };
    
    struct send_receive::callback_function_wrapper
    {
    public:
        socket_data_ptr socketData_;
        callback_func_t functionWrapped_;
        called_function call_;
    
    public:
        explicit callback_function_wrapper(const socket_data_ptr& socketData, 
                                        callback_func_t &&handler, called_function&& call);
        void operator()(const err_c &error_c, 
                                std::size_t bytesTransferred);
    };
}