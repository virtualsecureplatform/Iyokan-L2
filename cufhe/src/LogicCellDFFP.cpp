#include "LogicCellDFFP.hpp"

LogicCellDFFP::LogicCellDFFP(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "DFFP";
}

void LogicCellDFFP::Prepare() {
    if (input.size() != 1) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellDFFP::Execute(cufhe::Stream stream, bool reset) {
    if (!reset) {
        cufhe::Copy(*value, *input.at(0)->value, stream);
    }
    executed = true;
}

void LogicCellDFFP::Execute(bool reset) {
    if (!reset) {
        res = input.at(0)->res;
    }
    executed = true;
}

bool LogicCellDFFP::NoticeInputReady() {
    return false;
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[DFFP] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
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

bool LogicCellDFFP::Tick() {
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
