#!/usr/bin/env python3

import sys
import click
import commands


@click.group(help="Automation for working with Rigol DS1000Z Series of oscilloscopes.")
def main() -> None:
    pass


@main.command(help="Restore instrument to the default state.")
def reset() -> None:
    commands.run_reset()


@main.command(help="Configure scope to measure built in 1KHz calibration wave.")
def default() -> None:
    try:
        commands.run_default()
    except RuntimeError as e:
        sys.exit(str(e))


if __name__ == "__main__":
    main()
