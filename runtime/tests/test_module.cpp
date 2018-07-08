#include <memory>
#include "custom_utility.hpp"

#include "catch.hpp"
#include "data.hpp"
#include "module.hpp"

TEST_CASE("Message queues") {
    MessageQueue queue{2};
    DataPtr intTensors[3];

    for(int i = 0; i < 3; i++) {
        std::unique_ptr<IntTensor> result =
            make_unique<IntTensor>(NumSizes{1}, 1);
        result->contents()[0] = i;
        intTensors[i] = std::move(result);
    }

    SECTION("Not reaching capacity") {
        REQUIRE(queue.size() == 0);
        queue.enqueue(std::move(intTensors[0]));
        queue.enqueue(std::move(intTensors[1]));
        REQUIRE(queue.size() == 2);
        intTensors[0] = queue.dequeue();
        REQUIRE(queue.size() == 1);
        REQUIRE(static_cast<IntTensor *>(
            intTensors[0].get())->contents()[0] == 0);
    }

    SECTION("Reaching capacity") {
        queue.enqueue(std::move(intTensors[0]));
        queue.enqueue(std::move(intTensors[1]));
        queue.enqueue(std::move(intTensors[2]));
        REQUIRE(queue.size() == 2);
        intTensors[1] = queue.dequeue();
        REQUIRE(static_cast<IntTensor *>(
            intTensors[1].get())->contents()[0] == 1);
    }
}