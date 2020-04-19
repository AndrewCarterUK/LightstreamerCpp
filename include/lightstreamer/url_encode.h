#ifndef LIGHTSTREAMER_URL_ENCODE
#define LIGHTSTREAMER_URL_ENCODE

#include <string>

namespace lightstreamer {
    std::string url_encode(std::string const& input);

    std::string url_decode(std::string const& input);
}

#endif
