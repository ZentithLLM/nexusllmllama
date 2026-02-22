#pragma once

#include <string>
#include <vector>
#include <atomic>

#include "llama.h"
#include "sampling.h"

struct GenerationConfig {
    std::string prompt;
    int max_new_tokens = 256;
    std::vector<std::string> stop;
    SamplingParams sampling;
};

struct GenerationState {
    std::atomic<bool> cancelled{false};
    bool finished = false;
    std::string finish_reason = "";
    std::string full_text;
    std::string pending_token;

    std::vector<llama_token> prompt_tokens;
    std::vector<llama_token> generated_tokens;
    std::vector<llama_token> recent_tokens;
};

bool generation_init(
        llama_model* model,
        llama_context* ctx,
        const GenerationConfig& config,
        GenerationState& state,
        std::string& error
);

bool generation_step(
        llama_model* model,
        llama_context* ctx,
        const GenerationConfig& config,
        GenerationState& state,
        std::string& error
);

bool generation_is_stop_hit(const std::string& full_text, const std::vector<std::string>& stop);