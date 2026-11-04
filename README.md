
## Generalized Abstractions Library (gabs)

This project was developed to support the
[librlc](https://github.com/sigmundklaa/librlc.git) project. It aims to simplify
abstraction layers in libraries.

### Purpose

Many C libraries require abstractions over things such as OS specifics. This
is often implemented for the specific library in question. While this ensures
that the library can be self-contained, an application using the library
may end up with many different implementations of what is in practice the
same functionality.

Mutliple implementations of simple abstractions, such as OS synchronization
primitives, are not a problem in and of itself. However, issues may arise
if these abstractions interefere with another. Consider, for example, if two
different libraries both wish to provide logging functionality. If the loggers
of the two libraries do not happen to have a shared implementation, the
application would have to set up logger sinks for both loggers, that redirect
the output to a shared sink. This becomes tedious, especially as the number of
libraries used grows.

Another example is taken from the
[librlc](https://github.com/sigmundklaa/librlc.git) project. `librlc` sits
on layer 2 of the OSI layer model, and network data flows through this library.
For integration into existing network stacks, such as the `Zephyr` network stack
or `LwIP`, it is therefore desirable for the library to work with the packet
buffers of these network stacks. This prevents having to convert between the
format of these stacks, and an internal format of `librlc`.

To solve this, `libgabs` leaves the choice of implementation to the
application using the library. A library uses the abstractions it needs, and
the application links the implementations.

### Abstraction creation and implementation

An abstraction module is created by invoking `gabs_module`, which has the
following signature.

```cmake
gabs_module(
    <module_name>
    [REQUIRES <dependencies>...]
)
```

The possible arguments are:
- `module_name`: Name of abstraction module
- `REQUIRES`: Other `gabs` modules that this module depends on.

This creates two libraries: `<module_name>-iface`, which holds the headers of
the module, and `<module_name>`, which also holds its sources.

Each module requires an implementation, which can be provided by both the
module creator, or the application end user. To define an implementation
for a module, the `gabs_implement` function is used:
```cmake
gabs_implement(
    <implementation_name>
    IMPLEMENTS <module_name>
    [INCLUDE <include_dir>...]
    [SOURCES <source_file>...]
)
```

This also creates two libraries: `<implementation_name>-iface`, which holds the
headers of the implementation and of the module, and `<implementation_name>`,
which also holds the sources of both.

### Abstraction usage

The headers of a module include headers of its implementation, so a target
that uses a module links an implementation of it, and compiles its sources:
```cmake
target_link_libraries(myexe PRIVATE gabs-timer-posix)
```

The same goes for the modules an implementation uses: `gabs-timer-posix` uses
`gabs-log` and `gabs-mutex`, so a target that links it also links an
implementation of each.

A library that leaves the choice to the application links the module, and is
an `INTERFACE` library. Its sources are then compiled in each target that links
it, with the implementation that target links. In this example, the
executable uses the POSIX timer, and the test the manual timer:
```cmake
add_library(mylib INTERFACE)
target_sources(mylib INTERFACE mylib.c)
target_link_libraries(mylib INTERFACE gabs-timer)

add_executable(myexe main.c)
target_link_libraries(myexe PRIVATE mylib gabs-timer-posix)

add_executable(mytest test.c)
target_link_libraries(mytest PRIVATE mylib gabs-timer-manual)
```

An executable can link only one implementation of a module, directly or
through the libraries it links, unless the module allows more, as
`gabs-alloc` does.

### Zephyr

On Zephyr, the modules and implementations are enabled in Kconfig, such as
`CONFIG_GABS_TIMER` and `CONFIG_GABS_TIMER_MANUAL`, and compiled once, in
`app`. Other Zephyr libraries get their headers by linking `gabs-iface`:
```cmake
zephyr_library_link_libraries(gabs-iface)
```

Once the CMake specifics have been set up, both applicaton and library code
can use `gabs` as any other library.
