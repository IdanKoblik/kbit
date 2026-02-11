#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include "file_handler.h"

TEST(HandleFile, UnknownFile) {
    std::unique_ptr<TorrentFile> file = parseTorrent("notfound");
    EXPECT_EQ(file, nullptr);
}

TEST(HandleFile, ValidFile) {
    auto path = std::filesystem::path(__FILE__).parent_path() / "fixtures/fixture.torrent";
    std::unique_ptr<TorrentFile> file = parseTorrent(path.string());
    EXPECT_NE(file, nullptr);
    EXPECT_EQ(file->trackerURL, "http://tracker.test.local:6969/announce");
    EXPECT_EQ(file->infoHash, "8cbf2d88379332cc3fee09f0d0a7830f3335317a");
    EXPECT_TRUE(file->hashes.size() > 0);
}
