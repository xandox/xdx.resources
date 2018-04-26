#pragma once

#include <chrono>
#include <filesystem>
#include <vector>

using Clock = std::chrono::system_clock;
using Time = Clock::time_point;

struct ResourceInfo
{
    std::filesystem::path full_path;
    std::string id;
    Time timestamp;
    size_t size;

    ResourceInfo(const std::filesystem::path& full_path, const std::string& id);
};

std::vector<ResourceInfo> find_all_resources_in_directory(const std::filesystem::path& directory,
                                                          const std::vector<std::string>& files,
                                                          const std::string& prefix);
