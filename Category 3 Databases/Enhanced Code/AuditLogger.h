# ===========================
# File: src/AuditLogger.h
# ===========================
#pragma once
#include <string>
#include "Models.h"

class AuditLogger {
public:
    void logInvalidLogin(const InvalidLoginEvent& e);
    void logDataAnomaly(const DataAnomalyEvent& e);
};