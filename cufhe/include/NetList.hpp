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
    NetList(std::string json, bool v, bool isCipher);

    int ConvertJson(std::string jsonFile);

    void SetPortCipher(std::string portName, std::vector<cufhe::Ctxt *> valueArray);

    void SetROMCipher(int addr, std::vector<std::shared_ptr<cufhe::Ctxt>>& valueArray);

    void SetROMCipherAll(std::vector<std::shared_ptr<cufhe::Ctxt>>& valueArray);

    //void SetROMDecryptCipherAll(std::vector<std::shared_ptr<LweSample>> valueArray, std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey);

    void SetRAMCipher(int addr, std::vector<std::shared_ptr<cufhe::Ctxt>>& valueArray);

    void SetRAMCipherAll(std::vector<std::shared_ptr<cufhe::Ctxt>>& valueArray);

    //void SetRAMDecryptCipherAll(std::vector<std::shared_ptr<LweSample>> valueArray, std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey);

    std::vector<std::shared_ptr<cufhe::Ctxt>> GetPortCipher(std::string portName);

    //std::vector<std::shared_ptr<LweSample>> GetPortEncryptPlain(std::string portName, int width, std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey);

    std::vector<std::shared_ptr<cufhe::Ctxt>> GetRAMCipher(int addr);

    std::vector<std::shared_ptr<cufhe::Ctxt>> GetRAMCipherAll();

    //std::vector<std::shared_ptr<LweSample>> GetRAMEncryptPlainAll(std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey);

    void SetPortPlain(std::string portName, int value);

    void SetROMPlain(int addr, uint32_t value);

    void SetROMEncryptPlain(int addr, uint32_t value, cufhe::PriKey *secretKey);

    void SetROMPlainAll(std::vector<uint8_t>& valueArray);

    void SetRAMPlain(int addr, uint8_t value);

    void SetRAMEncryptPlain(int addr, uint8_t value, cufhe::PriKey *secretKey);

    void SetRAMPlainAll(std::vector<uint8_t>& valueArray);

    int GetPortPlain(std::string portName);

    int GetPortDecryptCipher(std::string portName, cufhe::PriKey *secretKey);

    uint32_t GetROMDecryptCipher(int addr, cufhe::PriKey *secretKey);

    int GetRAMPlain(int addr);

    uint8_t GetRAMDecryptCipher(int addr, cufhe::PriKey *secretKey);

    std::vector<uint8_t> GetRAMPlainAll();

    void DebugOutput();

    //void DebugRegisterWrite();

    void EnableReset();

    void DisableReset();

    bool execute;

    std::unordered_map<int, Logic *> Logics;

private:
    bool cipher = false;
    bool verbose = false;
    std::map<std::string, std::unordered_map<int, LogicPortIn *>> Inputs;
    std::map<std::string, std::unordered_map<int, LogicPortOut *>> Outputs;
    std::unordered_map<int, std::unordered_map<int, LogicCellROM *>> Rom;
    std::map<int, std::unordered_map<int, LogicCellRAM *>> Ram;
};

#endif  //IYOKAN_L2_NETLIST_HPP
