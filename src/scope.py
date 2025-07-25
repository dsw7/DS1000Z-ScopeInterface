import logging
import typing
import tcp_conn

Logger = logging.getLogger(__name__)


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

    def get_vertical_limits(self) -> tuple[float, float]:
        Logger.debug("Getting vertical limits")
        self.conn.write(":CHAN1:SCAL?")
        scale = self.conn.read()
        self._check_for_error()

        volts_per_div = float(scale)
        v_max = volts_per_div * 4
        v_min = volts_per_div * -4
        return v_min, v_max

    def set_rising_edge_trigger(self, trigger_level: float = 1.00) -> None:
        v_min, v_max = self.get_vertical_limits()

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
