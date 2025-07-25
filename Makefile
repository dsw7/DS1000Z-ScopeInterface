.PHONY = py
.DEFAULT_GOAL = py

py:
	@black src
	@echo
	@pylint src --exit-zero
	@mypy --strict src
