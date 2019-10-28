//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLMUX_HPP
#define IYOKAN_L2_LOGICCELLMUX_HPP

#include "Logic.hpp"

class LogicCellMUX : public Logic {
public:
    LogicCellMUX(int id) : Logic(id) {}

    void PrepareExecution() {
        if (input.size() != 3) {
            throw std::runtime_error("Input is not assigned");
        }
        if (output.size() == 0) {
            throw std::runtime_error("Output is not assigned");
        }
        InputCount = input.size();
        ReadyInputCount = 0;
    }

    void Execute(std::queue<Logic *> *ReadyQueue) {
        if (input.at(2)->res == 0) {
            res = input.at(0)->res;
        } else if (input.at(2)->res == 1) {
            res = input.at(1)->res;
        } else {
            throw std::runtime_error("invalid select signal");
        }
        executed = true;
        std::cout << "Executed:LogicCellMUX:" << id << std::endl;
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

    bool Tick() {
        executable = false;
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }
};

#endif //IYOKAN_L2_LOGICCELLMUX_HPP
