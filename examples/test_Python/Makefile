PYTHON ?= python

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	ENV_LD_PATH=DYLD_LIBRARY_PATH=../../ximc/macosx
endif

run:
	PYTHONPATH=../../wrappers/python $(ENV_LD_PATH) $(PYTHON) standardtest/test_Python.py
