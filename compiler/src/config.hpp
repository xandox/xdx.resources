#pragma once

#include <chrono>
#include <filesystem>

struct Config
{
    bool embedded_only = false;
    std::filesystem::path resources_directory;
    std::vector<std::string> files;
    std::string class_name;
    std::string include_prefix;
    std::string prefix;
    std::filesystem::path source_file;
    std::filesystem::path header_file;
};
