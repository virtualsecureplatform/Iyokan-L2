//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICPORTIN_HPP
#define IYOKAN_L2_LOGICPORTIN_HPP

#include "Logic.hpp"

class LogicPortIn : public Logic {
public:
    LogicPortIn(int id) : Logic(id) {}

    void PrepareExecution() {
        if (output.size() == 0) {
            throw std::runtime_error("Output is not assigned");
        }
    }

    void Execute(std::queue<Logic *> *ReadyQueue) {
        executed = true;
        std::cout << "Executed:LogicPortIn:" << id << std::endl;
        DependencyUpdate(ReadyQueue);
    }

    bool NoticeInputReady() {
        return executable;
    }

    void Set(int value) {
        res = value;
        executable = true;
    }

    void AddInput(Logic *logic) {

    }

    void AddOutput(Logic *logic) {
        output.push_back(logic);
    }

    bool Tick() {
        executable = true;
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }
};

#endif //IYOKAN_L2_LOGICPORTIN_HPP
