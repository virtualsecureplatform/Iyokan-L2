#ifndef IYOKAN_L2_LOGICCELLAND_HPP
#define IYOKAN_L2_LOGICCELLAND_HPP

#include "Logic.hpp"

class LogicCellAND : public Logic {
public:

    LogicCellAND(int id, int pri, tbb::concurrent_queue<Logic *> *queue, const TFheGateBootstrappingCloudKeySet *ck);

    void Prepare();

    void Execute();

    bool NoticeInputReady();

    void AddInput(Logic *logic);

    void AddOutput(Logic *logic);

    bool Tick(bool reset);

};

#endif //IYOKAN_L2_LOGICCELLAND_HPP
