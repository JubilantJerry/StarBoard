#include <sstream>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "data_serializer.hpp"

TEST_CASE("Serialization of data") {
    std::stringstream stream{};
    DataSerializer<std::stringstream> serializer{stream};

    std::unique_ptr<IntTensor> intTensor =
        make_unique<IntTensor>((NumSizes){2}, 1, 2);
    intTensor->contents()[0] = 3;
    intTensor->contents()[1] = 4;

    std::unique_ptr<FloatTensor> floatTensor =
        make_unique<FloatTensor>((NumSizes){1}, 1);
    floatTensor->contents()[0] = 0.9;

    SECTION("Integer tensor serialization") {
        serializer & *intTensor;

        DataPtr receivedData = deserializeData(stream);
        IntTensor &receivedIntData =
            *dynamic_cast<IntTensor *>(receivedData.get());

        REQUIRE(receivedIntData.numSizes() == 2);
        REQUIRE(receivedIntData.sizes()[0] == 1);
        REQUIRE(receivedIntData.sizes()[1] == 2);
        REQUIRE(receivedIntData.contents()[0] == 3);
        REQUIRE(receivedIntData.contents()[1] == 4);
    }

    SECTION("Float tensor serialization") {
        serializer & *floatTensor;

        DataPtr receivedData = deserializeData(stream);
        FloatTensor &receivedFloatData =
            *dynamic_cast<FloatTensor *>(receivedData.get());

        REQUIRE(receivedFloatData.contents()[0] == 0.9f);
    }


    SECTION("Branch serialization") {
        Branch branch{2};
        branch.setValue(0, std::move(intTensor));
        branch.setValue(1, std::move(floatTensor));

        serializer & branch;

        DataPtr receivedData = deserializeData(stream);
        Branch &receivedBranchData =
            *dynamic_cast<Branch *>(receivedData.get());

        REQUIRE(receivedBranchData.size() == 2);
        
        IntTensor &receivedIntData =
            *dynamic_cast<IntTensor *>(receivedBranchData.getValue(0).get());

        REQUIRE(receivedIntData.numSizes() == 2);
        REQUIRE(receivedIntData.sizes()[0] == 1);
        REQUIRE(receivedIntData.sizes()[1] == 2);
        
        FloatTensor &receivedFloatData =
            *dynamic_cast<FloatTensor *>(receivedBranchData.getValue(1).get());

        REQUIRE(receivedFloatData.contents()[0] == 0.9f);
    }
}

TEST_CASE("Serialization of messages") {
    std::stringstream stream{};

    std::unique_ptr<IntTensor> intTensor =
        make_unique<IntTensor>((NumSizes){1}, 1);
    intTensor->contents()[0] = 3;
    DataPtr message{std::move(intTensor)};

    serializeMessage(stream, message);

    DataPtr receivedData = deserializeMessage(stream);
    IntTensor &receivedIntData =
            *dynamic_cast<IntTensor *>(receivedData.get());

    REQUIRE(receivedIntData.contents()[0] == 3);
}