import dataclasses
import client


@dataclasses.dataclass
class Parameters:
    host: str = "10.0.0.17"
    port: int = 5555


def run_reset() -> None:
    pass


def run_default() -> None:
    params = Parameters()

    with client.ScopeConnection(host=params.host, port=params.port) as conn:
        conn.handshake()
