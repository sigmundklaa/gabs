
# Create an abstraction module, as the libraries `<module>` and
# `<module>-iface`. `<module>-iface` holds the headers of the module, and
# `<module>` also its sources.
#
#   gabs_module(<module> [REQUIRES <module>...])
function(gabs_module name)
    set(_mvargs REQUIRES)
    cmake_parse_arguments(
        gabs_module_arg
        ""
        ""
        "${_mvargs}"
        ${ARGN}
    )

    add_library(${name} INTERFACE)
    add_library(${name}-iface INTERFACE)

    target_link_libraries(${name}-iface INTERFACE gabs-core)
    target_link_libraries(${name} INTERFACE ${name}-iface)

    foreach(dep ${gabs_module_arg_REQUIRES})
        target_link_libraries(${name}-iface INTERFACE ${dep}-iface)
        target_link_libraries(${name} INTERFACE ${dep})
    endforeach()
endfunction()

# Store the name of the library holding the headers of a module or
# implementation in `var`.
#
#   gabs_get_iface(<var> <name>)
macro(gabs_get_iface var name)
    set(${var} ${name}-iface)
endmacro()

# Store the name of the library holding the sources of a module or
# implementation in `var`.
#
#   gabs_get_lib(<var> <name>)
macro(gabs_get_lib var name)
    set(${var} ${name})
endmacro()

# Add include directories to a module or implementation.
#
#   gabs_include(<name> [<directory>...])
function(gabs_include name)
    gabs_get_iface(_lib ${name})

    target_include_directories(${_lib} INTERFACE ${ARGN})
endfunction()

# Add sources to a module or implementation.
#
#   gabs_sources(<name> [<source>...])
function(gabs_sources name)
    gabs_get_lib(_lib ${name})

    target_sources(${_lib} INTERFACE ${ARGN})
endfunction()

# Create an implementation of a module, as the libraries `<implementation>`
# and `<implementation>-iface`. A target that links `<implementation>`
# compiles its sources and those of the module.
#
#   gabs_implement(
#       <implementation>
#       IMPLEMENTS <module>
#       [REQUIRES <module>...]
#       [INCLUDE <directory>...]
#       [SOURCES <source>...]
#   )
function(gabs_implement impl)
    set(_vargs IMPLEMENTS)
    set(_mvargs REQUIRES INCLUDE SOURCES)
    cmake_parse_arguments(
        gabs_implement_arg
        ""
        "${_vargs}"
        "${_mvargs}"
        ${ARGN}
    )

    if(NOT gabs_implement_arg_IMPLEMENTS)
        message(FATAL_ERROR "IMPLEMENTS keyword is required")
    endif()

    add_library(${impl} INTERFACE)
    add_library(${impl}-iface INTERFACE)

    target_link_libraries(
        ${impl}-iface
        INTERFACE
            ${gabs_implement_arg_IMPLEMENTS}-iface
    )
    target_link_libraries(
        ${impl}
        INTERFACE
            ${impl}-iface
            ${gabs_implement_arg_IMPLEMENTS}
    )

    foreach(dep ${gabs_implement_arg_REQUIRES})
        target_link_libraries(${impl}-iface INTERFACE ${dep}-iface)
        target_link_libraries(${impl} INTERFACE ${dep})
    endforeach()

    if(gabs_implement_arg_SOURCES)
        gabs_sources(${impl} ${gabs_implement_arg_SOURCES})
    endif()

    if(gabs_implement_arg_INCLUDE)
        gabs_include(${impl} ${gabs_implement_arg_INCLUDE})
    endif()
endfunction()
