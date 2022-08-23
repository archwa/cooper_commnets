.PHONY: all clean run setup

API_PATH=api/2018/
API_ROOT_PATH=api/
API_REPO_URI=https://github.com/dgitzel/ece303.git

LIB_PATH=lib/

PYTHON=python2
PYTHONPATH=$(LIB_PATH):$(LIB_PATH)$(API_PATH)

INPUT_FILE=sample.raw

all: $(LIB_PATH)

clean:
	rm -f *.log *.pyc

run: $(LIB_PATH)
	PYTHONPATH=$(PYTHONPATH) $(PYTHON) receive.py > receiver.log &
	PYTHONPATH=$(PYTHONPATH) $(PYTHON) send.py > sender.log < $(INPUT_FILE) 

$(LIB_PATH):
	mkdir $(LIB_PATH)
	git clone $(API_REPO_URI) $(LIB_PATH)$(API_ROOT_PATH)
