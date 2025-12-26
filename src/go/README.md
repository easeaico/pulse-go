# Pulse Go Bindings

Go language bindings for the Pulse Physiology Engine.

## Prerequisites

- **Go 1.20+** - [Download Go](https://go.dev/dl/)
- **PulseC shared library** - Must be built from the main project first
- **CGO enabled** - Required for C bindings (enabled by default on most systems)

## Building the PulseC Library

Before using the Go bindings, you must build the PulseC library from the main Pulse project:

```bash
# From project root
mkdir build && cd build
cmake ..
cmake --build .
```

This will produce `libPulseC.dylib` (macOS), `libPulseC.so` (Linux), or `PulseC.dll` (Windows) in the `bin/` directory.

## Quick Start

### Using the Build Script

The easiest way to build and run is using the provided build script:

```bash
cd src/go

# Check if PulseC library exists
./build.sh check

# Build the example
./build.sh build

# Build and run the example
./build.sh run

# Clean build artifacts
./build.sh clean
```

### Manual Build

```bash
cd src/go/examples

# Set library path (macOS)
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$(pwd)/../../../bin

# Set library path (Linux)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/../../../bin

# Build
go build -o pulse_example .

# Run
./pulse_example
```

## API Overview

### Package Import

```go
import "github.com/easeaico/pulse-go/pulse"
```

### Basic Usage

```go
// Initialize library
pulse.Initialize()
defer pulse.Deinitialize()

// Create engine
engine := pulse.NewPulseEngine(pulse.HumanAdultWholeBody, "./data")
defer engine.Destroy()

// Load patient state
err := engine.SerializeFromFile("StandardMale@0s.json", dataRequests, pulse.JSON)
if err != nil {
    log.Fatal(err)
}

// Run simulation
for i := 0; i < 100; i++ {
    engine.AdvanceTimeStep()
    data := engine.PullData(numRequests)
    // Process data...
}
```

### Available Functions

| Function | Description |
|----------|-------------|
| `Initialize()` | Initialize the Pulse library |
| `Deinitialize()` | Clean up the Pulse library |
| `Version()` | Get Pulse version string |
| `Hash()` | Get build hash |
| `NewPulseEngine()` | Create a new engine instance |
| `SerializeFromFile()` | Load state from file |
| `SerializeToFile()` | Save state to file |
| `InitializeEngine()` | Initialize with patient config |
| `AdvanceTimeStep()` | Advance simulation by one step |
| `PullData()` | Get current physiological values |
| `ProcessActions()` | Apply clinical actions |
| `LogToConsole()` | Enable/disable console logging |

## Troubleshooting

### Library Not Found

If you get a "library not found" error:

1. Ensure PulseC is built: `ls bin/libPulseC.*`
2. Set the library path environment variable:
   - **macOS**: `export DYLD_LIBRARY_PATH=/path/to/bin:$DYLD_LIBRARY_PATH`
   - **Linux**: `export LD_LIBRARY_PATH=/path/to/bin:$LD_LIBRARY_PATH`

### CGO Errors

Ensure CGO is enabled:
```bash
export CGO_ENABLED=1
```

## License

See the main project LICENSE file for licensing information.
