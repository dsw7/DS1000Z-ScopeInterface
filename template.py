# Prototype CLI + help messages by leveraging click

import click


@click.command()
@click.option(
    "-v",
    "--verbose",
    is_flag=True,
    help="Enable additional verbosity for debugging purposes",
)
@click.option("--port", default=5555, help="Oscilloscope port", show_default=True)
@click.option("--host", help="Oscilloscope host", required=True)
@click.argument("workflow")
def main(workflow):
    pass


if __name__ == "__main__":
    main()
