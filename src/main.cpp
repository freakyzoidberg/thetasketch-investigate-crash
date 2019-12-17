#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <theta_sketch.hpp>
#include <theta_union.hpp>
#include <theta_intersection.hpp>

using namespace std;
using namespace datasketches;

#define LOGK_DEFAULT 15
#define SEED_DEFAULT 9001

theta_union make_union_sketch() {
    return theta_union::builder().set_lg_k(LOGK_DEFAULT).set_seed(SEED_DEFAULT).build();
}

update_theta_sketch make_update_sketch() {
    auto sketch = update_theta_sketch::builder().set_lg_k(LOGK_DEFAULT).set_seed(SEED_DEFAULT).build();
    for (uint64_t i = 0; i < 9999999LL; i++) {
        sketch.update(i);
    }
    return sketch;
}

void pollute_mem(int n) {
    for (int i = 0; i < n; i++) {
        char *p = new char[1024 * 1024];
        for (int k = 0; i < 1024; i++) {
            p[1024 * k] = 'a';
        }
    }
    std::cout << "Memory polluted" << std::endl;
}


void fromHex(
        const std::string &in,
        void *const data
) {
    size_t length = in.length();
    auto *byteData = reinterpret_cast<unsigned char *>(data);

    std::stringstream hexStringStream;
    hexStringStream >> std::hex;
    for (size_t strIndex = 0, dataIndex = 0; strIndex < length; ++dataIndex) {
        // Read out and convert the string two characters at a time
        const char tmpStr[3] = {in[strIndex++], in[strIndex++], 0};

        // Reset and fill the string stream
        hexStringStream.clear();
        hexStringStream.str(tmpStr);

        // Do the conversion
        int tmpValue = 0;
        hexStringStream >> tmpValue;
        byteData[dataIndex] = static_cast<unsigned char>(tmpValue);
    }
}

int main(int argc, char **argv) {
    std::cout << "Starting" << std::endl;


    auto intersection = theta_intersection(SEED_DEFAULT);

    // First pass (the empty sketch from initiate)
    auto initial_data = make_union_sketch().get_result().serialize();
    std::cout << initial_data.second << std::endl;
    auto initial_sketch = compact_theta_sketch::deserialize(initial_data.first.get(),
                                                            initial_data.second,
                                                            SEED_DEFAULT);
    intersection.update(initial_sketch);

    auto data2 = intersection.get_result().serialize();

    // Read sketches from parquet extract
    std::ifstream file("/tmp/thetasketch/1.0.0/project/thirdparty/parquet/sketches.txt");
    std::string str;
    while (std::getline(file, str)) {
        // Add them in the intersection
        unsigned char bytes[str.length() / 2];
        fromHex(str, bytes);
        auto second_sketch = compact_theta_sketch::deserialize(bytes,
                                                               str.length()/2,
                                                               SEED_DEFAULT);
        intersection.update(second_sketch);
    }

    // force serialization of intersection
    auto data = intersection.get_result().serialize();

    std::cout << "Done: " << intersection.get_result().get_theta64() << std::endl;
}
