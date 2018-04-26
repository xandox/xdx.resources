
xdx_exe_begin(
    NAME xdx.resources.builder
    NAMESPACE xdx::resources
)

xdx_project_add_sources(
        config.hpp
        generator.cpp
        generator.hpp
        resources_builder.cpp
        resources_list.cpp
        resources_list.hpp
)

xdx_project_add_private_links(
        xdx.format
        xdx.cliopts
)

xdx_exe_end()
