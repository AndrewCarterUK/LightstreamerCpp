#include <lightstreamer/tcp_client.h>

namespace lightstreamer {
    TCPClient::TCPClient(
        net::io_context& ioc,
        std::shared_ptr<Handler> handler,
        Endpoint& endpoint
    ) :
        BaseClient<TCPClient>(ioc, handler, endpoint),
        m_websocket(net::make_strand(ioc)),
        m_tcp_stream(m_websocket.next_layer())
    { }

    void TCPClient::on_connect(beast::error_code error_code, tcp::resolver::results_type::endpoint_type)
    {
        if (error_code) {
            return handle_error_code(error_code);
        }

        after_connect();
    }
}
