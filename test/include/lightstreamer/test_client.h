#ifndef LIGHTSTREAMER_TEST_CLIENT
#define LIGHTSTREAMER_TEST_CLIENT

#include <lightstreamer/client.h>

namespace lightstreamer {
    class TestClient : public Client {
        public:
            void command(std::string const command, command_params const params) override {
                m_last_command = command;
                m_last_params = params;
            }

            void close(websocket::close_reason const& close_reason) override {
                m_last_close_reason = close_reason;
            }

            void last_command(std::string& command, command_params& params) {
                command = m_last_command;
                params = m_last_params;
            }

            void last_close(websocket::close_reason& close_reason) {
                close_reason = m_last_close_reason;
            }

        private:
            std::string m_last_command;
            command_params m_last_params;
            websocket::close_reason m_last_close_reason;
    };
}

#endif
