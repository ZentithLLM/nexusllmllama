#include <jni.h>
#include <string>
#include <vector>
#include "llama.h"
#include "common.h" // llama.cpp common utilities

extern "C" {

// Load Model
JNIEXPORT jlong JNICALL
Java_in_nishantapps_nexusllmllama_LlamaModel_nativeLoadModel(JNIEnv* env, jobject thiz, jstring path) {
    const char *model_path = env->GetStringUTFChars(path, 0);
    llama_model_params model_params = llama_model_default_params();
    model_params.use_mmap = true;
    llama_model* model = llama_load_model_from_file(model_path, model_params);
    env->ReleaseStringUTFChars(path, model_path);
    return reinterpret_cast<jlong>(model);
}

// Create Context
JNIEXPORT jlong JNICALL
Java_in_nishantapps_nexusllmllama_LlamaContext_nativeCreateContext(JNIEnv* env, jobject thiz, jlong model_ptr, jint n_ctx) {
    auto* model = reinterpret_cast<llama_model*>(model_ptr);
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = n_ctx;
    ctx_params.n_threads = 4;
    return reinterpret_cast<jlong>(llama_new_context_with_model(model, ctx_params));
}

// The Real Inference Loop (Temperature, Streaming, etc.)
JNIEXPORT void JNICALL
Java_in_nishantapps_nexusllmllama_LlamaChatSession_nativeInfer(
        JNIEnv* env, jobject thiz, jlong ctx_ptr, jstring prompt,
        jfloat temp, jfloat top_p, jobject callback) {

    auto* ctx = reinterpret_cast<llama_context*>(ctx_ptr);
    auto* model = llama_get_model(ctx);
    const char* prompt_str = env->GetStringUTFChars(prompt, 0);

    // 1. Tokenize input
    std::vector<llama_token> tokens = llama_tokenize(model, prompt_str, true, true);

    // 2. Setup Sampler (Temperature control)
    auto sparams = llama_sampler_chain_default_params();
    auto smpl = llama_sampler_chain_init(sparams);
    llama_sampler_chain_add(smpl, llama_sampler_init_top_p(top_p, 1));
    llama_sampler_chain_add(smpl, llama_sampler_init_temp(temp));
    llama_sampler_chain_add(smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

    // 3. Callback setup
    jclass cbClass = env->GetObjectClass(callback);
    jmethodID onTokenId = env->GetMethodID(cbClass, "onToken", "(Ljava/lang/String;)V");

    // 4. Generation Loop
    int n_cur = 0;
    while (n_cur < 512) { // Max tokens
        llama_decode(ctx, llama_batch_get_one(tokens.data(), tokens.size()));
        tokens.clear();

        const llama_token id = llama_sampler_sample(smpl, ctx, -1);
        if (llama_token_is_eog(model, id)) break;

        // Convert token to string
        char buf[128];
        int n = llama_token_to_piece(model, id, buf, sizeof(buf), 0, true);
        std::string piece(buf, n);

        // Stream back to Kotlin
        jstring jpiece = env->NewStringUTF(piece.c_str());
        env->CallVoidMethod(callback, onTokenId, jpiece);
        env->DeleteLocalRef(jpiece);

        tokens.push_back(id);
        n_cur++;
    }

    llama_sampler_free(smpl);
    env->ReleaseStringUTFChars(prompt, prompt_str);
}
}