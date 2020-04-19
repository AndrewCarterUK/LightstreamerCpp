#include <lightstreamer/base_client.h>
#include <lightstreamer/ssl_client.h>
#include <lightstreamer/tcp_client.h>
#include <lightstreamer/url_encode.h>

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <unordered_map>

namespace lightstreamer {
    template <typename Derived>
    BaseClient<Derived>::BaseClient(
        net::io_context& ioc,
        std::shared_ptr<Handler> handler,
        Endpoint& endpoint
    ) :
        m_resolver(net::make_strand(ioc)),
        m_handler(handler),
        m_endpoint(endpoint),
        m_shutdown(false)
    { }

    template <typename Derived>
    void BaseClient<Derived>::command(std::string const command, command_params const params)
    {
        std::string body = url_encode(command) + "\r\n";

        for (const auto& line_params : params) {
            for (const auto& item : line_params) {
                body += url_encode(item.first) + "=" + url_encode(item.second) + "&";
            }

            body += "\r\n";
        }

        send(body);
    }

    template <typename Derived>
    void BaseClient<Derived>::close(websocket::close_reason const& close_reason)
    {
        auto shared_close_reason{ std::make_shared<websocket::close_reason>(close_reason) };

        net::post(
            derived().m_websocket.get_executor(),
            beast::bind_front_handler(
                &BaseClient<Derived>::do_close,
                derived().shared_from_this(),
                shared_close_reason));
    }

    template <typename Derived>
    void BaseClient<Derived>::begin()
    {
        m_handler->on_init(derived().weak_from_this());

        m_resolver.async_resolve(
            m_endpoint.host,
            std::to_string(m_endpoint.port),
            beast::bind_front_handler(&BaseClient<Derived>::on_resolve, derived().shared_from_this()));

        derived().m_tcp_stream.expires_after(std::chrono::seconds(30));
    }

    template <typename Derived>
    Derived& BaseClient<Derived>::derived()
    {
        return static_cast<Derived&>(*this);
    }

    template <typename Derived>
    void BaseClient<Derived>::on_resolve(beast::error_code error_code, tcp::resolver::results_type results)
    {
        if (error_code) {
            return handle_error_code(error_code);
        }

        derived().m_tcp_stream.async_connect(
            results,
            beast::bind_front_handler(&BaseClient<Derived>::on_connect, derived().shared_from_this()));
    }

    template <typename Derived>
    void BaseClient<Derived>::after_connect()
    {
        derived().m_tcp_stream.expires_never();

        derived().m_websocket.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
    
        derived().m_websocket.set_option(websocket::stream_base::decorator([] (websocket::request_type& request_type) {
            request_type.set(http::field::sec_websocket_protocol, "TLCP-2.1.0.lightstreamer.com");
        }));

        derived().m_websocket.async_handshake(
            m_endpoint.host + ':' + std::to_string(m_endpoint.port),
            m_endpoint.path,
            beast::bind_front_handler(&BaseClient<Derived>::on_websocket_handshake, derived().shared_from_this()));
    }

    template <typename Derived>
    void BaseClient<Derived>::handle_error_code(beast::error_code error_code)
    {
        // Do not raise exceptions for error codes received during shutdown
        if (m_shutdown) {
            return;
        }

        if (error_code == websocket::error::closed) {
            m_shutdown = true;

            m_handler->on_server_close();

            return;
        }

        throw std::runtime_error(std::to_string(error_code.value()) + " -> " + error_code.message());
    }

    template <typename Derived>
    void BaseClient<Derived>::on_websocket_handshake(beast::error_code error_code)
    {
        if (error_code) {
            return handle_error_code(error_code);
        }

        bind_read_handler();

        m_handler->on_connect();
    }

    template <typename Derived>
    void BaseClient<Derived>::do_send(std::shared_ptr<std::string const> const& message)
    {
        m_queue.push_back(message);

        if (m_queue.size() > 1) {
            return;
        }

        write_queue_head();
    }

    template <typename Derived>
    void BaseClient<Derived>::on_write(beast::error_code error_code, std::size_t)
    {
        if (error_code) {
            return handle_error_code(error_code);
        }

        m_queue.erase(m_queue.begin());

        write_queue_head();
    }

    template <typename Derived>
    void BaseClient<Derived>::on_read(beast::error_code error_code, std::size_t)
    {
        if (error_code) {
            return handle_error_code(error_code);
        }

        auto buffer_string{ beast::buffers_to_string(m_buffer.data()) };
        std::size_t start_position{ 0 };

        while (1) {
            std::size_t crlf_position{ buffer_string.find("\r\n", start_position) };

            if (crlf_position == std::string::npos) {
                break;
            }

            std::vector<std::string> items;

            boost::split(items, buffer_string.substr(start_position, crlf_position), boost::is_any_of(","));

            std::for_each(items.begin(), items.end(), [] (std::string& item) {
                item = url_decode(item);
            });

            m_handler->on_message(items);

            start_position = crlf_position + 2;
        }

        m_buffer.consume(start_position);

        bind_read_handler();
                                                                        
    }

    template <typename Derived>
    void BaseClient<Derived>::do_close(std::shared_ptr<websocket::close_reason const> const& close_reason)
    {
        m_shutdown = true;

        derived().m_websocket.async_close(
            *close_reason,
            beast::bind_front_handler(&BaseClient<Derived>::on_close, derived().shared_from_this()));
    }

    template <typename Derived>
    void BaseClient<Derived>::on_close(beast::error_code error_code)
    {
        if (error_code) {
            return handle_error_code(error_code);
        }

        m_handler->on_client_close();                                                                
    }

    template <typename Derived>
    void BaseClient<Derived>::send(std::string& message)
    {
        auto shared_message{ std::make_shared<std::string>(message) };

        net::post(
            derived().m_websocket.get_executor(),
            beast::bind_front_handler(&BaseClient<Derived>::do_send, derived().shared_from_this(), shared_message));
    }

    template <typename Derived>
    void BaseClient<Derived>::write_queue_head()
    {
        if (m_queue.empty()) {
            return;
        }

        derived().m_websocket.async_write(
            net::buffer(*m_queue.front()),
            beast::bind_front_handler(&BaseClient<Derived>::on_write, derived().shared_from_this()));
    }

    template <typename Derived>
    void BaseClient<Derived>::bind_read_handler()
    {
        derived().m_websocket.async_read(
            m_buffer,
            beast::bind_front_handler(&BaseClient<Derived>::on_read, derived().shared_from_this()));
    }

    template class BaseClient<TCPClient>;
    template class BaseClient<SSLClient>;
}
