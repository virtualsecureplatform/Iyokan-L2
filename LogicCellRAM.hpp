//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLRAM_HPP
#define IYOKAN_L2_LOGICCELLRAM_HPP

#include "Logic.hpp"

class LogicCellRAM : public Logic {
public:
    LogicCellRAM(int id) : Logic(id) {
        Type = "RAM";
        res = 0;
    }

    void PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
        if(!created){
            value = new_gate_bootstrapping_ciphertext(bk->params);
            bootsCONSTANT(value, res, bk);
        }
    }
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

    void Set(int val, const TFheGateBootstrappingCloudKeySet *bk) {
        res = val & 0x1;
        value = new_gate_bootstrapping_ciphertext(bk->params);
        bootsCONSTANT(value, val & 0x1, bk);
        created = true;
    }

    int Get(TFheGateBootstrappingSecretKeySet *key) {
        return bootsSymDecrypt(value, key);
    }

    int Get() {
        return res;
    }

    bool Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
        if(!reset){
            res = input.at(0)->res;
            bootsCOPY(value, input.at(0)->value, key);
        }
        executable = true;
        executed = false;
        ReadyInputCount = 0;
        return executable;
    }
private:
    bool created = false;
};

#endif //IYOKAN_L2_LOGICCELLRAM_HPP
