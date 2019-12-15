//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLROM_HPP
#define IYOKAN_L2_LOGICCELLROM_HPP

#include "Logic.hpp"

class LogicCellROM : public Logic {
public:
    LogicCellROM(int id, int pri, bool isCipher);

    void Prepare();

    void Execute(cufhe::Stream stream, bool reset);

    void Execute(bool reset);

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick();

    void SetCipher(cufhe::Ctxt *val);

    cufhe::Ctxt *GetCipher();

    void SetPlain(int val);

private:
};

#endif  //IYOKAN_L2_LOGICCELLROM_HPP
