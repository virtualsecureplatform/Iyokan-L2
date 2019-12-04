//
// Created by naoki on 19/10/27.
//

#ifndef IYOKAN_L2_LOGICCELLDFFP_HPP
#define IYOKAN_L2_LOGICCELLDFFP_HPP

#include "Logic.hpp"

class LogicCellDFFP : public Logic {
public:
    LogicCellDFFP(int id, int pri, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *ck);

    LogicCellDFFP(int id, int pri, tbb::concurrent_queue<Logic *> *queue);

    void Prepare();

    void Execute();

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick(bool reset);
};

#endif //IYOKAN_L2_LOGICCELLDFFP_HPP
