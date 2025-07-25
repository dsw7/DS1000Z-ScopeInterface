import typing
import socket


class _TelnetConnection:

    def __init__(self, host: str, port: int) -> None:
        self.host: str = host
        self.port: int = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connect(self) -> None:
        try:
            self.socket.connect((self.host, self.port))
            print(f"Connected to {self.host}:{self.port}")
        except OSError as e:
            raise RuntimeError(
                f"Could not connect on {self.host}:{self.port}: {e}"
            ) from e

    def close(self) -> None:
        self.socket.close()
        print(f"Connection to {self.host}:{self.port} closed")

    def read(self, buffer_size: int = 4096) -> str:
        try:
            response = self.socket.recv(buffer_size)
        except OSError as e:
            raise RuntimeError(f"Failed to receive message: {e}") from e

        return response.decode()

    def write(self, command: str) -> None:
        try:
            self.socket.sendall(command.encode())
        except OSError as e:
            raise RuntimeError(f"Failed to send message: {e}") from e


class DeviceConnection:

    def __init__(self, host: str, port: int) -> None:
        self.conn = _TelnetConnection(host=host, port=port)

    def __enter__(self) -> typing.Self:
        self.conn.connect()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:  # type: ignore
        self.conn.close()

    def handshake(self) -> None:
        print("Handshaking with device")
        self.conn.write("*IDN?\n")
        response = self.conn.read()
        print("Received response:", response.strip())
