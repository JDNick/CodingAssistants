#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "Common.h"

// Forward declarations
class Order;
class Customer;
class Item;
class Warehouse;

// Sales analytics data structure
struct SalesMetrics {
    double totalRevenue;
    double averageOrderValue;
    int totalOrders;
    int totalItemsSold;
    double conversionRate;
    double customerRetentionRate;
    
    SalesMetrics() : totalRevenue(0.0), averageOrderValue(0.0), 
                     totalOrders(0), totalItemsSold(0),
                     conversionRate(0.0), customerRetentionRate(0.0) {}
};

// Inventory analytics
struct InventoryMetrics {
    int totalItems;
    int totalSKUs;
    double totalValue;
    int lowStockItems;
    int outOfStockItems;
    double turnoverRate;
    double averageAge;
    
    InventoryMetrics() : totalItems(0), totalSKUs(0), totalValue(0.0),
                         lowStockItems(0), outOfStockItems(0),
                         turnoverRate(0.0), averageAge(0.0) {}
};

// Customer analytics
struct CustomerMetrics {
    int totalCustomers;
    int activeCustomers;
    double averageLifetimeValue;
    double averageOrdersPerCustomer;
    int newCustomersThisMonth;
    double churnRate;
    
    CustomerMetrics() : totalCustomers(0), activeCustomers(0),
                        averageLifetimeValue(0.0), averageOrdersPerCustomer(0.0),
                        newCustomersThisMonth(0), churnRate(0.0) {}
};

// Time series data point
struct DataPoint {
    time_t timestamp;
    double value;
    std::string label;
    
    DataPoint() : timestamp(0), value(0.0), label("") {}
    DataPoint(time_t t, double v, const std::string& l) 
        : timestamp(t), value(v), label(l) {}
};

// Analytics Engine - Singleton
class AnalyticsEngine {
private:
    static AnalyticsEngine* instance;
    std::vector<DataPoint> revenueHistory;
    std::vector<DataPoint> orderHistory;
    std::map<ItemID, int> itemSalesCount;
    std::map<CustomerID, double> customerSpending;
    
    // Private constructor
    AnalyticsEngine();
    ~AnalyticsEngine();
    
    // Prevent copying
    AnalyticsEngine(const AnalyticsEngine&);
    AnalyticsEngine& operator=(const AnalyticsEngine&);
    
public:
    static AnalyticsEngine* getInstance();
    static void destroyInstance();
    
    // Data recording
    void recordSale(Order* order);
    void recordItemSale(ItemID itemId, int quantity);
    void recordCustomerSpending(CustomerID custId, double amount);
    
    // Metrics calculation
    SalesMetrics calculateSalesMetrics() const;
    InventoryMetrics calculateInventoryMetrics() const;
    CustomerMetrics calculateCustomerMetrics() const;
    
    // Trend analysis
    std::vector<DataPoint> getRevenueTrend(int days) const;
    std::vector<DataPoint> getOrderTrend(int days) const;
    std::vector<Item*> getTopSellingItems(int count) const;
    std::vector<Customer*> getTopSpendingCustomers(int count) const;
    
    // Forecasting
    double forecastRevenue(int daysAhead) const;
    int forecastOrderVolume(int daysAhead) const;
    
    // Reporting
    void generateSalesReport() const;
    void generateInventoryReport() const;
    void generateCustomerReport() const;
    void generateComprehensiveReport() const;
    
    // Utility
    void clearAnalyticsData();
};

// Report generator
class ReportGenerator {
private:
    std::string reportTitle;
    std::stringstream reportContent;
    time_t generatedDate;
    
public:
    ReportGenerator(const std::string& title);
    ~ReportGenerator();
    
    void addSection(const std::string& sectionTitle);
    void addLine(const std::string& line);
    void addKeyValue(const std::string& key, const std::string& value);
    void addKeyValue(const std::string& key, double value);
    void addKeyValue(const std::string& key, int value);
    void addTable(const std::vector<std::vector<std::string> >& data);
    void addSeparator();
    
    std::string generate() const;
    void saveToFile(const std::string& filename) const;
    void display() const;
};

// Performance monitor
class PerformanceMonitor {
private:
    static PerformanceMonitor* instance;
    std::map<std::string, clock_t> timers;
    std::map<std::string, long> counters;
    std::map<std::string, std::vector<double> > metrics;
    
    // Private constructor
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Prevent copying
    PerformanceMonitor(const PerformanceMonitor&);
    PerformanceMonitor& operator=(const PerformanceMonitor&);
    
public:
    static PerformanceMonitor* getInstance();
    static void destroyInstance();
    
    // Timer methods
    void startTimer(const std::string& name);
    void stopTimer(const std::string& name);
    double getTimerValue(const std::string& name) const;
    
    // Counter methods
    void incrementCounter(const std::string& name);
    void setCounter(const std::string& name, long value);
    long getCounter(const std::string& name) const;
    
    // Metric tracking
    void recordMetric(const std::string& name, double value);
    double getAverageMetric(const std::string& name) const;
    double getMinMetric(const std::string& name) const;
    double getMaxMetric(const std::string& name) const;
    
    // Reporting
    void displayPerformanceReport() const;
    void resetAllMetrics();
};

// Data validator
class DataValidator {
public:
    static bool validateItemData(Item* item);
    static bool validateOrderData(Order* order);
    static bool validateCustomerData(Customer* customer);
    static bool validateWarehouseData(Warehouse* warehouse);
    
    static std::vector<std::string> findDataInconsistencies();
    static bool performDataIntegrityCheck();
    static void generateValidationReport();
};

// Business intelligence helpers
namespace BusinessIntelligence {
    // Market basket analysis
    struct ItemPair {
        ItemID item1;
        ItemID item2;
        int coOccurrences;
        double confidence;
        
        ItemPair() : item1(0), item2(0), coOccurrences(0), confidence(0.0) {}
    };
    
    std::vector<ItemPair> findFrequentItemPairs();
    std::vector<Item*> getRecommendationsForItem(ItemID itemId);
    
    // Customer segmentation
    enum CustomerSegment {
        SEGMENT_HIGH_VALUE,
        SEGMENT_MEDIUM_VALUE,
        SEGMENT_LOW_VALUE,
        SEGMENT_AT_RISK,
        SEGMENT_NEW
    };
    
    CustomerSegment categorizeCustomer(Customer* customer);
    std::vector<Customer*> getCustomersBySegment(CustomerSegment segment);
    
    // Price optimization
    double calculateOptimalPrice(Item* item);
    double calculatePriceElasticity(Item* item);
    std::map<ItemID, double> suggestPriceAdjustments();
    
    // Demand forecasting
    int forecastDemand(ItemID itemId, int daysAhead);
    std::map<ItemID, int> getReorderSuggestions();
}

#endif // ANALYTICS_H
