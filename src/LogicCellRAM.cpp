#include "LogicCellRAM.hpp"

LogicCellRAM::LogicCellRAM(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue,
    const TFheGateBootstrappingCloudKeySet *ck) : Logic(id, pri, queue, ck) {
    Type = "RAM";
}

LogicCellRAM::LogicCellRAM(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue) : Logic(id, pri, queue) {
    Type = "RAM";
}

void LogicCellRAM::Prepare() {
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

void LogicCellRAM::Execute() {
    executed = true;
    executedQueue->push(this);
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

void LogicCellRAM::SetCipher(std::shared_ptr<LweSample> val) {
    bootsCOPY(value, val.get(), key);
}

void LogicCellRAM::SetPlain(int val) {
    res = val & 0x1;
}

LweSample *LogicCellRAM::GetCipher() {
    return value;
}

int LogicCellRAM::GetPlain() {
    return res;
}

bool LogicCellRAM::Tick(bool reset) {
    if (!reset) {
        if (cipher) {
            bootsCOPY(value, input.at(0)->value, key);
        } else {
            res = input.at(0)->res;
        }
    }
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
