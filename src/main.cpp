#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <theta_sketch.hpp>
#include <theta_union.hpp>
#include <theta_intersection.hpp>

using namespace std;
using namespace datasketches;

#define LOGK_DEFAULT 12
#define SEED_DEFAULT 9001

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
    if (argc < 2) {
        std::cerr << "Usage: "
                  << argv[0]
                  << " <path to sketches.txt (see thirdparty/parquet/sketches.txt)>"
                  << std::endl;
        return 1;
    }

    auto intersection = theta_intersection(SEED_DEFAULT);
    // Read sketches from parquet extract
    std::ifstream file(argv[1]);
    std::string str;
    while (std::getline(file, str)) {
        std::cout << "add More" << std::endl;
        // Add them in the intersection
        unsigned char bytes[str.length() / 2];
        fromHex(str, bytes);
        auto second_sketch = compact_theta_sketch::deserialize(bytes,
                                                               str.length() / 2,
                                                               SEED_DEFAULT);
        intersection.update(second_sketch);
    }

    // force serialization of intersection
    auto data = intersection.get_result().serialize();


    std::cout << "Done: " << intersection.get_result().get_estimate() << std::endl;
}
