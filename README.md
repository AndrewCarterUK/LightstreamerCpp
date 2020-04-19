# LightstreamerCpp

[![C++ Test](https://github.com/AndrewCarterUK/LightstreamerCpp/workflows/C++%20Test/badge.svg)](https://github.com/AndrewCarterUK/LightstreamerCpp/actions?query=workflow%3A%22C%2B%2B+Test%22+branch%3Amaster)

LightstreamerCpp is an asynchronous, thread safe client for the [Lightstreamer](https://lightstreamer.com/) protocol.

The library uses [Boost.Beast](https://www.boost.org/doc/libs/1_72_0/libs/beast/doc/html/index.html) for networking.

## Examples

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <lightstreamer.h>

class Handler : public lightstreamer::DefaultHandler {
    public:
        Handler(lightstreamer::Credentials const& credentials) : lightstreamer::DefaultHandler(credentials) { }

    protected:
        // This is triggered on receipt of a CONOK message
        void on_session(std::string const& session_id, int, int, std::string const&) override {
            std::cout << "Connected (session id: " << session_id << ")\n";

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
    lightstreamer::Credentials credentials { "user", "pass", "DEMO" };
    lightstreamer::Endpoint endpoint{ "push.lightstreamer.com", 80, "/lightstreamer" };
    // lightstreamer::Endpoint endpoint{ "push.lightstreamer.com", 443, "/lightstreamer" };

    auto handler{ std::make_shared<Handler>(credentials) };

    lightstreamer::net::io_context ioc;
    // lightstreamer::ssl::context sslc{ lightstreamer::ssl::context::tlsv12_client };

    std::make_shared<lightstreamer::TCPClient>(ioc, handler, endpoint)->begin();
    // std::make_shared<lightstreamer::SSLClient>(ioc, sslc, handler, endpoint)->begin();

    ioc.run();

    return EXIT_SUCCESS;
}
```
