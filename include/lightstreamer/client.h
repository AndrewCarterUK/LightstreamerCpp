#ifndef LIGHTSTREAMER_CLIENT
#define LIGHTSTREAMER_CLIENT

#include <lightstreamer/import.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace lightstreamer {
    typedef std::vector<std::unordered_map<std::string, std::string>> command_params;

    class Client {
        public:
            Client() { }
            virtual ~Client() { }

            virtual void command(std::string const command, command_params const params) = 0;
            virtual void close(websocket::close_reason const& close_reason) = 0;
    };
}

#endif
