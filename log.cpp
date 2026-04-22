#include "log.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <map>

LogManager::LogManager(const std::string& financeFile, const std::string& logFile)
    : financeFile(financeFile), logFile(logFile) {
    loadFinanceRecords();
    loadOperationLogs();
}

LogManager::~LogManager() {
    saveFinanceRecords();
    saveOperationLogs();
}

std::string LogManager::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void LogManager::loadFinanceRecords() {
    std::ifstream in(financeFile, std::ios::binary);
    if (!in) return;

    financeRecords.clear();
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        FinanceRecord record;
        size_t tsLen, opIdLen, opLen;

        in.read(reinterpret_cast<char*>(&record.income), sizeof(record.income));
        in.read(reinterpret_cast<char*>(&record.expenditure), sizeof(record.expenditure));

        in.read(reinterpret_cast<char*>(&tsLen), sizeof(tsLen));
        record.timestamp.resize(tsLen);
        in.read(&record.timestamp[0], tsLen);

        in.read(reinterpret_cast<char*>(&opIdLen), sizeof(opIdLen));
        record.operatorID.resize(opIdLen);
        in.read(&record.operatorID[0], opIdLen);

        in.read(reinterpret_cast<char*>(&opLen), sizeof(opLen));
        record.operation.resize(opLen);
        in.read(&record.operation[0], opLen);

        financeRecords.push_back(record);
    }
}

void LogManager::saveFinanceRecords() const {
    std::ofstream out(financeFile, std::ios::binary);
    if (!out) return;

    size_t count = financeRecords.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& record : financeRecords) {
        out.write(reinterpret_cast<const char*>(&record.income), sizeof(record.income));
        out.write(reinterpret_cast<const char*>(&record.expenditure), sizeof(record.expenditure));

        size_t tsLen = record.timestamp.length();
        out.write(reinterpret_cast<const char*>(&tsLen), sizeof(tsLen));
        out.write(record.timestamp.c_str(), tsLen);

        size_t opIdLen = record.operatorID.length();
        out.write(reinterpret_cast<const char*>(&opIdLen), sizeof(opIdLen));
        out.write(record.operatorID.c_str(), opIdLen);

        size_t opLen = record.operation.length();
        out.write(reinterpret_cast<const char*>(&opLen), sizeof(opLen));
        out.write(record.operation.c_str(), opLen);
    }
}

void LogManager::loadOperationLogs() {
    std::ifstream in(logFile, std::ios::binary);
    if (!in) return;

    operationLogs.clear();
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        OperationLog log;
        size_t tsLen, uidLen, opLen, detLen;

        in.read(reinterpret_cast<char*>(&tsLen), sizeof(tsLen));
        log.timestamp.resize(tsLen);
        in.read(&log.timestamp[0], tsLen);

        in.read(reinterpret_cast<char*>(&uidLen), sizeof(uidLen));
        log.userID.resize(uidLen);
        in.read(&log.userID[0], uidLen);

        in.read(reinterpret_cast<char*>(&opLen), sizeof(opLen));
        log.operation.resize(opLen);
        in.read(&log.operation[0], opLen);

        in.read(reinterpret_cast<char*>(&detLen), sizeof(detLen));
        log.details.resize(detLen);
        in.read(&log.details[0], detLen);

        operationLogs.push_back(log);
    }
}

void LogManager::saveOperationLogs() const {
    std::ofstream out(logFile, std::ios::binary);
    if (!out) return;

    size_t count = operationLogs.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& log : operationLogs) {
        size_t tsLen = log.timestamp.length();
        out.write(reinterpret_cast<const char*>(&tsLen), sizeof(tsLen));
        out.write(log.timestamp.c_str(), tsLen);

        size_t uidLen = log.userID.length();
        out.write(reinterpret_cast<const char*>(&uidLen), sizeof(uidLen));
        out.write(log.userID.c_str(), uidLen);

        size_t opLen = log.operation.length();
        out.write(reinterpret_cast<const char*>(&opLen), sizeof(opLen));
        out.write(log.operation.c_str(), opLen);

        size_t detLen = log.details.length();
        out.write(reinterpret_cast<const char*>(&detLen), sizeof(detLen));
        out.write(log.details.c_str(), detLen);
    }
}

void LogManager::addFinanceRecord(double income, double expenditure,
                                 const std::string& operatorID, const std::string& operation) {
    FinanceRecord record;
    record.income = income;
    record.expenditure = expenditure;
    record.timestamp = getCurrentTimestamp();
    record.operatorID = operatorID;
    record.operation = operation;

    financeRecords.push_back(record);
    saveFinanceRecords();
}

std::pair<double, double> LogManager::getFinanceSummary(int count) {
    double totalIncome = 0.0;
    double totalExpenditure = 0.0;

    int start = 0;
    if (count > 0 && count < static_cast<int>(financeRecords.size())) {
        start = financeRecords.size() - count;
    }

    for (int i = start; i < static_cast<int>(financeRecords.size()); ++i) {
        totalIncome += financeRecords[i].income;
        totalExpenditure += financeRecords[i].expenditure;
    }

    return {totalIncome, totalExpenditure};
}

std::vector<FinanceRecord> LogManager::getFinanceRecords(int count) {
    if (count <= 0 || count >= static_cast<int>(financeRecords.size())) {
        return financeRecords;
    }

    std::vector<FinanceRecord> result;
    int start = financeRecords.size() - count;
    for (int i = start; i < static_cast<int>(financeRecords.size()); ++i) {
        result.push_back(financeRecords[i]);
    }
    return result;
}

void LogManager::addOperationLog(const std::string& userID, const std::string& operation,
                                const std::string& details) {
    OperationLog log;
    log.timestamp = getCurrentTimestamp();
    log.userID = userID;
    log.operation = operation;
    log.details = details;

    operationLogs.push_back(log);
    saveOperationLogs();
}

std::vector<OperationLog> LogManager::getOperationLogs() {
    return operationLogs;
}

std::string LogManager::generateFinanceReport() {
    std::stringstream ss;
    ss << "=== Financial Report ===\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";

    auto summary = getFinanceSummary();
    ss << "Total Income: $" << std::fixed << std::setprecision(2) << summary.first << "\n";
    ss << "Total Expenditure: $" << summary.second << "\n";
    ss << "Net Profit: $" << (summary.first - summary.second) << "\n\n";

    ss << "Recent Transactions:\n";
    auto recentRecords = getFinanceRecords(10);
    for (const auto& record : recentRecords) {
        ss << "- " << record.timestamp << ": " << record.operation
           << " by " << record.operatorID;
        if (record.income > 0) {
            ss << " (+$" << record.income << ")";
        }
        if (record.expenditure > 0) {
            ss << " (-$" << record.expenditure << ")";
        }
        ss << "\n";
    }

    return ss.str();
}

std::string LogManager::generateEmployeeReport() {
    std::stringstream ss;
    ss << "=== Employee Activity Report ===\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";

    // Group operations by user
    std::map<std::string, std::vector<OperationLog>> userOperations;
    for (const auto& log : operationLogs) {
        userOperations[log.userID].push_back(log);
    }

    for (const auto& pair : userOperations) {
        ss << "User: " << pair.first << "\n";
        ss << "Total Operations: " << pair.second.size() << "\n";

        // Count operations by type
        std::map<std::string, int> operationCounts;
        for (const auto& log : pair.second) {
            operationCounts[log.operation]++;
        }

        ss << "Operation Breakdown:\n";
        for (const auto& opPair : operationCounts) {
            ss << "  - " << opPair.first << ": " << opPair.second << "\n";
        }
        ss << "\n";
    }

    return ss.str();
}

std::string LogManager::generateFullLog() {
    std::stringstream ss;
    ss << "=== System Log ===\n";
    ss << "Generated: " << getCurrentTimestamp() << "\n\n";

    ss << "--- Operation Logs ---\n";
    for (const auto& log : operationLogs) {
        ss << "[" << log.timestamp << "] " << log.userID << " " << log.operation;
        if (!log.details.empty()) {
            ss << " (" << log.details << ")";
        }
        ss << "\n";
    }

    ss << "\n--- Finance Records ---\n";
    for (const auto& record : financeRecords) {
        ss << "[" << record.timestamp << "] " << record.operatorID << " " << record.operation;
        if (record.income > 0) {
            ss << " Income: $" << std::fixed << std::setprecision(2) << record.income;
        }
        if (record.expenditure > 0) {
            ss << " Expenditure: $" << record.expenditure;
        }
        ss << "\n";
    }

    return ss.str();
}