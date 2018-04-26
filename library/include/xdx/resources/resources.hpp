#pragma once

#include <chrono>
#include <filesystem>
#include <string_view>
#include <vector>

namespace xdx::resources
{

using Content = std::vector<char>;
using ResourceId = std::string_view;

struct ResourceInfo
{
    ResourceId id;
    time_t timestamp;
    size_t size;

    constexpr ResourceInfo(ResourceId id, time_t timestamp, size_t size)
        : id{id}
        , timestamp{timestamp}
        , size{size} {
    }
};

struct iResources
{
    virtual ~iResources() = default;
    virtual bool has_resource(const ResourceId&) const = 0;
    virtual std::vector<ResourceInfo> list_resources() const = 0;
    virtual Content get_resource_content(const ResourceId&) const = 0;
};

struct EmbeddedOnlyResources : iResources
{};

class FileOrEmbeddedResources : iResources
{
public:
    FileOrEmbeddedResources(const std::filesystem::path& directory);

public:
    std::vector<ResourceInfo> list_resources() const override;
    Content get_resource_content(const ResourceId&) const override;

private:
    virtual std::vector<ResourceInfo> list_embedded_resources() const = 0;
    virtual Content get_embedded_resource_content(const ResourceId&) const = 0;

private:
    std::filesystem::path dir_;
};

}  // namespace xdx::resources
