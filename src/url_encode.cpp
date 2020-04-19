#include <lightstreamer/url_encode.h>

#include <iomanip>
#include <sstream>

namespace lightstreamer {
    std::string url_encode(std::string const& input)
    {
        std::ostringstream os;

        os.fill('0');
        os << std::hex << std::uppercase;

        for (char const &c: input) {
            if (std::isalnum(c) || (c == '_') || (c == '-')) {
                os << c;
            } else {
                os << '%' << std::setw(2) << static_cast<int>(c);
            }
        }

        return os.str();
    }

    int hex2int(char c)
    {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }

        if (c >= 'A' && c <= 'Z') {
            return (c - 'A') + 10;
        }

        if (c >= 'a' && c <= 'z') {
            return (c - 'a') + 10;
        }

        return -1;
    }

    std::string url_decode(std::string const& input)
    {
        std::ostringstream os;

        for (std::size_t i = 0; i < input.size(); i++) {
            char c = input[i];

            bool url_segment = (
                (c == '%') &&
                (i < input.size() - 2) &&
                std::isxdigit(input[i + 1]) &&
                std::isxdigit(input[i + 2]));

            if (!url_segment) {
                os << c;
                continue;
            }

            int hex_value = (hex2int(input[i + 1]) * 16) + hex2int(input[i + 2]);
            i += 2;
        
            os << static_cast<char>(hex_value);
        }

        return os.str();
    }
}