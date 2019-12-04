//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_NETLIST_HPP
#define IYOKAN_L2_NETLIST_HPP

#include <queue>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_priority_queue.h"

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include "Logic.hpp"
#include "LogicPortIn.hpp"
#include "LogicPortOut.hpp"
#include "LogicCellAND.hpp"
#include "LogicCellNAND.hpp"
#include "LogicCellXOR.hpp"
#include "LogicCellXNOR.hpp"
#include "LogicCellDFFP.hpp"
#include "LogicCellNOT.hpp"
#include "LogicCellOR.hpp"
#include "LogicCellORNOT.hpp"
#include "LogicCellNOR.hpp"
#include "LogicCellANDNOT.hpp"
#include "LogicCellMUX.hpp"
#include "LogicCellROM.hpp"
#include "LogicCellRAM.hpp"

#include "../picojson/picojson.h"

class NetList {
public:
    NetList(const char *json, bool v);

    void PrepareTFHE();

    int ConvertJson();

    void Set(std::string portName, int value);

    void SetROM(int addr, uint32_t value);

    void SetRAM(int addr, int value);

    int Get(std::string portName);

    int GetRAM(int addr);

    void DebugOutput();

    void DumpRAM();

    void DumpRAMtoFile(std::string path, int cycle);

    void BuggyKey();

    bool execute;
    TFheGateBootstrappingSecretKeySet *key;
    std::unordered_map<int, Logic *> Logics;
private:
    bool verbose = false;
    TFheGateBootstrappingParameterSet *params;
    std::map<std::string, std::unordered_map<int, LogicPortIn *>> Inputs;
    std::map<std::string, std::unordered_map<int, LogicPortOut *>> Outputs;
    std::unordered_map<int, std::unordered_map<int, LogicCellROM *>> Rom;
    std::map<int, std::unordered_map<int, LogicCellRAM *>> Ram;
    std::string JsonFile;
};

#endif //IYOKAN_L2_NETLIST_HPP
