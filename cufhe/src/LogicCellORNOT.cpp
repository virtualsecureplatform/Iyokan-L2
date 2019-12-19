#include "LogicCellORNOT.hpp"

LogicCellORNOT::LogicCellORNOT(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "ORNOT";
}

void LogicCellORNOT::Prepare() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned ORNOT");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellORNOT::Execute(cufhe::Stream stream, bool reset) {
    cufhe::gOrYN(*value, *input.at(0)->value, *input.at(1)->value, stream);
    executed = true;
}

void LogicCellORNOT::Execute(bool reset) {
    res = (input.at(0)->res | (~input.at(1)->res)) & 0x1;
    executed = true;
}

bool LogicCellORNOT::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[ORNOT] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellORNOT::AddInput(Logic *logic) {
    if (input.size() > 1) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellORNOT::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellORNOT::Tick() {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
