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


class ScopeConnection:

    def __init__(self, host: str, port: int) -> None:
        self.conn = _TelnetConnection(host=host, port=port)

    def __enter__(self) -> typing.Self:
        self.conn.connect()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:  # type: ignore
        self.conn.close()

    def _check_for_error(self) -> None:
        self.conn.write(":SYST:ERR?")
        response = self.conn.read()
        code, errmsg = response.split(",")

        if code != "0":
            raise RuntimeError(
                f"Scope returned an error. The code was: {code}. The message was: {errmsg}"
            )

    def handshake(self) -> None:
        print("Handshaking with device")

        self.conn.write("*IDN?")
        response = self.conn.read()

        self._check_for_error()
        components = response.split(",")

        if len(components) != 4:
            raise RuntimeError(
                "Something went wrong. *IDN? query did not return a valid ID string"
            )

        print("Handshake returned:")
        print(f"-- Name:              {components[0]}")
        print(f"-- Model:             {components[1]}")
        print(f"-- Serial number:     {components[2]}")
        print(f"-- Software version:  {components[3]}")
