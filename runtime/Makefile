# Define these parameters to reflect the proper directories.
TOP=..
NBALLERINA_BUILD_DIR=$(TOP)/build/nballerina
RUST_BUILD_DIR=$(TOP)/runtime

RUST=rustc

SRCS = lib.rs

all:
	$(RUST) --crate-type=dylib $(RUST_BUILD_DIR)/lib.rs

clean:
	rm *.so
