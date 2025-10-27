# ===========================
# File: src/UserRepository.cpp
# ===========================
#include "UserRepository.h"
#include "Database.h"
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;

static mongocxx::collection usersColl() {
    return Database::instance().db()["users"];
}

bool UserRepository::createUser(const User& u) {
    auto coll = usersColl();
    // enforce unique username via unique index in setup (see README script)
    document doc{};
    doc << "username" << u.username
        << "passwordHash" << u.passwordHash
        << "role" << u.role
        << "active" << u.active;
    try {
        coll.insert_one(doc.view());
        return true;
    } catch (const std::exception&) {
        return false; // likely duplicate key, or other error
    }
}

std::optional<User> UserRepository::findByUsername(const std::string& username) {
    auto coll = usersColl();
    auto result = coll.find_one(document{} << "username" << username << finalize);
    if (!result) return std::nullopt;
    auto v = result->view();

    User u{};
    if (auto id = v.find("_id"); id != v.end() && id->type() == bsoncxx::type::k_oid) {
        u.id = id->get_oid().value.to_string();
    }
    u.username = v["username"].get_string().value.to_string();
    u.passwordHash = v["passwordHash"].get_string().value.to_string();
    u.role = v["role"].get_string().value.to_string();
    if (auto it = v.find("active"); it != v.end() && it->type() == bsoncxx::type::k_bool) {
        u.active = it->get_bool().value;
    }
    return u;
}

bool UserRepository::updatePasswordHash(const std::string& username, const std::string& newHash) {
    auto coll = usersColl();
    auto result = coll.update_one(
        document{} << "username" << username << finalize,
        document{} << "$set" << open_document << "passwordHash" << newHash << close_document << finalize
    );
    return result && result->modified_count() == 1;
}