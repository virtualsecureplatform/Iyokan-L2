#include "LogicCellRAM.hpp"

LogicCellRAM::LogicCellRAM(int id) : Logic(id) {
    Type = "RAM";
    res = 0;
}

void LogicCellRAM::PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
    if (!created) {
        value = new_gate_bootstrapping_ciphertext(bk->params);
        bootsCONSTANT(value, res, bk);
    }
}

void LogicCellRAM::PrepareExecution() {
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

void LogicCellRAM::Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    if (res != bootsSymDecrypt(value, key)) {
        throw new std::runtime_error("value not matched: DFFP");
    }
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellRAM::Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellRAM::Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

bool LogicCellRAM::NoticeInputReady() {
    return false;
}

void LogicCellRAM::AddInput(Logic *logic) {
    if (input.size() > 0) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellRAM::AddOutput(Logic *logic) {
    output.push_back(logic);
}

void LogicCellRAM::Set(int val, const TFheGateBootstrappingCloudKeySet *bk) {
    res = val & 0x1;
    value = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(value, val & 0x1, bk);
    created = true;
}

int LogicCellRAM::Get(TFheGateBootstrappingSecretKeySet *key) {
    return bootsSymDecrypt(value, key);
}

int LogicCellRAM::Get() {
    return res;
}

bool LogicCellRAM::Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
    if (!reset) {
        res = input.at(0)->res;
        bootsCOPY(value, input.at(0)->value, key);
    }
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}

