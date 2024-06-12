BIN_TARGETS := test_csv main
BUILD_BIN_TARGETS := ./build/bin/test_csv # $(addprefix ./build/bin/,$(BIN_TARGETS)) # Only including one so that redundant_make runs only once. Might be a better way
CMAKE_TARGET := ./build/CMakeCache.txt

all: $(BIN_TARGETS) 

clean:
	rm -f $(BIN_TARGETS)
	rm -rf ./build
	rm -f CUnitAutomated-Results.xml

$(BIN_TARGETS): $(BUILD_BIN_TARGETS)
	ln -sf $(addprefix ./build/bin/, $@) $@

$(BUILD_BIN_TARGETS): $(CMAKE_TARGET) redundant_make

redundant_make:
	cd ./build && make

$(CMAKE_TARGET): build
	cd build && cmake3 ..

build:
	mkdir build

