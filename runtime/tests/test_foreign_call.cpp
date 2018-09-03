#include <string>
#include <utility>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "catch_test_util.hpp"
#include "data_interface.hpp"
#include "native_loader.hpp"

TEST_CASE("Call foreign addOne function") {
    DataBlock block{1, 0};
    DataBlock *blockPtr = &block;
    int origValue = 1337;

    {
        std::unique_ptr<IntTensor> intTensor;
        intTensor = make_unique<IntTensor>((NumSizes){1}, 1);
        intTensor->contents()[0] = origValue;
        block.setInputMsg(std::move(intTensor));
    }

    NativeLoader loader{};
    std::string libDir = buildDir + "lib/";
    ModuleFunction addOne = loader.load(
        libDir, "add_one", "addOne");
    addOne(blockPtr);

    {
        IntTensor *intTensor = (IntTensor *)(
            block.takeOutputMsg(0).get());
        REQUIRE(intTensor->contents()[0] == origValue + 1);
    }
}