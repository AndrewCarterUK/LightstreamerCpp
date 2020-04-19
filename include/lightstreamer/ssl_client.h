#ifndef LIGHTSTREAMER_SSL_CLIENT
#define LIGHTSTREAMER_SSL_CLIENT

#include <lightstreamer/base_client.h>
#include <lightstreamer/credentials.h>
#include <lightstreamer/endpoint.h>
#include <lightstreamer/handler.h>
#include <lightstreamer/import.h>

#include <memory>

namespace lightstreamer {
    typedef websocket::stream<beast::ssl_stream<beast::tcp_stream>> SSLWebSocket;

    class SSLClient : public BaseClient<SSLClient>, public std::enable_shared_from_this<SSLClient> {
        friend class BaseClient<SSLClient>;

        public:
            SSLClient(
                net::io_context& ioc,
                ssl::context& sslc,
                std::shared_ptr<Handler> handler,
                Endpoint& endpoint);

        protected:
            void on_connect(beast::error_code, tcp::resolver::results_type::endpoint_type) override;

        private:
            SSLWebSocket m_websocket;
            beast::ssl_stream<beast::tcp_stream>& m_ssl_stream;
            beast::tcp_stream& m_tcp_stream;

            void on_ssl_handshake(beast::error_code error_code);
    };
}

#endif
