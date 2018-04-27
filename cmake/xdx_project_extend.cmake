

macro(_xdx_project_add_resource_target _out_source _out_header _out_target)
    set(_xdx_pr_OPTIONS FILES_FIRST)
    set(_xdx_pr_ARGUMENTS HEADER_NAME SOURCE_NAME CUSTOM_NAMESPACE DIRECTORY PREFIX CLASS_NAME)
    set(_xdx_pr_MARGUMENTS FILES)
    cmake_parse_arguments(_ "${_xdx_pr_OPTIONS}" "${_xdx_pr_ARGUMENTS}" "${_xdx_pr_MARGUMENTS}" ${ARGN})

    __xdx_required(__HEADER_NAME "xdx_project_add_resources: HEADER_NAME is required")
    __xdx_required(__SOURCE_NAME "xdx_project_add_resources: SOURCE_NAME is required")
    __xdx_required(__CLASS_NAME "xdx_project_add_resources: CLASS_NAME is required")

    if (NOT __DIRECTORY)
        set(__DIRECTORY resources)
    endif()

    set(__DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${__DIRECTORY})

    if (NOT __PREFIX)
        set(__PREFIX resources)
    endif()

    if (NOT __CUSTOM_NAMESPACE)
        set(__CUSTOM_NAMESPACE ${_xdx_project_NAMESPACE})
    endif()

    set(_TARGET_NAME ${_xdx_project_NAME}_resources_${__CLASS_NAME})

    if (NOT "${__CUSTOM_NAMESPACE}" STREQUAL "")
        set(__CLASS_NAME "${__CUSTOM_NAMESPACE}::${__CLASS_NAME}")
    endif()

    if (__FILES_FIRST)
        set(FF_ARG "-f")
    endif()

    foreach (f ${__FILES})
        list(APPEND _FILES_ARG "-r" "${f}")
        list(APPEND _DEPS_SOURCES "${__DIRECTORY}/${f}")
    endforeach()


    file(MAKE_DIRECTORY "${_xdx_project_BINARY_SOURCE_DIR}")
    file(MAKE_DIRECTORY "${_xdx_project_BINARY_INCLUDE_DIR}/${_xdx_project_INCLUDE_PREFIX}")

    set(_SRC "${_xdx_project_BINARY_SOURCE_DIR}/${__SOURCE_NAME}")
    set(_HDR "${_xdx_project_BINARY_INCLUDE_DIR}/${_xdx_project_INCLUDE_PREFIX}/${__HEADER_NAME}")

    add_custom_command(
        OUTPUT ${_SRC} ${_HDR}
        COMMAND xdx.resources.builder ${FF_ARG} "-d" "${__DIRECTORY}" "-c" "${__CLASS_NAME}" "-S" "${_SRC}" 
                                      "-p" ${__PREFIX} "-H" "${_HDR}" "-I" ${_xdx_project_INCLUDE_PREFIX} ${_FILES_ARG}
        DEPENDS ${_DEPS_SOURCES}
    )

    add_custom_target(${_TARGET_NAME}
        DEPENDS ${_SRC} ${_HDR}
    )

    set(${_out_source} ${_SRC})
    set(${_out_header} ${_HDR})
    set(${_out_target} ${_TARGET_NAME})

endmacro()

macro(xdx_project_add_resources)
    _xdx_project_add_resource_target(_SRC _HDR _TARGET ${ARGN})
    xdx_project_add_public_links(xdx.resources)
    list(APPEND _xdx_project_SOURCES "${_SRC}")
    list(APPEND _xdx_project_HEADERS "${_HDR}")
    unset(_SRC)
    unset(_HDR)
    unset(_TARGET)
endmacro()
