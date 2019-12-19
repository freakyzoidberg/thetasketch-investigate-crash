//
// Created by Pierre Lacave on 19/12/2019.
//

#ifndef THETA_CLIENT_1_0_0_MEMORYGENERATIONTEST_H
#define THETA_CLIENT_1_0_0_MEMORYGENERATIONTEST_H

#include <theta_sketch.hpp>

class MemoryGenerationTest {
public:
    void run();
private:
    datasketches::update_theta_sketch make_update_sketch();
};





#endif //THETA_CLIENT_1_0_0_MEMORYGENERATIONTEST_H
