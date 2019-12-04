//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLXOR_HPP
#define IYOKAN_L2_LOGICCELLXOR_HPP

#include "Logic.hpp"

class LogicCellXOR : public Logic {
public:
    LogicCellXOR(int id, int pri, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *ck);

    LogicCellXOR(int id, int pri, tbb::concurrent_queue<Logic *> *queue);

    void Prepare();

    void Execute();

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick(bool reset);
};

#endif //IYOKAN_L2_LOGICCELLAND_HPP
