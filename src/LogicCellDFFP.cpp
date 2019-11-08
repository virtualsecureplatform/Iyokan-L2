#include "LogicCellDFFP.hpp"

LogicCellDFFP::LogicCellDFFP(int id) : Logic(id) {
    Type = "DFFP";
}

void LogicCellDFFP::PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
    res = 0;
    value = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(value, 0, bk);
}

void LogicCellDFFP::PrepareExecution() {
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

void LogicCellDFFP::Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    if (res != bootsSymDecrypt(value, key)) {
        throw new std::runtime_error("value not matched: DFFP");
    }
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellDFFP::Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellDFFP::Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

bool LogicCellDFFP::NoticeInputReady() {
    return false;
}

void LogicCellDFFP::AddInput(Logic *logic) {
    if (input.size() > 0) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellDFFP::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellDFFP::Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
    res = input.at(0)->res;
    bootsCOPY(value, input.at(0)->value, key);
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
