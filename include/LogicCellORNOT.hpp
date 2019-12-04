//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLORNOT_HPP
#define IYOKAN_L2_LOGICCELLORNOT_HPP

#include "Logic.hpp"

class LogicCellORNOT : public Logic {
public:
    LogicCellORNOT(int id, int pri, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *ck);

    void Prepare();

    void Execute();

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick(bool reset);
};

#endif //IYOKAN_L2_LOGICCELLORNOT_HPP
