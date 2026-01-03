
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
        _GABS_DEFAULT_IMPL
    BRIEF_DOCS
        "Default implementation used for module if none other is specified"
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
        _GABS_ADD
    BRIEF_DOCS
    "Whether or not to add module to library"
)

function(gabs_require)
    foreach(module ${ARGN})
        set_target_properties(${module} PROPERTIES _GABS_ADD ON)
    endforeach()
endfunction()

function(gabs_module name)
    set(_options ADD_ALWAYS ADD_TO_INTERFACE)
    set(_vargs DEFAULT)
    set(_mvargs REQUIRED)
    cmake_parse_arguments(
        gabs_module_arg
        "${_options}"
        "${_vargs}"
        "${_mvargs}"
        ${ARGN}
    )

    add_library(${name} INTERFACE)

    if(gabs_module_arg_ADD_ALWAYS)
        gabs_require(${name})
    else()
        set_target_properties(
            ${name}
            PROPERTIES
                _GABS_ADD OFF
        )
    endif()

    if(gabs_module_arg_REQUIRED)
        gabs_require(${gabs_module_arg_REQUIRED})
    endif()

    set_target_properties(
        ${name}
        PROPERTIES
            _GABS_DEFAULT_IMPL ${gabs_module_arg_DEFAULT}
            _GABS_IMPL ${gabs_module_arg_DEFAULT}
    )

    target_link_libraries(
        ${name}
        INTERFACE
            $<TARGET_PROPERTY:${name},_GABS_IMPL>
    )

    # Add to gabs library if the add property, at runtime, says to do so.
    # Otherwise this module will not be used.
    target_link_libraries(
        gabs
        PRIVATE
            $<$<BOOL:$<TARGET_PROPERTY:${name},_GABS_ADD>>:${name}>
    )

    # Add to interface target
    if(gabs_module_arg_ADD_TO_INTERFACE)
        target_link_libraries(
            gabs_iface
            INTERFACE
                $<$<BOOL:$<TARGET_PROPERTY:${name},_GABS_ADD>>:${name}>
        )
    endif()
endfunction()

function(gabs_select impl)
    get_target_property(_module ${impl} _GABS_IMPLEMENTS)

    gabs_require(${_module})

    get_target_property(_cur_impl ${module} _GABS_IMPL)
    get_target_property(_default_impl ${module} _GABS_DEFAULT_IMPL)

    if(NOT _cur_impl STREQUAL _default_impl)
        message(FATAL_ERROR "Module ${module} already has non-default
            implementation specified (${_default_impl})")
    endif()

    set_target_properties(${module} PROPERTIES _GABS_IMPL ${impl})
endfunction()

function(gabs_implement impl)
    set(_vargs IMPLEMENTS)
    cmake_parse_arguments(gabs_implement_arg "" "${_vargs}" "" ${ARGN})

    if(NOT gabs_implement_arg_IMPLEMENTS)
        message(FATAL_ERROR "IMPLEMENTS keyword is required")
    endif()

    add_library(${impl} INTERFACE)

    set_target_properties(
        ${impl}
        PROPERTIES
            _GABS_IMPLEMENTS ${gabs_implement_arg_IMPLEMENTS}
    )
endfunction()

