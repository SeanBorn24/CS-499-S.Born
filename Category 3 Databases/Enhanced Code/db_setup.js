# ===========================
# File: db_setup.js (Mongo shell helper)
# ===========================
// Run with: mongosh admin db_setup.js
// Creates least-privilege user and DB indexes

const dbName = "secure_app";
const appDb = db.getSiblingDB(dbName);

// Create application user with least privilege
try {
  db.createUser({
    user: "appUser",
    pwd:  passwordPrompt(),
    roles: [
      { role: "readWrite", db: dbName }
    ]
  });
  print("Created appUser");
} catch (e) { print(e); }

// Ensure indexes
appDb.users.createIndex({ username: 1 }, { unique: true });
appDb.invalid_login_events.createIndex({ when: 1 }, { expireAfterSeconds: 60 * 60 * 24 * 30 });
appDb.data_anomaly_events.createIndex({ when: 1 }, { expireAfterSeconds: 60 * 60 * 24 * 30 });

# ===========================
# Notes
# ===========================
# • Parameterized queries: The MongoDB C++ driver uses typed builders (no string-concatenated query text), inherently avoiding SQL injection vectors.
# • RBAC: Implemented at the app layer via RBAC::hasPermission. Pair with MongoDB DB user least-privilege role.
# • Audit Logging: Invalid logins + anomalies recorded with timestamps; TTL indexes auto-expire old entries.
# • Passwords: Replace the placeholder verifyPassword() with a proper hashing library (Argon2/bcrypt/PBKDF2).


# ===========================
# Single-file version: secure_app_single.cpp
# ===========================
# A compact, single-translation-unit build of the Category 3 enhancements so you can compile
# and run one file. You still need the MongoDB C++ driver available at compile time.

/*
Build (example):

  g++ -std=c++17 secure_app_single.cpp -o secure_app \
      `pkg-config --cflags --libs libmongocxx libmongoc-1.0`

Env:
  export MONGO_URI="mongodb://appUser:<PASS>@localhost:27017/?authSource=admin"
  export MONGO_DB="secure_app"
  export LOG_TTL_DAYS="30"

*/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/index.hpp>
#include <mongocxx/uri.hpp>

// ===========================
// Models
// ===========================
struct User {
    std::string id;            // ObjectId as hex
    std::string username;      // unique
    std::string passwordHash;  // store a real hash in production
    std::string role;          // "admin" or "user"
    bool active{true};
};

struct InvalidLoginEvent {
    std::string username;
    std::string ip;
    std::chrono::system_clock::time_point when;
    std::string reason; // e.g., "INVALID_PASSWORD"
};

// ===========================
// Database (singleton)
// ===========================
class Database {
public:
    static Database& instance() {
        static Database inst; return inst;
    }
    mongocxx::database db() { return _client->database(_dbName); }
    Database(const Database&) = delete; Database& operator=(const Database&) = delete;
private:
    Database() : _inst{} {
        const char* uri = std::getenv("MONGO_URI");
        const char* db  = std::getenv("MONGO_DB");
        if (!uri || !db) throw std::runtime_error("Set MONGO_URI and MONGO_DB env vars.");
        _client = std::make_unique<mongocxx::client>(mongocxx::uri{uri});
        _dbName = db;
    }
    mongocxx::instance _inst; // must outlive client
    std::unique_ptr<mongocxx::client> _client;
    std::string _dbName;
};

// ===========================
// RBAC
// ===========================
namespace RBAC {
static const std::unordered_map<std::string, std::unordered_set<std::string>> ROLE_PERMS = {
    {"admin", {"user:read","user:write","user:delete","log:read","log:write"}},
    {"user",  {"user:read","log:write"}},
};
inline bool hasPermission(const std::string& role, const std::string& perm) {
    auto it = ROLE_PERMS.find(role); if (it==ROLE_PERMS.end()) return false; return it->second.count(perm)>0;
}
}

// ===========================
// Audit Logger
// ===========================
class AuditLogger {
    static bsoncxx::types::b_date to_bdate(std::chrono::system_clock::time_point tp) {
        using namespace std::chrono; auto ms = duration_cast<milliseconds>(tp.time_since_epoch());
        return bsoncxx::types::b_date{bsoncxx::types::b_date::clock::time_point{milliseconds(ms.count())}};
    }
public:
    void logInvalidLogin(const InvalidLoginEvent& e) {
        using namespace bsoncxx::builder::stream;
        auto coll = Database::instance().db()["invalid_login_events"];
        document d; d << "username" << e.username << "ip" << e.ip << "when" << to_bdate(e.when) << "reason" << e.reason;
        coll.insert_one(d.view());
    }
};

// ===========================
// User Repository
// ===========================
class UserRepository {
public:
    bool createUser(const User& u) {
        using namespace bsoncxx::builder::stream;
        auto coll = Database::instance().db()["users"];
        document doc; doc << "username" << u.username << "passwordHash" << u.passwordHash << "role" << u.role << "active" << u.active;
        try { coll.insert_one(doc.view()); return true; } catch (...) { return false; }
    }
    std::optional<User> findByUsername(const std::string& username) {
        using namespace bsoncxx::builder::stream;
        auto coll = Database::instance().db()["users"];
        auto res = coll.find_one(document{} << "username" << username << finalize);
        if (!res) return std::nullopt; auto v = res->view();
        User u{}; if (auto it=v.find("_id"); it!=v.end() && it->type()==bsoncxx::type::k_oid) u.id = it->get_oid().value.to_string();
        u.username = v["username"].get_string().value.to_string();
        u.passwordHash = v["passwordHash"].get_string().value.to_string();
        u.role = v["role"].get_string().value.to_string();
        if (auto it=v.find("active"); it!=v.end() && it->type()==bsoncxx::type::k_bool) u.active = it->get_bool().value;
        return u;
    }
    bool updatePasswordHash(const std::string& username, const std::string& newHash) {
        using namespace bsoncxx::builder::stream;
        auto coll = Database::instance().db()["users"];
        auto res = coll.update_one(document{} << "username" << username << finalize,
                                   document{} << "$set" << open_document << "passwordHash" << newHash << close_document << finalize);
        return res && res->modified_count()==1;
    }
};

// ===========================
// AuthService
// ===========================
struct AuthResult { bool success; std::string role; std::string message; };

class AuthService {
public:
    AuthService(UserRepository& repo, AuditLogger& logger) : _repo(repo), _logger(logger) {}
    AuthResult login(const std::string& username, const std::string& password, const std::string& ip) {
        auto u = _repo.findByUsername(username);
        if (!u || !u->active) { _logger.logInvalidLogin({username, ip, std::chrono::system_clock::now(), "UNKNOWN_OR_INACTIVE"}); return {false, "", "Invalid credentials"}; }
        if (!verifyPassword(password, u->passwordHash)) { _logger.logInvalidLogin({username, ip, std::chrono::system_clock::now(), "INVALID_PASSWORD"}); return {false, "", "Invalid credentials"}; }
        return {true, u->role, "Welcome"};
    }
private:
    bool verifyPassword(const std::string& password, const std::string& storedHash) {
        // TODO: Replace with Argon2/bcrypt/PBKDF2 verification
        return password == storedHash; // placeholder ONLY for demo
    }
    UserRepository& _repo; AuditLogger& _logger;
};

// ===========================
// Index & TTL setup
// ===========================
static void ensureIndexesAndTTL() {
    using namespace bsoncxx::builder::stream;
    auto db = Database::instance().db();
    auto users = db["users"]; auto invalids = db["invalid_login_events"];

    try { document keys; keys << "username" << 1; mongocxx::options::index opts; opts.unique(true); users.create_index(keys.view(), opts); } catch (...) {}

    int days = 30; if (const char* env = std::getenv("LOG_TTL_DAYS")) { try { days = std::stoi(env);} catch(...){} }
    int ttl = days*24*3600;
    try { document keys; keys << "when" << 1; mongocxx::options::index opts; opts.expire_after(std::chrono::seconds(ttl)); invalids.create_index(keys.view(), opts);} catch(...){}
}

// ===========================
// Safe utility replacing unsafe samples from original Static Code.cpp
// ===========================
static int safe_sum_to_n(int n) {
    // Replaces do_something_useless with a checked accumulation
    if (n < 0 || n > 1'000'000) throw std::invalid_argument("n out of range");
    long long sum = 0; for (int i=0;i<=n;++i) sum += i; return static_cast<int>(sum);
}

// ===========================
// main
// ===========================
int main() {
    try {
        auto db = Database::instance().db();
        ensureIndexesAndTTL();

        UserRepository repo; AuditLogger logger; AuthService auth(repo, logger);

        // Seed a demo user if none exists (passwords should be hashed in production)
        if (!repo.findByUsername("admin")) {
            repo.createUser({"", "admin", "admin123", "admin", true});
            std::cout << "Seeded demo admin user (use hashed passwords in production).
";
        }

        std::string username, password;
        std::cout << "Username: "; std::getline(std::cin, username);
        std::cout << "Password: "; std::getline(std::cin, password);

        auto res = auth.login(username, password, "127.0.0.1");
        if (!res.success) { std::cout << res.message << "
"; return 1; }
        std::cout << "Logged in as role: " << res.role << "
";

        // Demonstrate RBAC-gated action
        if (RBAC::hasPermission(res.role, "user:read")) {
            std::cout << "[RBAC] You have permission to read user data.
";
        } else {
            std::cout << "[RBAC] Access denied for reading user data.
";
        }

        // Demonstrate a safe utility function (contrasts with original unsafe snippets)
        std::cout << "Safe sum to 100: " << safe_sum_to_n(100) << "
";

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "
"; return 2;
    }
}
