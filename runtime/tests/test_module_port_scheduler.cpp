#include "catch.hpp"
#include "module_port_scheduler.hpp"

TEST_CASE("Basic properties of the scheduler") {
    ModulePortScheduler scheduler{3};

    SECTION("Initial setup") {
        REQUIRE(scheduler.numDataReady() == 0);
        REQUIRE(scheduler.numModulePortsReady() == 3);
        REQUIRE(scheduler.numModulePortsPending() == 0);
    }

    SECTION("Values update based on data and module readiness") {
        scheduler.setDataReady(0, true);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 3);
        REQUIRE(scheduler.numModulePortsPending() == 1);

        scheduler.setModulePortReady(1, false);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 2);
        REQUIRE(scheduler.numModulePortsPending() == 1);

        scheduler.setModulePortReady(0, false);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 1);
        REQUIRE(scheduler.numModulePortsPending() == 0);

        scheduler.setModulePortReady(1, true);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 2);
        REQUIRE(scheduler.numModulePortsPending() == 0);

        scheduler.setModulePortReady(0, true);
        REQUIRE(scheduler.numDataReady() == 1);
        REQUIRE(scheduler.numModulePortsReady() == 3);
        REQUIRE(scheduler.numModulePortsPending() == 1);

        scheduler.setDataReady(0, false);
        REQUIRE(scheduler.numDataReady() == 0);
        REQUIRE(scheduler.numModulePortsReady() == 3);
        REQUIRE(scheduler.numModulePortsPending() == 0);
    }

    SECTION("Next module identified correctly") {
        scheduler.setDataReady(0, true);
        REQUIRE(scheduler.nextModulePort() == 0);

        scheduler.setDataReady(0, false);
        scheduler.setDataReady(1, true);
        REQUIRE(scheduler.nextModulePort() == 1);
    }

    SECTION("Next module accessed in round robin fashion") {
        scheduler.setDataReady(0, true);
        scheduler.setDataReady(1, true);
        REQUIRE(scheduler.nextModulePort() == 0);
        REQUIRE(scheduler.nextModulePort() == 1);
        REQUIRE(scheduler.nextModulePort() == 0);
        REQUIRE(scheduler.nextModulePort() == 1);

        scheduler.setModulePortReady(0, false);
        REQUIRE(scheduler.nextModulePort() == 1);
        REQUIRE(scheduler.nextModulePort() == 1);
    }
}