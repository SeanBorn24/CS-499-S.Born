# ===========================
# File: src/UserRepository.h
# ===========================
#pragma once
#include <optional>
#include <string>
#include "Models.h"

class UserRepository {
public:
    bool createUser(const User& u);                     // returns false if duplicate username
    std::optional<User> findByUsername(const std::string& username);
    bool updatePasswordHash(const std::string& username, const std::string& newHash);
};