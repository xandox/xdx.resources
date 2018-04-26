#pragma once

#include "config.hpp"
#include "resources_list.hpp"

void write_header(const Config& config);
void write_source(const Config& config, const std::vector<ResourceInfo>& resources);
