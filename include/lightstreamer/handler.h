#ifndef LIGHTSTREAMER_HANDLER
#define LIGHTSTREAMER_HANDLER

#include <lightstreamer/client.h>

#include <memory>
#include <string>
#include <vector>

namespace lightstreamer {
    class Handler {
        public:
            Handler() { }
            virtual ~Handler() { }

            virtual void on_init(std::weak_ptr<Client> client) = 0;

            virtual void on_connect() = 0;

            virtual void on_message(std::vector<std::string> const& items) = 0;

            virtual void on_client_close() = 0;

            virtual void on_server_close() = 0;
    };
}

#endif
