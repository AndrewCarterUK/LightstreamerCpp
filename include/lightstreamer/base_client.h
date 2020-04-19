#ifndef LIGHTSTREAMER_WEBSOCKET_SESSION
#define LIGHTSTREAMER_WEBSOCKET_SESSION

#include <lightstreamer/client.h>
#include <lightstreamer/credentials.h>
#include <lightstreamer/endpoint.h>
#include <lightstreamer/handler.h>
#include <lightstreamer/import.h>

#include <memory>
#include <string>
#include <vector>

namespace lightstreamer {
    template <typename Derived>
    class BaseClient : public Client {
        public:
            BaseClient(net::io_context& ioc, std::shared_ptr<Handler> handler, Endpoint& endpoint);

            void begin();

            void command(std::string const command, command_params const params) override;

            void close(websocket::close_reason const& close_reason) override;

        protected:
            virtual void on_connect(beast::error_code error_code, tcp::resolver::results_type::endpoint_type) = 0;

            virtual void after_connect() final;

            // Error handling
            void handle_error_code(beast::error_code error_code);

        private:
            tcp::resolver m_resolver;
            std::shared_ptr<Handler> m_handler;
            Endpoint m_endpoint;
            beast::flat_buffer m_buffer;
            std::vector<std::shared_ptr<std::string const>> m_queue;
            bool m_shutdown;

            // CRTP
            Derived& derived();

            // Event handlers
            void on_resolve(beast::error_code error_code, tcp::resolver::results_type results);

            void on_websocket_handshake(beast::error_code error_code);

            void do_send(std::shared_ptr<std::string const> const& message);

            void on_write(beast::error_code error_code, std::size_t);

            void on_read(beast::error_code error_code, std::size_t);

            void do_close(std::shared_ptr<websocket::close_reason const> const& close_reason);

            void on_close(beast::error_code error_code);

            // IO
            void send(std::string& message);

            void write_queue_head();

            void bind_read_handler();
    };
}

#endif
