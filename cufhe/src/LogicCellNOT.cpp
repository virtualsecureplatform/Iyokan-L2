#include "LogicCellNOT.hpp"

LogicCellNOT::LogicCellNOT(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "NOT";
}

void LogicCellNOT::Prepare() {
    if (input.size() != 1) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellNOT::Execute(cufhe::Stream stream, bool reset) {
    cufhe::gNot(*value, *input.at(0)->value, stream);
    executed = true;
}

void LogicCellNOT::Execute(bool reset) {
    res = (~input.at(0)->res) & 0x1;
    executed = true;
}

bool LogicCellNOT::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[NOT] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellNOT::AddInput(Logic *logic) {
    if (input.size() > 0) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellNOT::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellNOT::Tick() {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
