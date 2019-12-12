//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLDFFP_HPP
#define IYOKAN_L2_LOGICCELLDFFP_HPP

#include "Logic.hpp"

class LogicCellDFFP : public Logic {
public:
    LogicCellDFFP(int id, int pri, bool isCipher);

    void Prepare();

    void Execute(cufhe::Stream stream, bool reset);

    void Execute(bool reset);

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick();
};

#endif  //IYOKAN_L2_LOGICCELLDFFP_HPP
