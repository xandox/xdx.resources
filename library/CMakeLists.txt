
xdx_static_lib_begin(
    NAME xdx.resources
    NAMESPACE xdx::resources
)

xdx_project_add_headers(
    resources.hpp
)

xdx_project_add_sources(
    resources.cpp
)

xdx_project_add_public_links(
    xdx.format
)

xdx_static_lib_end()

