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
    const char *json,
    tbb::concurrent_queue<Logic *> *queue,
    const TFheGateBootstrappingCloudKeySet *cloudKey,
    bool v) {
    verbose = v;
    key = cloudKey;
    cipher = true;
    executedQueue = queue;
    ConvertJson(std::string(json));
}

NetList::NetList(
    const char *json,
    tbb::concurrent_queue<Logic *> *queue,
    bool v) {
    verbose = v;
    cipher = false;
    executedQueue = queue;
    ConvertJson(std::string(json));
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

void NetList::SetPortCipher(std::string portName, LweSample *valueArray) {
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

void NetList::SetROMCipher(int addr, LweSample *valueArray) {
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

void NetList::SetRAMCipher(int addr, LweSample *valueArray) {
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

LweSample *NetList::GetPortCipher(std::string portName) {
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

LweSample *NetList::GetRAMCipher(int addr) {
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
/*
void NetList::Set(std::string portName, int value) {
    int length = Inputs[portName].size();
    if (length == 0) {
        throw std::runtime_error("Unknown input port:" + portName);
    }
    for (int i = 0; i < length; i++) {
        Inputs[portName][i]->Set(value & 0x1, key);
        value = value >> 1;
    }
}

void NetList::SetROM(int addr, uint32_t value) {
    int length = Rom[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Rom Address:" + addr);
    }
    for (int i = 0; i < length; i++) {
        Rom[addr][i]->Set(value & 0x1, &key->cloud);
        value = value >> 1;
    }
}

void NetList::SetRAM(int addr, int value) {
    int length = Ram[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Ram Address:" + addr);
    }
    for (int i = 0; i < length; i++) {
        Ram[addr][i]->Set(value & 0x1, &key->cloud);
        value = value >> 1;
    }
}

int NetList::Get(std::string portName) {
    int length = Outputs[portName].size();
    if (length == 0) {
        throw std::runtime_error("Unknown output port:" + portName);
    }
    int value = 0;
    for (int i = length - 1; i > -1; i--) {
        value = value << 1;
#ifdef FHE
        value += Outputs[portName][i]->Get(key);
#else
        value += Outputs[portName][i]->Get();
#endif
    }
    return value;
}

int NetList::GetRAM(int addr) {
    int length = Ram[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Ram Address:" + addr);
    }
    int value = 0;
    for (int i = length - 1; i > -1; i--) {
        value = value << 1;
#ifdef FHE
        value += Ram[addr][i]->Get(key);
#else
        value += Ram[addr][i]->Get();
#endif
    }
    return value;
}

void NetList::DebugOutput() {
    std::cout << "---Debug Output---" << std::endl;
    for (auto item : Outputs) {
        std::cout << item.first << " : " << Get(item.first) << std::endl;
    }
}

void NetList::DumpRAM() {
    for (auto item : Ram) {
        if(item.first > 15 && item.first < 42){
            std::printf("RAM[%d] 0x%02X\n", item.first, GetRAM(item.first));
        }
    }
}

void NetList::DumpRAMtoFile(std::string path, int cycle) {
    std::string filePath = path + std::to_string(cycle) + ".dump";
    FILE* pFile;
    pFile = fopen(filePath.c_str(), "w");
    for(auto item : Ram){
        if(item.first > 15 && item.first < 42){
            std::fprintf(pFile, "%d %02X\n", item.first, GetRAM(item.first));
        }
    }
    fclose(pFile);
}

void NetList::BuggyKey(){
    key->lwe_key->key[0] = (~key->lwe_key->key[0])&0x1;
}
 */
