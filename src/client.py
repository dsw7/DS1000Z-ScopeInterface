import typing
import socket


class DeviceConnection:

    def __init__(self, host: str, port: int) -> None:
        self.host: str = host
        self.port: int = port
        self.socket: socket.socket | None = None

    def __enter__(self) -> typing.Self:
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        try:
            if self.socket is not None:
                self.socket.connect((self.host, self.port))
                print(f"Connected to {self.host}:{self.port}")
        except OSError as e:
            self.socket = None
            raise RuntimeError(
                f"Could not connect on {self.host}:{self.port}: {e}"
            ) from e

        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:  # type: ignore
        if self.socket is None:
            return

        self.socket.close()
        print(f"Connection to {self.host}:{self.port} closed")

    def read(self, buffer_size: int = 4096) -> str:
        if self.socket is None:
            raise RuntimeError("No active connection to read from.")

        try:
            response = self.socket.recv(buffer_size)
        except OSError as e:
            raise RuntimeError(f"Failed to receive message: {e}") from e

        return response.decode()

    def write(self, command: str) -> None:
        if self.socket is None:
            raise RuntimeError("No active connection to write to.")

        try:
            self.socket.sendall(command.encode())
        except OSError as e:
            raise RuntimeError(f"Failed to send message: {e}") from e

        print(f"Command sent: {command}")
