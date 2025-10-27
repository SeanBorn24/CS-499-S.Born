# ===========================
# File: src/AuditLogger.cpp
# ===========================
#include "AuditLogger.h"
#include "Database.h"
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <chrono>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;

static mongocxx::collection loginsColl() { return Database::instance().db()["invalid_login_events"]; }
static mongocxx::collection anomalyColl() { return Database::instance().db()["data_anomaly_events"]; }

static bsoncxx::types::b_date to_bdate(std::chrono::system_clock::time_point tp) {
    using namespace std::chrono;
    auto ms = duration_cast<milliseconds>(tp.time_since_epoch());
    return bsoncxx::types::b_date{bsoncxx::types::b_date::clock::time_point{milliseconds(ms.count())}};
}

void AuditLogger::logInvalidLogin(const InvalidLoginEvent& e) {
    document d{};
    d << "username" << e.username
      << "ip" << e.ip
      << "when" << to_bdate(e.when)
      << "reason" << e.reason;
    loginsColl().insert_one(d.view());
}

void AuditLogger::logDataAnomaly(const DataAnomalyEvent& e) {
    document d{};
    d << "username" << e.username
      << "type" << e.type
      << "details" << e.details
      << "when" << to_bdate(e.when);
    anomalyColl().insert_one(d.view());
}