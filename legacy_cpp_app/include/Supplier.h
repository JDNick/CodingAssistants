#ifndef SUPPLIER_H
#define SUPPLIER_H

#include "Common.h"

// Forward declaration
class Item;

// Supplier class
class Supplier {
private:
    SupplierID id;
    std::string name;
    std::string contactPerson;
    std::string phone;
    std::string email;
    std::string address;
    std::string city;
    std::string state;
    std::string zipCode;
    std::string country;
    double creditLimit;
    double currentBalance;
    bool isActive;
    int reliabilityRating; // 1-10 scale
    std::vector<Item*> suppliedItems;
    time_t lastOrderDate;
    int totalOrdersPlaced;
    
public:
    Supplier();
    Supplier(SupplierID suppId, const std::string& supplierName);
    ~Supplier();
    
    // Item management
    void addSuppliedItem(Item* item);
    bool removeSuppliedItem(ItemID itemId);
    std::vector<Item*> getSuppliedItems() const { return suppliedItems; }
    int getSuppliedItemCount() const { return static_cast<int>(suppliedItems.size()); }
    
    // Financial operations
    bool processPayment(double amount);
    bool addToBalance(double amount);
    double getAvailableCredit() const;
    bool isCreditAvailable(double amount) const;
    
    // Getters and setters
    SupplierID getId() const { return id; }
    void setId(SupplierID newId) { id = newId; }
    
    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }
    
    std::string getContactPerson() const { return contactPerson; }
    void setContactPerson(const std::string& contact) { contactPerson = contact; }
    
    std::string getPhone() const { return phone; }
    void setPhone(const std::string& phoneNum) { phone = phoneNum; }
    
    std::string getEmail() const { return email; }
    void setEmail(const std::string& emailAddr) { email = emailAddr; }
    
    std::string getAddress() const { return address; }
    void setAddress(const std::string& addr) { address = addr; }
    
    double getCreditLimit() const { return creditLimit; }
    void setCreditLimit(double limit) { creditLimit = limit; }
    
    double getCurrentBalance() const { return currentBalance; }
    
    bool getIsActive() const { return isActive; }
    void setIsActive(bool active) { isActive = active; }
    
    int getReliabilityRating() const { return reliabilityRating; }
    void setReliabilityRating(int rating);
    
    time_t getLastOrderDate() const { return lastOrderDate; }
    void updateLastOrderDate() { lastOrderDate = time(0); }
    
    int getTotalOrdersPlaced() const { return totalOrdersPlaced; }
    void incrementOrderCount() { totalOrdersPlaced++; }
    
    // Utility methods
    void displayInfo() const;
    std::string getFullAddress() const;
    bool validateSupplierData() const;
};

// SupplierManager class with Factory pattern
class SupplierManager {
private:
    static SupplierManager* instance;
    std::map<SupplierID, Supplier*> suppliers;
    SupplierID nextSupplierId;
    
    // Private constructor
    SupplierManager();
    ~SupplierManager();
    
    // Prevent copying
    SupplierManager(const SupplierManager&);
    SupplierManager& operator=(const SupplierManager&);
    
public:
    static SupplierManager* getInstance();
    static void destroyInstance();
    
    // Supplier operations
    Supplier* createSupplier(const std::string& name);
    bool deleteSupplier(SupplierID suppId);
    Supplier* getSupplier(SupplierID suppId) const;
    
    std::vector<Supplier*> getAllSuppliers() const;
    std::vector<Supplier*> getActiveSuppliers() const;
    std::vector<Supplier*> searchSuppliers(const std::string& keyword) const;
    std::vector<Supplier*> getSuppliersByRating(int minRating) const;
    
    // Utility
    void displayAllSuppliers() const;
    int getTotalSupplierCount() const;
    double getTotalOutstandingBalance() const;
    void clearAllSuppliers();
};

#endif // SUPPLIER_H
