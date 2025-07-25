import logging
import typing
import socket

Logger = logging.getLogger(__name__)


class _TelnetConnection:

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
        Logger.debug("Handshaking with device")

        self.conn.write("*IDN?")
        response = self.conn.read()

        self._check_for_error()
        components = response.split(",")

        if len(components) != 4:
            raise RuntimeError(
                "Something went wrong. *IDN? query did not return a valid ID string"
            )

        Logger.debug("Handshake returned:")
        Logger.debug("-- Name:              %s", components[0])
        Logger.debug("-- Model:             %s", components[1])
        Logger.debug("-- Serial number:     %s", components[2])
        Logger.debug("-- Software version:  %s", components[3])

    def reset(self) -> None:
        Logger.debug("Resetting device")

        self.conn.write("*RST")
        self._check_for_error()
