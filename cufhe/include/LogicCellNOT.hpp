//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLNOT_HPP
#define IYOKAN_L2_LOGICCELLNOT_HPP

#include "Logic.hpp"

class LogicCellNOT : public Logic {
public:
    LogicCellNOT(int id, int pri, bool isCipher);

    void Prepare();

    void Execute(cufhe::Stream stream, bool reset);

    void Execute(bool reset);

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick();
};

#endif  //IYOKAN_L2_LOGICCELLNOT_HPP
