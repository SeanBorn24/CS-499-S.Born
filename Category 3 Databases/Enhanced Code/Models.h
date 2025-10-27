# ===========================
# File: src/Models.h
# ===========================
#pragma once
#include <string>
#include <optional>
#include <chrono>

struct User {
    std::string id;            // Mongo _id as string (ObjectId hex)
    std::string username;      // unique
    std::string passwordHash;  // store hash, not plaintext
    std::string role;          // "admin", "user", ...
    bool        active{true};
};

struct InvalidLoginEvent {
    std::string username;
    std::string ip;
    std::chrono::system_clock::time_point when;
    std::string reason; // e.g., "INVALID_PASSWORD"
};

struct DataAnomalyEvent {
    std::string username;
    std::string type;   // e.g., "RATE_LIMIT", "MALFORMED_INPUT"
    std::string details;
    std::chrono::system_clock::time_point when;
};