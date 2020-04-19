#include <lightstreamer.h>

#include <iostream>
#include <memory>

class Handler : public lightstreamer::DefaultHandler {
    public:
        Handler(lightstreamer::Credentials const& credentials) : lightstreamer::DefaultHandler(credentials) { }

    protected:
        // This is triggered on receipt of a CONOK message
        void on_session(std::string const& session_id, int, int, std::string const&) override {
            std::cout << "Connected (session id: " << session_id << ")\n\n";

            int sub_id = subscribe("item item2 item3", "stock_name last_price", "MERGE", "QUOTE_ADAPTER");

            std::cout << "Requested subscription (sub_id: " << sub_id << ")\n";
        }

        // This is triggered on receipt of a CONERR message
        void on_session_error(int error_code, std::string const& error_message) override {
            std::cout << "Error connecting (code: " << error_code << ", message: " << error_message << ")\n";
        }

        // This is triggered on receipt of a SUBOK message
        void on_subscribe(int sub_id, int, int) override {
            std::cout << "Subscription confirmed (sub id: " << sub_id << ")\n";
        }

        // This is triggered on receipt of a U message
        void on_update(int sub_id, int index, std::vector<std::string> const& fields) override {
            std::cout << "Received update (sub id: " << sub_id << ", index: " << index << "), fields:";

            for (auto& field : fields) {
                std::cout << " '" << field << '\'';
            }

            std::cout << '\n';
        }

        // This is triggered on receipt of an UNSUB message
        void on_unsubscribe(int sub_id) override
        {
            std::cout << "Unsubscribe confirmed (sub id " << sub_id << ")\n";
        }

        // Triggered by: get_client()->close(lightstreamer::beast::websocket::close_code::normal);
        void on_client_close() override
        {
            std::cout << "Connection closed by client\n";
        }

        // Triggered when the server closes the connection
        void on_server_close() override
        {
            std::cout << "Connection closed by server\n";
        }
};

int main()
{
    lightstreamer::Endpoint endpoint{ "push.lightstreamer.com", 80, "/lightstreamer" };
    lightstreamer::Credentials credentials { "user", "pass", "DEMO" };

    auto handler{ std::make_shared<Handler>(credentials) };

    lightstreamer::net::io_context ioc;

    std::make_shared<lightstreamer::TCPClient>(ioc, handler, endpoint)->begin();

    ioc.run();

    return EXIT_SUCCESS;
}
