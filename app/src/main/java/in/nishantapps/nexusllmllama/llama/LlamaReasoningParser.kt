package `in`.nishantapps.nexusllmllama.llama

object LlamaReasoningParser {
    private val thinkRegex = Regex("<think>(.*?)</think>", setOf(RegexOption.DOT_MATCHES_ALL))

    fun hideThinking(chunk: LlamaGenerationChunk): LlamaGenerationChunk {
        val cleaned = chunk.fullText.replace(thinkRegex, "")
        return chunk.copy(fullText = cleaned)
    }
}