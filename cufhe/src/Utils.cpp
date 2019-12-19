// Include these two files for CPU computing.
#include <cufhe_gpu.cuh>
using namespace cufhe;

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include <iostream>
#include <memory>

#include "Utils.hpp"

std::shared_ptr<LweSample> make_lwe_sample(
    const TFheGateBootstrappingCloudKeySet& ckey)
{
    return std::shared_ptr<LweSample>{
        new_gate_bootstrapping_ciphertext(ckey.params),
        delete_gate_bootstrapping_ciphertext};
}

std::shared_ptr<PubKey> tfhe2cufhe(const TFheGateBootstrappingCloudKeySet& ckey)
{
    auto pubkey = std::make_shared<PubKey>();
    const LweBootstrappingKey* bk = ckey.bk;
    const LweKeySwitchKey* ksk = bk->ks;

    const int32_t n = bk->in_out_params->n;
    const int32_t N = bk->bk_params->tlwe_params->N;
    const int32_t k = bk->bk_params->tlwe_params->k;
    const int32_t l = bk->bk_params->l;

    // Check if the cuFHE params are equal to TFHE ones.
    auto cufhe_params = GetDefaultParam();
    assert(n == cufhe_params->lwe_n_);
    assert(N == cufhe_params->tlwe_n_);
    assert(k == cufhe_params->tlwe_k_);
    assert(l == cufhe_params->tgsw_decomp_size_);
    assert(ksk->t == cufhe_params->keyswitching_decomp_size_);
    assert(ksk->n == N * k);
    assert(ksk->basebit == cufhe_params->keyswitching_decomp_bits_);
    assert(ksk->base == (1 << ksk->basebit));
    assert(pubkey->ksk_->l() == ksk->t);
    assert(pubkey->ksk_->w() == ksk->basebit);
    assert(pubkey->ksk_->m() == ksk->n);
    assert(pubkey->bk_->SizeData() ==
           n * ((k + 1) * l) * (k + 1) * N * sizeof(Torus));
    assert(pubkey->ksk_->SizeData() ==
           ksk->n * ksk->t * ksk->base * Align512((n + 1) * sizeof(Torus)));

    // Read the bootstrapping key.
    for (int i = 0; i < n * ((k + 1) * l) * (k + 1) * N; i++) {
        int s = i % N;
        int r = (i / N) % (k + 1);
        int q = (i / N / (k + 1)) % ((k + 1) * l);
        int p = i / N / (k + 1) / ((k + 1) * l);
        pubkey->bk_->data()[i] = bk->bk[p].all_sample[q].a[r].coefsT[s];
    }

    // Read the key switch key.
    for (int p = 0; p < ksk->n; p++) {
        for (int q = 0; q < ksk->t; q++) {
            for (int r = 0; r < ksk->base; r++) {
                LweSample& src = ksk->ks[p][q][r];
                LWESample dst = pubkey->ksk_->ExtractLWESample(
                    pubkey->ksk_->GetLWESampleIndex(p, q, r));
                for (int s = 0; s < n; s++) {
                    dst.data()[s] = src.a[s];
                }
                dst.data()[n] = src.b;
            }
        }
    }

    return pubkey;
}

std::shared_ptr<Ctxt> tfhe2cufhe(const TFheGateBootstrappingCloudKeySet& ckey,
                                 const LweSample& ciphertext)
{
    const int32_t n = ckey.bk->in_out_params->n;

    // Check if the cuFHE params are equal to TFHE ones.
    auto cufhe_params = GetDefaultParam();
    assert(n == cufhe_params->lwe_n_);

    // Read the data.
    auto ctxt = std::make_shared<Ctxt>();
    for (int i = 0; i < n; i++) ctxt->lwe_sample_->data()[i] = ciphertext.a[i];
    ctxt->lwe_sample_->data()[n] = ciphertext.b;

    return std::move(ctxt);
}

std::vector<std::shared_ptr<Ctxt>> tfhe2cufhe(const TFheGateBootstrappingCloudKeySet& ckey,
                                                     std::vector<std::shared_ptr<LweSample>>& cipherArray){
    std::vector<std::shared_ptr<Ctxt>> cufheArray;
    for(std::shared_ptr<LweSample> cipherBit: cipherArray){
        cufheArray.push_back(tfhe2cufhe(ckey, *cipherBit.get()));
    }
    return cufheArray;
}

std::shared_ptr<LweSample> cufhe2tfhe(
    const TFheGateBootstrappingCloudKeySet& ckey, const Ctxt& ctxt)
{
    const int32_t n = ckey.bk->in_out_params->n;

    // Check if the cuFHE params are equal to TFHE ones.
    auto cufhe_params = GetDefaultParam();
    assert(n == cufhe_params->lwe_n_);

    // Write the data.
    std::shared_ptr<LweSample> ciphertext = make_lwe_sample(ckey);
    for (int i = 0; i < n; i++) ciphertext->a[i] = ctxt.lwe_sample_->data()[i];
    ciphertext->b = ctxt.lwe_sample_->data()[n];
    ciphertext->current_variance = 0;  // This is for debug, so doesn't matter.

    return ciphertext;
}

std::vector<std::shared_ptr<LweSample>> cufhe2tfhe(
    const TFheGateBootstrappingCloudKeySet& ckey, std::vector<std::shared_ptr<cufhe::Ctxt>>& cipherArray){

    std::vector<std::shared_ptr<LweSample>> cufheArray;
    for(auto cipherBit: cipherArray){
        cufheArray.push_back(cufhe2tfhe(ckey, *cipherBit.get()));
    }
    return cufheArray;
}

void test()
{
    /*
    // Get a cloud key.
    const TFheGateBootstrappingCloudKeySet& ckey = key->cloud;

    // Make cipher texts.
    std::shared_ptr<LweSample> lwesample0 = make_lwe_sample(ckey);
    std::shared_ptr<LweSample> lwesample1 = make_lwe_sample(ckey);
    int p0 = engine() % 2, p1 = engine() % 2, p2 = 1 - p0 * p1;
    bootsSymEncrypt(lwesample0.get(), p0, key.get());
    bootsSymEncrypt(lwesample1.get(), p1, key.get());

    // Convert TFHE values to cuFHE ones.
    std::shared_ptr<PubKey> pubkey = tfhe2cufhe(ckey);
    std::shared_ptr<Ctxt> ctxt0 = tfhe2cufhe(ckey, *lwesample0),
                          ctxt1 = tfhe2cufhe(ckey, *lwesample1),
                          ctxt2 = std::make_shared<Ctxt>();

    // NAND on cuFHE.
    Nand(*ctxt2, *ctxt0, *ctxt1, *pubkey);

    // Convert cuFHE values to TFHE ones.
    std::shared_ptr<LweSample> lwesample2 = cufhe2tfhe(ckey, *ctxt2);

    // Decrypt the result.
    // std::cerr << p0 << " nand " << p1 << " = " << p2 << std::endl;
    assert(bootsSymDecrypt(lwesample2.get(), key.get()) == p2);
     */
}
