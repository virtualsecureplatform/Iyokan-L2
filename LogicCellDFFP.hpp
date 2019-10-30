//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLDFFP_HPP
#define IYOKAN_L2_LOGICCELLDFFP_HPP

#include "Logic.hpp"

class LogicCellDFFP : public Logic {
public:
    LogicCellDFFP(int id) : Logic(id) {}

    void PrepareExecution() {
        if (input.size() != 1) {
            throw std::runtime_error("Input is not assigned");
        }
        if (output.size() == 0) {
            throw std::runtime_error("Output is not assigned");
        }
        executable = true;
        InputCount = input.size();
        ReadyInputCount = 0;
    }

    void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        if (res != bootsSymDecrypt(value, key)) {
            throw new std::runtime_error("value not matched: DFFP");
        }
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
        executed = true;
        ReadyQueue->push(this);
    }

    bool NoticeInputReady() {
        return false;
    }

    void AddInput(Logic *logic) {
        if (input.size() > 0) {
            throw std::runtime_error("Input is already assigned");
        }
        input.push_back(logic);
    }

    void AddOutput(Logic *logic) {
        output.push_back(logic);
    }

    bool Tick(const TFheGateBootstrappingCloudKeySet *key) {
        res = input.at(0)->res;
        bootsCOPY(value, input.at(0)->value, key);
        executable = true;
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }
};

#endif //IYOKAN_L2_LOGICCELLDFFP_HPP
