#include "LogicCellANDNOT.hpp"

LogicCellANDNOT::LogicCellANDNOT(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "ANDNOT";
}

void LogicCellANDNOT::Prepare() {
    if (input.size() != 2) {
        throw std::runtime_error("Input is not assigned ANDNOT");
    }
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    InputCount = input.size();
    ReadyInputCount = 0;
}

void LogicCellANDNOT::Execute(cufhe::Stream stream, bool reset) {
    cufhe::AndYN(*value, *input.at(0)->value, *input.at(1)->value, stream);
    executed = true;
}

void LogicCellANDNOT::Execute(bool reset) {
    res = (input.at(0)->res & (~input.at(1)->res)) & 0x1;
    executed = true;
}

bool LogicCellANDNOT::NoticeInputReady() {
    ReadyInputCount++;
    if (ReadyInputCount > InputCount) {
        throw std::runtime_error("[ANDNOT] ReadyInputCount is invalid");
    }
    return InputCount == ReadyInputCount;
}

void LogicCellANDNOT::AddInput(Logic *logic) {
    if (input.size() > 1) {
        throw std::runtime_error("Input is already assigned");
    }
    input.push_back(logic);
}

void LogicCellANDNOT::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicCellANDNOT::Tick() {
    executable = false;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
