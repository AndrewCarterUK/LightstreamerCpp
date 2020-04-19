#include <catch2/catch2.hpp>

#include <lightstreamer.h>
#include <lightstreamer/test_client.h>

#include <memory>
#include <string>

TEST_CASE( "on_connect", "DefaultHandler" ) {
    lightstreamer::Credentials credentials{ "user", "password", "DEMO" };
    lightstreamer::DefaultHandler handler{ credentials };
    std::shared_ptr<lightstreamer::TestClient> test_client = std::make_shared<lightstreamer::TestClient>();

    handler.on_init(test_client);

    handler.on_connect();

    std::string command;
    lightstreamer::command_params params;

    test_client->last_command(command, params);

    REQUIRE( command == "create_session" );
    REQUIRE( params.size() == 1 );
    REQUIRE( params[0]["LS_user"] == "user" );
    REQUIRE( params[0]["LS_password"] == "password" );
    REQUIRE( params[0]["LS_adapter_set"] == "DEMO" );
    REQUIRE( params[0]["LS_cid"] == "mgQkwtwdysogQz2BJ4Ji kOj2Bg" );
}

TEST_CASE( "on_message CONOK", "DefaultHandler" ) {
    class TestHandler : public lightstreamer::DefaultHandler {
        public:
            TestHandler(lightstreamer::Credentials const& credentials) : lightstreamer::DefaultHandler(credentials) { }

            std::string m_session_id;
            int m_request_limit;
            int m_keep_alive_time;
            std::string m_control_link;

        protected:
            void on_session(
                std::string const& session_id,
                int request_limit,
                int keep_alive_time,
                std::string const& control_link
            ) {
                m_session_id = session_id;
                m_request_limit = request_limit;
                m_keep_alive_time = keep_alive_time;
                m_control_link = control_link;
            }
    };

    lightstreamer::Credentials credentials{ "user", "password", "DEMO" };
    TestHandler handler{ credentials };
    std::shared_ptr<lightstreamer::TestClient> test_client = std::make_shared<lightstreamer::TestClient>();

    handler.on_init(test_client);

    handler.on_message({ "CONOK", "testsessionid", "50000", "5000", "*" });

    REQUIRE( handler.m_session_id == "testsessionid" );
    REQUIRE( handler.m_request_limit == 50000 );
    REQUIRE( handler.m_keep_alive_time == 5000 );
    REQUIRE( handler.m_control_link == "*" );
}

TEST_CASE( "on_message CONERR", "DefaultHandler" ) {
    class TestHandler : public lightstreamer::DefaultHandler {
        public:
            TestHandler(lightstreamer::Credentials const& credentials) : lightstreamer::DefaultHandler(credentials) { }

            int m_error_code;
            std::string m_error_message;

        protected:
            void on_session_error(int error_code, std::string const& error_message) {
                m_error_code = error_code;
                m_error_message = error_message;
            }
    };

    lightstreamer::Credentials credentials{ "user", "password", "DEMO" };
    TestHandler handler{ credentials };
    std::shared_ptr<lightstreamer::TestClient> test_client = std::make_shared<lightstreamer::TestClient>();

    handler.on_init(test_client);

    handler.on_message({ "CONERR", "67", "LS_cid parameter missing" });

    REQUIRE( handler.m_error_code == 67 );
    REQUIRE( handler.m_error_message == "LS_cid parameter missing" );
}

TEST_CASE( "on_subscribe SUBOK", "DefaultHandler" ) {
    class TestHandler : public lightstreamer::DefaultHandler {
        public:
            TestHandler(lightstreamer::Credentials const& credentials) : lightstreamer::DefaultHandler(credentials) { }

            int m_sub_id;
            int m_num_items;
            int m_num_fields;

        protected:
            void on_subscribe(int sub_id, int num_items, int num_fields) {
                m_sub_id = sub_id;
                m_num_items = num_items;
                m_num_fields = num_fields;
            }
    };

    lightstreamer::Credentials credentials{ "user", "password", "DEMO" };
    TestHandler handler{ credentials };
    std::shared_ptr<lightstreamer::TestClient> test_client = std::make_shared<lightstreamer::TestClient>();

    handler.on_init(test_client);

    handler.on_message({ "SUBOK", "1", "2", "3" });

    REQUIRE( handler.m_sub_id == 1 );
    REQUIRE( handler.m_num_items == 2 );
    REQUIRE( handler.m_num_fields == 3 );
}

TEST_CASE( "on_update U", "DefaultHandler" ) {
    class TestHandler : public lightstreamer::DefaultHandler {
        public:
            TestHandler(lightstreamer::Credentials const& credentials) : lightstreamer::DefaultHandler(credentials) { }

            int m_sub_id;
            int m_index;
            std::vector<std::string> m_fields;

        protected:
            void on_update(int sub_id, int index, std::vector<std::string> const& fields) {
                m_sub_id = sub_id;
                m_index = index;
                m_fields = fields;
            }
    };

    lightstreamer::Credentials credentials{ "user", "password", "DEMO" };
    TestHandler handler{ credentials };
    std::shared_ptr<lightstreamer::TestClient> test_client = std::make_shared<lightstreamer::TestClient>();

    handler.on_init(test_client);

    handler.on_message({ "U", "1", "3", "Foo|Bar|2.03" });

    REQUIRE( handler.m_sub_id == 1 );
    REQUIRE( handler.m_index == 3 );
    REQUIRE( handler.m_fields == std::vector<std::string>{ "Foo", "Bar", "2.03" } );
}

TEST_CASE( "on_unsubscribe UNSUB", "DefaultHandler" ) {
    class TestHandler : public lightstreamer::DefaultHandler {
        public:
            TestHandler(lightstreamer::Credentials const& credentials) : lightstreamer::DefaultHandler(credentials) { }

            int m_sub_id;

        protected:
            void on_unsubscribe(int sub_id) {
                m_sub_id = sub_id;
            }
    };

    lightstreamer::Credentials credentials{ "user", "password", "DEMO" };
    TestHandler handler{ credentials };
    std::shared_ptr<lightstreamer::TestClient> test_client = std::make_shared<lightstreamer::TestClient>();

    handler.on_init(test_client);

    handler.on_message({ "UNSUB", "1" });

    REQUIRE( handler.m_sub_id == 1 );
}
