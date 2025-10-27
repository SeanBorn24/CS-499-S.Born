# ===========================
# File: src/AuthService.cpp
# ===========================
#include "AuthService.h"
#include "RBAC.h"
#include <chrono>

AuthService::AuthService(UserRepository& repo, AuditLogger& logger)
: _repo(repo), _logger(logger) {}

bool AuthService::verifyPassword(const std::string& password, const std::string& storedHash) {
    // TODO: integrate a vetted password hashing lib (e.g., argon2, bcrypt, or PBKDF2)
    // Placeholder: NEVER do plain text compare in real systems
    return password == storedHash; // replace with proper hash verification
}

AuthResult AuthService::login(const std::string& username, const std::string& password, const std::string& ip) {
    auto u = _repo.findByUsername(username);
    if (!u || !u->active) {
        _logger.logInvalidLogin({username, ip, std::chrono::system_clock::now(), "UNKNOWN_USER_OR_INACTIVE"});
        return {false, "", "Invalid credentials"};
    }
    if (!verifyPassword(password, u->passwordHash)) {
        _logger.logInvalidLogin({username, ip, std::chrono::system_clock::now(), "INVALID_PASSWORD"});
        return {false, "", "Invalid credentials"};
    }
    return {true, u->role, "Welcome"};
}