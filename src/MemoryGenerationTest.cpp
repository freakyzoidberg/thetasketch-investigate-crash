//
// Created by Pierre Lacave on 19/12/2019.
//

#include "MemoryGenerationTest.h"
#include "common.h"
#include <iostream>
#include <random>
#include <theta_intersection.hpp>

using namespace datasketches;

void MemoryGenerationTest::run() {
    auto intersection = theta_intersection(SEED_DEFAULT);
    for (int i = 0; i < 10; i++) {
        auto sketch = this->make_update_sketch();
        auto ser = sketch.compact().serialize();
        auto second_sketch = compact_theta_sketch::deserialize(ser.first.get(),
                                                               ser.second,
                                                               SEED_DEFAULT);
        intersection.update(second_sketch);
    }

    std::cout << "Done: " << intersection.get_result().get_estimate() << std::endl;
}

update_theta_sketch MemoryGenerationTest::make_update_sketch() {
    auto sketch = update_theta_sketch::builder().set_lg_k(LOGK_DEFAULT).set_seed(SEED_DEFAULT).build();
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 999999);

    for (uint64_t i = 0; i < 999999LL; i++) {
        sketch.update(dist(rng));
    }
    return sketch;
}
