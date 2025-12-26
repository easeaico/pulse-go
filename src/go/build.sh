#!/bin/bash

# Build script for Pulse Go Bindings
# This script helps build and run the Go example for Pulse Physiology Engine

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BIN_DIR="$PROJECT_ROOT/bin"
EXAMPLES_DIR="$SCRIPT_DIR/examples"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

print_usage() {
    echo "Usage: $0 [command]"
    echo ""
    echo "Commands:"
    echo "  build    - Build the Go example binary"
    echo "  run      - Build and run the Go example"
    echo "  test     - Run Go tests"
    echo "  clean    - Clean build artifacts"
    echo "  check    - Check if PulseC library exists"
    echo ""
    echo "If no command is provided, 'build' is assumed."
}

check_library() {
    echo -e "${YELLOW}Checking for PulseC library...${NC}"
    
    if [[ "$OSTYPE" == "darwin"* ]]; then
        LIB_NAME="libPulseC.dylib"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        LIB_NAME="libPulseC.so"
    else
        LIB_NAME="PulseC.dll"
    fi
    
    if [ -f "$BIN_DIR/$LIB_NAME" ]; then
        echo -e "${GREEN}Found: $BIN_DIR/$LIB_NAME${NC}"
        return 0
    else
        echo -e "${RED}ERROR: $LIB_NAME not found in $BIN_DIR${NC}"
        echo ""
        echo "Please build the Pulse engine first using CMake:"
        echo "  mkdir build && cd build"
        echo "  cmake .."
        echo "  cmake --build ."
        echo ""
        return 1
    fi
}

setup_library_path() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        export DYLD_LIBRARY_PATH="$BIN_DIR:$DYLD_LIBRARY_PATH"
        echo "Set DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        export LD_LIBRARY_PATH="$BIN_DIR:$LD_LIBRARY_PATH"
        echo "Set LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
    fi
}

do_build() {
    echo -e "${YELLOW}Building Go example...${NC}"
    
    if ! check_library; then
        echo -e "${YELLOW}Warning: Library not found, build may fail at link time${NC}"
    fi
    
    cd "$EXAMPLES_DIR"
    
    # Set CGO flags
    export CGO_ENABLED=1
    export CGO_LDFLAGS="-L$BIN_DIR"
    
    go build -o pulse_example .
    
    echo -e "${GREEN}Build successful: $EXAMPLES_DIR/pulse_example${NC}"
}

do_run() {
    do_build
    
    echo ""
    echo -e "${YELLOW}Running example...${NC}"
    echo "----------------------------------------"
    
    setup_library_path
    
    cd "$EXAMPLES_DIR"
    ./pulse_example
}

do_test() {
    echo -e "${YELLOW}Running Go tests...${NC}"
    
    setup_library_path
    
    cd "$SCRIPT_DIR"
    go test ./...
}

do_clean() {
    echo -e "${YELLOW}Cleaning build artifacts...${NC}"
    
    if [ -f "$EXAMPLES_DIR/pulse_example" ]; then
        rm "$EXAMPLES_DIR/pulse_example"
        echo "Removed: $EXAMPLES_DIR/pulse_example"
    fi
    
    echo -e "${GREEN}Clean complete${NC}"
}

# Main
case "${1:-build}" in
    build)
        do_build
        ;;
    run)
        do_run
        ;;
    test)
        do_test
        ;;
    clean)
        do_clean
        ;;
    check)
        check_library
        ;;
    help|--help|-h)
        print_usage
        ;;
    *)
        echo -e "${RED}Unknown command: $1${NC}"
        print_usage
        exit 1
        ;;
esac
