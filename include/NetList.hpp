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
    NetList(const char *json, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *cloudKey, bool v);

    int ConvertJson(std::string jsonFile);

    void SetPortCipher(std::string portName, LweSample* valueArray);

    void SetPortPlain(std::string portName, int value);

    void SetROMCipher(int addr, LweSample* valueArray);

    void SetROMPlain(int addr, int value);

    void SetRAMCipher(int addr, LweSample* valueArray);

    void SetRAMPlain(int addr, uint8_t value);

    LweSample* GetPortCipher(std::string portName);

    int GetPortPlain(std::string portName);

    LweSample* GetRAMCipher(int addr);

    int GetRAMPlain(int addr);

    void DebugOutput();

    bool execute;

    std::unordered_map<int, Logic *> Logics;

private:
    bool cipher = false;
    bool verbose = false;
    const TFheGateBootstrappingCloudKeySet *key;
    tbb::concurrent_queue<Logic *> *executedQueue;
    std::map<std::string, std::unordered_map<int, LogicPortIn *>> Inputs;
    std::map<std::string, std::unordered_map<int, LogicPortOut *>> Outputs;
    std::unordered_map<int, std::unordered_map<int, LogicCellROM *>> Rom;
    std::map<int, std::unordered_map<int, LogicCellRAM *>> Ram;
};

#endif //IYOKAN_L2_NETLIST_HPP
