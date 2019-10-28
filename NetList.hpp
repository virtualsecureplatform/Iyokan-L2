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
            } else if (type == "XOR") {
                Logics[id] = new LogicCellXOR(id);
            } else if (type == "XNOR") {
                Logics[id] = new LogicCellXNOR(id);
            } else if (type == "DFFP") {
                Logics[id] = new LogicCellDFFP(id);
            }
        }
        for (const auto &e : ports) {  // vectorをrange-based-forでまわしている。
            picojson::object port = e.get<picojson::object>();
            std::string type = port.at("type").get<std::string>();
            std::string name = port.at("name").get<std::string>();
            int id = static_cast< int >(port.at("id").get<double>());
            picojson::array &bits = port.at("bits").get<picojson::array>();
            if (type == "input") {
                for (const auto &b : bits) {
                    int logic = static_cast< int >(b.get<double>());
                    Logics[id]->AddOutput(Logics[logic]);
                }
                std::cout << name << "(LogicID " << id << ") has to be set value" << std::endl;
            } else if (type == "output") {
                for (const auto &b : bits) {
                    int logic = static_cast< int >(b.get<double>());
                    Logics[id]->AddInput(Logics[logic]);
                }
                std::cout << name << "(LogicID " << id << ") has output value" << std::endl;
            }
        }
        for (const auto &e : cells) {  // vectorをrange-based-forでまわしている。
            picojson::object cell = e.get<picojson::object>();
            std::string type = cell.at("type").get<std::string>();
            int id = static_cast< int >(cell.at("id").get<double>());
            picojson::object input = cell.at("input").get<picojson::object>();
            picojson::object output = cell.at("output").get<picojson::object>();
            if (type == "AND" || type == "NAND" || type == "XOR" || type == "XNOR") {
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
        for (auto logic : Logics) {
            if (logic.second->Tick()) {
                ReadyQueue.push(logic.second);
            }
        }
    }

    void Set(int id, int value) {
        ((LogicPortIn *) Logics[id])->Set(value);
    }

    int Get(int id) {
        return ((LogicPortOut *) Logics[id])->Get();
    }

private:
    std::unordered_map<int, Logic *> Logics;
    std::queue<Logic *> ReadyQueue;
    std::string JsonFile;
};

#endif //IYOKAN_L2_NETLIST_HPP
