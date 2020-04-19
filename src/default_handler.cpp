#include <lightstreamer/default_handler.h>

#include <boost/algorithm/string.hpp>

namespace lightstreamer {
    DefaultHandler::DefaultHandler(Credentials const& credentials) :
        m_credentials(credentials),
        m_last_request_id(0),
        m_last_sub_id(0)
    { }

    void DefaultHandler::on_init(std::weak_ptr<Client> client)
    {
        m_client = client;
    }

    void DefaultHandler::on_connect()
    {
        get_client()->command("create_session", {{
            { "LS_user", m_credentials.user },
            { "LS_password", m_credentials.password },
            { "LS_adapter_set", m_credentials.adapter_set },
            { "LS_cid", m_credentials.client_id }
        }});
    }

    void DefaultHandler::on_message(std::vector<std::string> const& items)
    {
        if (items[0] == "CONOK") {
            on_session(items[1], std::stoi(items[2]), std::stoi(items[3]), items[4]);
            return;
        }

        if (items[0] == "CONERR") {
            on_session_error(std::stoi(items[1]), items[2]);
            return;
        }

        if (items[0] == "SUBOK") {
            on_subscribe(std::stoi(items[1]), std::stoi(items[2]), std::stoi(items[3]));
            return;
        }

        if (items[0] == "U") {
            std::vector<std::string> fields;

            int sub_id{ std::stoi(items[1]) };
            int index{ std::stoi(items[2]) };

            boost::split(fields, items[3], boost::is_any_of("|"));

            on_update(sub_id, index, fields);
            return;
        }

        if (items[0] == "UNSUB") {
            on_unsubscribe(std::stoi(items[1]));
            return;
        }
    }

    void DefaultHandler::on_client_close()
    {
        // do nothing
    }

    void DefaultHandler::on_server_close()
    {
        // do nothing
    }

    void DefaultHandler::on_session(std::string const&, int, int, std::string const&)
    {
        // do nothing
    }

    void DefaultHandler::on_session_error(int, std::string const&)
    {
        // do nothing
    }

    void DefaultHandler::on_subscribe(int, int, int)
    {
        // do nothing
    }

    void DefaultHandler::on_update(int, int, std::vector<std::string> const&)
    {
        // do nothing
    }

    void DefaultHandler::on_unsubscribe(int)
    {
        // do nothing
    }

    std::shared_ptr<Client> const DefaultHandler::get_client()
    {
        return m_client.lock();
    }

    int DefaultHandler::subscribe(
        std::string const& group,
        std::string const& schema,
        std::string const& mode,
        std::string const& data_adapter,
        bool snapshot)
    {
        m_mutex.lock();

        int request_id = ++m_last_request_id;
        int sub_id = ++m_last_sub_id;

        m_mutex.unlock();

        get_client()->command("control", {{
            { "LS_op", "add" },
            { "LS_reqId", std::to_string(request_id) },
            { "LS_subId", std::to_string(sub_id) },
            { "LS_mode", mode },
            { "LS_group", group },
            { "LS_schema", schema },
            { "LS_data_adapter", data_adapter },
            { "LS_snapshot", snapshot ? "true" : "false" }
        }});

        return m_last_sub_id;
    }

    void DefaultHandler::unsubscribe(int sub_id)
    {
        get_client()->command("control", {{
            { "LS_op", "delete" },
            { "LS_reqId", std::to_string(++m_last_request_id) },
            { "LS_subId", std::to_string(sub_id) }
        }});
    }
}
