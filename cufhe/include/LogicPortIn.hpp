//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICPORTIN_HPP
#define IYOKAN_L2_LOGICPORTIN_HPP

#include "Logic.hpp"

class LogicPortIn : public Logic {
public:
    LogicPortIn(int id, int pri, bool isCipher);

    void Prepare();

    void Execute(cufhe::Stream stream, bool reset);

    void Execute(bool reset);

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick();

    void SetCipher(cufhe::Ctxt *val);

    void SetPlain(int val);

private:
};

#endif  //IYOKAN_L2_LOGICPORTIN_HPP
