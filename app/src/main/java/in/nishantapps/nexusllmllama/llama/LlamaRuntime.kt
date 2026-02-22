package `in`.nishantapps.nexusllmllama.llama

class LlamaRuntime {
    private val bridge = NativeBridge()

    fun createSession(
        modelPtr: Long,
        ctxPtr: Long,
        systemInstruction: String,
        sampler: LlamaSamplerParams,
        wrapper: BuiltInWrapper = BuiltInWrapper.LLAMA3
    ): LlamaSession {
        val s = LlamaSession(
            bridge = bridge,
            modelPtr = modelPtr,
            ctxPtr = ctxPtr,
            systemPrompt = systemInstruction,
            defaultSampler = sampler
        )
        s.useWrapper(wrapper)
        return s
    }
}