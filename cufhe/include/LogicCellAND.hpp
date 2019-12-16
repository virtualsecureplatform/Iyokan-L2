#ifndef IYOKAN_L2_LOGICCELLAND_HPP
#define IYOKAN_L2_LOGICCELLAND_HPP

#include "Logic.hpp"

class LogicCellAND : public Logic {
public:
    LogicCellAND(int id, int pri, bool isCipher);

    void Prepare();

    void Execute(cufhe::Stream stream, bool reset);

    void Execute(bool reset);

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick();
};

#endif  //IYOKAN_L2_LOGICCELLAND_HPP
