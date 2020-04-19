#include <lightstreamer/ssl_client.h>

namespace lightstreamer {
    SSLClient::SSLClient(
        net::io_context& ioc,
        ssl::context& sslc,
        std::shared_ptr<Handler> handler,
        Endpoint& endpoint
    ) :
        BaseClient<SSLClient>(ioc, handler, endpoint),
        m_websocket(net::make_strand(ioc), sslc),
        m_ssl_stream(m_websocket.next_layer()),
        m_tcp_stream(m_ssl_stream.next_layer())
    { }

    void SSLClient::on_connect(beast::error_code error_code, tcp::resolver::results_type::endpoint_type)
    {
        if (error_code) {
            return handle_error_code(error_code);
        }

        m_websocket.next_layer().async_handshake(
            ssl::stream_base::client,
            beast::bind_front_handler(&SSLClient::on_ssl_handshake, shared_from_this()));
    }

    void SSLClient::on_ssl_handshake(beast::error_code error_code)
    {
        if (error_code) {
            return handle_error_code(error_code);
        }

        after_connect();
    }
}
