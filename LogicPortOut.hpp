//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICPORTOUT_HPP
#define IYOKAN_L2_LOGICPORTOUT_HPP

#include "Logic.hpp"

class LogicPortOut : public Logic {
public:
    LogicPortOut(int id) : Logic(id) {}

    void PrepareExecution() {
        if (input.size() == 0) {
            throw std::runtime_error("Input is not assigned");
        }
    }

    void Execute(std::queue<Logic *> *ReadyQueue) {
        executed = true;
        std::cout << "Executed:LogicPortOut:" << id << std::endl;
        DependencyUpdate(ReadyQueue);
    }

    bool NoticeInputReady() {
        executable = true;
        return executable;
    }

    int Get() {
        return input.front()->res;
    }

    void AddInput(Logic *logic) {
        if (input.size() > 0) {
            throw std::runtime_error("Input is already assigned");
        }
        if (input.size() == 0) {
            input.push_back(logic);
        }
    }

    void AddOutput(Logic *logic) {

    }

    bool Tick() {
        executable = false;
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }
};

#endif //IYOKAN_L2_LOGICPORTOUT_HPP
