package `in`.nishantapps.nexusllmllama.llama

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow
import kotlinx.coroutines.launch

/**
 * Public API surface. Constructor is internal so NativeBridge is never exposed publicly.
 */
class LlamaSession internal constructor(
    private val bridge: NativeBridge,
    private val modelPtr: Long,
    private val ctxPtr: Long,
    private var systemPrompt: String,
    private var defaultSampler: LlamaSamplerParams,
    private var chatWrapper: ChatWrapper = BuiltInChatWrappers.Llama3
) {
    private val history = mutableListOf<LlamaMessage>()

    fun setSystemInstruction(v: String) { systemPrompt = v }
    fun addUser(text: String) { history += LlamaMessage(LlamaRole.USER, text) }
    fun addAssistant(text: String) { history += LlamaMessage(LlamaRole.ASSISTANT, text) }
    fun clearHistory() = history.clear()

    /** Select built-in wrapper (similar concept to node-llama-cpp chatWrappers) */
    fun useWrapper(wrapper: BuiltInWrapper) {
        chatWrapper = when (wrapper) {
            BuiltInWrapper.LLAMA3 -> BuiltInChatWrappers.Llama3
            BuiltInWrapper.CHAT_ML -> BuiltInChatWrappers.ChatMl
            BuiltInWrapper.GEMMA -> BuiltInChatWrappers.Gemma
            BuiltInWrapper.QWEN -> BuiltInChatWrappers.Qwen
            BuiltInWrapper.DEEPSEEK -> BuiltInChatWrappers.DeepSeek
            BuiltInWrapper.ALPACA -> BuiltInChatWrappers.Alpaca
            BuiltInWrapper.GENERAL -> BuiltInChatWrappers.General
        }
    }

    /** User-defined custom wrapper */
    fun useCustomWrapper(wrapper: ChatWrapper) {
        chatWrapper = wrapper
    }

    fun stream(sampler: LlamaSamplerParams = defaultSampler, includeThinking: Boolean = true): Flow<LlamaGenerationChunk> = callbackFlow {
        val prompt = PromptBuilder.render(systemPrompt, history, chatWrapper)

        val generationId = bridge.nativeStartGeneration(
            modelPtr = modelPtr,
            ctxPtr = ctxPtr,
            prompt = prompt,
            temperature = sampler.temperature,
            topK = sampler.topK,
            topP = sampler.topP,
            minP = sampler.minP,
            typicalP = sampler.typicalP,
            repeatPenalty = sampler.repeatPenalty,
            frequencyPenalty = sampler.frequencyPenalty,
            presencePenalty = sampler.presencePenalty,
            seed = sampler.seed,
            maxNewTokens = sampler.maxNewTokens,
            stop = sampler.stop.toTypedArray()
        )

        val job = launch(Dispatchers.Default) {
            while (true) {
                val chunk = bridge.nativePollGeneration(generationId)
                val parsed = if (includeThinking) chunk else LlamaReasoningParser.hideThinking(chunk)
                trySend(parsed)
                if (parsed.isFinished) break
            }
        }

        awaitClose {
            job.cancel()
            bridge.nativeCancelGeneration(generationId)
            bridge.nativeDisposeGeneration(generationId)
        }
    }
}