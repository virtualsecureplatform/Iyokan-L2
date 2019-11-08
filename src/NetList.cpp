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
#include "Config.hpp"

NetList::NetList(const char *json, bool v) {
    verbose = v;
    JsonFile = std::string(json);
    ConvertJson();
    PrepareTFHE();
}

void NetList::PrepareTFHE() {
    //generate a keyset
    const int minimum_lambda = 110;
    params = new_default_gate_bootstrapping_parameters(minimum_lambda);

    //generate a random key
    uint32_t seed[] = {314, 1592, 657};
    tfhe_random_generator_setSeed(seed, 3);
    key = new_random_gate_bootstrapping_secret_keyset(params);
    for (auto logic : Logics) {
        logic.second->PrepareTFHE(&(key->cloud));
    }
}

int NetList::ConvertJson() {
    std::ifstream ifs(JsonFile, std::ios::in);
    if (ifs.fail()) {
        std::cerr << "failed to read " << JsonFile << std::endl;
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
        int id = static_cast< int >(port.at("id").get<double>());
        int priority = static_cast< int >(port.at("priority").get<double>());
        if (type == "input") {
            Logics[id] = new LogicPortIn(id);
        } else if (type == "output") {
            Logics[id] = new LogicPortOut(id);
        }
        Logics[id]->priority = priority;
    }
    for (const auto &e : cells) {  // vectorをrange-based-forでまわしている。
        picojson::object cell = e.get<picojson::object>();
        std::string type = cell.at("type").get<std::string>();
        int id = static_cast< int >(cell.at("id").get<double>());
        int priority = static_cast< int >(cell.at("priority").get<double>());
        if (type == "AND") {
            Logics[id] = new LogicCellAND(id);
        } else if (type == "NAND") {
            Logics[id] = new LogicCellNAND(id);
        } else if (type == "ANDNOT") {
            Logics[id] = new LogicCellANDNOT(id);
        } else if (type == "XOR") {
            Logics[id] = new LogicCellXOR(id);
        } else if (type == "XNOR") {
            Logics[id] = new LogicCellXNOR(id);
        } else if (type == "DFFP") {
            Logics[id] = new LogicCellDFFP(id);
        } else if (type == "NOT") {
            Logics[id] = new LogicCellNOT(id);
        } else if (type == "NOR") {
            Logics[id] = new LogicCellNOR(id);
        } else if (type == "OR") {
            Logics[id] = new LogicCellOR(id);
        } else if (type == "ORNOT") {
            Logics[id] = new LogicCellORNOT(id);
        } else if (type == "MUX") {
            Logics[id] = new LogicCellMUX(id);
        } else if (type == "ROM") {
            Logics[id] = new LogicCellROM(id);
        } else if (type == "RAM") {
            Logics[id] = new LogicCellRAM(id);
        } else {
            throw std::runtime_error("Not implemented:" + type);
        }
        Logics[id]->priority = priority;
    }
    for (const auto &e : ports) {  // vectorをrange-based-forでまわしている。
        picojson::object port = e.get<picojson::object>();
        std::string type = port.at("type").get<std::string>();
        std::string name = port.at("name").get<std::string>();
        int id = static_cast< int >(port.at("id").get<double>());
        picojson::array &bits = port.at("bits").get<picojson::array>();
        std::string portName = port.at("portName").get<std::string>();
        int portBit = static_cast< int >(port.at("portBit").get<double>());
        if (type == "input") {
            for (const auto &b : bits) {
                int logic = static_cast< int >(b.get<double>());
                Logics[id]->AddOutput(Logics[logic]);
            }
            Inputs[portName][portBit] = (LogicPortIn *) Logics[id];
        } else if (type == "output") {
            for (const auto &b : bits) {
                int logic = static_cast< int >(b.get<double>());
                Logics[id]->AddInput(Logics[logic]);
            }
            Outputs[portName][portBit] = (LogicPortOut *) Logics[id];
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
        int id = static_cast< int >(cell.at("id").get<double>());
        picojson::object input = cell.at("input").get<picojson::object>();
        picojson::object output = cell.at("output").get<picojson::object>();
        if (type == "AND" || type == "NAND" || type == "XOR" || type == "XNOR" || type == "NOR" ||
            type == "ANDNOT" || type == "OR" || type == "ORNOT") {
            int A = static_cast< int >(input.at("A").get<double>());
            int B = static_cast< int >(input.at("B").get<double>());
            picojson::array &Y = output.at("Y").get<picojson::array>();
            Logics[id]->AddInput(Logics[A]);
            Logics[id]->AddInput(Logics[B]);
            for (const auto &y : Y) {
                int bitY = static_cast< int >(y.get<double>());
                Logics[id]->AddOutput(Logics[bitY]);
            }
        } else if (type == "DFFP") {
            int D = static_cast< int >(input.at("D").get<double>());
            picojson::array &Q = output.at("Q").get<picojson::array>();
            Logics[id]->AddInput(Logics[D]);
            for (const auto &q : Q) {
                int bitQ = static_cast< int >(q.get<double>());
                Logics[id]->AddOutput(Logics[bitQ]);
            }
        } else if (type == "NOT") {
            int A = static_cast< int >(input.at("A").get<double>());
            picojson::array &Y = output.at("Y").get<picojson::array>();
            Logics[id]->AddInput(Logics[A]);
            for (const auto &y : Y) {
                int bitY = static_cast< int >(y.get<double>());
                Logics[id]->AddOutput(Logics[bitY]);
            }
        } else if (type == "MUX") {
            int A = static_cast< int >(input.at("A").get<double>());
            int B = static_cast< int >(input.at("B").get<double>());
            int S = static_cast< int >(input.at("S").get<double>());
            picojson::array &Y = output.at("Y").get<picojson::array>();
            Logics[id]->AddInput(Logics[A]);
            Logics[id]->AddInput(Logics[B]);
            Logics[id]->AddInput(Logics[S]);
            for (const auto &y : Y) {
                int bitY = static_cast< int >(y.get<double>());
                Logics[id]->AddOutput(Logics[bitY]);
            }
        } else if (type == "ROM") {
            picojson::array &Q = output.at("Q").get<picojson::array>();
            int romAddress = static_cast< int >(cell.at("romAddress").get<double>());
            int romBit = static_cast< int >(cell.at("romBit").get<double>());
            for (const auto &q : Q) {
                int bitQ = static_cast< int >(q.get<double>());
                Logics[id]->AddOutput(Logics[bitQ]);
            }
            Rom[romAddress][romBit] = (LogicCellROM *) Logics[id];
        } else if (type == "RAM") {
            int ramAddress = static_cast< int >(cell.at("ramAddress").get<double>());
            int ramBit = static_cast< int >(cell.at("ramBit").get<double>());
            int D = static_cast< int >(input.at("D").get<double>());
            picojson::array &Q = output.at("Q").get<picojson::array>();
            Logics[id]->AddInput(Logics[D]);
            for (const auto &q : Q) {
                int bitQ = static_cast< int >(q.get<double>());
                Logics[id]->AddOutput(Logics[bitQ]);
            }
            Ram[ramAddress][ramBit] = (LogicCellRAM *) Logics[id];
        } else {
            throw std::runtime_error("Not executed");
        }
    }
    return 0;
}


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

void NetList::SetROM(int addr, int value) {
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
        std::printf("RAM[%d] 0x%02X\n", item.first, GetRAM(item.first));
    }
}
