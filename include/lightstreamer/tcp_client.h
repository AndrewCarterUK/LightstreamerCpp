#ifndef LIGHTSTREAMER_TCP_CLIENT
#define LIGHTSTREAMER_TCP_CLIENT

#include <lightstreamer/base_client.h>
#include <lightstreamer/credentials.h>
#include <lightstreamer/endpoint.h>
#include <lightstreamer/handler.h>
#include <lightstreamer/import.h>

#include <memory>

namespace lightstreamer {
    typedef websocket::stream<beast::tcp_stream> TCPWebSocket;

    class TCPClient : public BaseClient<TCPClient>, public std::enable_shared_from_this<TCPClient> {
        friend class BaseClient<TCPClient>;

        public:
            TCPClient(
                net::io_context& ioc,
                std::shared_ptr<Handler> handler,
                Endpoint& endpoint);

        protected:
            void on_connect(beast::error_code error_code, tcp::resolver::results_type::endpoint_type) override;

        private:
            TCPWebSocket m_websocket;
            beast::tcp_stream& m_tcp_stream;
    };
}

#endif
