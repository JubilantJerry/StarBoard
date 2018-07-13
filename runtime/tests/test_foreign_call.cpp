#include <iostream>
#include <utility>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "data_interface.hpp"
#include "native_loader.hpp"

#define LIB_DIR "build/lib/"

TEST_CASE("Call foreign addOne function") {
    DataBlock block{1};
    DataBlock *blockPtr = &block;
    int origValue = 1337;

    {
        std::unique_ptr<IntTensor> intTensor;
        intTensor = make_unique<IntTensor>((NumSizes){1}, 1);
        intTensor->contents()[0] = origValue;
        block.setInputMsg(std::move(intTensor));
    }

    NativeLoader loader{};
    ModuleFunction function = loader.load(
        LIB_DIR, "add_one", "addOne");
    function(blockPtr);

    {
        IntTensor *intTensor = (IntTensor *)(
            block.takeOutputMsg(0).get());
        REQUIRE(intTensor->contents()[0] == origValue + 1);
    }
}