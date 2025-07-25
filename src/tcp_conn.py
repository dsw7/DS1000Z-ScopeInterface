import logging
import socket

Logger = logging.getLogger(__name__)


class TCPConnection:

    def __init__(self, host: str, port: int) -> None:
        self.host: str = host
        self.port: int = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connect(self) -> None:
        try:
            self.socket.connect((self.host, self.port))
            Logger.debug("Connected to %s:%i", self.host, self.port)
        except OSError as e:
            raise RuntimeError(
                f"Could not connect on {self.host}:{self.port}: {e}"
            ) from e

    def close(self) -> None:
        self.socket.close()
        Logger.debug("Connection to %s:%i closed", self.host, self.port)

    def write(self, command: str) -> None:
        if not command.endswith("\n"):
            command += "\n"

        try:
            self.socket.sendall(command.encode())
        except OSError as e:
            raise RuntimeError(f"Failed to send message: {e}") from e

    def read(self, buffer_size: int = 4096) -> str:
        try:
            response = self.socket.recv(buffer_size)
        except OSError as e:
            raise RuntimeError(f"Failed to receive message: {e}") from e

        return response.decode().strip()
