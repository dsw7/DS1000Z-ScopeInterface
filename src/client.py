import socket


def connect_to_device(host: str, port: int) -> None:

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.connect((host, port))
        except OSError as e:
            raise RuntimeError(f"Could not connect on {host}:{port}: {e}") from e

        command = "*IDN?\n"
        s.sendall(command.encode())

        response = s.recv(4096)
        print("Received response:", response.decode())
