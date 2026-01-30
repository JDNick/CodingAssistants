#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Common.h"

// Forward declaration
class Order;

// Customer loyalty tiers
enum LoyaltyTier {
    TIER_BRONZE = 0,
    TIER_SILVER = 1,
    TIER_GOLD = 2,
    TIER_PLATINUM = 3
};

// Customer class
class Customer {
private:
    CustomerID id;
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string phone;
    std::string address;
    std::string city;
    std::string state;
    std::string zipCode;
    std::string country;
    time_t registrationDate;
    time_t lastPurchaseDate;
    double lifetimeValue;
    int totalOrders;
    LoyaltyTier loyaltyTier;
    int loyaltyPoints;
    bool isActive;
    std::vector<Order*> orderHistory;
    std::string preferredPaymentMethod;
    
public:
    Customer();
    Customer(CustomerID custId, const std::string& fName, const std::string& lName);
    ~Customer();
    
    // Order management
    void addOrder(Order* order);
    std::vector<Order*> getOrderHistory() const { return orderHistory; }
    int getOrderCount() const { return static_cast<int>(orderHistory.size()); }
    
    // Loyalty management
    void addLoyaltyPoints(int points);
    bool redeemLoyaltyPoints(int points);
    void updateLoyaltyTier();
    double getLoyaltyDiscount() const;
    
    // Getters and setters
    CustomerID getId() const { return id; }
    void setId(CustomerID newId) { id = newId; }
    
    std::string getFirstName() const { return firstName; }
    void setFirstName(const std::string& name) { firstName = name; }
    
    std::string getLastName() const { return lastName; }
    void setLastName(const std::string& name) { lastName = name; }
    
    std::string getFullName() const { return firstName + " " + lastName; }
    
    std::string getEmail() const { return email; }
    void setEmail(const std::string& emailAddr);
    
    std::string getPhone() const { return phone; }
    void setPhone(const std::string& phoneNum) { phone = phoneNum; }
    
    std::string getAddress() const { return address; }
    void setAddress(const std::string& addr) { address = addr; }
    
    time_t getRegistrationDate() const { return registrationDate; }
    time_t getLastPurchaseDate() const { return lastPurchaseDate; }
    void updateLastPurchaseDate() { lastPurchaseDate = time(0); }
    
    double getLifetimeValue() const { return lifetimeValue; }
    void addToLifetimeValue(double amount) { lifetimeValue += amount; }
    
    int getTotalOrders() const { return totalOrders; }
    void incrementOrderCount() { totalOrders++; }
    
    LoyaltyTier getLoyaltyTier() const { return loyaltyTier; }
    int getLoyaltyPoints() const { return loyaltyPoints; }
    
    bool getIsActive() const { return isActive; }
    void setIsActive(bool active) { isActive = active; }
    
    // Utility methods
    void displayInfo() const;
    void displayOrderHistory() const;
    std::string getFullAddress() const;
    bool validateCustomerData() const;
    int getDaysSinceRegistration() const;
    int getDaysSinceLastPurchase() const;
    std::string getLoyaltyTierName() const;
};

// CustomerManager - Singleton pattern
class CustomerManager {
private:
    static CustomerManager* instance;
    std::map<CustomerID, Customer*> customers;
    CustomerID nextCustomerId;
    
    // Private constructor
    CustomerManager();
    ~CustomerManager();
    
    // Prevent copying
    CustomerManager(const CustomerManager&);
    CustomerManager& operator=(const CustomerManager&);
    
public:
    static CustomerManager* getInstance();
    static void destroyInstance();
    
    // Customer operations
    Customer* createCustomer(const std::string& firstName, const std::string& lastName);
    bool deleteCustomer(CustomerID custId);
    Customer* getCustomer(CustomerID custId) const;
    
    std::vector<Customer*> getAllCustomers() const;
    std::vector<Customer*> getActiveCustomers() const;
    std::vector<Customer*> searchCustomers(const std::string& keyword) const;
    std::vector<Customer*> getCustomersByTier(LoyaltyTier tier) const;
    std::vector<Customer*> getTopCustomersByValue(int count) const;
    
    // Statistics
    int getTotalCustomerCount() const { return static_cast<int>(customers.size()); }
    double getTotalCustomerLifetimeValue() const;
    double getAverageCustomerValue() const;
    
    // Utility
    void displayAllCustomers() const;
    void clearAllCustomers();
};

#endif // CUSTOMER_H
