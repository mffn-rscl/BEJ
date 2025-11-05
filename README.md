# BEJ Parser

Binary Encoded JSON (BEJ) parser implementation in C. Converts BEJ binary format to standard JSON.

## Features

- Parse BEJ format (integers, strings, nested objects)
- Convert to JSON output
- Dictionary-based field name resolution
- Memory-safe parsing and cleanup

## Project Structure
```
BEJ/
├── src/              # Source files
│   ├── main.c
│   ├── bej_parse.c
│   └── dictionary.c
├── include/          # Header files
├── tests/            # Unit tests
├── bin/              # Binary data files (generated)
├── json/             # JSON output (generated)
└── docs/             # Doxygen documentation
```

## Building

### Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

### Manual compilation
```bash
gcc src/main.c src/bej_parse.c src/dictionary.c -Iinclude -o bej_parser
```

## Running
```bash
./bej_parser
```

Output will be generated in `json/result.json`

## Testing

### Build tests
```bash
gcc tests/test_bej.c src/bej_parse.c src/dictionary.c -Iinclude -o test_bej
```

### Run tests
```bash
./test_bej
```

## Documentation

Generate documentation using Doxygen:
```bash
doxygen Doxyfile
```

HTML documentation will be in `docs/html/`

## Example

Input BEJ data represents:
```json
{
  "CapacityMiB": 65536,
  "DataWidthBits": 64,
  "ErrorCorrection": "NoECC",
  "MemoryLocation": {
    "Channel": 0,
    "Slot": 0
  }
}
```

## Requirements

- C99 compiler (gcc/clang)
- CMake 3.10+ (optional)
- Doxygen (for documentation)

## License

MIT
