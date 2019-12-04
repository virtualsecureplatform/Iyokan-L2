#include "LogicCellROM.hpp"

LogicCellROM::LogicCellROM(
        int id,
        int pri,
        tbb::concurrent_queue<Logic *> *queue,
        const TFheGateBootstrappingCloudKeySet *ck
) : Logic(id, pri, queue, ck) {
    Type = "ROM";
}

void LogicCellROM::Prepare() {
    if (input.size() != 0) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    if (!created) {
        if (cipher) {
            value = new_gate_bootstrapping_ciphertext(key->params);
            bootsCONSTANT(value, 0, key);
        } else {
            res = 0;
        }
    }

    executable = true;
    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellROM::Execute() {
    executed = true;
    executedQueue->push(this);
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

void LogicCellROM::SetCipher(LweSample *val) {
    bootsCOPY(value, val, key);
    created = true;
}

void LogicCellROM::SetPlain(int val) {
    res = val & 0x1;
    created = true;
}

bool LogicCellROM::Tick(bool reset) {
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
