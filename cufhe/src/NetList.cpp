#include <queue>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <string.h>

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
    bool v,
    bool isCipher) {
    verbose = v;
    cipher = isCipher;
    ConvertJson(json);
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
            Logics[id] = new LogicPortIn(id, priority, cipher);
        } else if (type == "output") {
            Logics[id] = new LogicPortOut(id, priority, cipher);
        }
        Logics[id]->priority = priority;
    }
    for (const auto &e : cells) {  // vectorをrange-based-forでまわしている。
        picojson::object cell = e.get<picojson::object>();
        std::string type = cell.at("type").get<std::string>();
        int id = static_cast<int>(cell.at("id").get<double>());
        int priority = static_cast<int>(cell.at("priority").get<double>());
        if (type == "AND") {
            Logics[id] = new LogicCellAND(id, priority, cipher);
        } else if (type == "NAND") {
            Logics[id] = new LogicCellNAND(id, priority, cipher);
        } else if (type == "ANDNOT") {
            Logics[id] = new LogicCellANDNOT(id, priority, cipher);
        } else if (type == "XOR") {
            Logics[id] = new LogicCellXOR(id, priority, cipher);
        } else if (type == "XNOR") {
            Logics[id] = new LogicCellXNOR(id, priority, cipher);
        } else if (type == "DFFP") {
            Logics[id] = new LogicCellDFFP(id, priority, cipher);
        } else if (type == "NOT") {
            Logics[id] = new LogicCellNOT(id, priority, cipher);
        } else if (type == "NOR") {
            Logics[id] = new LogicCellNOR(id, priority, cipher);
        } else if (type == "OR") {
            Logics[id] = new LogicCellOR(id, priority, cipher);
        } else if (type == "ORNOT") {
            Logics[id] = new LogicCellORNOT(id, priority, cipher);
        } else if (type == "MUX") {
            Logics[id] = new LogicCellMUX(id, priority, cipher);
        } else if (type == "ROM") {
            Logics[id] = new LogicCellROM(id, priority, cipher);
        } else if (type == "RAM") {
            Logics[id] = new LogicCellRAM(id, priority, cipher);
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

void NetList::SetPortCipher(std::string portName, std::vector<cufhe::Ctxt *> valueArray) {
    int length = Inputs[portName].size();
    if (length == 0) {
        throw std::runtime_error("Unknown input port:" + portName);
    }
    if (valueArray.size() != length) {
        throw std::runtime_error("Invalid value");
    }
    for (int i = 0; i < length; i++) {
        Inputs[portName][i]->SetCipher(valueArray.at(i));
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

void NetList::SetROMCipher(int byte_addr, std::vector<std::shared_ptr<cufhe::Ctxt>> &valueArray) {
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
        Rom[addr][i + (word_num)*8]->SetCipher(valueArray.at(i).get());
    }
}

void NetList::SetROMCipherAll(std::vector<std::shared_ptr<cufhe::Ctxt>>& valueArray) {
    for (int i = 0; i < valueArray.size(); i++) {
        Rom[i / 32][i % 32]->SetCipher(valueArray.at(i).get());
    }
}

/*
void NetList::SetROMDecryptCipherAll(
    std::vector<std::shared_ptr<LweSample>> valueArray,
    std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey) {
    for (int i = 0; i < valueArray.size(); i++) {
        Rom[i / 32][i % 32]->SetPlain(bootsSymDecrypt(valueArray.at(i).get(), secretKey.get()) & 0x1);
    }
}
 */

/*
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
 */

void NetList::SetROMPlain(int addr, uint32_t value) {
    int length = Rom[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Rom Address:" + addr);
    }
    for (int i = 0; i < length; i++) {
        Rom[addr][i]->SetPlain(value & 0x1);
        value = value >> 1;
    }
}

void NetList::SetROMEncryptPlain(int addr, uint32_t value, cufhe::PriKey *secretKey) {
    int length = Rom[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Rom Address:" + addr);
    }
    cufhe::Ptxt plainValue;
    for (int i = 0; i < length; i++) {
        cufhe::Ctxt cipherValue;
        plainValue = value & 0x1;
        cufhe::Encrypt(cipherValue, plainValue, *secretKey);
        value = value >> 1;
        cufhe::Synchronize();
        Rom[addr][i]->SetCipher(&cipherValue);
    }
}

void NetList::SetRAMEncryptPlain(int addr, uint8_t value, cufhe::PriKey *secretKey) {
    int length = Ram[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Ram Address:" + addr);
    }
    cufhe::Ptxt plainValue;
    for (int i = 0; i < length; i++) {
        cufhe::Ctxt cipherValue;
        plainValue = value & 0x1;
        cufhe::Encrypt(cipherValue, plainValue, *secretKey);
        value = value >> 1;
        cufhe::Synchronize();
        Ram[addr][i]->SetCipher(&cipherValue);
    }
}

uint32_t NetList::GetROMDecryptCipher(int addr, cufhe::PriKey *secretKey) {
    int length = Rom[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Rom Address:" + addr);
    }
    uint32_t value = 0;
    cufhe::Ptxt plainValue;
    for (int i = length - 1; i > -1; i--) {
        value = value << 1;
        cufhe::Decrypt(plainValue, *Rom[addr][i]->GetCipher(), *secretKey);
        value += plainValue.message_;
    }
    return value;
}

void NetList::SetRAMCipher(int addr, std::vector<std::shared_ptr<cufhe::Ctxt>>& valueArray) {
    if (valueArray.size() != 8) {
        throw std::runtime_error("Invalid value");
    }
    int length = Ram[addr].size();
    if (length == 0) {
        throw std::runtime_error("Unknown Ram Address:" + addr);
    }
    for (int i = 0; i < length; i++) {
        Ram[addr][i]->SetCipher(valueArray.at(i).get());
    }
}

void NetList::SetRAMCipherAll(std::vector<std::shared_ptr<cufhe::Ctxt>>& valueArray){
    for(int i = 0;i<valueArray.size();i++){
       Ram[i/8] [i%8]->SetCipher(valueArray.at(i).get());
    }
}

/*
void NetList::SetRAMDecryptCipherAll(
    std::vector<std::shared_ptr<LweSample>> valueArray,
    std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey){
    for(int i = 0;i<valueArray.size();i++){
        Ram[i/8] [i%8]->SetPlain(bootsSymDecrypt(valueArray.at(i).get(), secretKey.get()));
    }
}
 */

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

std::vector<cufhe::Ctxt *> NetList::GetPortCipher(std::string portName) {
    int length = Outputs[portName].size();
    if (length == 0) {
        throw std::runtime_error("Unknown output port:" + portName);
    }
    std::vector<cufhe::Ctxt *> valueArray;
    for (int i = 0; i < length; i++) {
        valueArray.push_back(Outputs[portName][i]->GetCipher());
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

int NetList::GetPortDecryptCipher(std::string portName, cufhe::PriKey *secretKey) {
    int length = Outputs[portName].size();
    if (length == 0) {
        throw std::runtime_error("Unknown output port:" + portName);
    }
    int value = 0;
    cufhe::Ptxt plainValue;
    for (int i = length - 1; i > -1; i--) {
        value = value << 1;
        cufhe::Decrypt(plainValue, *Outputs[portName][i]->GetCipher(), *secretKey);
        value += plainValue.message_;
    }
    return value;
}

std::vector<std::shared_ptr<cufhe::Ctxt>> NetList::GetRAMCipher(int addr) {
    std::vector<std::shared_ptr<cufhe::Ctxt>> valueArray;
    for (int i = 0; i < 8; i++) {
        valueArray.push_back(std::shared_ptr<cufhe::Ctxt>(Ram[addr][i]->GetCipher()));
    }
    return valueArray;
}

std::vector<std::shared_ptr<cufhe::Ctxt>> NetList::GetRAMCipherAll() {
    std::vector<std::shared_ptr<cufhe::Ctxt>> valueArray;
    for (int i = 0; i < Ram.size()*8; i++) {
        valueArray.push_back(std::shared_ptr<cufhe::Ctxt>(Ram[i/8][i%8]->GetCipher()));
    }
    return valueArray;
}

/*
std::vector<std::shared_ptr<LweSample>> NetList::GetRAMEncryptPlainAll(
    std::shared_ptr<TFheGateBootstrappingSecretKeySet> secretKey
    ){
    std::vector<std::shared_ptr<LweSample>> valueArray;
    for (int i = 0; i < Ram.size()*8; i++) {
        std::shared_ptr<LweSample> value{
            new_gate_bootstrapping_ciphertext(secretKey.get()->params),
            delete_gate_bootstrapping_ciphertext};
        bootsSymEncrypt(value.get(), Ram[i/8][i%8]->GetPlain() & 0x1, secretKey.get());
        valueArray.push_back(value);
    }
    return valueArray;
}
 */

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
void NetList::DebugRegisterWrite(){
    static int reg[16] = {0};

    int x0 = GetPortPlain("io_regOut_x0");
    if(reg[0] != x0){
       printf("x0 <= %x\n", x0);
       reg[0] = x0;
    }

    int x8 = GetPortPlain("io_regOut_x8");
    if(reg[8] != x8){
        printf("x8 <= %x\n", x8);
        reg[8] = x8;
    }

    int x10 = GetPortPlain("io_regOut_x10");
    if(reg[10] != x10){
        printf("x10 <= %x\n", x10);
        reg[10] = x10;
    }

    int x11 = GetPortPlain("io_regOut_x11");
    if(reg[11] != x11){
        printf("x11 <= %x\n", x11);
        reg[11] = x11;
    }

}
*/

void NetList::EnableReset() {
    if (cipher) {
        cufhe::Ctxt value;
        std::vector<cufhe::Ctxt *> valueArray;
        cufhe::ConstantOne(value);
        cufhe::Synchronize();
        valueArray.push_back(&value);
        SetPortCipher("reset", valueArray);
    } else {
        SetPortPlain("reset", 1);
    }
}

void NetList::DisableReset() {
    if (cipher) {
        cufhe::Ctxt value;
        std::vector<cufhe::Ctxt *> valueArray;
        cufhe::ConstantZero(value);
        cufhe::Synchronize();
        valueArray.push_back(&value);
        SetPortCipher("reset", valueArray);
    } else {
        SetPortPlain("reset", 0);
    }
}
/*
void NetList::BuggyKey(){
    key->lwe_key->key[0] = (~key->lwe_key->key[0])&0x1;
}
 */
