#include "LogicCellNAND.hpp"

LogicCellNAND::LogicCellNAND(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue,
    const TFheGateBootstrappingCloudKeySet *ck) : Logic(id, pri, queue, ck) {
    Type = "NAND";
}

LogicCellNAND::LogicCellNAND(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue) : Logic(id, pri, queue) {
    Type = "NAND";
}

void LogicCellNAND::Prepare() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellNAND::Execute() {
    if (cipher) {
        bootsNAND(value, input.at(0)->value, input.at(1)->value, key);
    } else {
        res = (~(input.at(0)->res & input.at(1)->res)) & 0x1;
    }
    executed = true;
    executedQueue->push(this);
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

bool LogicCellNAND::Tick(bool reset) {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
