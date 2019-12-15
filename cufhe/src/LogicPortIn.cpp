#include "LogicPortIn.hpp"

LogicPortIn::LogicPortIn(
    int id,
    int pri,
    bool isCipher) : Logic(id, pri, isCipher) {
    Type = "INPUT";
}

void LogicPortIn::Prepare() {
    if (output.size() == 0) {
        throw std::runtime_error("Output is not assigned");
    }

    executable = true;
}

void LogicPortIn::Execute(cufhe::Stream stream, bool reset) {
    executed = true;
}

void LogicPortIn::Execute(bool reset) {
    executed = true;
}

bool LogicPortIn::NoticeInputReady() {
    return false;
}

void LogicPortIn::SetCipher(cufhe::Ctxt *val) {
    cufhe::Copy(*value, *val);
    cufhe::Synchronize();
}

void LogicPortIn::SetPlain(int val) {
    res = val & 0x1;
}

void LogicPortIn::AddInput(Logic *logic) {
}

void LogicPortIn::AddOutput(Logic *logic) {
    output.push_back(logic);
}

bool LogicPortIn::Tick() {
    executable = true;
    executed = false;
    ReadyInputCount = 0;
    return executable;
}
