import sys
import logging
import click
import commands


@click.group(help="Automation for working with Rigol DS1000Z Series of oscilloscopes.")
@click.option("-d", "--debug", is_flag=True, help="Enable debug logging.")
def main(debug: bool) -> None:
    if debug:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)


@main.command(help="Restore instrument to the default state.")
def reset() -> None:
    try:
        commands.run_reset()
    except RuntimeError as e:
        sys.exit(str(e))


@main.command(help="Configure scope to measure built in 1KHz calibration wave.")
def default() -> None:
    try:
        commands.run_default()
    except RuntimeError as e:
        sys.exit(str(e))


if __name__ == "__main__":
    main()
