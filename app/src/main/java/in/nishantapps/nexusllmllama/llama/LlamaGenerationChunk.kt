package `in`.nishantapps.nexusllmllama.llama

data class LlamaGenerationChunk(
    val tokenText: String,
    val fullText: String,
    val isFinished: Boolean,
    val finishReason: String? = null,
    val reasoningText: String? = null
)