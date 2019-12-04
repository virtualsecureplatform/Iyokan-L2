#include "LogicCellNAND.hpp"

LogicCellNAND::LogicCellNAND(int id) : Logic(id) {
    Type = "NAND";
}

void LogicCellNAND::PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
    res = 0;
    value = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(value, 0, bk);
}

void LogicCellNAND::PrepareExecution() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }
    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellNAND::Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    bootsNAND(value, input.at(0)->value, input.at(1)->value, &key->cloud);
    res = (~(input.at(0)->res & input.at(1)->res)) & 0x1;
    if (res != bootsSymDecrypt(value, key)) {
        throw new std::runtime_error("value not matched: NAND");
    }
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellNAND::Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    bootsNAND(value, input.at(0)->value, input.at(1)->value, key);
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellNAND::Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
    res = (~(input.at(0)->res & input.at(1)->res)) & 0x1;
    executed = true;
    ReadyQueue->push(this);
}

bool LogicCellNAND::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[NAND] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellNAND::AddInput(Logic *logic) {
    if (input.size() > 1) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellNAND::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellNAND::Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
