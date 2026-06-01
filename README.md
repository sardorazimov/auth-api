# miransas-agent 🚀

A high-performance, zero-dependency, lightweight system monitoring daemon written in pure C. Specifically architected for Unix environments and heavily optimized for Apple Silicon (macOS architecture). 

The `miransas-agent` operates invisibly in the background as a native POSIX daemon, communicating directly with the XNU kernel to stream precise hardware metrics over the UDP protocol formatted as structured JSON strings.

---

## 👁️ Core Philosophy & Architecture

Most modern monitoring agents carry heavy runtimes (like Go, Rust, or Python) or rely on third-party libraries (like cJSON). While great for rapid development, they introduce runtime overhead, garbage collection pauses, or external security dependency risks.

`miransas-agent` is designed under the **"Zero-Overhead"** principle:
1. **Direct Kernel Interfacing:** Instead of parsing text-heavy shell commands (like `top` or `ps`), it uses low-level system traps (`sysctl` and `mach/mach.h`) to read metrics from the kernel space in microseconds.
2. **Pure C JSON Serialization:** It leverages highly optimized `snprintf` memory buffers to build valid JSON objects natively, executing with zero dynamic heap allocations during runtime.
3. **Non-Blocking UDP Streaming:** Metrics are dispatched via connectionless UDP sockets. If the central ingest server (e.g., `Binboi` or custom dashboards) goes down, the agent never blocks system execution; it simply keeps running without memory leaks or retries.

---

## 🌀 How It Works (The Low-Level Mechanics)

### 1. POSIX Daemonization (The Ghost Mode)
When executed, the agent immediately detaches itself from the controlling terminal and vanishes into the background through a strict 6-step Unix process:
* **Double Forking:** Calling `fork()` twice cuts off the process from any parent shell, ensuring it cannot re-acquire a controlling terminal.
* **Session Leadership:** `setsid()` detaches the process from its current session and creates a brand-new independent session group.
* **I/O Redirection:** Standard streams (`stdin`, `stdout`, `stderr`) are completely closed and bound to `/dev/null` (the Unix black hole), cutting all terminal ties while preventing crashes on print statements.

### 2. Kernel Telemetry Gathering
* **Memory Tracking:** The agent queries `host_statistics64` via the Mach microkernel port. It retrieves raw page counts (`free_count` and `speculative_count`), calculates the exact hardware page size, and accurately deduces the system's absolute untouched physical memory.
* **CPU Load Tracking:** It reads processor tick differentials across all physical cores. By measuring the delta change of `CPU_STATE_USER`, `CPU_STATE_SYSTEM`, and `CPU_STATE_IDLE` ticks between precise microsecond intervals, it calculates the true hardware-level CPU utilization percentage.

---

## ⚙️ Compilation & Lifecycle Management

Since the codebase relies solely on standard POSIX and native macOS headers, compilation requires nothing but a generic C compiler (`clang` or `gcc`).

### Compilation
To compile the raw code into an aggressively optimized static binary (`-O3` compiler optimization flag):
```bash
make