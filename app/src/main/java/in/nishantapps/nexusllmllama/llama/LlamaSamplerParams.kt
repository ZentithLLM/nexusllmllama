package `in`.nishantapps.nexusllmllama.llama

data class LlamaSamplerParams(
    val temperature: Float = 0.7f,
    val topK: Int = 40,
    val topP: Float = 0.95f,
    val minP: Float = 0.05f,
    val typicalP: Float = 1.0f,
    val repeatPenalty: Float = 1.1f,
    val frequencyPenalty: Float = 0f,
    val presencePenalty: Float = 0f,
    val seed: Int = 0,
    val maxNewTokens: Int = 512,
    val stop: List<String> = emptyList()
)