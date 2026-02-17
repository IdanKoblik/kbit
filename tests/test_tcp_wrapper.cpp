#include "net/tcp_wrapper.h"
#include "net/resolve.h"
#include <gtest/gtest.h>

namespace {

class TcpWrapperTest : public ::testing::Test {
protected:
    net::TcpWrapper tcp;
};

TEST_F(TcpWrapperTest, ResolveGoogleCom) {
    auto info = net::resolve("google.com", 80);
    EXPECT_FALSE(info.host.empty());
    EXPECT_EQ(info.port, 80);

    std::cout << "Resolved host: " << info.host << ", port: " << info.port << std::endl;
}

TEST_F(TcpWrapperTest, ConnectGoogleCom) {
    auto info = net::resolve("google.com", 80);
    ASSERT_FALSE(info.host.empty());

    bool connected = tcp.connect(info);
    EXPECT_TRUE(connected);
}

};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
