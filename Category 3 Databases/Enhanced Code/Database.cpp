# ===========================
# File: src/Database.cpp
# ===========================
#include "Database.h"
#include <cstdlib>
#include <stdexcept>

Database& Database::instance() {
    static Database inst;
    return inst;
}

Database::Database() : _inst{} {
    const char* uri = std::getenv("MONGO_URI");
    const char* db  = std::getenv("MONGO_DB");
    if (!uri || !db) {
        throw std::runtime_error("MONGO_URI and MONGO_DB must be set as environment variables");
    }
    _client = std::make_unique<mongocxx::client>(mongocxx::uri{uri});
    _dbName = db;
}

mongocxx::database Database::db() {
    return _client->database(_dbName);
}