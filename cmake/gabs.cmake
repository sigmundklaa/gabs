
define_property(
    TARGET
    PROPERTY
        _GABS_IMPLEMENTS
    BRIEF_DOCS
        "Which module the target implements"
)

define_property(
    TARGET
    PROPERTY
        _GABS_IMPL
    BRIEF_DOCS
        "Currently selected implementation"
)

define_property(
    TARGET
    PROPERTY
        _GABS_ALLOW_MANY
    BRIEF_DOCS
        "Allow multiple implementations"
)

define_property(
    TARGET
    PROPERTY
        _GABS_ADD
    BRIEF_DOCS
        "Whether or not to add module to library"
)

if(NOT GABS_LIBRARY)
    set(GABS_LIBRARY gabs)
endif()

if(NOT GABS_IFACE_LIBRARY)
    set(GABS_IFACE_LIBRARY gabs_iface)
endif()

function(gabs_require)
    foreach(module ${ARGN})
        set_target_properties(${module} PROPERTIES _GABS_ADD ON)
    endforeach()
endfunction()

function(gabs_module name)
    set(_options ADD_ALWAYS ADD_TO_INTERFACE ALLOW_MANY)
    set(_vargs "")
    set(_mvargs REQUIRES DEFAULT)
    cmake_parse_arguments(
        gabs_module_arg
        "${_options}"
        "${_vargs}"
        "${_mvargs}"
        ${ARGN}
    )

    # Slightly confusing distinction: _iface libraries are meant to be
    # used as a typical interface library, that is, headers, compile
    # options and such. While the library without the prefix is of type
    # INTERFACE, it should be treated as a source library as this is what
    # should contain the sources that will be compiled in the ${GABS_LIBRARY}
    # target.
    add_library(${name} INTERFACE)
    add_library(${name}_iface INTERFACE)

    target_link_libraries(
        ${name}
        INTERFACE
            ${GABS_IFACE_LIBRARY}
            ${name}_iface
    )

    if(gabs_module_arg_ADD_ALWAYS)
        gabs_require(${name})
    else()
        set_target_properties(
            ${name}
            PROPERTIES
                _GABS_ADD OFF
        )
    endif()

    # Link dependencies
    if(gabs_module_arg_REQUIRES)
        target_link_libraries(${name} INTERFACE ${gabs_module_arg_REQUIRES})

        list(
            TRANSFORM
                gabs_module_arg_REQUIRES
            APPEND
                _iface
            OUTPUT_VARIABLE
                ifaces
        )
        target_link_libraries(${name}_iface INTERFACE ${ifaces})
    endif()

    set_target_properties(
        ${name}
        PROPERTIES
            _GABS_IMPL ""
            _GABS_ALLOW_MANY ${gabs_module_arg_ALLOW_MANY}
    )

    # Link implementation to module, if it has been defined.
    set(_impl_exp "$<TARGET_PROPERTY:${name},_GABS_IMPL>")
    set(_impl_exp_bool "$<BOOL:${_impl_exp}>")
    set(_impl "$<IF:${_impl_exp_bool},${_impl_exp},${gabs_module_arg_DEFAULT}>")
    target_link_libraries(${name} INTERFACE "$<$<BOOL:${_impl}>:${_impl}>")

    # Link implementation interface to this interface, if the implementation
    # has been specified.
    set(_impl_iface "$<LIST:TRANSFORM,${_impl},APPEND,_iface>")
    target_link_libraries(
        ${name}_iface
        INTERFACE
            "$<$<BOOL:${_impl}>:${_impl_iface}>"
    )

    # Add to gabs library if the add property, in the generator stage, says to
    # do so. Otherwise this module will not be used.
    set(_is_added "$<TARGET_PROPERTY:${name},_GABS_ADD>")
    set(_is_added_bool "$<BOOL:${_is_added}>")
    target_link_libraries(
        ${GABS_LIBRARY}
        PRIVATE
            "$<${_is_added_bool}:${name}>"
    )

    # Add to interface target if _ADD=ON
    if(gabs_module_arg_ADD_TO_INTERFACE)
        target_link_libraries(
            ${GABS_IFACE_LIBRARY}
            INTERFACE
                "$<${_is_added_bool}:${name}_iface>"
        )
    endif()
endfunction()

function(gabs_provide)
    foreach(impl ${ARGN})
        get_target_property(_module ${impl} _GABS_IMPLEMENTS)

        get_target_property(_allow_many ${_module} _GABS_ALLOW_MANY)
        if(_allow_many)
            set_property(TARGET ${_module} APPEND PROPERTY _GABS_IMPL ${impl})

            continue()
        endif()

        set_target_properties(${_module} PROPERTIES _GABS_IMPL ${impl})
    endforeach()
endfunction()

function(gabs_select)
    foreach(impl ${ARGN})
        get_target_property(_module ${impl} _GABS_IMPLEMENTS)

        gabs_require(${_module})
        gabs_provide(${impl})
    endforeach()
endfunction()

macro(gabs_get_iface var name)
    set(${var} ${name}_iface)
endmacro()

macro(gabs_get_lib var name)
    set(${var} ${name})
endmacro()

function(gabs_include name)
    gabs_get_iface(_lib ${name})

    target_include_directories(${_lib} INTERFACE ${ARGN})
endfunction()

function(gabs_sources name)
    gabs_get_lib(_lib ${name})

    target_sources(${_lib} INTERFACE ${ARGN})
endfunction()

function(gabs_implement impl)
    set(_vargs IMPLEMENTS)
    set(_mvargs INCLUDE SOURCES)
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
    add_library(${impl}_iface INTERFACE)

    set_target_properties(
        ${impl}
        PROPERTIES
            _GABS_IMPLEMENTS ${gabs_implement_arg_IMPLEMENTS}
    )

    if(gabs_implement_arg_SOURCES)
        gabs_sources(${impl} ${gabs_implement_arg_SOURCES})
    endif()

    if(gabs_implement_arg_INCLUDE)
        gabs_include(${impl} ${gabs_implement_arg_INCLUDE})
    endif()
endfunction()
