package `in`.nishantapps.nexusllmllama.llama

fun interface ChatWrapper {
    fun render(system: String, history: List<LlamaMessage>): String
}
enum class BuiltInWrapper { LLAMA3, CHAT_ML, GEMMA, QWEN, DEEPSEEK, ALPACA, GENERAL }

object BuiltInChatWrappers {
    val Llama3 = ChatWrapper { system, history ->
        buildString {
            append("<|begin_of_text|><|start_header_id|>system<|end_header_id|>\n")
            append(system).append("<|eot_id|>")
            for (m in history) {
                val role = when (m.role) {
                    LlamaRole.USER -> "user"
                    LlamaRole.ASSISTANT -> "assistant"
                    LlamaRole.SYSTEM -> "system"
                    LlamaRole.THINKING -> "assistant"
                }
                append("<|start_header_id|>").append(role).append("<|end_header_id|>\n")
                append(m.content).append("<|eot_id|>")
            }
            append("<|start_header_id|>assistant<|end_header_id|>\n")
        }
    }

    val ChatMl = ChatWrapper { system, history ->
        buildString {
            append("<|system|>\n").append(system).append("\n")
            history.forEach { m ->
                val tag = when (m.role) {
                    LlamaRole.USER -> "user"
                    LlamaRole.ASSISTANT -> "assistant"
                    LlamaRole.SYSTEM -> "system"
                    LlamaRole.THINKING -> "assistant"
                }
                append("<|").append(tag).append("|>\n").append(m.content).append("\n")
            }
            append("<|assistant|>\n")
        }
    }

    val Gemma = ChatWrapper { system, history ->
        buildString {
            append("<start_of_turn>system\n").append(system).append("<end_of_turn>\n")
            history.forEach { m ->
                val role = if (m.role == LlamaRole.USER) "user" else "model"
                append("<start_of_turn>").append(role).append("\n")
                append(m.content).append("<end_of_turn>\n")
            }
            append("<start_of_turn>model\n")
        }
    }

    val Qwen = ChatMl
    val DeepSeek = ChatMl
    val Alpaca = ChatWrapper { system, history ->
        buildString {
            append("### Instruction:\n").append(system).append("\n")
            history.forEach { m ->
                val role = if (m.role == LlamaRole.USER) "User" else "Assistant"
                append("### ").append(role).append(":\n").append(m.content).append("\n")
            }
            append("### Assistant:\n")
        }
    }
    val General = ChatMl
}