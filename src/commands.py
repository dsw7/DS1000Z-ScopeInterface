import dataclasses
import matplotlib.pyplot as plt
import scope


@dataclasses.dataclass
class Parameters:
    host: str = "10.0.0.17"
    port: int = 5555


def run_reset() -> None:
    params = Parameters()

    with scope.ScopeConnection(host=params.host, port=params.port) as conn:
        conn.handshake()
        conn.reset()


def run_default() -> None:
    params = Parameters()

    with scope.ScopeConnection(host=params.host, port=params.port) as conn:
        conn.handshake()
        conn.set_timebase(secs_per_div=0.0005)
        conn.set_channel_scale(volts_per_div=1.00)
        conn.set_rising_edge_trigger(trigger_level=1.00)
        conn.set_horizontal_position(t_pos=0.00)
        conn.set_vertical_position(v_pos=0.00)
        conn.set_single_shot()
        results = conn.read_waveform_data()
        plt.xlabel("Time (s)")
        plt.ylabel("Voltage (V)")
        plt.plot(results.time, results.voltage)
        plt.show()
