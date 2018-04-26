#include "resources_list.hpp"

#include <set>
#include <vector>

inline std::string make_id(const std::filesystem::path& base, std::filesystem::path file, const std::string& prefix) {
    std::vector<std::string> components;
    while (file != base) {
        components.push_back(file.filename());
        file = file.parent_path();
    }
    auto begin = components.rbegin() + 1;
    auto end = components.rend();

    std::ostringstream stream;
    stream << prefix;
    for (auto it = begin; it != end; ++it) {
        stream << '/';
        stream << *it;
    }
    return stream.str();
}

ResourceInfo::ResourceInfo(const std::filesystem::path& full_path, const std::string& id)
    : full_path{full_path}
    , id{id}
    , timestamp{last_write_time(full_path)}
    , size{file_size(full_path)} {
}

std::vector<ResourceInfo> find_all_resources_in_directory(const std::filesystem::path& directory,
                                                          const std::vector<std::string>& files,
                                                          const std::string& prefix) {
    std::set<std::string> files_table;
    for (const auto& f : files) {
        files_table.insert(prefix + "/" + f);
    }
    std::vector<ResourceInfo> result;
    using namespace std::filesystem;
    recursive_directory_iterator begin{absolute(directory)};
    recursive_directory_iterator end{};

    const auto base_path = absolute(directory).parent_path();

    for (auto it = begin; it != end; ++it) {
        if (it->status().type() != file_type::regular) {
            continue;
        }
        const auto fpath = it->path();
        auto id = make_id(base_path, fpath, prefix);
        if (files_table.find(id) != files_table.end()) {
            result.emplace_back(fpath, id);
        }
    }

    return result;
}

std::vector<ResourceInfo> find_all_resources_in_source(const std::filesystem::path& filename) {
    std::vector<ResourceInfo> result;
    return result;
}
