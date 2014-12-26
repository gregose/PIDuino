XGOARCH = mips
XGOOS = linux
XPATH = /home/gose/x-tools/mips-unknown-linux-gnu/bin

BIN = ui
XBIN = ui-$(XGOARCH)

TARGETS=$(BIN) $(XBIN)
SOURCES=$(wildcard *.go)

all: $(TARGETS)

$(XBIN): $(SOURCES)
	PATH=$(XPATH):$(PATH) GOOS=$(XGOOS) GOARCH=$(XGOARCH) go build -compiler gccgo --gccgoflags "-static" -o $@

$(BIN): $(SOURCES)
	go build -compiler gccgo -o $@

.PHONY: clean

clean: $(TARGETS)
	rm -f $^
