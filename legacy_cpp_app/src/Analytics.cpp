#include "Analytics.h"
#include "Item.h"
#include "Order.h"
#include "Customer.h"
#include "Warehouse.h"
#include <fstream>
#include <iomanip>
#include <set>

// AnalyticsEngine implementation (Singleton)
AnalyticsEngine* AnalyticsEngine::instance = NULL;

AnalyticsEngine::AnalyticsEngine() {
}

AnalyticsEngine::~AnalyticsEngine() {
    revenueHistory.clear();
    orderHistory.clear();
    itemSalesCount.clear();
    customerSpending.clear();
}

AnalyticsEngine* AnalyticsEngine::getInstance() {
    if (instance == NULL) {
        instance = new AnalyticsEngine();
    }
    return instance;
}

void AnalyticsEngine::destroyInstance() {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

void AnalyticsEngine::recordSale(Order* order) {
    if (order == NULL) return;
    
    time_t now = time(0);
    double amount = order->calculateTotalWithShipping();
    
    std::stringstream ss;
    ss << "Order #" << order->getId();
    DataPoint revenuePoint(now, amount, ss.str());
    revenueHistory.push_back(revenuePoint);
    
    DataPoint orderPoint(now, 1.0, ss.str());
    orderHistory.push_back(orderPoint);
}

void AnalyticsEngine::recordItemSale(ItemID itemId, int quantity) {
    std::map<ItemID, int>::iterator it = itemSalesCount.find(itemId);
    if (it != itemSalesCount.end()) {
        it->second += quantity;
    } else {
        itemSalesCount[itemId] = quantity;
    }
}

void AnalyticsEngine::recordCustomerSpending(CustomerID custId, double amount) {
    std::map<CustomerID, double>::iterator it = customerSpending.find(custId);
    if (it != customerSpending.end()) {
        it->second += amount;
    } else {
        customerSpending[custId] = amount;
    }
}

SalesMetrics AnalyticsEngine::calculateSalesMetrics() const {
    SalesMetrics metrics;
    
    // Calculate total revenue
    for (std::vector<DataPoint>::const_iterator it = revenueHistory.begin();
         it != revenueHistory.end(); ++it) {
        metrics.totalRevenue += it->value;
    }
    
    // Calculate total orders
    metrics.totalOrders = static_cast<int>(orderHistory.size());
    
    // Calculate average order value
    if (metrics.totalOrders > 0) {
        metrics.averageOrderValue = metrics.totalRevenue / metrics.totalOrders;
    }
    
    // Calculate total items sold
    for (std::map<ItemID, int>::const_iterator it = itemSalesCount.begin();
         it != itemSalesCount.end(); ++it) {
        metrics.totalItemsSold += it->second;
    }
    
    // Estimate conversion rate (simplified)
    CustomerManager* custMgr = CustomerManager::getInstance();
    int totalCustomers = custMgr->getTotalCustomerCount();
    if (totalCustomers > 0) {
        metrics.conversionRate = (static_cast<double>(customerSpending.size()) / totalCustomers) * 100.0;
    }
    
    // Estimate retention rate (simplified)
    int repeatCustomers = 0;
    for (std::map<CustomerID, double>::const_iterator it = customerSpending.begin();
         it != customerSpending.end(); ++it) {
        Customer* cust = custMgr->getCustomer(it->first);
        if (cust != NULL && cust->getTotalOrders() > 1) {
            repeatCustomers++;
        }
    }
    if (customerSpending.size() > 0) {
        metrics.customerRetentionRate = (static_cast<double>(repeatCustomers) / customerSpending.size()) * 100.0;
    }
    
    return metrics;
}

InventoryMetrics AnalyticsEngine::calculateInventoryMetrics() const {
    InventoryMetrics metrics;
    
    WarehouseNetwork* whNetwork = WarehouseNetwork::getInstance();
    std::vector<Warehouse*> warehouses = whNetwork->getAllWarehouses();
    
    std::set<ItemID> uniqueItems;
    
    for (std::vector<Warehouse*>::const_iterator it = warehouses.begin();
         it != warehouses.end(); ++it) {
        std::vector<InventoryRecord*> records = (*it)->getAllInventoryRecords();
        
        for (std::vector<InventoryRecord*>::const_iterator recIt = records.begin();
             recIt != records.end(); ++recIt) {
            InventoryRecord* record = *recIt;
            if (record == NULL || record->getItem() == NULL) continue;
            
            ItemID itemId = record->getItem()->getId();
            uniqueItems.insert(itemId);
            
            metrics.totalItems += record->getQuantity();
            metrics.totalValue += record->getItem()->calculatePrice() * record->getQuantity();
            
            if (record->getQuantity() == 0) {
                metrics.outOfStockItems++;
            } else if (record->needsReorder()) {
                metrics.lowStockItems++;
            }
        }
    }
    
    metrics.totalSKUs = static_cast<int>(uniqueItems.size());
    
    // Calculate turnover rate (simplified)
    if (metrics.totalItems > 0) {
        metrics.turnoverRate = (static_cast<double>(itemSalesCount.size()) / metrics.totalSKUs) * 100.0;
    }
    
    return metrics;
}

CustomerMetrics AnalyticsEngine::calculateCustomerMetrics() const {
    CustomerMetrics metrics;
    
    CustomerManager* custMgr = CustomerManager::getInstance();
    std::vector<Customer*> customers = custMgr->getAllCustomers();
    
    metrics.totalCustomers = static_cast<int>(customers.size());
    
    double totalLifetimeValue = 0.0;
    int totalOrders = 0;
    time_t oneMonthAgo = time(0) - (30 * 24 * 60 * 60);
    
    for (std::vector<Customer*>::const_iterator it = customers.begin();
         it != customers.end(); ++it) {
        Customer* cust = *it;
        if (cust == NULL) continue;
        
        if (cust->getIsActive()) {
            metrics.activeCustomers++;
        }
        
        totalLifetimeValue += cust->getLifetimeValue();
        totalOrders += cust->getTotalOrders();
        
        if (cust->getRegistrationDate() >= oneMonthAgo) {
            metrics.newCustomersThisMonth++;
        }
        
        // Calculate churn (customers who haven't ordered in 90 days)
        int daysSinceLastPurchase = cust->getDaysSinceLastPurchase();
        if (daysSinceLastPurchase > 90) {
            metrics.churnRate += 1.0;
        }
    }
    
    if (metrics.totalCustomers > 0) {
        metrics.averageLifetimeValue = totalLifetimeValue / metrics.totalCustomers;
        metrics.averageOrdersPerCustomer = static_cast<double>(totalOrders) / metrics.totalCustomers;
        metrics.churnRate = (metrics.churnRate / metrics.totalCustomers) * 100.0;
    }
    
    return metrics;
}

std::vector<DataPoint> AnalyticsEngine::getRevenueTrend(int days) const {
    std::vector<DataPoint> trend;
    time_t cutoff = time(0) - (days * 24 * 60 * 60);
    
    for (std::vector<DataPoint>::const_iterator it = revenueHistory.begin();
         it != revenueHistory.end(); ++it) {
        if (it->timestamp >= cutoff) {
            trend.push_back(*it);
        }
    }
    
    return trend;
}

std::vector<DataPoint> AnalyticsEngine::getOrderTrend(int days) const {
    std::vector<DataPoint> trend;
    time_t cutoff = time(0) - (days * 24 * 60 * 60);
    
    for (std::vector<DataPoint>::const_iterator it = orderHistory.begin();
         it != orderHistory.end(); ++it) {
        if (it->timestamp >= cutoff) {
            trend.push_back(*it);
        }
    }
    
    return trend;
}

std::vector<Item*> AnalyticsEngine::getTopSellingItems(int count) const {
    std::vector<Item*> topItems;
    
    // Create a sorted list of items by sales count
    std::vector<std::pair<int, ItemID> > sortedItems;
    for (std::map<ItemID, int>::const_iterator it = itemSalesCount.begin();
         it != itemSalesCount.end(); ++it) {
        sortedItems.push_back(std::make_pair(it->second, it->first));
    }
    
    // Sort descending by sales count
    for (size_t i = 0; i < sortedItems.size(); ++i) {
        for (size_t j = i + 1; j < sortedItems.size(); ++j) {
            if (sortedItems[j].first > sortedItems[i].first) {
                std::pair<int, ItemID> temp = sortedItems[i];
                sortedItems[i] = sortedItems[j];
                sortedItems[j] = temp;
            }
        }
    }
    
    // Get top N items (simplified - would need item lookup in real system)
    int limit = count < static_cast<int>(sortedItems.size()) ? count : static_cast<int>(sortedItems.size());
    for (int i = 0; i < limit; ++i) {
        // In a real system, we'd look up the item from inventory
        // For now, we just track the IDs
        topItems.push_back(NULL); // Placeholder
    }
    
    return topItems;
}

std::vector<Customer*> AnalyticsEngine::getTopSpendingCustomers(int count) const {
    std::vector<Customer*> topCustomers;
    
    // Create a sorted list of customers by spending
    std::vector<std::pair<double, CustomerID> > sortedCustomers;
    for (std::map<CustomerID, double>::const_iterator it = customerSpending.begin();
         it != customerSpending.end(); ++it) {
        sortedCustomers.push_back(std::make_pair(it->second, it->first));
    }
    
    // Sort descending by spending
    for (size_t i = 0; i < sortedCustomers.size(); ++i) {
        for (size_t j = i + 1; j < sortedCustomers.size(); ++j) {
            if (sortedCustomers[j].first > sortedCustomers[i].first) {
                std::pair<double, CustomerID> temp = sortedCustomers[i];
                sortedCustomers[i] = sortedCustomers[j];
                sortedCustomers[j] = temp;
            }
        }
    }
    
    // Get top N customers
    CustomerManager* custMgr = CustomerManager::getInstance();
    int limit = count < static_cast<int>(sortedCustomers.size()) ? count : static_cast<int>(sortedCustomers.size());
    for (int i = 0; i < limit; ++i) {
        Customer* cust = custMgr->getCustomer(sortedCustomers[i].second);
        if (cust != NULL) {
            topCustomers.push_back(cust);
        }
    }
    
    return topCustomers;
}

double AnalyticsEngine::forecastRevenue(int daysAhead) const {
    // Simple linear forecast based on recent trend
    if (revenueHistory.empty()) return 0.0;
    
    std::vector<DataPoint> recentData = getRevenueTrend(30);
    if (recentData.empty()) return 0.0;
    
    double totalRevenue = 0.0;
    for (std::vector<DataPoint>::const_iterator it = recentData.begin();
         it != recentData.end(); ++it) {
        totalRevenue += it->value;
    }
    
    double dailyAverage = totalRevenue / 30.0;
    return dailyAverage * daysAhead;
}

int AnalyticsEngine::forecastOrderVolume(int daysAhead) const {
    // Simple forecast based on recent order rate
    std::vector<DataPoint> recentOrders = getOrderTrend(30);
    if (recentOrders.empty()) return 0;
    
    double dailyAverage = static_cast<double>(recentOrders.size()) / 30.0;
    return static_cast<int>(dailyAverage * daysAhead);
}

void AnalyticsEngine::generateSalesReport() const {
    SalesMetrics metrics = calculateSalesMetrics();
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║         SALES ANALYTICS REPORT         ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Total Revenue: $" << std::fixed << std::setprecision(2) << metrics.totalRevenue << "\n";
    std::cout << "Total Orders: " << metrics.totalOrders << "\n";
    std::cout << "Average Order Value: $" << metrics.averageOrderValue << "\n";
    std::cout << "Total Items Sold: " << metrics.totalItemsSold << "\n";
    std::cout << "Conversion Rate: " << metrics.conversionRate << "%\n";
    std::cout << "Customer Retention: " << metrics.customerRetentionRate << "%\n";
    std::cout << "\n";
    std::cout << "Forecast (30 days):\n";
    std::cout << "  Expected Revenue: $" << forecastRevenue(30) << "\n";
    std::cout << "  Expected Orders: " << forecastOrderVolume(30) << "\n";
}

void AnalyticsEngine::generateInventoryReport() const {
    InventoryMetrics metrics = calculateInventoryMetrics();
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║      INVENTORY ANALYTICS REPORT        ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Total Items: " << metrics.totalItems << "\n";
    std::cout << "Total SKUs: " << metrics.totalSKUs << "\n";
    std::cout << "Total Value: $" << std::fixed << std::setprecision(2) << metrics.totalValue << "\n";
    std::cout << "Low Stock Items: " << metrics.lowStockItems << "\n";
    std::cout << "Out of Stock Items: " << metrics.outOfStockItems << "\n";
    std::cout << "Turnover Rate: " << metrics.turnoverRate << "%\n";
}

void AnalyticsEngine::generateCustomerReport() const {
    CustomerMetrics metrics = calculateCustomerMetrics();
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║      CUSTOMER ANALYTICS REPORT         ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Total Customers: " << metrics.totalCustomers << "\n";
    std::cout << "Active Customers: " << metrics.activeCustomers << "\n";
    std::cout << "Average Lifetime Value: $" << std::fixed << std::setprecision(2) << metrics.averageLifetimeValue << "\n";
    std::cout << "Average Orders per Customer: " << std::setprecision(1) << metrics.averageOrdersPerCustomer << "\n";
    std::cout << "New Customers (This Month): " << metrics.newCustomersThisMonth << "\n";
    std::cout << "Churn Rate: " << std::setprecision(2) << metrics.churnRate << "%\n";
}

void AnalyticsEngine::generateComprehensiveReport() const {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║    COMPREHENSIVE ANALYTICS REPORT      ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    
    char buffer[80];
    time_t now = time(0);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    std::cout << "\nReport Generated: " << buffer << "\n";
    
    generateSalesReport();
    generateInventoryReport();
    generateCustomerReport();
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║            TOP PERFORMERS              ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    std::vector<Customer*> topCustomers = getTopSpendingCustomers(5);
    std::cout << "Top 5 Spending Customers:\n";
    for (size_t i = 0; i < topCustomers.size(); ++i) {
        if (topCustomers[i] != NULL) {
            std::cout << "  " << (i + 1) << ". " << topCustomers[i]->getFullName()
                     << " - $" << std::fixed << std::setprecision(2) 
                     << topCustomers[i]->getLifetimeValue() << "\n";
        }
    }
}

void AnalyticsEngine::clearAnalyticsData() {
    revenueHistory.clear();
    orderHistory.clear();
    itemSalesCount.clear();
    customerSpending.clear();
}

// ReportGenerator implementation
ReportGenerator::ReportGenerator(const std::string& title) 
    : reportTitle(title), generatedDate(time(0)) {
    reportContent << "╔════════════════════════════════════════╗\n";
    reportContent << "║  " << std::setw(38) << std::left << title << "║\n";
    reportContent << "╚════════════════════════════════════════╝\n\n";
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&generatedDate));
    reportContent << "Generated: " << buffer << "\n\n";
}

ReportGenerator::~ReportGenerator() {
}

void ReportGenerator::addSection(const std::string& sectionTitle) {
    reportContent << "\n";
    reportContent << "═══ " << sectionTitle << " ═══\n";
    reportContent << "\n";
}

void ReportGenerator::addLine(const std::string& line) {
    reportContent << line << "\n";
}

void ReportGenerator::addKeyValue(const std::string& key, const std::string& value) {
    reportContent << std::setw(30) << std::left << key << ": " << value << "\n";
}

void ReportGenerator::addKeyValue(const std::string& key, double value) {
    reportContent << std::setw(30) << std::left << key << ": $" 
                 << std::fixed << std::setprecision(2) << value << "\n";
}

void ReportGenerator::addKeyValue(const std::string& key, int value) {
    reportContent << std::setw(30) << std::left << key << ": " << value << "\n";
}

void ReportGenerator::addTable(const std::vector<std::vector<std::string> >& data) {
    if (data.empty()) return;
    
    // Calculate column widths
    std::vector<size_t> colWidths(data[0].size(), 0);
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size() && j < colWidths.size(); ++j) {
            if (data[i][j].length() > colWidths[j]) {
                colWidths[j] = data[i][j].length();
            }
        }
    }
    
    // Print table
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            reportContent << std::setw(static_cast<int>(colWidths[j] + 2)) << std::left << data[i][j];
        }
        reportContent << "\n";
        
        // Add separator after header
        if (i == 0) {
            for (size_t j = 0; j < colWidths.size(); ++j) {
                reportContent << std::string(colWidths[j] + 2, '-');
            }
            reportContent << "\n";
        }
    }
}

void ReportGenerator::addSeparator() {
    reportContent << "----------------------------------------\n";
}

std::string ReportGenerator::generate() const {
    return reportContent.str();
}

void ReportGenerator::saveToFile(const std::string& filename) const {
    std::ofstream file(filename.c_str());
    if (file.is_open()) {
        file << generate();
        file.close();
        LOG_INFO("Report saved to: " + filename);
    } else {
        LOG_ERROR("Failed to save report to: " + filename);
    }
}

void ReportGenerator::display() const {
    std::cout << generate();
}

// PerformanceMonitor implementation (Singleton)
PerformanceMonitor* PerformanceMonitor::instance = NULL;

PerformanceMonitor::PerformanceMonitor() {
}

PerformanceMonitor::~PerformanceMonitor() {
    timers.clear();
    counters.clear();
    metrics.clear();
}

PerformanceMonitor* PerformanceMonitor::getInstance() {
    if (instance == NULL) {
        instance = new PerformanceMonitor();
    }
    return instance;
}

void PerformanceMonitor::destroyInstance() {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

void PerformanceMonitor::startTimer(const std::string& name) {
    timers[name] = clock();
}

void PerformanceMonitor::stopTimer(const std::string& name) {
    std::map<std::string, clock_t>::iterator it = timers.find(name);
    if (it != timers.end()) {
        clock_t elapsed = clock() - it->second;
        double seconds = static_cast<double>(elapsed) / CLOCKS_PER_SEC;
        recordMetric(name + "_time", seconds);
    }
}

double PerformanceMonitor::getTimerValue(const std::string& name) const {
    std::map<std::string, clock_t>::const_iterator it = timers.find(name);
    if (it != timers.end()) {
        clock_t elapsed = clock() - it->second;
        return static_cast<double>(elapsed) / CLOCKS_PER_SEC;
    }
    return 0.0;
}

void PerformanceMonitor::incrementCounter(const std::string& name) {
    counters[name]++;
}

void PerformanceMonitor::setCounter(const std::string& name, long value) {
    counters[name] = value;
}

long PerformanceMonitor::getCounter(const std::string& name) const {
    std::map<std::string, long>::const_iterator it = counters.find(name);
    if (it != counters.end()) {
        return it->second;
    }
    return 0;
}

void PerformanceMonitor::recordMetric(const std::string& name, double value) {
    metrics[name].push_back(value);
}

double PerformanceMonitor::getAverageMetric(const std::string& name) const {
    std::map<std::string, std::vector<double> >::const_iterator it = metrics.find(name);
    if (it == metrics.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (std::vector<double>::const_iterator vit = it->second.begin();
         vit != it->second.end(); ++vit) {
        sum += *vit;
    }
    return sum / it->second.size();
}

double PerformanceMonitor::getMinMetric(const std::string& name) const {
    std::map<std::string, std::vector<double> >::const_iterator it = metrics.find(name);
    if (it == metrics.end() || it->second.empty()) {
        return 0.0;
    }
    
    double minVal = it->second[0];
    for (std::vector<double>::const_iterator vit = it->second.begin();
         vit != it->second.end(); ++vit) {
        if (*vit < minVal) minVal = *vit;
    }
    return minVal;
}

double PerformanceMonitor::getMaxMetric(const std::string& name) const {
    std::map<std::string, std::vector<double> >::const_iterator it = metrics.find(name);
    if (it == metrics.end() || it->second.empty()) {
        return 0.0;
    }
    
    double maxVal = it->second[0];
    for (std::vector<double>::const_iterator vit = it->second.begin();
         vit != it->second.end(); ++vit) {
        if (*vit > maxVal) maxVal = *vit;
    }
    return maxVal;
}

void PerformanceMonitor::displayPerformanceReport() const {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║       PERFORMANCE REPORT               ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    std::cout << "Counters:\n";
    for (std::map<std::string, long>::const_iterator it = counters.begin();
         it != counters.end(); ++it) {
        std::cout << "  " << std::setw(30) << std::left << it->first 
                 << ": " << it->second << "\n";
    }
    
    std::cout << "\nMetrics:\n";
    for (std::map<std::string, std::vector<double> >::const_iterator it = metrics.begin();
         it != metrics.end(); ++it) {
        if (!it->second.empty()) {
            std::cout << "  " << it->first << ":\n";
            std::cout << "    Average: " << std::fixed << std::setprecision(6) 
                     << getAverageMetric(it->first) << "\n";
            std::cout << "    Min: " << getMinMetric(it->first) << "\n";
            std::cout << "    Max: " << getMaxMetric(it->first) << "\n";
        }
    }
}

void PerformanceMonitor::resetAllMetrics() {
    timers.clear();
    counters.clear();
    metrics.clear();
}

// DataValidator implementation
bool DataValidator::validateItemData(Item* item) {
    if (item == NULL) return false;
    
    if (item->getName().empty()) {
        LOG_ERROR("Item has empty name");
        return false;
    }
    
    if (item->getBasePrice() < 0) {
        LOG_ERROR("Item has negative price");
        return false;
    }
    
    if (item->getStockQuantity() < 0) {
        LOG_ERROR("Item has negative stock");
        return false;
    }
    
    return true;
}

bool DataValidator::validateOrderData(Order* order) {
    if (order == NULL) return false;
    
    if (order->getCustomer() == NULL) {
        LOG_ERROR("Order has no customer");
        return false;
    }
    
    if (order->getItemCount() == 0) {
        LOG_ERROR("Order has no items");
        return false;
    }
    
    if (order->getShippingAddress().empty()) {
        LOG_ERROR("Order has no shipping address");
        return false;
    }
    
    return true;
}

bool DataValidator::validateCustomerData(Customer* customer) {
    if (customer == NULL) return false;
    
    if (customer->getFirstName().empty() || customer->getLastName().empty()) {
        LOG_ERROR("Customer has incomplete name");
        return false;
    }
    
    if (!customer->getEmail().empty() && !Utils::validateEmail(customer->getEmail())) {
        LOG_ERROR("Customer has invalid email");
        return false;
    }
    
    return true;
}

bool DataValidator::validateWarehouseData(Warehouse* warehouse) {
    if (warehouse == NULL) return false;
    
    if (warehouse->getName().empty()) {
        LOG_ERROR("Warehouse has empty name");
        return false;
    }
    
    if (warehouse->getCurrentCapacity() > warehouse->getMaxCapacity()) {
        LOG_ERROR("Warehouse capacity exceeded");
        return false;
    }
    
    return true;
}

std::vector<std::string> DataValidator::findDataInconsistencies() {
    std::vector<std::string> inconsistencies;
    
    // Check for orphaned items, customers without orders, etc.
    // This is a simplified version
    
    CustomerManager* custMgr = CustomerManager::getInstance();
    std::vector<Customer*> customers = custMgr->getAllCustomers();
    
    for (std::vector<Customer*>::const_iterator it = customers.begin();
         it != customers.end(); ++it) {
        if (*it != NULL && !validateCustomerData(*it)) {
            inconsistencies.push_back("Invalid customer data: " + (*it)->getFullName());
        }
    }
    
    return inconsistencies;
}

bool DataValidator::performDataIntegrityCheck() {
    LOG_INFO("Performing data integrity check...");
    
    std::vector<std::string> issues = findDataInconsistencies();
    
    if (issues.empty()) {
        LOG_INFO("Data integrity check passed");
        return true;
    } else {
        std::stringstream ss;
        ss << "Data integrity check found " << issues.size() << " issues";
        LOG_ERROR(ss.str());
        for (size_t i = 0; i < issues.size(); ++i) {
            LOG_ERROR("  - " + issues[i]);
        }
        return false;
    }
}

void DataValidator::generateValidationReport() {
    ReportGenerator report("Data Validation Report");
    
    report.addSection("Validation Results");
    
    std::vector<std::string> issues = findDataInconsistencies();
    
    report.addKeyValue("Total Issues Found", static_cast<int>(issues.size()));
    
    if (!issues.empty()) {
        report.addSection("Issues");
        for (size_t i = 0; i < issues.size(); ++i) {
            report.addLine(issues[i]);
        }
    }
    
    report.display();
}

// BusinessIntelligence namespace implementations
namespace BusinessIntelligence {
    std::vector<ItemPair> findFrequentItemPairs() {
        std::vector<ItemPair> pairs;
        
        // This would analyze order history to find items frequently bought together
        // Simplified implementation
        
        OrderManager* orderMgr = OrderManager::getInstance();
        std::vector<Order*> orders = orderMgr->getAllOrders();
        
        // Build co-occurrence matrix (simplified)
        std::map<std::pair<ItemID, ItemID>, int> coOccurrences;
        
        for (std::vector<Order*>::const_iterator it = orders.begin();
             it != orders.end(); ++it) {
            std::vector<OrderItem*> items = (*it)->getItems();
            
            for (size_t i = 0; i < items.size(); ++i) {
                for (size_t j = i + 1; j < items.size(); ++j) {
                    if (items[i]->getItem() != NULL && items[j]->getItem() != NULL) {
                        ItemID id1 = items[i]->getItem()->getId();
                        ItemID id2 = items[j]->getItem()->getId();
                        
                        if (id1 > id2) {
                            ItemID temp = id1;
                            id1 = id2;
                            id2 = temp;
                        }
                        
                        coOccurrences[std::make_pair(id1, id2)]++;
                    }
                }
            }
        }
        
        // Convert to ItemPair vector
        for (std::map<std::pair<ItemID, ItemID>, int>::const_iterator it = coOccurrences.begin();
             it != coOccurrences.end(); ++it) {
            ItemPair pair;
            pair.item1 = it->first.first;
            pair.item2 = it->first.second;
            pair.coOccurrences = it->second;
            pair.confidence = static_cast<double>(it->second) / orders.size();
            pairs.push_back(pair);
        }
        
        return pairs;
    }
    
    std::vector<Item*> getRecommendationsForItem(ItemID itemId) {
        std::vector<Item*> recommendations;
        
        std::vector<ItemPair> pairs = findFrequentItemPairs();
        
        // Find pairs containing the given item
        for (std::vector<ItemPair>::const_iterator it = pairs.begin();
             it != pairs.end(); ++it) {
            if (it->item1 == itemId || it->item2 == itemId) {
                // Would look up and add the other item
                // Simplified implementation
            }
        }
        
        return recommendations;
    }
    
    CustomerSegment categorizeCustomer(Customer* customer) {
        if (customer == NULL) return SEGMENT_LOW_VALUE;
        
        double ltv = customer->getLifetimeValue();
        int daysSinceLastPurchase = customer->getDaysSinceLastPurchase();
        int daysSinceRegistration = customer->getDaysSinceRegistration();
        
        if (daysSinceRegistration < 30) {
            return SEGMENT_NEW;
        }
        
        if (daysSinceLastPurchase > 90) {
            return SEGMENT_AT_RISK;
        }
        
        if (ltv >= 10000.0) {
            return SEGMENT_HIGH_VALUE;
        } else if (ltv >= 2000.0) {
            return SEGMENT_MEDIUM_VALUE;
        } else {
            return SEGMENT_LOW_VALUE;
        }
    }
    
    std::vector<Customer*> getCustomersBySegment(CustomerSegment segment) {
        std::vector<Customer*> segmentCustomers;
        
        CustomerManager* custMgr = CustomerManager::getInstance();
        std::vector<Customer*> allCustomers = custMgr->getAllCustomers();
        
        for (std::vector<Customer*>::const_iterator it = allCustomers.begin();
             it != allCustomers.end(); ++it) {
            if (categorizeCustomer(*it) == segment) {
                segmentCustomers.push_back(*it);
            }
        }
        
        return segmentCustomers;
    }
    
    double calculateOptimalPrice(Item* item) {
        if (item == NULL) return 0.0;
        
        // Simplified price optimization based on demand and competition
        double basePrice = item->getBasePrice();
        int stockLevel = item->getStockQuantity();
        
        // Adjust based on stock level
        if (stockLevel < MIN_STOCK_LEVEL) {
            return basePrice * 1.15; // Increase price for low stock
        } else if (stockLevel > MAX_STOCK_LEVEL / 2) {
            return basePrice * 0.95; // Decrease price for overstocked items
        }
        
        return basePrice;
    }
    
    double calculatePriceElasticity(Item* item) {
        if (item == NULL) return 0.0;
        
        // Simplified elasticity calculation
        // In reality, this would analyze historical sales data at different price points
        return -1.5; // Example elasticity coefficient
    }
    
    std::map<ItemID, double> suggestPriceAdjustments() {
        std::map<ItemID, double> suggestions;
        
        // Would analyze all items and suggest price changes
        // Simplified implementation
        
        return suggestions;
    }
    
    int forecastDemand(ItemID /* itemId */, int daysAhead) {
        // Simplified demand forecasting
        // Would use historical sales data and trends
        
        // Use analytics data to forecast
        
        return daysAhead * 5; // Simplified: assume 5 units per day
    }
    
    std::map<ItemID, int> getReorderSuggestions() {
        std::map<ItemID, int> suggestions;
        
        WarehouseNetwork* whNetwork = WarehouseNetwork::getInstance();
        std::vector<Warehouse*> warehouses = whNetwork->getAllWarehouses();
        
        for (std::vector<Warehouse*>::const_iterator it = warehouses.begin();
             it != warehouses.end(); ++it) {
            std::vector<InventoryRecord*> needReorder = (*it)->getItemsNeedingReorder();
            
            for (std::vector<InventoryRecord*>::const_iterator recIt = needReorder.begin();
                 recIt != needReorder.end(); ++recIt) {
                if ((*recIt)->getItem() != NULL) {
                    ItemID itemId = (*recIt)->getItem()->getId();
                    int recommended = (*recIt)->getReorderQuantity();
                    
                    suggestions[itemId] = recommended;
                }
            }
        }
        
        return suggestions;
    }
}
