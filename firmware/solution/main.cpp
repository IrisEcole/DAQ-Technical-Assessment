#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>
#include <dbcppp/Network.h> 


// Helper: Convert hex string to bytes
std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i + 1 < hex.length(); i += 2) {
        bytes.push_back(static_cast<uint8_t>(std::stoul(hex.substr(i, 2), nullptr, 16)));
    }
    return bytes;
}

int main() {
    std::vector<std::unique_ptr<dbcppp::INetwork>> controlDBCs;
    std::vector<std::unique_ptr<dbcppp::INetwork>> sensorDBCs;
    std::vector<std::unique_ptr<dbcppp::INetwork>> tractiveDBCs;

    //Load buses from bus files, uses relative path 
    {
        std::ifstream f("../dbc-files/ControlBus.dbc");
        if (!f) { std::cerr << "Failed to open ControlBus.dbc\n"; return 1; }
        controlDBCs.push_back(dbcppp::INetwork::LoadDBCFromIs(f));
    }

    {
        std::ifstream f("../dbc-files/SensorBus.dbc");
        if (!f) { std::cerr << "Failed to open SensorBus.dbc\n"; return 1; }
        sensorDBCs.push_back(dbcppp::INetwork::LoadDBCFromIs(f));
    }
    {
        std::ifstream f("../dbc-files/TractiveBus.dbc");
        if (!f) { std::cerr << "Failed to open TractiveBus.dbc\n"; return 1; }
        tractiveDBCs.push_back(dbcppp::INetwork::LoadDBCFromIs(f));
    }

    //Load dump log from relative path, opens output text right there
    std::ifstream logFile("../dump.log");
    std::ofstream outFile("output.txt");
    if (!logFile.is_open()) { std::cerr << "Failed to open dump.log\n"; return 1; }

    std::string line;
    while (std::getline(logFile, line)) {
        if (line.empty()) continue;

        // Format in log: (timestamp) interface CANID#data

        // Extract timestamp
        size_t tsStart = line.find('('), tsEnd = line.find(')');
        if (tsStart == std::string::npos || tsEnd == std::string::npos) continue;
        double timestamp = std::stod(line.substr(tsStart + 1, tsEnd - tsStart - 1));

        // Extract interface name 
        size_t ifaceStart = tsEnd + 2, ifaceEnd = line.find(' ', ifaceStart);
        std::string iface = line.substr(ifaceStart, ifaceEnd - ifaceStart);

        // Extract CAN ID and data payload
        size_t hashPos = line.find('#', ifaceEnd);
        std::string canIdStr = line.substr(ifaceEnd + 1, hashPos - ifaceEnd - 1);
        std::string dataStr = (hashPos + 1 < line.size()) ? line.substr(hashPos + 1) : "";
        uint32_t canId = std::stoul(canIdStr, nullptr, 16);
        std::vector<uint8_t> data = hexStringToBytes(dataStr);


        //Gets the DBC that have macthing
        std::vector<dbcppp::INetwork*> dbcs;
        if (iface == "can0") for (auto& dbc : controlDBCs) dbcs.push_back(dbc.get());
        else if (iface == "can1") for (auto& dbc : sensorDBCs) dbcs.push_back(dbc.get());
        else if (iface == "can2") for (auto& dbc : tractiveDBCs) dbcs.push_back(dbc.get());


        for (auto dbc : dbcs) {
            if (!dbc) continue;
            //dbc->Messages() returns all the messages defined for that dbc
            for (const auto& msg : dbc->Messages()) {
                //If the can id for that message does not correspond to log go to the next msg
                if (msg.Id() != canId) continue;

                // Loop through all signals in the message
                for (const auto& sig : msg.Signals()) {
                    // Save current stream state
                    std::ios oldState(nullptr);
                    oldState.copyfmt(outFile);

                    // Print timestamp in fixed-point format with 6 decimals
                    outFile << std::fixed << std::setprecision(6)
                            << "(" << timestamp << "): " << sig.Name() << ": ";

                    // Restore previous state for signal value formatting
                    outFile.copyfmt(oldState);

                    // Decode signal value if data is available
                    if (!data.empty()) {
                        //Turns the data from log 
                        double value = sig.RawToPhys(sig.Decode(data.data()));

                        // Print integer if exact, otherwise trim trailing zeros for float
                        if (value == static_cast<int64_t>(value)) {
                            outFile << static_cast<int64_t>(value) << std::endl;
                        } else {
                            std::ostringstream oss;
                            oss << std::fixed << std::setprecision(6) << value;
                            std::string s = oss.str();
                            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
                            if (s.back() == '.') s.pop_back();
                            outFile << s << std::endl;
                        }
                    } else {
                        //Else no data
                        outFile << "<no data>" << std::endl;
                    }
                } 
            } 
        } 
    } 

    logFile.close();
    outFile.close();
    std::cout << "Parsing complete. Output written to output.txt\n";

    return 0;
}
