#include "Customer.h"
#include "Order.h"

// Customer implementation
Customer::Customer()
    : id(0), firstName(""), lastName(""), email(""), phone(""),
      address(""), city(""), state(""), zipCode(""), country("USA"),
      registrationDate(time(0)), lastPurchaseDate(0), lifetimeValue(0.0),
      totalOrders(0), loyaltyTier(TIER_BRONZE), loyaltyPoints(0),
      isActive(true), preferredPaymentMethod("Credit Card") {
}

Customer::Customer(CustomerID custId, const std::string& fName, const std::string& lName)
    : id(custId), firstName(fName), lastName(lName), email(""), phone(""),
      address(""), city(""), state(""), zipCode(""), country("USA"),
      registrationDate(time(0)), lastPurchaseDate(0), lifetimeValue(0.0),
      totalOrders(0), loyaltyTier(TIER_BRONZE), loyaltyPoints(0),
      isActive(true), preferredPaymentMethod("Credit Card") {
}

Customer::~Customer() {
    // Note: We don't delete orders as they're managed by OrderManager
    orderHistory.clear();
}

void Customer::addOrder(Order* order) {
    if (order == NULL) {
        LOG_ERROR("Cannot add NULL order to customer");
        return;
    }
    orderHistory.push_back(order);
}

void Customer::addLoyaltyPoints(int points) {
    if (points <= 0) {
        LOG_ERROR("Invalid loyalty points amount");
        return;
    }
    loyaltyPoints += points;
    updateLoyaltyTier();
}

bool Customer::redeemLoyaltyPoints(int points) {
    if (points <= 0) {
        LOG_ERROR("Invalid redemption amount");
        return false;
    }
    
    if (loyaltyPoints < points) {
        LOG_ERROR("Insufficient loyalty points");
        return false;
    }
    
    loyaltyPoints -= points;
    updateLoyaltyTier();
    return true;
}

void Customer::updateLoyaltyTier() {
    if (lifetimeValue >= 10000.0 || loyaltyPoints >= 5000) {
        loyaltyTier = TIER_PLATINUM;
    } else if (lifetimeValue >= 5000.0 || loyaltyPoints >= 2500) {
        loyaltyTier = TIER_GOLD;
    } else if (lifetimeValue >= 2000.0 || loyaltyPoints >= 1000) {
        loyaltyTier = TIER_SILVER;
    } else {
        loyaltyTier = TIER_BRONZE;
    }
}

double Customer::getLoyaltyDiscount() const {
    switch (loyaltyTier) {
        case TIER_PLATINUM: return 0.20; // 20% discount
        case TIER_GOLD: return 0.15;     // 15% discount
        case TIER_SILVER: return 0.10;   // 10% discount
        case TIER_BRONZE: return 0.05;   // 5% discount
        default: return 0.0;
    }
}

void Customer::setEmail(const std::string& emailAddr) {
    if (!Utils::validateEmail(emailAddr)) {
        LOG_ERROR("Invalid email address");
        return;
    }
    email = emailAddr;
}

void Customer::displayInfo() const {
    std::cout << "==========================================\n";
    std::cout << "Customer ID: " << id << "\n";
    std::cout << "Name: " << getFullName() << "\n";
    std::cout << "Email: " << email << "\n";
    std::cout << "Phone: " << phone << "\n";
    std::cout << "Address: " << getFullAddress() << "\n";
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtime(&registrationDate));
    std::cout << "Registration Date: " << buffer << "\n";
    
    if (lastPurchaseDate > 0) {
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtime(&lastPurchaseDate));
        std::cout << "Last Purchase: " << buffer << "\n";
    }
    
    std::cout << "Lifetime Value: $" << lifetimeValue << "\n";
    std::cout << "Total Orders: " << totalOrders << "\n";
    std::cout << "Loyalty Tier: " << getLoyaltyTierName() << "\n";
    std::cout << "Loyalty Points: " << loyaltyPoints << "\n";
    std::cout << "Loyalty Discount: " << (getLoyaltyDiscount() * 100) << "%\n";
    std::cout << "Status: " << (isActive ? "Active" : "Inactive") << "\n";
    std::cout << "==========================================\n";
}

void Customer::displayOrderHistory() const {
    std::cout << "\n========== Order History for " << getFullName() << " ==========\n";
    if (orderHistory.empty()) {
        std::cout << "No orders found.\n";
    } else {
        for (std::vector<Order*>::const_iterator it = orderHistory.begin();
             it != orderHistory.end(); ++it) {
            if (*it != NULL) {
                (*it)->displayOrderSummary();
            }
        }
    }
    std::cout << "Total orders: " << orderHistory.size() << "\n";
}

std::string Customer::getFullAddress() const {
    std::stringstream ss;
    if (!address.empty()) ss << address;
    if (!city.empty()) ss << ", " << city;
    if (!state.empty()) ss << ", " << state;
    if (!zipCode.empty()) ss << " " << zipCode;
    if (!country.empty()) ss << ", " << country;
    return ss.str();
}

bool Customer::validateCustomerData() const {
    if (firstName.empty() || lastName.empty()) {
        LOG_ERROR("Customer name is incomplete");
        return false;
    }
    
    if (!email.empty() && !Utils::validateEmail(email)) {
        LOG_ERROR("Invalid email address");
        return false;
    }
    
    return true;
}

int Customer::getDaysSinceRegistration() const {
    time_t now = time(0);
    double seconds = difftime(now, registrationDate);
    return static_cast<int>(seconds / (60 * 60 * 24));
}

int Customer::getDaysSinceLastPurchase() const {
    if (lastPurchaseDate == 0) return -1;
    
    time_t now = time(0);
    double seconds = difftime(now, lastPurchaseDate);
    return static_cast<int>(seconds / (60 * 60 * 24));
}

std::string Customer::getLoyaltyTierName() const {
    switch (loyaltyTier) {
        case TIER_PLATINUM: return "Platinum";
        case TIER_GOLD: return "Gold";
        case TIER_SILVER: return "Silver";
        case TIER_BRONZE: return "Bronze";
        default: return "Unknown";
    }
}

// CustomerManager implementation (Singleton)
CustomerManager* CustomerManager::instance = NULL;

CustomerManager::CustomerManager() : nextCustomerId(1) {
}

CustomerManager::~CustomerManager() {
    clearAllCustomers();
}

CustomerManager* CustomerManager::getInstance() {
    if (instance == NULL) {
        instance = new CustomerManager();
    }
    return instance;
}

void CustomerManager::destroyInstance() {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

Customer* CustomerManager::createCustomer(const std::string& firstName, const std::string& lastName) {
    CustomerID id = nextCustomerId++;
    Customer* newCustomer = new Customer(id, firstName, lastName);
    customers[id] = newCustomer;
    
    LOG_INFO("Created customer: " + firstName + " " + lastName + " (ID: " + Utils::intToString(id) + ")");
    return newCustomer;
}

bool CustomerManager::deleteCustomer(CustomerID custId) {
    std::map<CustomerID, Customer*>::iterator it = customers.find(custId);
    if (it == customers.end()) {
        LOG_ERROR("Customer not found");
        return false;
    }
    
    delete it->second;
    customers.erase(it);
    return true;
}

Customer* CustomerManager::getCustomer(CustomerID custId) const {
    std::map<CustomerID, Customer*>::const_iterator it = customers.find(custId);
    if (it == customers.end()) {
        return NULL;
    }
    return it->second;
}

std::vector<Customer*> CustomerManager::getAllCustomers() const {
    std::vector<Customer*> allCustomers;
    for (std::map<CustomerID, Customer*>::const_iterator it = customers.begin();
         it != customers.end(); ++it) {
        allCustomers.push_back(it->second);
    }
    return allCustomers;
}

std::vector<Customer*> CustomerManager::getActiveCustomers() const {
    std::vector<Customer*> activeCustomers;
    for (std::map<CustomerID, Customer*>::const_iterator it = customers.begin();
         it != customers.end(); ++it) {
        if (it->second->getIsActive()) {
            activeCustomers.push_back(it->second);
        }
    }
    return activeCustomers;
}

std::vector<Customer*> CustomerManager::searchCustomers(const std::string& keyword) const {
    std::vector<Customer*> results;
    
    for (std::map<CustomerID, Customer*>::const_iterator it = customers.begin();
         it != customers.end(); ++it) {
        if (it->second->getFirstName().find(keyword) != std::string::npos ||
            it->second->getLastName().find(keyword) != std::string::npos ||
            it->second->getEmail().find(keyword) != std::string::npos) {
            results.push_back(it->second);
        }
    }
    
    return results;
}

std::vector<Customer*> CustomerManager::getCustomersByTier(LoyaltyTier tier) const {
    std::vector<Customer*> results;
    
    for (std::map<CustomerID, Customer*>::const_iterator it = customers.begin();
         it != customers.end(); ++it) {
        if (it->second->getLoyaltyTier() == tier) {
            results.push_back(it->second);
        }
    }
    
    return results;
}

std::vector<Customer*> CustomerManager::getTopCustomersByValue(int count) const {
    std::vector<Customer*> allCustomers = getAllCustomers();
    
    // Sort by lifetime value (descending)
    for (size_t i = 0; i < allCustomers.size(); ++i) {
        for (size_t j = i + 1; j < allCustomers.size(); ++j) {
            if (allCustomers[j]->getLifetimeValue() > allCustomers[i]->getLifetimeValue()) {
                Customer* temp = allCustomers[i];
                allCustomers[i] = allCustomers[j];
                allCustomers[j] = temp;
            }
        }
    }
    
    // Return top N customers
    std::vector<Customer*> topCustomers;
    int limit = count < static_cast<int>(allCustomers.size()) ? count : static_cast<int>(allCustomers.size());
    for (int i = 0; i < limit; ++i) {
        topCustomers.push_back(allCustomers[i]);
    }
    
    return topCustomers;
}

double CustomerManager::getTotalCustomerLifetimeValue() const {
    double total = 0.0;
    for (std::map<CustomerID, Customer*>::const_iterator it = customers.begin();
         it != customers.end(); ++it) {
        total += it->second->getLifetimeValue();
    }
    return total;
}

double CustomerManager::getAverageCustomerValue() const {
    if (customers.empty()) return 0.0;
    return getTotalCustomerLifetimeValue() / customers.size();
}

void CustomerManager::displayAllCustomers() const {
    std::cout << "\n========== All Customers ==========\n";
    for (std::map<CustomerID, Customer*>::const_iterator it = customers.begin();
         it != customers.end(); ++it) {
        it->second->displayInfo();
    }
    std::cout << "Total customers: " << customers.size() << "\n";
    std::cout << "Total lifetime value: $" << getTotalCustomerLifetimeValue() << "\n";
}

void CustomerManager::clearAllCustomers() {
    for (std::map<CustomerID, Customer*>::iterator it = customers.begin();
         it != customers.end(); ++it) {
        delete it->second;
    }
    customers.clear();
    nextCustomerId = 1;
}
