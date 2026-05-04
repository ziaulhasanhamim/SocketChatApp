using System.Buffers;
using System.IO.Pipelines;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using GUIClient.DTOs;

namespace GUIClient.Services;

public sealed class SocketChatService : IChatService
{
    readonly TcpClient tcpClient = new();
    NetworkStream? tcpStream;
    public event Action<ChatMessage> OnMessageReceived = delegate { };
    public event Action<List<string>> OnUserListUpdated = delegate { };
    public event Action OnDisconnected = delegate { };

    public async Task ConnectAsync(string username)
    {
        await tcpClient.ConnectAsync("127.0.0.1", 8080);
        tcpStream = tcpClient.GetStream();
        await WriteMessageAsync(tcpStream, username);
        _ = Task.Run(async () =>
        {
            while (tcpClient.Connected)
            {
                try
                {
                    var payload = await ReadMessageAysnc(tcpStream);
                    var json = JsonSerializer.Deserialize<JsonDocument>(payload);
                    var msgType = json!.RootElement.GetProperty("messageType").GetString();
                    if (msgType == "broadcast" || msgType == "private-message" || msgType == "server-message")
                    {
                        var sender = json.RootElement.GetProperty("sender").GetString();
                        var content = json.RootElement.GetProperty("content").GetString();
                        var type = msgType switch
                        {
                            "broadcast" => ChatMessageType.Broadcast,
                            "private-message" => ChatMessageType.Private,
                            "server-message" => ChatMessageType.ServerMessage,
                            _ => throw new InvalidDataException("Unknown message type")
                        };
                        OnMessageReceived(new ChatMessage(sender, content!, type));
                    }
                    else if (msgType == "client-list")
                    {
                        var clientsJson = json.RootElement.GetProperty("clients").EnumerateArray();
                        var clients = clientsJson.Select(c => c.GetString()!).ToList();
                        OnUserListUpdated(clients);
                    }
                }
                catch (Exception)
                {
                    break;
                }
            }
            OnDisconnected();
        });
    }

    public async Task SendMessageAsync(string message)
    {
        var msgObj = new
        {
            messageType = "broadcast",
            content = message
        };
        var msgJson = JsonSerializer.Serialize(msgObj);
        await WriteMessageAsync(tcpStream!, msgJson);
    }

    public async Task SendPrivateMessageAsync(string recipient, string message)
    {
        var tcpStream = tcpClient.GetStream();
        var msgObj = new
        {
            messageType = "private-message",
            receiver = recipient,
            content = message
        };
        var msgJson = JsonSerializer.Serialize(msgObj);
        await WriteMessageAsync(tcpStream, msgJson);
    }

    private async Task<bool> WriteMessageAsync(NetworkStream stream, string message)
    {
        var bytes = Encoding.UTF8.GetBytes(message);
        var lengthPrefix = BitConverter.GetBytes(bytes.Length);
        if (BitConverter.IsLittleEndian)
            Array.Reverse(lengthPrefix);
        await stream.WriteAsync(lengthPrefix);
        await stream.WriteAsync(bytes);
        return true;
    }

    private async Task<byte[]> ReadExactAsync(NetworkStream stream, int size)
    {
        var buffer = new byte[size];
        var offset = 0;
        while (offset < size)
        {
            int read = await stream.ReadAsync(buffer.AsMemory(offset, size - offset));
            if (read == 0)
            {
                throw new IOException("Disconnected while reading from stream");
            }

            offset += read;
        }

        return buffer;
    }

    public async Task<byte[]> ReadMessageAysnc(NetworkStream stream)
    {
        byte[] lengthBuffer = await ReadExactAsync(stream, 4);

        if (BitConverter.IsLittleEndian)
            Array.Reverse(lengthBuffer);

        int length = BitConverter.ToInt32(lengthBuffer, 0);

        if (length <= 0 || length > 10_000_000)
            throw new InvalidDataException("Invalid message length");

        byte[] payload = await ReadExactAsync(stream, length);

        return payload;
    }

    public ValueTask DisposeAsync()
    {
        tcpClient?.Close();
        return ValueTask.CompletedTask;
    }
}