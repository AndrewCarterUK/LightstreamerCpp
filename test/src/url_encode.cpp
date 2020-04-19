#include <catch2/catch2.hpp>

#include <lightstreamer/url_encode.h>

#include <string>

TEST_CASE( "encoding", "url_encode" ) {
    std::string const input{ "hello , my name is + andrew|?" };
    std::string const expected_output{ "hello%20%2C%20my%20name%20is%20%2B%20andrew%7C%3F" };

    REQUIRE( lightstreamer::url_encode(input) == expected_output );
}

TEST_CASE( "decoding", "url_decode" ) {
    std::string const input{ "hello%20%2C%20my%20name%20is%20%2B%20andrew%7C%3F" };
    std::string const expected_output{ "hello , my name is + andrew|?" };

    REQUIRE( lightstreamer::url_decode(input) == expected_output );
}
