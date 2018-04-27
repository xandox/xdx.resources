#include <gtest/gtest.h>

#include <xdx/resources/test_embided_resources.hpp>

TEST(xdx_resources_embided_only_tests, has_resources) {
    xdx::resources::TestEmbidedResources resources;
    ASSERT_TRUE(resources.has_resource("rr/1.txt"));
    ASSERT_TRUE(resources.has_resource("rr/inner_dir/2.txt"));
    ASSERT_FALSE(resources.has_resource("rr/some_not_exists"));
}

TEST(xdx_resources_embided_only_tests, resources_info) {
    xdx::resources::TestEmbidedResources resources;
    const auto rlist = resources.list_resources();
    ASSERT_EQ(2, rlist.size());
    ASSERT_EQ("rr/1.txt", rlist[0].id);
    ASSERT_EQ("rr/inner_dir/2.txt", rlist[1].id);
}

TEST(xdx_resources_embided_only_tests, content) {
    xdx::resources::TestEmbidedResources resources;

    {
        const auto content = resources.get_resource_content("rr/1.txt");
        std::cout << std::string(content.begin(), content.end()) << std::endl;
    }

    {
        const auto content = resources.get_resource_content("rr/inner_dir/2.txt");
        std::cout << std::string(content.begin(), content.end()) << std::endl;
    }
}
