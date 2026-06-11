# CLI Image Processor

A high-performance CLI image processing application written in C++17.
It supports 24-bit uncompressed BMP files and provides multiple filters,
geometric transformations, histogram analysis, and pipeline chaining.

## Features

- **Filters**: Grayscale, Threshold, Blur (3x3 box), Sharpen (3x3)
- **Geometric Ops**: Crop, Resize (bilinear), Flip (horizontal/vertical)
- **Pipeline**: Chain multiple filters via `--pipeline`
- **Parallel Processing**: Multi-threaded convolution with optional SIMD (AVX2)
- **Histogram**: Console ASCII visualization of grayscale distribution
- **Logging**: Optional file-based operation logging

## Architecture

- **Strategy Pattern**: Each filter inherits from `FilterBase`
- **Builder Pattern**: `FilterPipelineBuilder` constructs sequential filter chains
- **Abstract Class**: `FilterBase` defines the common interface for all operations

## Build

```bash
cmake -B build -S .
cmake --build build --parallel
```

## Usage

```bash
# Single filter
./ImageProcessor --input in.bmp --output out.bmp --filter grayscale

# Pipeline
./ImageProcessor --input in.bmp --output out.bmp --pipeline "grayscale,blur,threshold:128"

# Geometric transformations
./ImageProcessor --input in.bmp --output out.bmp --crop 10,10,100,100 --resize 50,50 --flip horizontal

# Histogram with logging
./ImageProcessor --input in.bmp --output out.bmp --filter grayscale --histogram --log log.txt
```

## Options

| Option | Description |
|--------|-------------|
| `-i, --input <path>` | Input BMP file |
| `-o, --output <path>` | Output BMP file |
| `-f, --filter <name>` | Single filter |
| `-p, --pipeline <string>` | Comma-separated filter chain |
| `--crop <x,y,w,h>` | Crop region |
| `--resize <w,h>` | Resize dimensions |
| `--flip <direction>` | `horizontal` or `vertical` |
| `--histogram` | Print grayscale histogram |
| `-t, --threads <n>` | Thread count (0 = auto) |
| `-l, --log <path>` | Log file path |
| `-h, --help` | Show help |

## Requirements

- C++17 or later
- CMake 3.14+
- Compiler with AVX2 support (optional, falls back to scalar)

## License

This project is part of a SW assignment submission.
