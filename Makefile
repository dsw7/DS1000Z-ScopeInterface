.PHONY = clean py
.DEFAULT_GOAL = py

clean:
	@rm -rfv dist/ *.egg-info/

py:
	@black src
	@echo
	@pylint src --exit-zero
	@mypy --strict src
