#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <pthread.h>
#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

NetList netList("../ExUnitTest.json");

void *Execute(void *args) {
    while (netList.execute) {
        netList.Execute();
    }
    return NULL;
}

void *ExecuteAndManage(void *args) {
    while (netList.DepencyUpdate()) {
        netList.Execute();
    }
    netList.Tick();
    while (netList.DepencyUpdate()) {
        netList.Execute();
    }
    netList.execute = false;
    return NULL;
}

int main() {

    netList.ConvertJson();
    netList.PrepareTFHE();

    netList.Set("reset", 0);
    netList.Set("io_in_inA", 28);
    netList.Set("io_in_inB", 14);
    netList.Set("io_in_opcode", 0);
    netList.Set("io_in_pcOpcode", 1);
    netList.Set("io_enable", 1);
    netList.Set("io_flush", 0);

    netList.PrepareExecution();

    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    netList.execute = true;
    pthread_t thread_1, thread_2, thread_3, thread_4, thread_5, thread_6, thread_7, thread_8;
    pthread_create(&thread_1, NULL, ExecuteAndManage, NULL);
    pthread_create(&thread_2, NULL, Execute, NULL);
    pthread_create(&thread_3, NULL, Execute, NULL);
    pthread_create(&thread_4, NULL, Execute, NULL);
    pthread_create(&thread_5, NULL, Execute, NULL);
    pthread_create(&thread_6, NULL, Execute, NULL);
    pthread_create(&thread_7, NULL, Execute, NULL);
    pthread_create(&thread_8, NULL, Execute, NULL);
    pthread_join(thread_1, NULL);
    end = std::chrono::system_clock::now();

    double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                                      1000.0);
    printf("execution time %lf[ms]\n", time);
    std::cout << "Result:" << netList.Get("io_out_res") << std::endl;
}

