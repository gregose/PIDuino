XGOARCH = mips
XGOOS = linux
XPATH = /home/gose/x-tools/mips-unknown-linux-gnu/bin

BIN = ui
XBIN = ui-$(XGOARCH)

SOURCES = PIDuino-ui.go
ASSET_FILES = $(wildcard ./views/**/*)
ASSET_DIR = ./views/...
ASSET_OUTPUT = ui-views.go
ASSET_DEV_OUTPUT = ui-views-dev.go
TARGETS = $(BIN) $(XBIN)

all: $(TARGETS)

$(ASSET_OUTPUT): $(ASSET_FILES)
	go-bindata -o $@ $(ASSET_DIR)

$(ASSET_DEV_OUTPUT): $(ASSET_FILES)
	go-bindata -debug -o $@ $(ASSET_DIR)

$(XBIN): $(SOURCES) $(ASSET_OUTPUT)
	PATH=$(XPATH):$(PATH) GOOS=$(XGOOS) GOARCH=$(XGOARCH) go build -compiler gccgo --gccgoflags "-static" -o $@ $^

$(BIN): $(SOURCES) $(ASSET_DEV_OUTPUT)
	go build -compiler gccgo -o $@ $^

.PHONY: clean

clean:
	rm -f $(TARGETS) $(ASSET_OUTPUT) $(ASSET_DEV_OUTPUT)
