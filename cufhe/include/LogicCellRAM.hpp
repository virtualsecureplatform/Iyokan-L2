//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLRAM_HPP
#define IYOKAN_L2_LOGICCELLRAM_HPP

#include "Logic.hpp"

class LogicCellRAM : public Logic {
public:
    LogicCellRAM(int id, int pri, bool isCipher);

    void Prepare();

    void Execute(cufhe::Stream stream, bool reset);

    void Execute(bool reset);

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick();

    //void SetCipher(std::shared_ptr<LweSample> val);

    void SetPlain(int val);

    //LweSample *GetCipher();

    int GetPlain();

private:
};

#endif  //IYOKAN_L2_LOGICCELLRAM_HPP
