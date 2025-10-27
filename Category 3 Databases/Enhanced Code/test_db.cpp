# ===========================
# File: tests/test_db.cpp (optional skeleton)
# ===========================
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "Database.h"
#include "UserRepository.h"

TEST_CASE("User CRUD basic") {
    UserRepository repo;
    auto u = repo.findByUsername("testuser");
    if (u) {
        REQUIRE(repo.updatePasswordHash("testuser", "x"));
    } else {
        REQUIRE(repo.createUser({"", "testuser", "x", "user", true}));
    }
    auto u2 = repo.findByUsername("testuser");
    REQUIRE(u2);
    REQUIRE(u2->username == "testuser");
}