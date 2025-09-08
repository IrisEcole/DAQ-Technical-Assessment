

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>
#include <dbcppp/Network.h> 
#include "parser.hpp"

// Helper: Convert hex string to bytes
std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i + 1 < hex.length(); i += 2) {
        bytes.push_back(static_cast<uint8_t>(std::stoul(hex.substr(i, 2), nullptr, 16)));
    }
    return bytes;
}



CanFrame parseCanLine(const std::string& line)
{
    CanFrame frame{};
    size_t tsStart = line.find('('), tsEnd = line.find(')');
    frame.timestamp = std::stod(line.substr(tsStart + 1, tsEnd - tsStart - 1));

    size_t ifaceStart = tsEnd + 2;
    size_t ifaceEnd = line.find(' ', ifaceStart);
    frame.iface = line.substr(ifaceStart, ifaceEnd - ifaceStart);

    size_t hashPos = line.find('#', ifaceEnd);
    std::string canIdStr = line.substr(ifaceEnd + 1, hashPos - ifaceEnd - 1);
    frame.canId = std::stoul(canIdStr, nullptr, 16);

    std::string dataStr = (hashPos + 1 < line.size()) ? line.substr(hashPos + 1) : "";
    frame.data = hexStringToBytes(dataStr);

    return frame;
}


int runParser(std::istream& logFile, std::ostream& outFile,
              const std::vector<std::unique_ptr<dbcppp::INetwork>>& controlDBCs,
              const std::vector<std::unique_ptr<dbcppp::INetwork>>& sensorDBCs,
              const std::vector<std::unique_ptr<dbcppp::INetwork>>& tractiveDBCs) 
{
    std::string line;
    while (std::getline(logFile, line)) {
        if (line.empty()) continue;

        CanFrame frame;
        try {
            frame = parseCanLine(line);
        } catch (...) {
            continue; // skip invalid lines
        }

        std::vector<dbcppp::INetwork*> dbcs;
        if (frame.iface == "can0")
            for (auto& dbc : controlDBCs) dbcs.push_back(dbc.get());
        else if (frame.iface == "can1")
            for (auto& dbc : sensorDBCs) dbcs.push_back(dbc.get());
        else if (frame.iface == "can2")
            for (auto& dbc : tractiveDBCs) dbcs.push_back(dbc.get());

        for (auto dbc : dbcs) {
            if (!dbc) continue;
            for (const auto& msg : dbc->Messages()) {
                if (msg.Id() != frame.canId) continue;

                for (const auto& sig : msg.Signals()) {
                    std::ios oldState(nullptr);
                    oldState.copyfmt(outFile);

                    outFile << std::fixed << std::setprecision(6)
                            << "(" << frame.timestamp << "): "
                            << sig.Name() << ": ";

                    outFile.copyfmt(oldState);

                    if (!frame.data.empty()) {
                        double value = sig.RawToPhys(sig.Decode(frame.data.data()));
                        if (value == static_cast<int64_t>(value)) {
                            outFile << static_cast<int64_t>(value) << '\n';
                        } else {
                            std::ostringstream oss;
                            oss << std::fixed << std::setprecision(6) << value;
                            std::string s = oss.str();
                            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
                            if (s.back() == '.') s.pop_back();
                            outFile << s << '\n';
                        }
                    } else {
                        outFile << "<no data>\n";
                    }
                }
            }
        }
    }

    return 0;
}
