//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLNOT_HPP
#define IYOKAN_L2_LOGICCELLNOT_HPP

#include "Logic.hpp"

class LogicCellNOT : public Logic {
public:
    LogicCellNOT(int id) : Logic(id) {
        Type = "NOT";
    }
    void PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
        res = 0;
        value = new_gate_bootstrapping_ciphertext(bk->params);
        bootsCONSTANT(value, 0, bk);
    }

    void PrepareExecution() {
        if (input.size() != 1) {
            throw std::runtime_error("Input is not assigned");
        }
        if (output.size() == 0) {
            throw std::runtime_error("Output is not assigned");
        }
        InputCount = input.size();
        ReadyInputCount = 0;
    }

    void Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        bootsNOT(value, input.at(0)->value, &key->cloud);
        res = (~input.at(0)->res) & 0x1;
        if (res != bootsSymDecrypt(value, key)) {
            throw new std::runtime_error("value not matched: NOT");
        }
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
        bootsNOT(value, input.at(0)->value, key);
        executed = true;
        ReadyQueue->push(this);
    }

    void Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
        res = (~input.at(0)->res) & 0x1;
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
        if (input.size() > 0) {
            throw std::runtime_error("Input is already assigned");
        }
        input.push_back(logic);
    }

    void AddOutput(Logic *logic) {
        output.push_back(logic);
    }

    bool Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
        executable = false;
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }

};

#endif //IYOKAN_L2_LOGICCELLNOT_HPP
