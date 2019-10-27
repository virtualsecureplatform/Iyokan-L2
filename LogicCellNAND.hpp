//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLNAND_HPP
#define IYOKAN_L2_LOGICCELLNAND_HPP

#include "Logic.hpp"

class LogicCellNAND : public Logic {
public:
    LogicCellNAND(int id) : Logic(id) {}

    void PrepareExecution() {
        if (input.size() != 2) {
            throw std::runtime_error("Input is not assigned");
        }
        if (output.size() == 0) {
            throw std::runtime_error("Output is not assigned");
        }
        InputCount = input.size();
        ReadyInputCount = 0;
    }

    void Execute(std::queue<Logic *> *ReadyQueue) {
        res = (~(input.at(0)->res & input.at(1)->res)) & 0x1;
        executed = true;
        std::cout << "Executed:LogicCellNAND:" << id << std::endl;
        DependencyUpdate(ReadyQueue);
    }

    bool NoticeInputReady() {
        ReadyInputCount++;
        if (ReadyInputCount > InputCount) {
            throw std::runtime_error("ReadyInputCount is invalid");
        }
        return InputCount == ReadyInputCount;
    }

    void AddInput(Logic *logic) {
        if (input.size() > 2) {
            throw std::runtime_error("Input is already assigned");
        }
        input.push_back(logic);
    }

    void AddOutput(Logic *logic) {
        output.push_back(logic);
    }

};

#endif //IYOKAN_L2_LOGICCELLNAND_HPP
