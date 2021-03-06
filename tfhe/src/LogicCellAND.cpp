#include "LogicCellAND.hpp"

LogicCellAND::LogicCellAND(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue,
    const TFheGateBootstrappingCloudKeySet *ck) : Logic(id, pri, queue, ck) {
    Type = "AND";
}

LogicCellAND::LogicCellAND(
    int id,
    int pri,
    tbb::concurrent_queue<Logic *> *queue) : Logic(id, pri, queue) {
    Type = "AND";
}

void LogicCellAND::Prepare() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellAND::Execute() {
    if (cipher) {
        bootsAND(value, input.at(0)->value, input.at(1)->value, key);
    } else {
        res = input.at(0)->res & input.at(1)->res;
    }
    executed = true;
    executedQueue->push(this);
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

bool LogicCellAND::Tick(bool reset) {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
