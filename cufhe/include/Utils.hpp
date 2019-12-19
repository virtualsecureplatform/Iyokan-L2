//
// Created by naoki on 19/11/05.
//

#ifndef IYOKAN_L2_UTILS_HPP
#define IYOKAN_L2_UTILS_HPP
#include "cufhe_gpu.cuh"
#include "cufhe.h"
#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

std::shared_ptr<cufhe::PubKey> tfhe2cufhe(const TFheGateBootstrappingCloudKeySet& ckey);

std::shared_ptr<cufhe::Ctxt> tfhe2cufhe(const TFheGateBootstrappingCloudKeySet& ckey,
                                 const LweSample& ciphertext);

std::vector<std::shared_ptr<cufhe::Ctxt>> tfhe2cufhe(const TFheGateBootstrappingCloudKeySet& ckey,
    std::vector<std::shared_ptr<LweSample>>& cipherArray);

std::shared_ptr<LweSample> cufhe2tfhe(
    const TFheGateBootstrappingCloudKeySet& ckey, const cufhe::Ctxt& ctxt);

std::vector<std::shared_ptr<LweSample>> cufhe2tfhe(
    const TFheGateBootstrappingCloudKeySet& ckey, std::vector<std::shared_ptr<cufhe::Ctxt>>& cipherArray);

#endif  //IYOKAN_L2_UTILS_HPP
