//
// Created by Pierre Lacave on 19/12/2019.
//

#ifndef THETA_CLIENT_1_0_0_SKETCHFROMTEXTTEST_H
#define THETA_CLIENT_1_0_0_SKETCHFROMTEXTTEST_H

#include <iostream>

class SketchFromTextTest {
public:
    void run(int argc, char **argvs);
private:
    void fromHex(const std::string &in, void *const data);
};

#endif //THETA_CLIENT_1_0_0_SKETCHFROMTEXTTEST_H
