#include "LogicCellMUX.hpp"

LogicCellMUX::LogicCellMUX(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "MUX";
}

void LogicCellMUX::Prepare() {
    if (input.size() != 3) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellMUX::Execute(cufhe::Stream stream, bool reset) {
    cufhe::Mux(*value, *input.at(2)->value, *input.at(1)->value, *input.at(0)->value, stream);
    executed = true;
}

void LogicCellMUX::Execute(bool reset) {
    if (input.at(2)->res == 0) {
        res = input.at(0)->res;
    } else if (input.at(2)->res == 1) {
        res = input.at(1)->res;
    } else {
        throw std::runtime_error("invalid select signal");
    }
    executed = true;
}

bool LogicCellMUX::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[MUX] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellMUX::AddInput(Logic *logic) {
    if (input.size() > 2) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellMUX::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellMUX::Tick() {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
