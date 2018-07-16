#include "catch.hpp"
#include "catch_test_util.hpp"
#include "stream_handle.hpp"

TEST_CASE("String stream handle") {
    StringStreamHandle handle{};
    int sendData = 0xDEADBEEF;
    int receiveData;

    handle.write((char const *)&sendData, sizeof(int));
    handle.read((char *)&receiveData, sizeof(int));

    REQUIRE(receiveData == sendData);
}

TEST_CASE("Local stream handle") {
    StreamHandlePtr handle;
    std::string endpointName = "starboard_test";
    int sendData = 0xDEADBEEF;
    int receiveData;

    forkChild(
    [&] {
        handle = LocalStreamHandle::clientEnd(endpointName);

        handle->write((char const *)&sendData, sizeof(int));
        handle->read((char *)&receiveData, sizeof(int));
        
        return (receiveData == sendData);
    }, [&] {
        handle = LocalStreamHandle::serverEnd(endpointName);

        handle->write((char const *)&sendData, sizeof(int));
        handle->read((char *)&receiveData, sizeof(int));

        return (receiveData == sendData);
    });
}