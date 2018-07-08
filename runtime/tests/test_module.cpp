#include <memory>
#include <custom_utility.hpp>

#include "catch.hpp"
#include "data.hpp"
#include "module.hpp"


// class LoggingDataHandler: public DataHandler {
// public:
//     std::stringbuf log;

//     void receiveData(DataBlock &&block) {
//         log += "Received data: \n" +
//             inputMsg_getIntTensor(&block)->contents[0];
//     }
// }

// TEST_CASE("Module correctly offloading functionality") {
//     ModuleHandlerPtr moduleHandler = make_unique<LoggingDataHandler>();
//     DataPtr intTensor = make_unique<IntTensor>(NumSizes{1}, 1);
//     static_cast<IntTensor *>(intTensor.get())->contents()[0] = 7;

//     SECTION("Single port module with no resource requirements") {
//         Module module{1, moduleHandler};
//         REQUIRE(module.isReady(0));
//         module.claim(0);
//         REQUIRE(module.isReady(0));
//         module.offerData(std::move(intTensor));
//     }
    

// }