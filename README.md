
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

To solve this, `libgabs` attempts to shift the responsibility of implementation
of the abstractions off to the application using the library. This is simply
handled in the application `CMakeLists.txt`, by invoking
`gabs_select(<implementation>)` (if not specified, a default implementation
will be used if it exists).

### Abstraction creation and implementation

An abstraction module is created by invoking `gabs_module`, which has the
following signature.

```cmake
gabs_module(
    <module_name>
    [ADD_TO_INTERFACE]
    [ADD_ALWAYS]
    [ALLOW_MANY]
    [REQUIRES <dependencies>...]
    [DEFAULT <default>...]
)
```

The possible arguments are:
- `module_name`: Name of abstraction module
- `ADD_TO_INTERFACE`: Link interface of abstraction to interface of `gabs`. This
should be used for modules that require inclusion of implementation-specific
header files.
- `ADD_ALWAYS`: Abstraction should always be included. This is used for core
modules such as the `gabs-toolchain` module.
- `ALLOW_MANY`: Allow multiple implementations. If set, the module must ensure
that multiple implementations do not cause a conflict.
- `REQUIRES`: Other `gabs` modules that this module depends on.
- `DEFAULT`: Default implementation if no other implementation has been
specified. This can take multiple implementations if `ALLOW_MANY` is set.

Each module requires an implementation, which can be provided by both the
module creator, or the application end user. To define an implementation
for a module, the `gabs_implement` function is used:
```cmake
gabs_implement(
    <implementation_name>
    [IMPLEMENTS <module_name>]
    [INCLUDE <include_dir>...]
    [SOURCES <source_file>...]
)
```

This creates an implementation candidate, named `<implementation_name>`, for
the abstraction module `<module_name>`. This can be selected by the application
using `gabs_select(<implementation_name>)`. Alternatively, the default
implementation can be overridden with `gabs_provide`:
```cmake
gabs_provide([<implementaton_name>...])
```

This sets the default implementation of `module_name` to `implementation_name`,
but does not add the module.

### Abstraction usage

Once an abstraction module has been defined, it a library can use it by
using the `gabs_require` function, with the following signature:
```cmake
gabs_require([<module_name>...])
```

An example of this could look like:
```cmake
# Assuming `gabs` has already been added to the project
gabs_require(<module1_name> <module2_name> ... <moduleN_name>)

target_link_libraries(<my_project> PUBLIC gabs)
```

Once the library has specified its required abstractions, it is the users
responsibility to ensure that implementations are provided. In most cases,
sensible defaults should exist so that the average user does not need to do
anything. If the user wants to specify an implementation, however, or if
there is no sensible default provided, an implementation can be selected with
the function `gabs_select`:
```cmake
gabs_select([<implementation_name>...])
```

Once the CMake specifics have been set up, both applicaton and library code
can use `gabs` as any other library.
