import win32pipe, win32file, pywintypes
import time
rl_send = r'\\.\pipe\rl_send'
rl_receive = r'\\.\pipe\rl_receive'

def connect_pipe_out():
    while True:
        try:
            win32pipe.WaitNamedPipe(r'\\.\pipe\rl_receive', 2000)
            return win32file.CreateFile(
                r'\\.\pipe\rl_receive',
                win32file.GENERIC_WRITE,
                0,
                None,
                win32file.OPEN_EXISTING,
                0,
                None
            )
        except pywintypes.error:
            time.sleep(0.05)

while True:
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
    print("Waiting for Mod to connect...")
    win32pipe.ConnectNamedPipe(pipe_in, None)
    print("Mod connected!")

    pipe_out = connect_pipe_out()
    print("Connected to received pipe")

    try:
        while True:
            # Read data from the mod
            resp = win32file.ReadFile(pipe_in, 1024)
            msg = resp[1].decode()
            print("Received from mod:", msg)

            reply = f"Python server got {msg}"
            win32file.WriteFile(pipe_out, reply.encode())
            print("Sent to mod:", reply)
    except:
        print("Closing pipe...")
        win32file.CloseHandle(pipe_in)
    