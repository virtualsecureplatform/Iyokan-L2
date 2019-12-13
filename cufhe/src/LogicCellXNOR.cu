#include "LogicCellXNOR.hpp"

LogicCellXNOR::LogicCellXNOR(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "XNOR";
}

void LogicCellXNOR::Prepare() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellXNOR::Execute(cufhe::Stream stream, bool reset) {
    cufhe::Xnor(*value, *input.at(0)->value, *input.at(1)->value, stream);
    executed = true;
}

void LogicCellXNOR::Execute(bool reset) {
    res = (~(input.at(0)->res ^ input.at(1)->res)) & 0x1;
    executed = true;
}

bool LogicCellXNOR::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[XNOR] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellXNOR::AddInput(Logic *logic) {
    if (input.size() > 1) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellXNOR::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellXNOR::Tick() {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
