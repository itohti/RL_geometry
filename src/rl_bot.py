import win32pipe, win32file, pywintypes

rl_send = r'\\.\pipe\rl_send'
rl_receive = r'\\.\pipe\rl_receive'

# Create a named pipe server
pipe_in = win32pipe.CreateNamedPipe(
    rl_send,
    win32pipe.PIPE_ACCESS_INBOUND,       # read from client
    win32pipe.PIPE_TYPE_MESSAGE |         # message-type pipe
    win32pipe.PIPE_READMODE_MESSAGE | 
    win32pipe.PIPE_WAIT,
    1,                                   # max instances
    65536, 65536,                        # out/in buffer size
    0,                                   # default timeout
    None
)


pipe_out = win32file.CreateFile(
    rl_receive,
    win32file.GENERIC_WRITE,
    0,
    None,
    win32file.OPEN_EXISTING,
    0,
    None
)

print("Waiting for Mod to connect...")
win32pipe.ConnectNamedPipe(pipe_in, None)
print("Mod connected!")

try:
    while True:
        # Read data from the mod
        resp = win32file.ReadFile(pipe_in, 1024)
        msg = resp[1].decode()
        print("Received from mod:", msg)

        reply = f"Python server got {msg}"
        win32file.WriteFile(pipe_out, reply.encode())
        print("Sent to mod:", reply)
except KeyboardInterrupt:
    print("Closing pipe...")
    win32file.CloseHandle(pipe_in)
finally:
    win32file.CloseHandle(pipe_in)
    