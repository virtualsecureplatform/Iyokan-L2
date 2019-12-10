//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLROM_HPP
#define IYOKAN_L2_LOGICCELLROM_HPP

#include "Logic.hpp"

class LogicCellROM : public Logic {
public:
    LogicCellROM(int id, int pri, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *ck);

    LogicCellROM(int id, int pri, tbb::concurrent_queue<Logic *> *queue);

    void Prepare();

    void Execute();

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick(bool reset);

    void SetCipher(std::shared_ptr<LweSample> val);

    void SetPlain(int val);

private:
};

#endif  //IYOKAN_L2_LOGICCELLROM_HPP
