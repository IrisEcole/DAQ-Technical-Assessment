#pragma once
#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include <memory>
#include <dbcppp/Network.h>

struct CanFrame {
    double timestamp;
    std::string iface;
    uint32_t canId;
    std::vector<uint8_t> data;
};

std::vector<uint8_t> hexStringToBytes(const std::string& hex);
CanFrame parseCanLine(const std::string& line);

int runParser(std::istream& logFile, std::ostream& outFile,
              const std::vector<std::unique_ptr<dbcppp::INetwork>>& controlDBCs,
              const std::vector<std::unique_ptr<dbcppp::INetwork>>& sensorDBCs,
              const std::vector<std::unique_ptr<dbcppp::INetwork>>& tractiveDBCs);
