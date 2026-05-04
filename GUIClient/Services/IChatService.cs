using GUIClient.DTOs;

namespace GUIClient.Services;

public interface IChatService : IAsyncDisposable
{
    Task ConnectAsync(string username);
    Task SendMessageAsync(string message);
    Task SendPrivateMessageAsync(string recipient, string message);
    event Action<ChatMessage> OnMessageReceived;
    event Action<List<string>> OnUserListUpdated;
    event Action OnDisconnected;
}