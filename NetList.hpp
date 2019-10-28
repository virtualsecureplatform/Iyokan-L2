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

#include "picojson/picojson.h"

class NetList {
public:
    NetList(const char *json) {
        JsonFile = std::string(json);
    }

    int ConvertJson() {
        std::ifstream ifs(JsonFile, std::ios::in);
        if (ifs.fail()) {
            std::cerr << "failed to read test.json" << std::endl;
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
            //std::cout << type << std::endl;
            //std::cout << id << std::endl;
            if (type == "input") {
                //std::cout << "INSERT input" << std::endl;
                Logics[id] = new LogicPortIn(id);
            } else if (type == "output") {
                //std::cout << "INSERT output" << std::endl;
                Logics[id] = new LogicPortOut(id);
            }
        }
        for (const auto &e : cells) {  // vectorをrange-based-forでまわしている。
            picojson::object cell = e.get<picojson::object>();
            std::string type = cell.at("type").get<std::string>();
            int id = static_cast< int >(cell.at("id").get<double>());
            //std::cout << type << std::endl;
            //std::cout << id << std::endl;
            if (type == "AND") {
                //std::cout << "INSERT AND" << std::endl;
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
            } else {
                throw std::runtime_error("Not implemented:" + type);
            }
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
                Inputs[portName][portBit] = (LogicPortIn *)Logics[id];
                std::cout << name << "(LogicID " << id << ") has to be set value" << std::endl;
            } else if (type == "output") {
                for (const auto &b : bits) {
                    int logic = static_cast< int >(b.get<double>());
                    Logics[id]->AddInput(Logics[logic]);
                }
                Outputs[portName][portBit] = (LogicPortOut *)Logics[id];
                std::cout << name << "(LogicID " << id << ") has output value" << std::endl;
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
            } else {
                throw std::runtime_error("Not executed");
            }
        }
        return 0;
    }

    void PrepareExecution() {
        for (auto logic : Logics) {
            logic.second->PrepareExecution();
            if (logic.second->executable) {
                ReadyQueue.push(logic.second);
            }
        }
    }

    void Execute() {
        while (ReadyQueue.size() > 0) {
            ReadyQueue.front()->Execute(&ReadyQueue);
            ReadyQueue.pop();
            //std::cout << "ReadQueueSize: " << ReadyQueue.size() << std::endl;
        }
    }

    void Tick() {
        Execute();
        for (auto logic : Logics) {
            if (logic.second->Tick()) {
                ReadyQueue.push(logic.second);
            }
        }
    }

    void Set(int id, int value) {
        ((LogicPortIn *) Logics[id])->Set(value&0x1);
    }

    void Set(int msb, int lsb, int value){
        for(int i=lsb;i<msb+1;i++){
            ((LogicPortIn *)Logics[i])->Set(value&0x1);
            value = value >> 1;
        }
    }

    void Set(std::string portName, int value){
        int length = Inputs[portName].size();
        if(length == 0){
            throw std::runtime_error("Unknown input port:"+portName);
        }
        for(int i=0;i<length;i++){
            Inputs[portName][i]->Set(value&0x1);
            value = value >> 1;
        }
    }

    int Get(int id) {
        return ((LogicPortOut *) Logics[id])->Get();
    }

    int Get(int msb, int lsb){
        int value = 0;
        for(int i=msb;i>lsb-1;i--){
            value = value << 1;
            value += ((LogicPortOut *)Logics[i])->Get();
        }
        return value;
    }

    int Get(std::string portName){
        int length = Outputs[portName].size();
        if(length == 0){
            throw std::runtime_error("Unknown output port:"+portName);
        }
        int value = 0;
        for(int i=length-1;i>-1;i--){
            value = value << 1;
            value += Outputs[portName][i]->Get();
        }
        return value;
    }

    void Stats() {
        int cnt = Logics.size();
        int executed_cnt = 0;
        for (auto logic : Logics) {
            if (logic.second->executed) {
                executed_cnt++;
            }
        }
        std::cout << "Logics: " << cnt << " Executed: " << executed_cnt << std::endl;
    }

    void SetExecutable(int id) {
        Logics[id]->SetExecutable();
    }

private:
    std::queue<Logic *> ReadyQueue;
    std::unordered_map<int, Logic *> Logics;
    std::unordered_map<std::string, std::unordered_map<int, LogicPortIn *>> Inputs;
    std::unordered_map<std::string, std::unordered_map<int, LogicPortOut *>> Outputs;
    std::string JsonFile;
};

#endif //IYOKAN_L2_NETLIST_HPP
