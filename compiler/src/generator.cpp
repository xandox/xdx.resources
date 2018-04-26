#include "generator.hpp"

#include <xdx/format.hpp>

#include <fstream>
#include <iomanip>

namespace
{

std::pair<std::vector<std::string>, std::string> namespaces_and_class(const std::string& class_name) {
    std::vector<std::string> namespaces;
    static std::string delimeter{"::"};
    size_t search_position = 0;
    for (size_t position = class_name.find(delimeter, search_position); position != class_name.npos;
         position = class_name.find(delimeter, search_position)) {

        namespaces.push_back(class_name.substr(search_position, position - search_position));
        search_position = position + delimeter.size();
    }
    return {namespaces, class_name.substr(search_position)};
}

void write_namespaces_header(std::ostream& stream, const std::vector<std::string>& namespaces) {
    using namespace std;
    if (!namespaces.empty()) {
        for (const auto& ns : namespaces) {
            xdx::print(stream, "namespace $0 \\{ "sv, ns);
        }
        stream << "\n\n";
    }
}

void write_namespaces_footer(std::ostream& stream, const std::vector<std::string>& namespaces) {
    if (!namespaces.empty()) {
        stream << '\n';
        for (size_t i = 0; i < namespaces.size(); ++i) {
            stream << "} ";
        }
    }
}

void write_include_header(std::ostream& stream, const Config& config) {
    stream << "#include ";
    if (config.include_prefix.empty()) {
        stream << "\"" << config.header_file.filename().string() << "\"\n";
    } else {
        stream << "<" << config.include_prefix;
        if (config.include_prefix.back() != '/') {
            stream << '/';
        }
        stream << config.header_file.filename().string() << ">\n";
    }
    stream << '\n';

    stream << "#include <map>\n";
    stream << "#include <utility>\n";
    stream << "#include <cstddef>\n";
    stream << "#include <vector>\n";
    stream << "#include <string>\n";

    stream << '\n';
}

void write_resources_table(std::ostream& stream, const std::vector<ResourceInfo>& resources) {
    using namespace std;
    stream << "static constexpr ::xdx::resources::ResourceInfo resource_table[] = {\n";
    for (const auto& ri : resources) {
        xdx::print(stream, "    \\{ \"$0\", $1, $2 },\n"sv, ri.id, Clock::to_time_t(ri.timestamp), ri.size);
    }
    stream << "};\n\n";
}

void write_resource_content(std::ostream& stream, const ResourceInfo& ri, size_t idx) {
    using namespace std;
    stream << std::dec;
    xdx::print(stream, "static constexpr size_t resource_$0_size = $1;\n"sv, idx, ri.size);
    xdx::print(stream, "static constexpr char resource_$0_content[] = \\{"sv, idx);

    const auto buffer_size = std::min(size_t{1024}, ri.size);
    std::vector<char> content(buffer_size);

    std::ifstream file(ri.full_path, std::ios::in | std::ios::binary);

    size_t read = 0;
    size_t writen = 0;
    while (read < ri.size) {
        content.resize(buffer_size);
        file.read(content.data(), content.size());
        content.resize(file.gcount());
        read += content.size();

        for (const auto& ch : content) {
            if ((writen % 10) == 0) {
                stream << "\n    ";
            }
            stream << "0x";
            stream << std::hex << std::setw(2) << std::setfill('0')
                   << static_cast<int>(*reinterpret_cast<const unsigned char*>(&ch)) << ", ";
            writen += 1;
        }
    }

    stream << "\n};\n\n";
}

void write_resources_content(std::ostream& stream, const std::vector<ResourceInfo>& resources) {
    for (size_t i = 0; i < resources.size(); ++i) {
        write_resource_content(stream, resources[i], i);
    }
}

void write_resources_ptr_map(std::ostream& stream, const std::vector<ResourceInfo>& resources) {
    using namespace std;
    stream << "using PtrInfo = std::pair<const char*, size_t>;\n";
    stream << "static const std::map<::xdx::resources::ResourceId, PtrInfo> resources_map = {\n";
    for (size_t i = 0; i < resources.size(); ++i) {
        const auto& ri = resources[i];
        xdx::print(stream, "    \\{ \"$0\", \\{ resource_$1_content, resource_$1_size } },\n"sv, ri.id, i);
    }
    stream << "};\n\n";
}

void write_has_function(std::ostream& stream, const std::string& class_name) {
    stream << "bool " << class_name << "::has_resource(const ::xdx::resources::ResourceId& id) const {\n";
    stream << "    return resources_map.find(id) != resources_map.end();\n";
    stream << "}\n\n";
}

void write_list_function(std::ostream& stream, const std::string& class_name, bool embedded_only) {
    stream << "std::vector<::xdx::resources::ResourceInfo> " << class_name
           << "::" << (embedded_only ? "list_resources" : "list_embedded_resources") << "() const {\n";

    stream << "    return std::vector<::xdx::resources::ResourceInfo>(resource_table, resource_table + "
              "std::size(resource_table));\n";
    stream << "}\n\n";
}

void write_get_function(std::ostream& stream, const std::string& class_name, bool embedded_only) {
    stream << "::xdx::resources::Content " << class_name
           << "::" << (embedded_only ? "get_resource_content" : "get_embedded_resource_content")
           << "(const ::xdx::resources::ResourceId& id) const {\n";

    stream << "    if (!has_resource(id)) {\n";
    stream << "        throw std::invalid_argument(std::string(\"unknown resource id '\") + std::string(id) + "
              "\"'\");\n";
    stream << "    }\n\n";

    stream << "    const auto it = resources_map.find(id);\n";
    stream << "    return ::xdx::resources::Content(it->second.first, it->second.first + it->second.second);\n";

    stream << "}\n\n";
}

}  // namespace

void write_header(const Config& config) {
    auto [namespaces, class_name] = namespaces_and_class(config.class_name);
    std::ofstream stream(config.header_file);
    if (!stream) {
        throw std::runtime_error(xdx::fmt("can't open file '$0' for write", config.header_file.string()));
    }
    stream << "#pragma once\n\n";
    stream << "#include <xdx/resources/resources.hpp>\n\n";

    write_namespaces_header(stream, namespaces);

    stream << "class " << class_name << " : public ";
    if (config.embedded_only) {
        stream << "::xdx::resources::EmbeddedOnlyResources\n{\n";
        stream << "public:\n";
        stream << "    std::vector<::xdx::resources::ResourceInfo> list_resources() const override;\n";
        stream << "    ::xdx::resources::Content get_resource_content(const ::xdx::resources::ResourceId&) const "
                  "override;\n";
        stream << "    bool has_resource(const ::xdx::resources::ResourceId&) const override;";
        stream << "};\n";

    } else {
        stream << "::xdx::resources::FileOrEmbeddedResources\n{\n";
        stream << "public:\n";
        stream << "    " << class_name << "(const std::filesystem::path& directory)\n";
        stream << "        : ::xdx::resources::FileOrEmbeddedResources(directory) {}\n\n";
        stream << "\n";
        stream << "    bool has_resource(const ::xdx::resources::ResourceId&) const override;";
        stream << "\n";
        stream << "private:\n";
        stream << "    std::vector<::xdx::resources::ResourceInfo> list_embedded_resources() const override;\n";
        stream << "    ::xdx::resources::Content get_embedded_resource_content(const ::xdx::resources::ResourceId&) "
                  "const "
                  "override;\n";
        stream << "};\n";
    }

    write_namespaces_footer(stream, namespaces);
    stream << '\n';
}

void write_source(const Config& config, const std::vector<ResourceInfo>& resources) {
    auto [namespaces, class_name] = namespaces_and_class(config.class_name);
    std::ofstream stream(config.source_file);
    if (!stream) {
        throw std::runtime_error(xdx::fmt("can't open file '$0' for write", config.source_file.string()));
    }

    write_include_header(stream, config);
    write_namespaces_header(stream, namespaces);

    stream << "namespace {\n";
    write_resources_table(stream, resources);
    write_resources_content(stream, resources);
    write_resources_ptr_map(stream, resources);
    stream << "}\n\n";

    write_has_function(stream, class_name);
    write_list_function(stream, class_name, config.embedded_only);
    write_get_function(stream, class_name, config.embedded_only);

    write_namespaces_footer(stream, namespaces);
}
