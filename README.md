# Distributed Memory

Distributed Memory is a C++ course project for the **Operating Systems and Distributed Programming** class of the **Computer Engineering degree at the University of Pisa**.

The repository implements a simple distributed shared-memory model based on:

- a multithreaded server that stores a range of memory blocks;
- a client-side library used by applications to map, update, write, unmap, and wait on remote blocks;
- two small test programs that use the library to coordinate through distributed memory.

## Repository Layout

- `src/`: server sources and client library
- `test/`: demo programs, configuration file, and sample input text
- `test.sh`: convenience script that starts two servers and runs the demo
- `Doxyfile`, `mainpage.h`: Doxygen documentation support

## Main Components

### Server

The server executable is built as `src/server`. Each server instance owns a contiguous block-ID range:

```text
./server <port> <first_block_id> <last_block_id>
```

The server accepts TCP client connections and handles each client in a dedicated thread. The protocol supports five operations:

- `MAP`
- `UNMAP`
- `UPDATE`
- `WRITE`
- `WAIT`

The server-side logic is mainly implemented in:

- `src/main.cpp`
- `src/dm.cpp`
- `src/block.cpp`

### Client Library

The client API is defined in `src/distmem.h` and implemented in `src/distmem.cpp` through the `DM_client` class.

Typical usage:

1. Create one `DM_client` object.
2. Call `dm_init()` with a configuration file.
3. Map blocks into local memory with `dm_block_map()`.
4. Synchronize and exchange data with `dm_block_update()`, `dm_block_write()`, and `dm_block_wait()`.
5. Release blocks with `dm_block_unmap()`.

The library keeps one persistent TCP connection per configured server.

## Build

The project uses `make` and `g++`.

Build everything:

```bash
make
```

Clean build artifacts:

```bash
make clean
```

This produces:

- `src/server`
- `test/countspace`
- `test/countword`

## Configuration File

Clients discover the distributed memory layout through a configuration file such as [`test/dm.conf`](/home/valerio/projects/Distributed-Memory/test/dm.conf:1).

The file defines:

- block size through `DIMBLOCK`
- one or more servers with `Address`, `Port`, and managed `ID` range

Example:

```ini
DIMBLOCK=128

Address=127.0.0.1
Port=1234
ID=0-255

Address=127.0.0.1
Port=5678
ID=256-511
```

In the current codebase, the server block size is compiled as `128` bytes in `src/block.h`, so the configuration should match that value.

## Demo Programs

The `test/` directory contains two small applications:

- `countspace`: reads `test/divina`, writes it into distributed memory, counts spaces, and exchanges results with the other client
- `countword`: reads the same content from distributed memory, counts words, and exchanges results with `countspace`

Both programs are examples of coordination through shared distributed blocks rather than production-ready applications.

## Running the Demo

After building, start two servers from `src/`:

```bash
cd src
./server 1234 0 255
./server 5678 256 511
```

Then, from `test/`, run the sample clients in separate terminals:

```bash
./countspace dm.conf
./countword dm.conf
```

Or use the helper script from the repository root:

```bash
sh ./test.sh
```

The script starts the two server instances, runs both demo clients for three cycles, and compares their outputs.

## Documentation

The project contains a `Doxyfile` and source-level Doxygen comments. If Doxygen is installed, documentation can be generated with:

```bash
doxygen Doxyfile
```

## Notes

- The implementation uses POSIX threads and BSD sockets.
- The code reflects an academic project structure and coding style from 2010.
- The bundled demo expects local TCP sockets on ports `1234` and `5678` to be available.

## License

See [`LICENSE.md`](/home/valerio/projects/Distributed-Memory/LICENSE.md:1).
