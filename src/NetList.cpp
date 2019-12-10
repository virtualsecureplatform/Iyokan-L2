#include <queue>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <string.h>
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_priority_queue.h"

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include "NetList.hpp"
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

NetList::NetList(
    std::string json,
    tbb::concurrent_queue<Logic *> *queue,
    const TFheGateBootstrappingCloudKeySet *cloudKey,
    bool v) {
    verbose = v;
    key = cloudKey;
    cipher = true;
    executedQueue = queue;
    ConvertJson(json);
}

NetList::NetList(
    std::string json,
    tbb::concurrent_queue<Logic *> *queue,
    bool v) {
    verbose = v;
    cipher = false;
    executedQueue = queue;
    ConvertJson(json);
}

void PrepareTFHE() {
    //generate a keyset
    const int minimum_lambda = 110;
    TFheGateBootstrappingParameterSet *params = new_default_gate_bootstrapping_parameters(minimum_lambda);

    //generate a random key
    uint32_t seed[] = {314, 1592, 657};
    tfhe_random_generator_setSeed(seed, 3);
    TFheGateBootstrappingSecretKeySet *key = new_random_gate_bootstrapping_secret_keyset(params);
}

int NetList::ConvertJson(std::string jsonFile) {
    std::ifstream ifs(jsonFile, std::ios::in);
    if (ifs.fail()) {
        std::cerr << "failed to read " << jsonFile << std::endl;
        return 1;
    }
    const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    picojson::value v;
    const std::string err = picojson::parse(v, json);
    if (err.empty() == false) {
        std::cerr << err << std::endl;
        return 2;
    }
    picojson::object &obj = v.get<picojson::object>();
    picojson::array &cells = obj["cells"].get<picojson::array>();
    picojson::array &ports = obj["ports"].get<picojson::array>();
    for (const auto &e : ports) {  // vectorをrange-based-forでまわしている。
        picojson::object port = e.get<picojson::object>();
        std::string type = port.at("type").get<std::string>();
        int id = static_cast<int>(port.at("id").get<double>());
        int priority = static_cast<int>(port.at("priority").get<double>());
        if (type == "input") {
            if (cipher) {
                Logics[id] = new LogicPortIn(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicPortIn(id, priority, executedQueue);
            }
        } else if (type == "output") {
            if (cipher) {
                Logics[id] = new LogicPortOut(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicPortOut(id, priority, executedQueue);
            }
        }
        Logics[id]->priority = priority;
    }
    for (const auto &e : cells) {  // vectorをrange-based-forでまわしている。
        picojson::object cell = e.get<picojson::object>();
        std::string type = cell.at("type").get<std::string>();
        int id = static_cast<int>(cell.at("id").get<double>());
        int priority = static_cast<int>(cell.at("priority").get<double>());
        if (type == "AND") {
            if (cipher) {
                Logics[id] = new LogicCellAND(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellAND(id, priority, executedQueue);
            }
        } else if (type == "NAND") {
            if (cipher) {
                Logics[id] = new LogicCellNAND(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellNAND(id, priority, executedQueue);
            }
        } else if (type == "ANDNOT") {
            if (cipher) {
                Logics[id] = new LogicCellANDNOT(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellANDNOT(id, priority, executedQueue);
            }
        } else if (type == "XOR") {
            if (cipher) {
                Logics[id] = new LogicCellXOR(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellXOR(id, priority, executedQueue);
            }
        } else if (type == "XNOR") {
            if (cipher) {
                Logics[id] = new LogicCellXNOR(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellXNOR(id, priority, executedQueue);
            }
        } else if (type == "DFFP") {
            if (cipher) {
                Logics[id] = new LogicCellDFFP(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellDFFP(id, priority, executedQueue);
            }
        } else if (type == "NOT") {
            if (cipher) {
                Logics[id] = new LogicCellNOT(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellNOT(id, priority, executedQueue);
            }
        } else if (type == "NOR") {
            if (cipher) {
                Logics[id] = new LogicCellNOR(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellNOR(id, priority, executedQueue);
            }
        } else if (type == "OR") {
            if (cipher) {
                Logics[id] = new LogicCellOR(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellOR(id, priority, executedQueue);
            }
        } else if (type == "ORNOT") {
            if (cipher) {
                Logics[id] = new LogicCellORNOT(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellORNOT(id, priority, executedQueue);
            }
        } else if (type == "MUX") {
            if (cipher) {
                Logics[id] = new LogicCellMUX(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellMUX(id, priority, executedQueue);
            }
        } else if (type == "ROM") {
            if (cipher) {
                Logics[id] = new LogicCellROM(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellROM(id, priority, executedQueue);
            }
        } else if (type == "RAM") {
            if (cipher) {
                Logics[id] = new LogicCellRAM(id, priority, executedQueue, key);
            } else {
                Logics[id] = new LogicCellRAM(id, priority, executedQueue);
            }
        } else {
            throw std::runtime_error("Not implemented:" + type);
        }
        Logics[id]->priority = priority;
    }
    for (const auto &e : ports) {  // vectorをrange-based-forでまわしている。
        picojson::object port = e.get<picojson::object>();
        std::string type = port.at("type").get<std::string>();
        std::string name = port.at("name").get<std::string>();
        int id = static_cast<int>(port.at("id").get<double>());
        picojson::array &bits = port.at("bits").get<picojson::array>();
        std::string portName = port.at("portName").get<std::string>();
        int portBit = static_cast<int>(port.at("portBit").get<double>());
        if (type == "input") {
            for (const auto &b : bits) {
                int logic = static_cast<int>(b.get<double>());
                Logics[id]->AddOutput(Logics[logic]);
            }
            Inputs[portName][portBit] = (LogicPortIn *)Logics[id];
        } else if (type == "output") {
            for (const auto &b : bits) {
                int logic = static_cast<int>(b.get<double>());
                Logics[id]->AddInput(Logics[logic]);
            }
            Outputs[portName][portBit] = (LogicPortOut *)Logics[id];
        }
    }
    if (verbose) {
        for (const auto p : Inputs) {
            std::cout << "Input port: " << p.first << std::endl;
        }
        for (const auto p : Outputs) {
            std::cout << "Output Port: " << p.first << std::endl;
        }
    }
    for (const auto &e : cells) {  // vectorをrange-based-forでまわしている。
        picojson::object cell = e.get<picojson::object>();
        std::string type = cell.at("type").get<std::string>();
        int id = static_cast<int>(cell.at("id").get<double>());
        picojson::object input = cell.at("input").get<picojson::object>();
        picojson::object output = cell.at("output").get<picojson::object>();
        if (type == "AND" || type == "NAND" || type == "XOR" || type == "XNOR" || type == "NOR" ||
            type == "ANDNOT" || type == "OR" || type == "ORNOT") {
            int A = static_cast<int>(input.at("A").get<double>());
            int B = static_cast<int>(input.at("B").get<double>());
            picojson::array &Y = output.at("Y").get<picojson::array>();
            Logics[id]->AddInput(Logics[A]);
            Logics[id]->AddInput(Logics[B]);
            for (const auto &y : Y) {
                int bitY = static_cast<int>(y.get<double>());
                Logics[id]->AddOutput(Logics[bitY]);
            }
        } else if (type == "DFFP") {
            int D = static_cast<int>(input.at("D").get<double>());
            picojson::array &Q = output.at("Q").get<picojson::array>();
            Logics[id]->AddInput(Logics[D]);
            for (const auto &q : Q) {
                int bitQ = static_cast<int>(q.get<double>());
                Logics[id]->AddOutput(Logics[bitQ]);
            }
        } else if (type == "NOT") {
            int A = static_cast<int>(input.at("A").get<double>());
            picojson::array &Y = output.at("Y").get<picojson::array>();
            Logics[id]->AddInput(Logics[A]);
            for (const auto &y : Y) {
                int bitY = static_cast<int>(y.get<double>());
                Logics[id]->AddOutput(Logics[bitY]);
            }
        } else if (type == "MUX") {
            int A = static_cast<int>(input.at("A").get<double>());
            int B = static_cast<int>(input.at("B").get<double>());
            int S = static_cast<int>(input.at("S").get<double>());
            picojson::array &Y = output.at("Y").get<picojson::array>();
            Logics[id]->AddInput(Logics[A]);
            Logics[id]->AddInput(Logics[B]);
            Logics[id]->AddInput(Logics[S]);
            for (const auto &y : Y) {
                int bitY = static_cast<int>(y.get<double>());
                Logics[id]->AddOutput(Logics[bitY]);
            }
        } else if (type == "ROM") {
            picojson::array &Q = output.at("Q").get<picojson::array>();
            int romAddress = static_cast<int>(cell.at("romAddress").get<double>());
            int romBit = static_cast<int>(cell.at("romBit").get<double>());
            for (const auto &q : Q) {
                int bitQ = static_cast<int>(q.get<double>());
                Logics[id]->AddOutput(Logics[bitQ]);
            }
            Rom[romAddress][romBit] = (LogicCellROM *)Logics[id];
        } else if (type == "RAM") {
            int ramAddress = static_cast<int>(cell.at("ramAddress").get<double>());
            int ramBit = static_cast<int>(cell.at("ramBit").get<double>());
            int D = static_cast<int>(input.at("D").get<double>());
            picojson::array &Q = output.at("Q").get<picojson::array>();
            Logics[id]->AddInput(Logics[D]);
            for (const auto &q : Q) {
                int bitQ = static_cast<int>(q.get<double>());
                Logics[id]->AddOutput(Logics[bitQ]);
            }
            Ram[ramAddress][ramBit] = (LogicCellRAM *)Logics[id];
        } else {
            throw std::runtime_error("Not executed");
        }
    }
    return 0;
}

void NetList::SetPortCipher(std::string portName, std::vector<std::shared_ptr<LweSample>> valueArray) {
    int length = Inputs[portName].size();
    if (length == 0) {
        throw std::runtime_error("Unknown input port:" + portName);
    }
    if (valueArray.size() != length) {
        throw std::runtime_error("Invalid value");
    }
    for (int i = 0; i < length; i++) {
        Inputs[portName][i]->SetCipher(valueArray.at(i).get());
    }
}

void NetList::SetPortPlain(std::string portName, int value) {
    int length = Inputs[portName].size();
    if (length == 0) {
        throw std::runtime_error("Unknown input port:" + portName);
    }
    for (int i = 0; i < length; i++) {
        Inputs[portName][i]->SetPlain(value & 0x1);
        value = value >> 1;
    }
}

void NetList::SetROMCipher(int byte_addr, std::vector<std::shared_ptr<LweSample>> valueArray) {
    int addr = byte_addr / 4;
    int word_num = byte_addr % 4;
    int length = Rom[addr].size();
    if (valueArray.size() != 8) {
        throw std::runtime_error("Invalid value");
    }
    if (length == 0) {
        throw std::runtime_error("Unknown Rom Address:" + addr);
    }
    for (int i = 0; i < length; i++) {
        Rom[addr][i + (word_num)*8]->SetCipher(valueArray.at(i));
    }
}

void NetList::SetROMCipherAll(std::vector<std::shared_ptr<LweSample>> valueArray) {
    for (int i = 0; i < valueArray.size(); i++) {
        Rom[i / 32][i % 32]->SetCipher(valueArray.at(i));
    }
}

void NetList::SetROMDecryptCipherAll(
    std::vector<std::shared_ptr<LweSample>> valueArray,
    std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey) {
    for (int i = 0; i < valueArray.size(); i++) {
        Rom[i / 32][i % 32]->SetPlain(bootsSymDecrypt(valueArray.at(i).get(), secretKey.get()) & 0x1);
    }
}

std::vector<std::shared_ptr<LweSample>> NetList::GetPortEncryptPlain(std::string portName, int width, std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey) {
    int portValue = GetPortPlain(portName);
    std::vector<std::shared_ptr<LweSample>> valueArray;
    for (int i = 0; i < width; i++) {
        std::shared_ptr<LweSample> value{
            new_gate_bootstrapping_ciphertext(secretKey.get()->params),
            delete_gate_bootstrapping_ciphertext};
        bootsSymEncrypt(value.get(), portValue & 0x1, secretKey.get());
        valueArray.push_back(value);
        portValue = portValue >> 1;
    }
    return valueArray;
}

void NetList::SetROMPlain(int addr, int value) {
    int length = Rom[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Rom Address:" + addr);
    }
    for (int i = 0; i < length; i++) {
        Rom[addr][i]->SetPlain(value & 0x1);
        value = value >> 1;
    }
}

void NetList::SetRAMCipher(int addr, std::vector<std::shared_ptr<LweSample>> valueArray) {
    if (valueArray.size() != 8) {
        throw std::runtime_error("Invalid value");
    }
    int length = Ram[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Ram Address:" + addr);
    }
    for (int i = 0; i < length; i++) {
        Ram[addr][i]->SetCipher(valueArray.at(i));
    }
}

void NetList::SetRAMPlain(int addr, uint8_t value) {
    int length = Ram[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Ram Address:" + addr);
    }
    for (int i = 0; i < length; i++) {
        Ram[addr][i]->SetPlain(value & 0x1);
        value = value >> 1;
    }
}

std::vector<std::shared_ptr<LweSample>> NetList::GetPortCipher(std::string portName) {
    int length = Outputs[portName].size();
    if (length == 0) {
        throw std::runtime_error("Unknown output port:" + portName);
    }
    std::vector<std::shared_ptr<LweSample>> valueArray;
    for (int i = 0; i < length; i++) {
        valueArray.push_back(std::shared_ptr<LweSample>(Outputs[portName][i]->GetCipher()));
    }
    return valueArray;
}

int NetList::GetPortPlain(std::string portName) {
    int length = Outputs[portName].size();
    if (length == 0) {
        throw std::runtime_error("Unknown output port:" + portName);
    }
    int value = 0;
    for (int i = length - 1; i > -1; i--) {
        value = value << 1;
        value += Outputs[portName][i]->GetPlain();
    }
    return value;
}

std::vector<std::shared_ptr<LweSample>> NetList::GetRAMCipher(int addr) {
    std::vector<std::shared_ptr<LweSample>> valueArray;
    for (int i = 0; i < 8; i++) {
        valueArray.push_back(std::shared_ptr<LweSample>(Ram[addr][i]->GetCipher()));
    }
    return valueArray;
}

std::vector<std::shared_ptr<LweSample>> NetList::GetRAMCipherAll() {
    std::vector<std::shared_ptr<LweSample>> valueArray;
    for (int i = 0; i < Ram.size()*8; i++) {
        valueArray.push_back(std::shared_ptr<LweSample>(Ram[i/8][i%8]->GetCipher()));
    }
    return valueArray;
}

int NetList::GetRAMPlain(int addr) {
    int length = Ram[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Ram Address:" + addr);
    }
    int value = 0;
    for (int i = length - 1; i > -1; i--) {
        value = value << 1;
        value += Ram[addr][i]->GetPlain();
    }
    return value;
}

void NetList::DebugOutput() {
    std::cout << "---Debug Output---" << std::endl;
    if (cipher) {
    } else {
        for (auto item : Outputs) {
            std::cout << item.first << " : " << GetPortPlain(item.first) << std::endl;
        }
    }
}

void NetList::EnableReset() {
    if(cipher){
        std::shared_ptr<LweSample> value{
            new_gate_bootstrapping_ciphertext(key->params),
            delete_gate_bootstrapping_ciphertext};
        bootsCONSTANT(value.get(), 1, key);
        std::vector<std::shared_ptr<LweSample>> valueArray;
        valueArray.push_back(value);
        SetPortCipher("reset", valueArray);
    }else{
        SetPortPlain("reset", 1);
    }
}

void NetList::DisableReset() {
    if(cipher){
        std::shared_ptr<LweSample> value{
            new_gate_bootstrapping_ciphertext(key->params),
            delete_gate_bootstrapping_ciphertext};
        bootsCONSTANT(value.get(), 0, key);
        std::vector<std::shared_ptr<LweSample>> valueArray;
        valueArray.push_back(value);
    }else{
        SetPortPlain("reset", 0);
    }
}
/*
void NetList::BuggyKey(){
    key->lwe_key->key[0] = (~key->lwe_key->key[0])&0x1;
}
 */
