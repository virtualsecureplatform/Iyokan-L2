//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLANDNOT_HPP
#define IYOKAN_L2_LOGICCELLANDNOT_HPP

#include "Logic.hpp"

class LogicCellANDNOT : public Logic {
public:
    LogicCellANDNOT(int id) : Logic(id) {
        Type = "ANDNOT";
    }

    void PrepareExecution() {
        if (input.size() != 2) {
            throw std::runtime_error("Input is not assigned ANDNOT");
        }
        if (output.size() == 0) {
            throw std::runtime_error("Output is not assigned");
        }
        InputCount = input.size();
        ReadyInputCount = 0;
    }

    void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        bootsANDYN(value, input.at(0)->value, input.at(1)->value, &key->cloud);
        res = (input.at(0)->res & (~input.at(1)->res)) & 0x1;
        if (res != bootsSymDecrypt(value, key)) {
            throw new std::runtime_error("value not matched: ANDNOT");
        }
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        bootsANDYN(value, input.at(0)->value, input.at(1)->value, key);
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
        res = (input.at(0)->res & (~input.at(1)->res)) & 0x1;
        executed = true;
        //std::cout << "Executed:LogicCellANDNOT:" << id << std::endl;
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
        if (input.size() > 1) {
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

#endif //IYOKAN_L2_LOGICCELLANDNOT_HPP