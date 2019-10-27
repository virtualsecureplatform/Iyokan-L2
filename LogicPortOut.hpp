//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICPORTOUT_HPP
#define IYOKAN_L2_LOGICPORTOUT_HPP

#include "Logic.hpp"

class LogicPortOut : public Logic {
public:
    LogicPortOut(int id) : Logic(id) {}

    void Execute(std::queue<Logic *> *ReadyQueue) {
        res = input.front()->res;
        executed = true;
        std::cout << "Executed:LogicPortOut:" << id << std::endl;
        DependencyUpdate(ReadyQueue);
    }

    bool NoticeInputReady() {
        executable = true;
        return executable;
    }

    void PrepareExecution() {
        executable = false;
        executed = false;
    }

    int Get() {
        return res;
    }

    void AddInput(Logic *logic){
        if(input.size() == 0){
            input.push_back(logic);
        }
    }
};

#endif //IYOKAN_L2_LOGICPORTOUT_HPP
