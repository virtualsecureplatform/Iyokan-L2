//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLXNOR_HPP
#define IYOKAN_L2_LOGICCELLXNOR_HPP

#include "Logic.hpp"

class LogicCellXNOR : public Logic {
public:
    LogicCellXNOR(int id, int pri, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *ck);

    void Prepare();

    void Execute();

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick(bool reset);
};

#endif //IYOKAN_L2_LOGICCELLAND_HPP
