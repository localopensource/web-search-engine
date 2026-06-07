# WebSearchEngine

A minimal HTTP search engine server written in C++11. It indexes web pages using an in-memory inverted index, exposes a plain HTTP interface on port 8080, and returns matching page URLs for any single-term query submitted via a browser or HTTP client.

---

## Features

- **Inverted index.** Tokenises page content by whitespace and maps each term to the set of pages that contain it, enabling fast constant-time lookups.
- **HTTP server.** Listens on a raw TCP socket and handles GET requests without any external web framework.
- **Search interface.** Serves a minimal HTML form at `/` and returns query results at `/search?query=<term>`.
- **Lightweight by design.** No runtime dependencies beyond the C++ standard library and POSIX sockets.

---

## Architecture

```
         +------------------+
         |     Browser /    |
         |    HTTP Client   |
         +--------+---------+
                  |  TCP (port 8080)
                  v
         +------------------+
         |   startServer()  |  accept() loop
         +--------+---------+
                  |
                  v
         +------------------+
         | handleRequest()  |  parses raw GET request
         +--------+---------+
                  |
                  v
         +------------------+
         |  SearchEngine    |  tokenise + query
         +--------+---------+
                  |
                  v
         +------------------+
         | InvertedIndex    |  unordered_map<token, set<pageId>>
         +------------------+
```

---

## Repository Structure

```
WebSearchEngine/
├── CMakeLists.txt
├── README.md
└── main.cpp
```

---

## Getting Started

Clone the repository and enter the project directory:

```bash
git clone https://github.com/localopensource/web-search-engine.git
cd web-search-engine
```

---

## Dependencies

The project relies only on the C++ standard library and POSIX networking headers (`<netinet/in.h>`, `<unistd.h>`). No third-party libraries are required.

You will need a C++11-compatible compiler and CMake 3.10 or later.

### Ubuntu / Debian

```bash
sudo apt-get update
sudo apt-get install build-essential cmake
```

### macOS

The required toolchain ships with Xcode Command Line Tools:

```bash
xcode-select --install
brew install cmake
```

---

## Building

### 1. Configure

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

### 2. Compile

```bash
cmake --build build
```

This produces a binary named `WebSearchEngine` in the `build/` directory.

---

## Usage

Start the server:

```bash
./build/WebSearchEngine
```

The server binds to all interfaces on port 8080 and prints a confirmation:

```
Server is running on port 8080...
```

### Search via browser

Open a browser and navigate to:

```
http://localhost:8080
```

Use the form to enter a search term and press Search.

### Search via curl

```bash
curl "http://localhost:8080/search?query=algorithms"
```

### Example response

```
Found: https://example.com<br>Found: https://example2.com<br>
```

To stop the server, press Ctrl+C in your terminal.

---

## Adding Pages

Pages are indexed at startup inside `main()`. To add your own pages, call `engine.addPage()` before `startServer()`:

```cpp
engine.addPage(WebPage("https://yoursite.com", "your page content here"));
```

Each page is tokenised by whitespace. Search queries are matched against individual tokens, so multi-word queries are not supported in the current version.

---

## Limitations

- Single-term queries only. Multi-word queries are not intersected across tokens.
- No URL decoding. Query strings containing `%20` or `+` for spaces will not match correctly.
- The HTTP parser is minimal and does not handle headers, keep-alive, or malformed requests robustly.
- Pages are held entirely in memory; the index does not persist between restarts.
- The server is single-threaded: each request blocks the accept loop until it completes.
