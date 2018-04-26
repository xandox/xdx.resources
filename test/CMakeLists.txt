
xdx_test_begin(
    NAME xdx.resources.test
    NAMESPACE xdx::resources
)

xdx_project_add_sources(
    embided_only.tests.cpp
    files_first.tests.cpp
)

xdx_project_add_resources(
    SOURCE_NAME test_embided_resources.cpp
    HEADER_NAME test_embided_resources.hpp
    DIRECTORY resources
    CLASS_NAME TestEmbidedResources
    PREFIX rr
    FILES
        1.txt
        inner_dir/2.txt
)

xdx_project_add_resources(
    FILES_FIRST
    SOURCE_NAME test_files_first_resources.cpp
    HEADER_NAME test_files_first_resources.hpp
    DIRECTORY resources
    CLASS_NAME TestFilesFirstResources
    PREFIX rf
    FILES
        1.txt
        inner_dir/2.txt
)


xdx_test_end()
