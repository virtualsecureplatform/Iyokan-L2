//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICPORT_H
#define IYOKAN_L2_LOGICPORT_H

#include "Logic.hpp"
#include <iostream>
#include <queue>

class LogicPort : public Logic {
public:
    LogicPort(int id) : Logic(id) {}

    void Execute(std::queue<Logic *> *ReadyQueue) {
        std::cout << "Executed:Logic" << id << std::endl;
        executed = true;
        DependencyUpdate(ReadyQueue);
    }
};

#endif //IYOKAN_L2_LOGICPORT_H
