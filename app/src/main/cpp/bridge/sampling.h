#pragma once

#include "llama.h"

struct SamplingParams {
    float temperature = 0.7f;
    int top_k = 40;
    float top_p = 0.95f;
    float min_p = 0.05f;
    float typical_p = 1.0f;
    float repeat_penalty = 1.1f;
    float frequency_penalty = 0.0f;
    float presence_penalty = 0.0f;
    int seed = 0;
};

inline llama_token sample_next_token(
        llama_context* ctx,
        const SamplingParams& p,
        const llama_token* last_tokens,
        size_t last_tokens_count
) {
    // Current llama.cpp uses sampler-chain API instead of legacy llama_sample_* helpers.
    llama_sampler_chain_params chain_params = llama_sampler_chain_default_params();
    llama_sampler* chain = llama_sampler_chain_init(chain_params);

    if (chain == nullptr)
        return 0;

    // penalties first (history-aware)
    if (last_tokens != nullptr && last_tokens_count > 0) {
        llama_sampler_chain_add(chain, llama_sampler_init_penalties(
                (int32_t)last_tokens_count,
                p.repeat_penalty,
                p.frequency_penalty,
                p.presence_penalty
        ));
    }

    // distribution shaping
    if (p.top_k > 0)
        llama_sampler_chain_add(chain, llama_sampler_init_top_k(p.top_k));

    if (p.typical_p > 0.0f && p.typical_p < 1.0f)
        llama_sampler_chain_add(chain, llama_sampler_init_typical(p.typical_p, 1));

    if (p.top_p > 0.0f && p.top_p < 1.0f)
        llama_sampler_chain_add(chain, llama_sampler_init_top_p(p.top_p, 1));

    if (p.min_p > 0.0f)
        llama_sampler_chain_add(chain, llama_sampler_init_min_p(p.min_p, 1));

    llama_token tok;
    if (p.temperature <= 0.0f) {
        llama_sampler_chain_add(chain, llama_sampler_init_greedy());
        tok = llama_sampler_sample(chain, ctx, -1);
    } else {
        llama_sampler_chain_add(chain, llama_sampler_init_temp(p.temperature));
        llama_sampler_chain_add(chain, llama_sampler_init_dist(p.seed));
        tok = llama_sampler_sample(chain, ctx, -1);
    }

    llama_sampler_free(chain);
    return tok;
}