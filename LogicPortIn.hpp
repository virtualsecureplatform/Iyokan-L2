//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICPORTIN_HPP
#define IYOKAN_L2_LOGICPORTIN_HPP

#include "Logic.hpp"

class LogicPortIn : public Logic {
public:
    LogicPortIn(int id) : Logic(id) {}

    void Execute(std::queue<Logic *> *ReadyQueue) {
        executed = true;
        std::cout << "Executed:LogicPortIn:" << id << std::endl;
        DependencyUpdate(ReadyQueue);
    }

    void PrepareExecution() {
    }

    bool NoticeInputReady() {
        return executable;
    }

    void Set(int value) {
        res = value;
        executable = true;
    }

    void AddOutput(Logic * logic){
        output.push_back(logic);
    }
};

#endif //IYOKAN_L2_LOGICPORTIN_HPP
