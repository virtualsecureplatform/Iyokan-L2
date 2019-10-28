#include <iostream>
#include <queue>
#include <vector>
#include <typeinfo>

#include "NetList.hpp"

int main() {
    NetList netList("../test.json");
    netList.ConvertJson();
    netList.Set("reset", 0);
    netList.Set("io_in_inA", 16);
    netList.Set("io_in_inB", 4);
    netList.Set("io_in_opcode", 0);
    netList.Set("io_enable", 1);
    netList.Set("io_flush", 0);

    netList.PrepareExecution();
    netList.Tick();
    netList.Tick();
    std::cout << "Result:" << netList.Get("io_out_res") << std::endl;
}
