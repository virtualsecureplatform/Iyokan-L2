//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLMUX_HPP
#define IYOKAN_L2_LOGICCELLMUX_HPP

#include "Logic.hpp"

class LogicCellMUX : public Logic {
public:
    LogicCellMUX(int id) : Logic(id) {
        Type = "MUX";
    }

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

    void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        bootsMUX(value, input.at(2)->value, input.at(1)->value, input.at(0)->value, &key->cloud);
        if (input.at(2)->res == 0) {
            res = input.at(0)->res;
        } else if (input.at(2)->res == 1) {
            res = input.at(1)->res;
        } else {
            throw std::runtime_error("invalid select signal");
        }
        if (res != bootsSymDecrypt(value, key)) {
            throw new std::runtime_error("value not matched: MUX");
        }
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        bootsMUX(value, input.at(2)->value, input.at(1)->value, input.at(0)->value, key);
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
        if (input.at(2)->res == 0) {
            res = input.at(0)->res;
        } else if (input.at(2)->res == 1) {
            res = input.at(1)->res;
        } else {
            throw std::runtime_error("invalid select signal");
        }
        executed = true;
        ReadyQueue->push(this);
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

    bool Tick(const TFheGateBootstrappingCloudKeySet *key) {
        executable = false;
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }
};

#endif //IYOKAN_L2_LOGICCELLMUX_HPP
