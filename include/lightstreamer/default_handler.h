#ifndef LIGHTSTREAMER_DEFAULT_HANDLER
#define LIGHTSTREAMER_DEFAULT_HANDLER

#include <lightstreamer/client.h>
#include <lightstreamer/credentials.h>
#include <lightstreamer/handler.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace lightstreamer {
    class DefaultHandler : public Handler {
        public:
            DefaultHandler(Credentials const& credentials);

            void on_init(std::weak_ptr<Client> client) override;

            void on_connect() override;

            void on_message(std::vector<std::string> const& items) override;

            void on_client_close() override;

            void on_server_close() override;

        protected:
            virtual void on_session(
                std::string const& session_id,
                int request_limit,
                int keep_alive_time,
                std::string const& control_link);

            virtual void on_session_error(int error_code, std::string const& error_message);

            virtual void on_subscribe(int sub_id, int num_items, int num_fields);

            virtual void on_update(int sub_id, int index, std::vector<std::string> const& fields);

            virtual void on_unsubscribe(int sub_id);

            std::shared_ptr<Client> const get_client();

            int subscribe(
                std::string const& group,
                std::string const& schema,
                std::string const& mode =  "MERGE",
                std::string const& data_adapter = "",
                bool snapshot = true);

            void unsubscribe(int sub_id);

        private:
            Credentials const m_credentials;
            std::weak_ptr<Client> m_client;
            int m_last_request_id;
            int m_last_sub_id;
    };
}

#endif
