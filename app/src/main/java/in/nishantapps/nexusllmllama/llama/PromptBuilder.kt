package `in`.nishantapps.nexusllmllama.llama

object PromptBuilder {
    fun render(system: String, history: List<LlamaMessage>, wrapper: ChatWrapper): String {
        return wrapper.render(system, history)
    }
}