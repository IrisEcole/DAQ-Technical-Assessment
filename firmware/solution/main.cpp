#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>
#include <dbcppp/Network.h> 
#include "parser.cpp"

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
    runParser(logFile, outFile, controlDBCs, sensorDBCs, tractiveDBCs);

    std::cout << "Parsing complete. Output written to output.txt\n";
    return 0;
//     std::string line;
//    while (std::getline(logFile, line)) {
//     if (line.empty()) continue;

//     // Use your helper instead of duplicating parsing logic
//     CanFrame frame;
//     try {
//         frame = parseCanLine(line);
//     } catch (const std::exception& e) {
//         std::cerr << "Failed to parse line: " << e.what() << "\n";
//         continue; // skip invalid lines safely
//     }

//     // Select correct DBCs based on interface
//     std::vector<dbcppp::INetwork*> dbcs;
//     if (frame.iface == "can0")
//         for (auto& dbc : controlDBCs) dbcs.push_back(dbc.get());
//     else if (frame.iface == "can1")
//         for (auto& dbc : sensorDBCs) dbcs.push_back(dbc.get());
//     else if (frame.iface == "can2")
//         for (auto& dbc : tractiveDBCs) dbcs.push_back(dbc.get());

//     // Process the message using the DBCs
//     for (auto dbc : dbcs) {
//         if (!dbc) continue;
//         for (const auto& msg : dbc->Messages()) {
//             if (msg.Id() != frame.canId) continue;

//             // Loop through all signals in the message
//             for (const auto& sig : msg.Signals()) {
//                 std::ios oldState(nullptr);
//                 oldState.copyfmt(outFile);

//                 outFile << std::fixed << std::setprecision(6)
//                         << "(" << frame.timestamp << "): "
//                         << sig.Name() << ": ";

//                 outFile.copyfmt(oldState);

//                 if (!frame.data.empty()) {
//                     double value = sig.RawToPhys(sig.Decode(frame.data.data()));
//                     if (value == static_cast<int64_t>(value)) {
//                         outFile << static_cast<int64_t>(value) << '\n';
//                     } else {
//                         std::ostringstream oss;
//                         oss << std::fixed << std::setprecision(6) << value;
//                         std::string s = oss.str();
//                         s.erase(s.find_last_not_of('0') + 1, std::string::npos);
//                         if (s.back() == '.') s.pop_back();
//                         outFile << s << '\n';
//                     }
//                 } else {
//                     outFile << "<no data>\n";
//                 }
//             }
//         }
//     }
// }


//     logFile.close();
//     outFile.close();
    std::cout << "Parsing complete. Output written to output.txt\n";

    return 0;
}
