# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`libgabs` ("Generalized Abstractions") is a C/C++ CMake library providing OS/platform
abstraction modules (timer, mutex, semaphore, random, log, alloc, pbuf, ...) with
swappable backend implementations (POSIX, Zephyr, spdlog, disabled, etc.). It exists
to support [librlc](https://github.com/sigmundklaa/librlc.git) and similar libraries
that need to stay portable across host and embedded (Zephyr) targets without every
consuming application/library reinventing the same abstraction. See `README.md` for
the full rationale and the `gabs_module`/`gabs_implement`/`gabs_select`/`gabs_require`
CMake API — read it before touching build files.

## Build

A bare `cmake -S . -B build` fails with `No SOURCES given to target: gabs`, and that
is expected: modules only contribute sources once something calls `gabs_require`, and
nothing in this repo does so by default. libgabs is normally consumed via
`add_subdirectory` from an application that requires the modules it needs.

To configure and build in-repo, enable the tests — that is the one configuration here
that requires modules:

```sh
cmake -S . -B build -DGABS_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build              # or: ./build/tests/semaphore/gabs-tests-semaphore
```

Requires Catch2 v3 (`libcatch2-dev`). Single test:
`./build/tests/semaphore/gabs-tests-semaphore "semaphore::count_accumulates"`,
or by suite tag: `... "[semaphore]"`.

Zephyr build: this repo is also a Zephyr module (`zephyr/module.yml`), consumed via
`ZEPHYR_EXTRA_MODULES` or west manifest from a downstream Zephyr project — it is not
built standalone under Zephyr from within this repo. On the Zephyr side, each module
has its own isolated twister application in `tests/<module>/zephyr/`, the same way
Zephyr's own in-tree tests are laid out — there is no single combined Zephyr test
suite. Twister discovers all of them under the shared root:

```sh
west twister -T tests -p native_sim
```

A single module's suite can be targeted directly, e.g. `west twister -T tests/mutex/zephyr -p native_sim`.

Formatting: `clang-format` config is in `.clang-format` (LLVM-based, 8-space indent,
80-column limit, tabs never). Run it on changed `.c`/`.h`/`.cc`/`.hh` files.

## Architecture: the module/implementation system

This is the single most important thing to understand before editing any `CMakeLists.txt`
or adding a new backend. The machinery lives in `cmake/gabs.cmake`.

- **Module** (`gabs_module(gabs-<name> ...)`): declares an abstraction point (e.g.
  `gabs-timer`, `gabs-mutex`, `gabs-log`). Options:
  - `ADD_TO_INTERFACE`: link the module's interface (headers) into `gabs_iface`. Needed
    whenever consumers must see implementation-specific headers (almost always, since
    the `_def.h` indirection below requires it).
  - `ADD_ALWAYS`: module is unconditionally required (used by core modules like
    `gabs-compiler`).
  - `ALLOW_MANY`: multiple implementations of this module may be active simultaneously
    (e.g. `gabs-alloc` allows both `std` and `dynamic` allocators at once).
  - `REQUIRES <modules...>`: dependencies on other gabs modules.
  - `DEFAULT <impl...>`: fallback implementation(s) used if the consuming app doesn't
    call `gabs_select`.
- **Implementation** (`gabs_implement(<impl-name> IMPLEMENTS gabs-<name> INCLUDE <dir> SOURCES <files>)`):
  a concrete backend for a module, e.g. `gabs-timer-posix implements gabs-timer`.
- **Consumers**: a library calls `gabs_require(gabs-<name>)` to declare it needs that
  abstraction, and links against `gabs`/`gabs_iface`. The application (or the top-level
  `CMakeLists.txt`/`zephyr/CMakeLists.txt` here) picks the concrete backend with
  `gabs_select(<impl-name>)`, or overrides just the default via `gabs_provide(<impl-name>)`
  without forcing the module to be required.

Every module directory follows the same shape: a `CMakeLists.txt` calling `gabs_module()`
then `add_subdirectory()` per backend, and each backend subdirectory has its own
`CMakeLists.txt` calling `gabs_implement()`. Follow this pattern (see `timer/`,
`mutex/`, `semaphore/`, `random/`, `log/` as references) when adding a new module or backend.

The two top-level integration points wire up defaults:
- `CMakeLists.txt` (host/standalone build) — adds all module subdirectories, then
  `gabs_provide(gabs-timer-posix)`.
- `zephyr/CMakeLists.txt` (Zephyr module build) — uses `zephyr_library_named`/
  `add_oot_subdirectory` instead of `add_subdirectory`, and calls `gabs_provide(...)`
  for every module's Zephyr backend (e.g. `gabs-log-zephyr`, `gabs-mutex-zephyr`, ...).

When adding a new backend for a Zephyr target, remember to also wire its `gabs_provide`
call into `zephyr/CMakeLists.txt`.

## Architecture: public headers and the `_def.h` indirection

Public API headers live under `include/gabs/<module>.h` (e.g. `include/gabs/timer.h`,
`include/gabs/mutex.h`). These declare the module's function prototypes/macros against
opaque types, then pull in an implementation-specific definitions header via a
guarded include, e.g. in `include/gabs/timer.h`:

```c
#define GABS_TIMER_INCLUDE_DEF
#include <gabs_timer_def.h>
#undef GABS_TIMER_INCLUDE_DEF
```

Each backend provides its own `gabs_<module>_def.h` (e.g. `timer/posix/gabs_timer_def.h`,
`timer/zephyr/gabs_timer_def.h`) in its own include path, added via `gabs_implement`'s
`INCLUDE` argument. That header defines the concrete types (`gabs_timer`, `gabs_timer_ctx`,
...) and guards against direct inclusion with `#ifndef GABS_<MODULE>_INCLUDE_DEF / #error`.
This is how a single public API surface (`include/gabs/timer.h`) resolves to different
concrete types/implementations depending on which backend CMake selected — there is no
runtime polymorphism, it's compile-time selection via which `_def.h` ends up on the
include path. `gabs-compiler` (`include/gabs/compiler.h` / `gabs_compiler_def.h`) works
the same way and provides `GABS_API`/`GABS_INLINE` used throughout the other headers.

When adding a function to a module's public API, update the module header in
`include/gabs/<module>.h` and implement it in every backend under `<module>/<backend>/`.

## Tests

Tests verify that the *public API behaves identically across backends*; per-backend
internal unit testing is deliberately out of scope. The test framework is itself a gabs
module (`testing/`, header `include/gabs/testing/testing.h`) with a Catch2 backend for
the host and a ZTest backend for Zephyr, so one source compiles under both.

- `tests/<module>/` — one directory per module (e.g. `tests/mutex/`), holding the
  platform-agnostic `.cc` suite shared by both harnesses, plus:
  - `tests/<module>/CMakeLists.txt` — host harness: an isolated `gabs-tests-<module>`
    Catch2 executable via `target_sources`, listed with `add_subdirectory` from
    `tests/CMakeLists.txt`.
  - `tests/<module>/zephyr/` — an isolated twister application for that module
    (`CMakeLists.txt`, `prj.conf`, `testcase.yaml`), pulling in `../<module>.cc` via
    `target_sources(app ...)`. This mirrors how Zephyr's own in-tree tests are laid
    out: one self-contained test application per directory, not one shared app.
  - Add a new module suite by adding a new `tests/<module>/` directory with both of
    the above, and an `add_subdirectory` for the host side in `tests/CMakeLists.txt`.
- `tests/CMakeLists.txt` — host harness root: selects the Catch2 testing backend, then
  `add_subdirectory`s each `tests/<module>/`.

Test sources are always C++, since Catch2 requires it — this also keeps the public
headers verified C++-clean. Macros: `GABS_SUITE`/`GABS_SUITE_F`, `GABS_TEST`/`GABS_TEST_F`,
`GABS_ASSERT_*` (fatal) and `GABS_EXPECT_*` (non-fatal), `GABS_TEST_FAIL`/`GABS_TEST_SKIP`.
Assertions take no message — both frameworks derive one from the stringified expression.

Three rules that are easy to get wrong: use `GABS_EXPECT_*` (never `GABS_ASSERT_*`) in a
fixture destructor, because a failed assert unwinds by throwing under Catch2; only
assert behaviour that *every* backend guarantees — e.g. `tests/mutex/mutex.cc`
deliberately leaves recursive locking untested, since POSIX rejects it and Zephyr allows it;
and never name a `GABS_SUITE_F` fixture class `<suite>_fixture` — the ztest backend
generates a wrapper type with exactly that name (`ZTEST_F` requires it), so it collides
with a same-named fixture in the same translation unit. This only breaks the Zephyr
build, not Catch2, so it's easy to miss without building under `west`.

## Conventions

- Public macros/types are prefixed `GABS_`/`gabs_`; header guards are `GABS_<PATH>_H__`.
- `GABS_BEGIN_DECL`/`GABS_END_DECL` (from `include/gabs/core/util.h`) wrap declarations
  for C++ `extern "C"` linkage — use them in new public/def headers.
- `.hh`/`.cc` files exist for a few modules (e.g. `alloc/dynamic.hh`, `log/spdlog.cc`,
  `include/gabs/cc/*.hh`) providing C++-specific helpers; plain `.h`/`.c` is the default.
- Commit messages follow `<module>: <subarea>: <Imperative summary>`, e.g.
  `timer: posix: Zero-initialize context`, `log: zephyr: Handle CONFIG_LOG=n`.
