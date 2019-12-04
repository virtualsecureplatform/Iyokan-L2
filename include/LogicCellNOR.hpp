//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLNOR_HPP
#define IYOKAN_L2_LOGICCELLNOR_HPP

#include "Logic.hpp"

class LogicCellNOR : public Logic {
public:
    LogicCellNOR(int id, int pri, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *ck);

    void Prepare();

    void Execute();

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick(bool reset);
};

#endif //IYOKAN_L2_LOGICCELLNOR_HPP
