#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <sstream>

// Legacy preprocessor macros
#define MAX_BUFFER_SIZE 1024
#define MIN_STOCK_LEVEL 10
#define MAX_STOCK_LEVEL 10000
#define DEFAULT_WAREHOUSE_CAPACITY 50000
#define MAX_SUPPLIERS 100
#define MAX_CATEGORIES 50
#define DISCOUNT_THRESHOLD 1000.0
#define TAX_RATE 0.08
#define SHIPPING_BASE_COST 15.99

// Complex macro with multiple parameters
#define CALCULATE_TOTAL_COST(base, qty, discount, tax) \
    ((base * qty) * (1.0 - discount) * (1.0 + tax))

#define VALIDATE_POINTER(ptr) \
    if ((ptr) == NULL) { \
        std::cerr << "ERROR: NULL pointer at " << __FILE__ << ":" << __LINE__ << std::endl; \
        return false; \
    }

#define LOG_ERROR(msg) \
    std::cerr << "[ERROR] " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl;

#define LOG_INFO(msg) \
    std::cout << "[INFO] " << msg << std::endl;

#define LOG_DEBUG(msg) \
    std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl;

// Type definitions
typedef unsigned int ItemID;
typedef unsigned int WarehouseID;
typedef unsigned int SupplierID;
typedef unsigned int OrderID;
typedef unsigned int CustomerID;

// Enumerations
enum ItemStatus {
    STATUS_AVAILABLE = 0,
    STATUS_RESERVED = 1,
    STATUS_OUT_OF_STOCK = 2,
    STATUS_DISCONTINUED = 3,
    STATUS_ON_ORDER = 4
};

enum OrderStatus {
    ORDER_PENDING = 0,
    ORDER_PROCESSING = 1,
    ORDER_SHIPPED = 2,
    ORDER_DELIVERED = 3,
    ORDER_CANCELLED = 4,
    ORDER_RETURNED = 5
};

enum PaymentMethod {
    PAYMENT_CASH = 0,
    PAYMENT_CREDIT_CARD = 1,
    PAYMENT_DEBIT_CARD = 2,
    PAYMENT_CHECK = 3,
    PAYMENT_WIRE_TRANSFER = 4
};

enum WarehouseZone {
    ZONE_A = 0,
    ZONE_B = 1,
    ZONE_C = 2,
    ZONE_D = 3,
    ZONE_OVERFLOW = 4
};

// Global utility functions
namespace Utils {
    inline std::string intToString(int value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
    
    inline std::string doubleToString(double value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
    
    inline std::string getCurrentTimestamp() {
        time_t now = time(0);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return std::string(buffer);
    }

    inline double calculateDiscount(double totalAmount) {
        if (totalAmount > 5000.0) return 0.15;
        if (totalAmount > 2000.0) return 0.10;
        if (totalAmount > DISCOUNT_THRESHOLD) return 0.05;
        return 0.0;
    }

    inline bool validateEmail(const std::string& email) {
        return email.find('@') != std::string::npos;
    }

    inline std::string generateOrderNumber() {
        std::stringstream ss;
        ss << "ORD-" << time(0) << "-" << rand() % 10000;
        return ss.str();
    }
}

#endif // COMMON_H
