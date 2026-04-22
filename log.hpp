#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <vector>
#include <fstream>
#include <chrono>

struct FinanceRecord {
    double income;
    double expenditure;
    std::string timestamp;
    std::string operatorID;
    std::string operation;
};

struct OperationLog {
    std::string timestamp;
    std::string userID;
    std::string operation;
    std::string details;
};

class LogManager {
private:
    std::vector<FinanceRecord> financeRecords;
    std::vector<OperationLog> operationLogs;
    std::string financeFile;
    std::string logFile;

    void loadFinanceRecords();
    void loadOperationLogs();
    void saveFinanceRecords() const;
    void saveOperationLogs() const;

    std::string getCurrentTimestamp();

public:
    LogManager(const std::string& financeFile = "finance.dat",
               const std::string& logFile = "logs.dat");
    ~LogManager();

    // Finance operations
    void addFinanceRecord(double income, double expenditure,
                         const std::string& operatorID, const std::string& operation);
    std::pair<double, double> getFinanceSummary(int count = -1);
    std::vector<FinanceRecord> getFinanceRecords(int count = -1);

    // Operation log operations
    void addOperationLog(const std::string& userID, const std::string& operation,
                        const std::string& details = "");
    std::vector<OperationLog> getOperationLogs();

    // Report generation
    std::string generateFinanceReport();
    std::string generateEmployeeReport();
    std::string generateFullLog();
};

#endif // LOG_HPP