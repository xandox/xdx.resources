#include <xdx/format.hpp>
#include <xdx/resources/resources.hpp>

#include <fstream>

namespace xdx::resources
{
FileOrEmbeddedResources::FileOrEmbeddedResources(const std::filesystem::path& directory)
    : dir_(directory) {
}

inline bool can_read(const std::filesystem::path& file_name) {
    std::ifstream stream(file_name, std::ios::in | std::ios::binary);
    return stream.good();
}

std::vector<ResourceInfo> FileOrEmbeddedResources::list_resources() const {
    auto known_resources = list_embedded_resources();
    for (auto& res : known_resources) {
        const auto file_path = dir_ / res.id;
        if (is_regular_file(file_path) && can_read(file_path)) {
            res.size = file_size(file_path);
        }
    }
    return known_resources;
}

Content FileOrEmbeddedResources::get_resource_content(const ResourceId& id) const {
    if (!has_resource(id)) {
        throw std::invalid_argument(fmt("unknown resource id: '{id}'", {{"id", id}}));
    }

    const auto file_path = dir_ / id;
    if (is_regular_file(file_path) && can_read(file_path)) {
        auto size = file_size(file_path);
        Content content(size);
        std::ifstream stream(file_path, std::ios::in | std::ios::binary);
        size_t offset = 0;
        while (offset < size) {
            stream.read(content.data() + offset, content.size() - offset);
            offset += stream.gcount();
        }
        return content;
    }

    return get_embedded_resource_content(id);
}

}  // namespace xdx::resources
