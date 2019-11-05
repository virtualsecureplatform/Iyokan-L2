//
// Created by naoki on 19/11/05.
//

#ifndef IYOKAN_L2_UTILS_HPP
#define IYOKAN_L2_UTILS_HPP

class compare_f {
public:
    bool operator()(const Logic *u, const Logic *v) const {
        return u->priority < v->priority;
    }
};

#endif //IYOKAN_L2_UTILS_HPP
