#ifndef LIGHTSTREAMER_CREDENTIALS
#define LIGHTSTREAMER_CREDENTIALS

#include <string>

namespace lightstreamer {
    struct Credentials {
        std::string user;
        std::string password;
        std::string adapter_set = "DEFAULT";
        std::string client_id = "mgQkwtwdysogQz2BJ4Ji kOj2Bg";
    };
}

#endif
