#include "LogicCellROM.hpp"

LogicCellROM::LogicCellROM(int id) : Logic(id) {
    Type = "ROM";
}

void LogicCellROM::PrepareTFHE(const TFheGateBootstrappingCloudKeySet *bk) {
    if (!created) {
        value = new_gate_bootstrapping_ciphertext(bk->params);
        bootsCONSTANT(value, 0, bk);
    }
}

void LogicCellROM::PrepareExecution() {
    if (input.size() != 0) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }
    executable = true;
    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellROM::Execute(TFheGateBootstrappingSecretKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    if (res != bootsSymDecrypt(value, key)) {
        throw new std::runtime_error("value not matched: DFFP");
    }
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellROM::Execute(const TFheGateBootstrappingCloudKeySet *key, tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

void LogicCellROM::Execute(tbb::concurrent_queue<Logic *> *ReadyQueue) {
    executed = true;
    ReadyQueue->push(this);
}

bool LogicCellROM::NoticeInputReady() {
    return true;
}

void LogicCellROM::AddInput(Logic *logic) {
    if (input.size() > 0) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellROM::AddOutput(Logic *logic) {
    output.push_back(logic);
}

void LogicCellROM::Set(int val, const TFheGateBootstrappingCloudKeySet *bk) {
    res = val & 0x1;
    value = new_gate_bootstrapping_ciphertext(bk->params);
    bootsCONSTANT(value, val & 0x1, bk);
    created = true;
}

bool LogicCellROM::Tick(const TFheGateBootstrappingCloudKeySet *key, bool reset) {
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
