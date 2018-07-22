#include <sstream>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "stream_handle.hpp"
#include "stream_protocol.hpp"

TEST_CASE("Serialization of module port numbers") {
    StringStreamHandle stream{};

    int modulePortNum = 72;
    serializeModulePortNum(stream, modulePortNum);

    int receivedModulePortNum = deserializeModulePortNum(stream);
    REQUIRE(receivedModulePortNum == modulePortNum);
}

TEST_CASE("Serialization of messages") {
    StringStreamHandle stream{};

    std::unique_ptr<IntTensor> intTensor =
        make_unique<IntTensor>((NumSizes){1}, 1);
    intTensor->contents()[0] = 3;
    DataPtr message{std::move(intTensor)};

    serializeMessage(stream, message);

    DataPtr receivedData = deserializeMessage(stream);
    IntTensor &receivedIntData =
            *dynamic_cast<IntTensor *>(receivedData.get());

    REQUIRE(receivedIntData.numSizes() == 1);
    REQUIRE(receivedIntData.sizes()[0] == 1);
    REQUIRE(receivedIntData.contents()[0] == 3);
}
