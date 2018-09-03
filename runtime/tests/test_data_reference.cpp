#include <utility>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "data.hpp"
#include "data_reference.hpp"

static void fill(DataReference &dataReference, int value) {
    DataSharedPtr data;

    if (dataReference.canDirectWriteAcquire()) {
        data = dataReference.directWriteAcquire();
    } else {
        data = dataReference.setWriteAcquire(
            make_unique<IntTensor>(NumSizes{1}, 1));
    }

    static_cast<IntTensor *>(data.get())->contents()[0] = value;

    dataReference.writeFinalize();
}

static int getInt(DataSharedPtr const &data) {
    return static_cast<IntTensor *>(data.get())->contents()[0];
}

TEST_CASE("Data reference double buffering behavior") {
    DataReference dataReference{};

    SECTION("Data reference single reader and writer") {
        REQUIRE(!dataReference.readAcquire());

        fill(dataReference, 3);
        REQUIRE(getInt(dataReference.readAcquire()) == 3);

        fill(dataReference, 4);
        REQUIRE(getInt(dataReference.readAcquire()) == 4);

        fill(dataReference, 5);
        REQUIRE(getInt(dataReference.readAcquire()) == 5);
    }

    SECTION("Data reference multiple readers") {
        fill(dataReference, 3);

        DataSharedPtr readData0 = dataReference.readAcquire();
        DataSharedPtr readData1 = dataReference.readAcquire();

        REQUIRE(getInt(readData0) == 3);
        REQUIRE(getInt(readData1) == 3);
    }
}