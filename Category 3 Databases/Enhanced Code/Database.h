# ===========================
# File: src/Database.h
# ===========================
#pragma once
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <memory>
#include <string>

class Database {
public:
    static Database& instance();

    mongocxx::database db();

    // Disallow copying
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

private:
    Database();

    mongocxx::instance _inst; // must live as long as the app
    std::unique_ptr<mongocxx::client> _client;
    std::string _dbName;
};