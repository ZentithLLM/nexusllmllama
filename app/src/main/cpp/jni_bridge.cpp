#include <jni.h>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>

#include "llama.h"

struct RuntimeHandle {
    bool initialized = false;
};

struct ModelHandle {
    llama_model* model = nullptr;
};

struct ContextHandle {
    llama_context* ctx = nullptr;
};

struct GenerationHandle {
    std::atomic<bool> cancelled{false};
    std::string full;
    bool finished = false;
};

static std::mutex g_genMutex;
static std::unordered_map<long, GenerationHandle*> g_generations;
static long g_nextGenerationId = 1;

static jlong toJLong(void* p) { return reinterpret_cast<jlong>(p); }
static void* fromJLong(jlong p) { return reinterpret_cast<void*>(p); }

extern "C" JNIEXPORT jlong JNICALL
Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativeCreateRuntime(JNIEnv*, jobject, jint, jint, jint) {
    auto* r = new RuntimeHandle();
    llama_backend_init();
    r->initialized = true;
    return toJLong(r);
}

extern "C" JNIEXPORT void JNICALL
Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativeDisposeRuntime(JNIEnv*, jobject, jlong runtimePtr) {
auto* r = static_cast<RuntimeHandle*>(fromJLong(runtimePtr));
if (!r) return;
if (r->initialized) llama_backend_free();
delete r;
}

extern "C" JNIEXPORT jlong JNICALL
        Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativeLoadModel(
        JNIEnv* env,
        jobject,
        jlong,
        jstring modelPath,
jint nGpuLayers,
        jboolean,
        jboolean
) {
const char* chars = env->GetStringUTFChars(modelPath, nullptr);
std::string path(chars ? chars : "");
env->ReleaseStringUTFChars(modelPath, chars);

llama_model_params mp = llama_model_default_params();
mp.n_gpu_layers = nGpuLayers;

auto* m = new ModelHandle();
m->model = llama_model_load_from_file(path.c_str(), mp);
if (!m->model) {
delete m;
env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "Model load failed");
return 0;
}
return toJLong(m);
}

extern "C" JNIEXPORT void JNICALL
Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativeDisposeModel(JNIEnv*, jobject, jlong modelPtr) {
auto* m = static_cast<ModelHandle*>(fromJLong(modelPtr));
if (!m) return;
if (m->model) llama_model_free(m->model);
delete m;
}

extern "C" JNIEXPORT jlong JNICALL
        Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativeCreateContext(JNIEnv* env, jobject, jlong modelPtr, jint nCtx, jint threads) {
auto* m = static_cast<ModelHandle*>(fromJLong(modelPtr));
if (!m || !m->model) {
env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "Model missing");
return 0;
}

llama_context_params cp = llama_context_default_params();
cp.n_ctx = nCtx;
cp.n_threads = threads;
cp.n_threads_batch = threads;

auto* c = new ContextHandle();
c->ctx = llama_init_from_model(m->model, cp);
if (!c->ctx) {
delete c;
env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "Context create failed");
return 0;
}
return toJLong(c);
}

extern "C" JNIEXPORT void JNICALL
Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativeDisposeContext(JNIEnv*, jobject, jlong ctxPtr) {
auto* c = static_cast<ContextHandle*>(fromJLong(ctxPtr));
if (!c) return;
if (c->ctx) llama_free(c->ctx);
delete c;
}

extern "C" JNIEXPORT jlong JNICALL
Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativeStartGeneration(
        JNIEnv*, jobject, jlong, jlong, jstring,
        jfloat, jint, jfloat, jfloat, jfloat,
        jfloat, jfloat, jfloat, jint, jint, jobjectArray
) {
    std::lock_guard<std::mutex> lk(g_genMutex);
    long id = g_nextGenerationId++;
    g_generations[id] = new GenerationHandle();
    return static_cast<jlong>(id);
}

extern "C" JNIEXPORT jobject JNICALL
        Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativePollGeneration(JNIEnv* env, jobject, jlong generationId) {
// Implement token-by-token polling with llama decode loop.
// Return Kotlin LlamaGenerationChunk object each poll.
return nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativeCancelGeneration(JNIEnv*, jobject, jlong generationId) {
std::lock_guard<std::mutex> lk(g_genMutex);
auto it = g_generations.find((long)generationId);
if (it != g_generations.end()) it->second->cancelled.store(true);
}

extern "C" JNIEXPORT void JNICALL
Java_in_nishantapps_nexusllmllama_llama_NativeBridge_nativeDisposeGeneration(JNIEnv*, jobject, jlong generationId) {
std::lock_guard<std::mutex> lk(g_genMutex);
auto it = g_generations.find((long)generationId);
if (it == g_generations.end()) return;
delete it->second;
g_generations.erase(it);
}