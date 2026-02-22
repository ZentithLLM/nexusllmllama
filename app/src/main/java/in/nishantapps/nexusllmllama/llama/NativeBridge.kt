package `in`.nishantapps.nexusllmllama.llama

internal class NativeBridge {
    companion object {
        init { System.loadLibrary("nexus_llama_jni") }
    }

    external fun nativeCreateRuntime(gpuMode: Int, maxThreads: Int, logLevel: Int): Long
    external fun nativeDisposeRuntime(runtimePtr: Long)

    external fun nativeLoadModel(
        runtimePtr: Long,
        modelPath: String,
        nGpuLayers: Int,
        useMmap: Boolean,
        useMlock: Boolean
    ): Long
    external fun nativeDisposeModel(modelPtr: Long)

    external fun nativeCreateContext(modelPtr: Long, contextSize: Int, threads: Int): Long
    external fun nativeDisposeContext(ctxPtr: Long)

    external fun nativeStartGeneration(
        modelPtr: Long,
        ctxPtr: Long,
        prompt: String,
        temperature: Float,
        topK: Int,
        topP: Float,
        minP: Float,
        typicalP: Float,
        repeatPenalty: Float,
        frequencyPenalty: Float,
        presencePenalty: Float,
        seed: Int,
        maxNewTokens: Int,
        stop: Array<String>
    ): Long

    external fun nativePollGeneration(generationId: Long): LlamaGenerationChunk
    external fun nativeCancelGeneration(generationId: Long)
    external fun nativeDisposeGeneration(generationId: Long)
}