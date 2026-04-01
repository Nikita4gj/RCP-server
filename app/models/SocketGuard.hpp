#pragma  once


#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <tuple>

#include "../utils/errors.hpp"

namespace rpc::models
{
    class SocketGuard
    {
        int _fd;
        
        public:
            explicit SocketGuard(): _fd{-1} {}

            explicit SocketGuard(int socket_fd) : _fd{socket_fd}{}

            SocketGuard(SocketGuard&) = delete;
            SocketGuard operator=(SocketGuard&) = delete;

            SocketGuard(SocketGuard&& other) : _fd{-1}
            {
                _fd = other._fd;
                other._fd = -1;
            }

            SocketGuard& operator=(SocketGuard&& other)
            {
                if(_fd!=-1)
                    ::close(_fd);

                _fd = other._fd;
                other._fd = -1;

                return *this;
            }

            void set_nonblock()
            {
                int flags = fcntl(_fd, F_GETFL);
                fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
            }

            using OPTION = std::tuple<int, int, int>;

            template<class... OPTIONS>
            void set_options(OPTIONS... tuples)
            {
                static_assert(
                    (std::is_constructible_v<OPTION, std::decay_t<OPTIONS>> && ...),
                    "The type must be constructed in std::tuple<int, int, int>"
                );

                auto set_option = [this](OPTION option) mutable
                {
                    auto [level, optname, opt] = option;
                    if(setsockopt(_fd, level, optname, &opt, sizeof(opt))<0)
                        throw_errno("SocketGuard, set_options");
                };

                (set_option(tuples), ...);    
            }

            const int& get(){return _fd;}

            int release(){int f = _fd; _fd = -1; return f;}

            void close()
            {
                if(_fd != -1)
                {
                    ::close(_fd);
                    _fd = -1;
                }
            }

            ~SocketGuard()
            {
                if(_fd !=-1) ::close(_fd);
            }
    };
}
