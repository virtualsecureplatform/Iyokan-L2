#include "LogicCellAND.hpp"

LogicCellAND::LogicCellAND(int id) : Logic(id) {
    Type = "AND";
}

void LogicCellAND::PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
    res = 0;
    value = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(value, 0, bk);
}

void LogicCellAND::PrepareExecution() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }
    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellAND::Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    bootsAND(value, input.at(0)->value, input.at(1)->value, &key->cloud);
    res = input.at(0)->res & input.at(1)->res;
    if (res != bootsSymDecrypt(value, key)) {
        throw new std::runtime_error("value not matched: AND");
    }
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellAND::Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    bootsAND(value, input.at(0)->value, input.at(1)->value, key);
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellAND::Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
    res = input.at(0)->res & input.at(1)->res;
    executed = true;
    ReadyQueue->push(this);
}

bool LogicCellAND::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[AND] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellAND::AddInput(Logic *logic) {
    if (input.size() > 1) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellAND::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellAND::Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
