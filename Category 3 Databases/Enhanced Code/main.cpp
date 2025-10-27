# ===========================
# File: src/main.cpp
# ===========================
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "Database.h"
#include "UserRepository.h"
#include "AuditLogger.h"
#include "AuthService.h"
#include "RBAC.h"

static void ensureIndexesAndTTL();

int main() {
    try {
        // Touch DB once to ensure connection
        auto database = Database::instance().db();
        ensureIndexesAndTTL();

        UserRepository repo;
        AuditLogger logger;
        AuthService auth(repo, logger);

        // Demo: create a user if not exists (passwordHash is plaintext for demo — replace with hash!)
        if (!repo.findByUsername("admin")) {
            repo.createUser({"", "admin", "admin123", "admin", true});
            std::cout << "Seeded admin user (DEMO — replace with hashed password).\n";
        }

        std::string username, password;
        std::cout << "Username: ";
        std::getline(std::cin, username);
        std::cout << "Password: ";
        std::getline(std::cin, password);

        auto res = auth.login(username, password, "127.0.0.1");
        if (!res.success) {
            std::cout << res.message << "\n";
            return 1;
        }

        std::cout << "Logged in with role: " << res.role << "\n";

        // RBAC example usage
        if (RBAC::hasPermission(res.role, "user:read")) {
            std::cout << "You can read user data.\n";
        } else {
            std::cout << "Access denied for reading user data.\n";
        }

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 2;
    }
    return 0;
}

#include <mongocxx/collection.hpp>
#include <mongocxx/options/index.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <chrono>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;

static void ensureIndexesAndTTL() {
    auto db = Database::instance().db();
    auto users = db["users"];
    auto invalids = db["invalid_login_events"];
    auto anomalies = db["data_anomaly_events"];

    // Unique index on username
    try {
        bsoncxx::builder::stream::document keys;
        keys << "username" << 1;
        mongocxx::options::index idx_opts{};
        idx_opts.unique(true);
        users.create_index(keys.view(), idx_opts);
    } catch (...) {}

    // TTL index on event collections
    int days = 30;
    if (const char* env = std::getenv("LOG_TTL_DAYS")) {
        try { days = std::stoi(env); } catch (...) {}
    }
    int ttl_seconds = days * 24 * 3600;

    try {
        // invalid_login_events: expire after TTL based on 'when'
        bsoncxx::builder::stream::document keys;
        keys << "when" << 1;
        mongocxx::options::index idx_opts{};
        idx_opts.expire_after(std::chrono::seconds(ttl_seconds));
        invalids.create_index(keys.view(), idx_opts);
    } catch (...) {}

    try {
        // data_anomaly_events: expire after TTL based on 'when'
        bsoncxx::builder::stream::document keys;
        keys << "when" << 1;
        mongocxx::options::index idx_opts{};
        idx_opts.expire_after(std::chrono::seconds(ttl_seconds));
        anomalies.create_index(keys.view(), idx_opts);
    } catch (...) {}
}