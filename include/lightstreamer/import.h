#ifndef LIGHTSTREAMER_IMPORT
#define LIGHTSTREAMER_IMPORT

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

namespace lightstreamer {
    namespace net = boost::asio;
    using tcp = net::ip::tcp;
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace websocket = beast::websocket;
    namespace ssl = net::ssl;
}

#endif
