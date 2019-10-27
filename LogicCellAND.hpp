//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLAND_HPP
#define IYOKAN_L2_LOGICCELLAND_HPP
#include "Logic.hpp"
class LogicCellAND : public Logic {
public:
    LogicCellAND(int id) : Logic(id) {}

    void PrepareExecution(){
        InputCount = input.size();
        ReadyInputCount = 0;
    }

    void Execute(std::queue<Logic *> *ReadyQueue){
        res = input.at(0)->res & input.at(1)->res;
        executed = true;
        std::cout << "Executed:LogicCellAND:" << id << std::endl;
        DependencyUpdate(ReadyQueue);
    }

    bool NoticeInputReady(){
        ReadyInputCount++;
        return InputCount == ReadyInputCount;
    }

    void AddInput(Logic *logic){
        input.push_back(logic);
    }

    void AddOutput(Logic * logic){
        output.push_back(logic);
    }

};
#endif //IYOKAN_L2_LOGICCELLAND_HPP
