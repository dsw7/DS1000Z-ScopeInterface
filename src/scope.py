import dataclasses
import logging
import typing
import tcp_conn

Logger = logging.getLogger(__name__)


@dataclasses.dataclass
class WaveformPreamble:
    points: int
    xincrement: float


@dataclasses.dataclass
class WaveformResults:
    time: list[float]
    voltage: list[float]


class ScopeConnection:

    def __init__(self, host: str, port: int) -> None:
        self.conn = tcp_conn.TCPConnection(host=host, port=port)

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

    def _get_vertical_limits(self) -> tuple[float, float]:
        Logger.debug("Getting vertical limits")

        self.conn.write(":CHAN1:SCAL?")
        scale = self.conn.read()
        self._check_for_error()

        volts_per_div = float(scale)
        v_max = volts_per_div * 4
        v_min = volts_per_div * -4
        return v_min, v_max

    def _get_horizontal_limits(self) -> tuple[float, float]:
        Logger.debug("Getting horizontal limits")

        self.conn.write(":TIM:MAIN:SCAL?")
        scale = self.conn.read()
        self._check_for_error()

        secs_per_div = float(scale)
        t_max = secs_per_div * 6
        t_min = secs_per_div * -6
        return t_min, t_max

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

    def set_timebase(self, secs_per_div: float = 0.0005) -> None:
        if secs_per_div <= 0 or secs_per_div > 50:
            raise RuntimeError("Timebase must be > 0 and <= 50 seconds")

        Logger.debug("Setting timebase to %f seconds", secs_per_div)
        self.conn.write(f":TIM:MAIN:SCAL {secs_per_div}")
        self._check_for_error()

    def set_channel_scale(self, volts_per_div: float = 1.00) -> None:
        if volts_per_div < -0.01 or volts_per_div > 100:
            raise RuntimeError("Vertical scale must be between 0.01V and 100V")

        Logger.debug("Setting vertical scale to %f volts", volts_per_div)
        self.conn.write(f":CHAN1:SCAL {volts_per_div}")
        self._check_for_error()

    def set_rising_edge_trigger(self, trigger_level: float = 1.00) -> None:
        v_min, v_max = self._get_vertical_limits()

        if trigger_level < v_min or trigger_level > v_max:
            # I/O between machine and scope will crash if we try to set trigger outside of limits
            raise RuntimeError(
                f"Trigger outside limits. The vertical limits are {v_min}V and +{v_max}V"
            )

        self.conn.write(":TRIG:MODE EDGE")
        self._check_for_error()

        Logger.debug("Setting trigger level to %f volts", trigger_level)
        self.conn.write(f":TRIG:EDG:LEV {trigger_level}")
        self._check_for_error()

        self.conn.write(":TRIG:EDG:SOUR CHAN1")
        self._check_for_error()

        self.conn.write(":TRIG:EDG:SLOP POS")
        self._check_for_error()

    def set_horizontal_position(self, t_pos: float = 0.00) -> None:
        t_min, t_max = self._get_horizontal_limits()

        if t_pos < t_min or t_pos > t_max:
            raise RuntimeError(
                f"Horizontal position (x) outside limits. The limits are {t_min} s and +{t_max} s"
            )

        Logger.debug("Setting horizontal position to %f seconds", t_pos)
        self.conn.write(f":TIM:MAIN:OFFS {t_pos}")
        self._check_for_error()

    def set_vertical_position(self, v_pos: float = 0.00) -> None:
        v_min, v_max = self._get_vertical_limits()

        if v_pos < v_min or v_pos > v_max:
            raise RuntimeError(
                f"Vertical position (y) outside limits. The limits are {v_min}V and +{v_max}V"
            )

        Logger.debug("Setting vertical position to %f volts", v_pos)
        self.conn.write(f":CHAN1:OFFS {v_pos}")
        self._check_for_error()

    def set_single_shot(self) -> None:
        Logger.debug("Setting scope to single trigger mode")
        self.conn.write(":SING")
        self._check_for_error()

    def read_waveform_preamble(self) -> WaveformPreamble:
        Logger.debug("Reading waveform preamble")

        self.conn.write(":WAV:PRE?")
        # self._check_for_error()
        raw_data = self.conn.read()
        preamble = raw_data.split(",")

        return WaveformPreamble(points=int(preamble[2]), xincrement=float(preamble[4]))

    def read_waveform_data(self) -> WaveformResults:
        Logger.debug("Setting the channel from which the waveform data will be read")
        self.conn.write(":WAV:SOUR CHAN1")
        self._check_for_error()

        Logger.debug(
            "Setting the waveform reading mode to read all data displayed on screen"
        )
        self.conn.write(":WAV:MODE NORM")
        self._check_for_error()

        Logger.debug("Setting the return format of the waveform data to ASCII")
        self.conn.write(":WAV:FORM ASC")
        self._check_for_error()

        preamble = self.read_waveform_preamble()

        Logger.debug("Querying data")
        self.conn.write(":WAV:DATA?")

        raw_data = self.conn.read()
        # self._check_for_error()

        voltage = []
        time = []
        t = 0.00

        for d in raw_data.split(",")[1:-1]:
            voltage.append(float(d))
            time.append(t)
            t += preamble.xincrement

        return WaveformResults(voltage=voltage, time=time)
