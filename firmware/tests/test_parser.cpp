#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../solution/parser.hpp"  
#include <sstream>
#include <dbcppp/Network.h> 
#include <fstream>     

using namespace Catch;


TEST_CASE("hexStringToBytes converts hex correctly", "[hex]") {
    std::string hex = "B1B8E368";
    std::vector<uint8_t> expected = {0xB1, 0xB8, 0xE3, 0x68};
    REQUIRE(hexStringToBytes(hex) == expected);
}

TEST_CASE("parseCanLine parses valid CAN line", "[parse]") {
    std::string line = "(1730892639.317234) can1 705#B1B8E3680F488B72";

    CanFrame frame = parseCanLine(line);

    REQUIRE(frame.timestamp == Approx(1730892639.317234));
    REQUIRE(frame.iface == "can1");
    REQUIRE(frame.canId == 0x705);
    std::vector<uint8_t> expectedData = {0xB1,0xB8,0xE3,0x68,0x0F,0x48,0x8B,0x72};
    REQUIRE(frame.data == expectedData);
}

TEST_CASE("parseCanLine handles empty data", "[parse]") {
    std::string line = "(1730892639.317234) can0 100#";

    CanFrame frame = parseCanLine(line);

    REQUIRE(frame.timestamp == Approx(1730892639.317234));
    REQUIRE(frame.iface == "can0");
    REQUIRE(frame.canId == 0x100);
    REQUIRE(frame.data.empty());
}

TEST_CASE("parseCanLine throws on invalid line", "[parse]") {
    std::string line = "invalid line without proper format";

    REQUIRE_THROWS_AS(parseCanLine(line), std::exception);
}
//tests
//Proper extraction of sensor values
//Endianness, bit-length, and scaling calculations
//Handling multiple DBC files defining the same CAN ID

TEST_CASE("runParser works with in-memory streams", "[runParser]") {
    std::vector<std::unique_ptr<dbcppp::INetwork>> controlDBCs;
    std::vector<std::unique_ptr<dbcppp::INetwork>> sensorDBCs;
    std::vector<std::unique_ptr<dbcppp::INetwork>> tractiveDBCs;

    {
        std::ifstream f("../dbc-files/ControlBus.dbc");
        if (!f) { FAIL("Failed to open ControlBus.dbc"); }
        controlDBCs.push_back(dbcppp::INetwork::LoadDBCFromIs(f));
    }

    {
        std::ifstream f("../dbc-files/SensorBus.dbc");
        if (!f) { FAIL("Failed to open SensorBus.dbc"); }
        sensorDBCs.push_back(dbcppp::INetwork::LoadDBCFromIs(f));
    }

    {
        std::ifstream f("../dbc-files/TractiveBus.dbc");
        if (!f) { FAIL("Failed to open TractiveBus.dbc"); }
        tractiveDBCs.push_back(dbcppp::INetwork::LoadDBCFromIs(f));
    }

    std::istringstream logStream(
        "(1730892639.316946) can1 709#FF7F0080A3BC\n"
        "(1730892639.317588) can2 6B0#000012F51D784900\n"
    );
    std::ostringstream outStream;
    REQUIRE(runParser(logStream, outStream,controlDBCs, sensorDBCs, tractiveDBCs) == 0);

    std::string out = outStream.str();
    REQUIRE(!out.empty());
    REQUIRE(out.find("(1730892639.316946): CoolantPressureFanOUT: -1724.5") != std::string::npos);
    REQUIRE(out.find("(1730892639.316946): CoolantPressureFanIN: -3276.8") != std::string::npos);
    REQUIRE(out.find("(1730892639.316946): CoolantInverterPressure: 3276.7") != std::string::npos);

    REQUIRE(out.find("(1730892639.317588): Pack_Current: 0") != std::string::npos);
    REQUIRE(out.find("(1730892639.317588): Pack_Inst_Voltage: 485.3") != std::string::npos);
    REQUIRE(out.find("(1730892639.317588): Pack_SOC: 14.5") != std::string::npos);
    REQUIRE(out.find("(1730892639.317588): Relay_State: 30793") != std::string::npos);

}

