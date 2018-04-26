#include "config.hpp"
#include "generator.hpp"
#include "resources_list.hpp"

#include <xdx/cliopts/cliopts.hpp>

#include <filesystem>

int main(int argc, const char** argv) {
    xdx::cliopts::Builder optsbuilder(argv[0], "Generate cpp/hpp files for embedding resources");
    optsbuilder.flag('f', "files-first", "Resources will be able to be overridden with files on disk")
        .argument<std::string>('d', "resource-directory", "Directories for files lookup", true, "DIRECTORY")
        .argument<std::string>('c', "class-name",
                               "Class name. May be prefixed with names. Ex: my::name::space::Resources", true, "STRING")
        .argument<std::string>('S', "cpp-filename", "Path to filename for cpp", true, "FILE")
        .argument<std::string>('H', "hpp-Filename", "Path to filename for hpp", true, "FILE")
        .argument<std::string>('I', "include-prefix",
                               "Include prefix path for include header to source file. If empty will use local "
                               "include.",
                               std::string(), "STRING")
        .argument<std::string>('p', "prefix", "Resources prefix name", true, "STRING")
        .argument_list<std::string>('r', "resource", "Files to include to resources", true, "FILE")
        .flag('h', "help", "Print this message");

    const auto options = optsbuilder.get_options();

    const auto parse_results = xdx::cliopts::parse_argv(options, argc, argv);

    if (parse_results.error || options->find_flag('h')->is_set()) {
        xdx::cliopts::Printer printer(options);
        std::cerr << "Usage: " << options->get_name() << " ";
        printer.print_short(std::cerr);
        std::cerr << std::endl;
        printer.print_long(std::cerr);
        std::cerr << std::endl;
    }

    Config config;
    config.embedded_only = !options->find_flag('f')->is_set();
    config.resources_directory = options->find_typed_argument<std::string>('d')->get_value();
    config.class_name = options->find_typed_argument<std::string>('c')->get_value();
    config.source_file = options->find_typed_argument<std::string>('S')->get_value();
    config.header_file = options->find_typed_argument<std::string>('H')->get_value();
    config.include_prefix = options->find_typed_argument<std::string>('I')->get_value();
    config.files = options->find_typed_argument_list<std::string>('r')->get_values();
    config.prefix = options->find_typed_argument<std::string>('p')->get_value();

    const auto resources = find_all_resources_in_directory(config.resources_directory, config.files, config.prefix);
    write_header(config);
    write_source(config, resources);
}
