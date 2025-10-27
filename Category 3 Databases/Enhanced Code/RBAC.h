# ===========================
# File: src/RBAC.h
# ===========================
#pragma once
#include <string>
#include <unordered_set>
#include <unordered_map>

namespace RBAC {

static inline const std::unordered_map<std::string, std::unordered_set<std::string>> ROLE_PERMS = {
    {"admin", {"user:read","user:write","user:delete","log:read","log:write"}},
    {"user",  {"user:read","log:write"}}
};

inline bool hasPermission(const std::string& role, const std::string& perm) {
    auto it = ROLE_PERMS.find(role);
    if (it == ROLE_PERMS.end()) return false;
    return it->second.count(perm) > 0;
}

} // namespace RBAC