#include "generation_worker.h"

#include <algorithm>

static bool decode_tokens(llama_context* ctx, const std::vector<llama_token>& tokens, std::string& error) {
    if (tokens.empty()) return true;

    llama_batch batch = llama_batch_init((int)tokens.size(), 0, 1);
    for (size_t i = 0; i < tokens.size(); ++i) {
        batch.token[i] = tokens[i];
        batch.pos[i] = (llama_pos)i;
        batch.n_seq_id[i] = 1;
        batch.seq_id[i][0] = 0;
        batch.logits[i] = (i == tokens.size() - 1);
    }

    const int rc = llama_decode(ctx, batch);
    llama_batch_free(batch);

    if (rc != 0) {
        error = "llama_decode failed with code " + std::to_string(rc);
        return false;
    }
    return true;
}

static bool tokenize_prompt(
        const llama_vocab* vocab,
        const std::string& prompt,
        std::vector<llama_token>& out,
        std::string& error
) {
    int n = llama_tokenize(vocab, prompt.c_str(), (int)prompt.size(), nullptr, 0, true, true);

    // Some llama.cpp versions return negative required length for probe calls.
    if (n < 0) n = -n;

    if (n <= 0) {
        error = "Failed to tokenize prompt";
        return false;
    }

    out.resize((size_t)n);
    const int written = llama_tokenize(vocab, prompt.c_str(), (int)prompt.size(), out.data(), n, true, true);
    if (written < 0) {
        error = "Failed to output prompt tokens";
        return false;
    }

    out.resize((size_t)written);
    return !out.empty();
}

bool generation_is_stop_hit(const std::string& full_text, const std::vector<std::string>& stop) {
    for (const auto& s : stop) {
        if (!s.empty() && full_text.size() >= s.size()) {
            if (full_text.rfind(s) == full_text.size() - s.size())
                return true;
        }
    }
    return false;
}

bool generation_init(
        llama_model* model,
        llama_context* ctx,
        const GenerationConfig& config,
        GenerationState& state,
        std::string& error
) {
    state.finished = false;
    state.finish_reason.clear();
    state.full_text.clear();
    state.pending_token.clear();
    state.generated_tokens.clear();
    state.recent_tokens.clear();

    const llama_vocab* vocab = llama_model_get_vocab(model);
    if (vocab == nullptr) {
        error = "Model vocab is null";
        return false;
    }

    if (!tokenize_prompt(vocab, config.prompt, state.prompt_tokens, error))
        return false;

    if (!decode_tokens(ctx, state.prompt_tokens, error))
        return false;

    state.recent_tokens = state.prompt_tokens;
    return true;
}

bool generation_step(
        llama_model* model,
        llama_context* ctx,
        const GenerationConfig& config,
        GenerationState& state,
        std::string& error
) {
    if (state.cancelled.load()) {
        state.finished = true;
        state.finish_reason = "cancelled";
        return true;
    }

    if ((int)state.generated_tokens.size() >= config.max_new_tokens) {
        state.finished = true;
        state.finish_reason = "max_tokens";
        return true;
    }

    const llama_vocab* vocab = llama_model_get_vocab(model);
    const llama_token next = sample_next_token(
            ctx,
            config.sampling,
            state.recent_tokens.empty() ? nullptr : state.recent_tokens.data(),
            state.recent_tokens.size()
    );

    if (next == llama_vocab_eos(vocab)) {
        state.finished = true;
        state.finish_reason = "eos";
        return true;
    }

    state.generated_tokens.push_back(next);
    state.recent_tokens.push_back(next);
    if (state.recent_tokens.size() > 128)
        state.recent_tokens.erase(state.recent_tokens.begin());

    std::string token_text(32, '\0');
    int n = llama_token_to_piece(vocab, next, token_text.data(), (int)token_text.size(), 0, true);
    while (n < 0) {
        token_text.resize(token_text.size() * 2);
        n = llama_token_to_piece(vocab, next, token_text.data(), (int)token_text.size(), 0, true);
    }
    token_text.resize((size_t)n);

    state.pending_token = token_text;
    state.full_text += token_text;

    if (generation_is_stop_hit(state.full_text, config.stop)) {
        state.finished = true;
        state.finish_reason = "stop";
        return true;
    }

    std::vector<llama_token> one{next};
    if (!decode_tokens(ctx, one, error))
        return false;

    return true;
}