#include <iostream>
#include <chrono>

#include "NetList.hpp"
#include <unistd.h>
#include "ExecManager.hpp"
#include "cufhe_gpu.cuh"

const std::string usageMsg =  "Usage: [-p] [-v] -c <cycle> -t <thread_num> -l <logic_file_name> -i <cipher_file_name> -o <result_file_name> [-s <secretKeyFile>]";
int main(int argc, char *argv[]) {
    int opt;
    opterr = 0;
    bool perfMode = false;
    bool verbose = true;
    int execCycle = 1;
    int threadNum = 0;
    std::string logicFile = "../../test/test-mux-4bit.json";
    std::string cipherFile = "";
    std::string resultFile = "";
    std::string secretKeyFile = "";
    bool testMode = false;
    while ((opt = getopt(argc, argv, "vpc:t:l:i:o:s:")) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 'p':
                perfMode = true;
                break;
            case 'c':
                execCycle = atoi(optarg);
                break;
            case 't':
                threadNum = atoi(optarg);
                break;
            case 'l':
                logicFile = optarg;
                break;
            case 'i':
                cipherFile = optarg;
                break;
            case 'o':
                resultFile = optarg;
                break;
            case 's':
                secretKeyFile = optarg;
                testMode = true;
                break;
            default:
                std::cout << usageMsg << std::endl;
                exit(1);
                break;
        }
    }
    /*
    if(execCycle != 0){
        std::cout << "ExecCycle:" << execCycle << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(threadNum != 0){
        std::cout << "ThreadNum:" << threadNum << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(logicFile != ""){
        std::cout << "LogicFile:" << logicFile << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(cipherFile != ""){
        std::cout << "CipherFile:" << cipherFile << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(resultFile != ""){
        std::cout << "ResultFile:" << resultFile << std::endl;
    }else{
        std::cout << usageMsg << std::endl;
        exit(1);
    }

    if(secretKeyFile != ""){
        std::cout << "SecretKeyFile:" << secretKeyFile << std::endl;
    }

    if(testMode){
        std::cout << "Running on TestMode" << std::endl;
    }
     */
    cufhe::PriKey *secretKey;
    cufhe::PubKey *cloudKey;
    if(!testMode){
        cudaSetDevice(0);
        cufhe::SetSeed();
        secretKey = new cufhe::PriKey;
        cloudKey = new cufhe::PubKey;
        cufhe::KeyGen(*cloudKey, *secretKey);
        cufhe::Initialize(*cloudKey);
    }

    ExecManager manager(threadNum, execCycle, verbose, !testMode);

    NetList netList(logicFile, verbose, !testMode);

    if(!testMode){
        cufhe::Ptxt A_0;
        cufhe::Ptxt B_0;
        A_0.message_ = 1;
        B_0.message_ = 0;

        cufhe::Ctxt cA_0;
        cufhe::Ctxt cB_0;

        Encrypt(cA_0, A_0, *secretKey);
        Encrypt(cB_0, B_0, *secretKey);

        std::vector<cufhe::Ctxt *> c_inA;
        c_inA.push_back(&cA_0);
        c_inA.push_back(&cB_0);
        c_inA.push_back(&cB_0);
        c_inA.push_back(&cB_0);

        std::vector<cufhe::Ctxt *> c_inB;
        c_inB.push_back(&cB_0);
        c_inB.push_back(&cA_0);
        c_inB.push_back(&cB_0);
        c_inB.push_back(&cB_0);

        std::vector<cufhe::Ctxt *> c_sel;
        c_sel.push_back(&cA_0);

        cufhe::Synchronize();
        /*
        netList.SetPortCipher("io_inA", c_inA);
        netList.SetPortCipher("io_inB", c_inB);
        netList.SetPortCipher("io_sel", c_sel);
         */
        netList.SetROMEncryptPlain(0, 0x0E018835, secretKey);
    }else{
        netList.SetPortPlain("io_inA", 1);
        netList.SetPortPlain("io_inB", 2);
        netList.SetPortPlain("io_sel", 0);
    }


    /*
     */
    /*
    netList.SetROMPlain(0, 0x15040035);
    netList.SetROMPlain(1, 0x000E0208);
    netList.SetRAMPlain(6, 0x2A);
    netList.SetRAMPlain(7, 0x2B);
     */

    manager.SetNetList(&netList);
    manager.Prepare();

    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    manager.Start();
    end = std::chrono::system_clock::now();

    double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                                      1000.0);
    if (perfMode) {
        printf("%d, %d, %lf, %d\n", threadNum, execCycle, time, manager.GetExecutedLogicNum());
    } else {
        if(!testMode){
            cufhe::Synchronize();
            uint32_t res = netList.GetROMDecryptCipher(0, secretKey);
            std::printf("ROM[0x01]: 0x%X\n", res);
            /*
            auto res = netList.GetPortCipher("io_out");
            cufhe::Synchronize();
            for(auto cbit : res){
                cufhe::Ptxt val;
                cufhe::Decrypt(val, *cbit, *secretKey);
                std::cout << val.message_ << std::endl;
            }
            cufhe::CleanUp();
             */
        }
        printf("Execution time %lf[ms]\n", time);
        netList.DebugOutput();
        manager.Stats();
    }
}
