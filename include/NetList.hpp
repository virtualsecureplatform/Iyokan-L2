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
    NetList(std::string json, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *cloudKey,
            bool v);

    NetList(std::string json, tbb::concurrent_queue<Logic *> *queue, bool v);

    int ConvertJson(std::string jsonFile);

    void SetPortCipher(std::string portName, std::vector<std::shared_ptr<LweSample>> valueArray);

    void SetROMCipher(int addr, std::vector<std::shared_ptr<LweSample>> valueArray);

    void SetROMCipherAll(std::vector<std::shared_ptr<LweSample>> valueArray);

    void SetROMDecryptCipherAll(std::vector<std::shared_ptr<LweSample>> valueArray, std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey);

    void SetRAMCipher(int addr, std::vector<std::shared_ptr<LweSample>> valueArray);

    std::vector<std::shared_ptr<LweSample>> GetPortCipher(std::string portName);

    std::vector<std::shared_ptr<LweSample>> GetPortEncryptPlain(std::string portName, int width, std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey);

    std::vector<std::shared_ptr<LweSample>> GetRAMCipher(int addr);

    std::vector<std::shared_ptr<LweSample>> GetRAMCipherAll();

    void SetPortPlain(std::string portName, int value);

    void SetROMPlain(int addr, int value);

    void SetRAMPlain(int addr, uint8_t value);

    int GetPortPlain(std::string portName);

    int GetRAMPlain(int addr);

    void DebugOutput();

    void EnableReset();

    void DisableReset();

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

#endif  //IYOKAN_L2_NETLIST_HPP
