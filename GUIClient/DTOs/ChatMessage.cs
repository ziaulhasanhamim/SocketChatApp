namespace GUIClient.DTOs;

public enum ChatMessageType {
    Broadcast, Private, ServerMessage
}

public record ChatMessage(string? Sender, string Content, ChatMessageType Type);