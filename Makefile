.DEFUALT_GOAL := run

clean:
	rm -rf __pycache__

run:
	python3 mpv-remote

setup: requirements.txt
	pip install -r requirements.txt

.PHONY: run clean
