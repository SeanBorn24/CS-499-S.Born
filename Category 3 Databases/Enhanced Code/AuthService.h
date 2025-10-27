# ===========================
# File: src/AuthService.h
# ===========================
#pragma once
#include <string>
#include <optional>
#include "UserRepository.h"
#include "AuditLogger.h"

struct AuthResult {
    bool success;
    std::string role;     // empty if not authenticated
    std::string message;  // safe, non-verbose
};

class AuthService {
public:
    AuthService(UserRepository& repo, AuditLogger& logger);

    // Validate credentials (password hashing/verification abstracted)
    AuthResult login(const std::string& username, const std::string& password, const std::string& ip);

private:
    UserRepository& _repo;
    AuditLogger& _logger;

    bool verifyPassword(const std::string& password, const std::string& storedHash);
};