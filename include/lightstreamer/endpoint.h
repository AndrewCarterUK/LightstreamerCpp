#ifndef LIGHTSTREAMER_ENDPOINT
#define LIGHTSTREAMER_ENDPOINT

#include <string>

namespace lightstreamer {
    struct Endpoint {
        std::string host;
        int port;
        std::string path;
    };
}

#endif
