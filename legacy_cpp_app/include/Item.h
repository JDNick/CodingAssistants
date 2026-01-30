#ifndef ITEM_H
#define ITEM_H

#include "Common.h"

// Forward declarations
class Category;
class Supplier;

// Base Item class with virtual functions
class Item {
protected:
    ItemID id;
    std::string name;
    std::string description;
    double basePrice;
    int stockQuantity;
    ItemStatus status;
    Category* category;
    Supplier* supplier;
    std::string sku;
    double weight;
    std::string dimensions;
    time_t lastUpdated;
    std::vector<std::string> tags;
    
public:
    Item();
    Item(ItemID id, const std::string& name, double price);
    Item(const Item& other);
    virtual ~Item();
    
    // Virtual methods for polymorphism
    virtual double calculatePrice() const;
    virtual double calculateShippingCost() const;
    virtual std::string getDescription() const;
    virtual bool isAvailable() const;
    virtual void updateStock(int quantity);
    virtual Item* clone() const;
    
    // Getter and setter methods
    ItemID getId() const { return id; }
    void setId(ItemID newId) { id = newId; }
    
    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }
    
    double getBasePrice() const { return basePrice; }
    void setBasePrice(double price) { basePrice = price; }
    
    int getStockQuantity() const { return stockQuantity; }
    void setStockQuantity(int qty) { stockQuantity = qty; }
    
    ItemStatus getStatus() const { return status; }
    void setStatus(ItemStatus newStatus) { status = newStatus; }
    
    Category* getCategory() const { return category; }
    void setCategory(Category* cat) { category = cat; }
    
    Supplier* getSupplier() const { return supplier; }
    void setSupplier(Supplier* supp) { supplier = supp; }
    
    std::string getSku() const { return sku; }
    void setSku(const std::string& newSku) { sku = newSku; }
    
    double getWeight() const { return weight; }
    void setWeight(double w) { weight = w; }
    
    void addTag(const std::string& tag);
    std::vector<std::string> getTags() const { return tags; }
    
    // Operators
    Item& operator=(const Item& other);
    bool operator==(const Item& other) const;
    bool operator<(const Item& other) const;
    
    // Utility methods
    void displayInfo() const;
    bool needsRestock() const;
    void markAsDiscontinued();
};

// Specialized item types
class ElectronicsItem : public Item {
private:
    std::string manufacturer;
    int warrantyMonths;
    std::string model;
    bool hasExtendedWarranty;
    double powerConsumption;
    
public:
    ElectronicsItem();
    ElectronicsItem(ItemID id, const std::string& name, double price, 
                    const std::string& mfr, int warranty);
    virtual ~ElectronicsItem();
    
    virtual double calculatePrice() const;
    virtual double calculateShippingCost() const;
    virtual std::string getDescription() const;
    virtual Item* clone() const;
    
    std::string getManufacturer() const { return manufacturer; }
    void setManufacturer(const std::string& mfr) { manufacturer = mfr; }
    
    int getWarrantyMonths() const { return warrantyMonths; }
    void setWarrantyMonths(int months) { warrantyMonths = months; }
    
    bool extendWarranty(int additionalMonths);
};

class PerishableItem : public Item {
private:
    time_t expirationDate;
    double temperature;
    bool requiresRefrigeration;
    std::string storageInstructions;
    
public:
    PerishableItem();
    PerishableItem(ItemID id, const std::string& name, double price, time_t expDate);
    virtual ~PerishableItem();
    
    virtual double calculatePrice() const;
    virtual bool isAvailable() const;
    virtual std::string getDescription() const;
    virtual Item* clone() const;
    
    time_t getExpirationDate() const { return expirationDate; }
    void setExpirationDate(time_t date) { expirationDate = date; }
    
    bool isExpired() const;
    int daysUntilExpiration() const;
    double getDiscountBasedOnExpiration() const;
};

class FragileItem : public Item {
private:
    std::string packagingRequirements;
    bool requiresInsurance;
    double insuranceValue;
    std::string handlingInstructions;
    
public:
    FragileItem();
    FragileItem(ItemID id, const std::string& name, double price);
    virtual ~FragileItem();
    
    virtual double calculateShippingCost() const;
    virtual std::string getDescription() const;
    virtual Item* clone() const;
    
    void setPackagingRequirements(const std::string& req) { packagingRequirements = req; }
    void setInsuranceValue(double value) { insuranceValue = value; requiresInsurance = true; }
};

// Template class for specialized item containers
template<typename T>
class ItemContainer {
private:
    std::vector<T*> items;
    std::string containerName;
    int maxCapacity;
    
public:
    ItemContainer(const std::string& name, int capacity) 
        : containerName(name), maxCapacity(capacity) {}
    
    ~ItemContainer() {
        for (typename std::vector<T*>::iterator it = items.begin(); 
             it != items.end(); ++it) {
            delete *it;
        }
        items.clear();
    }
    
    bool addItem(T* item) {
        if (items.size() >= static_cast<size_t>(maxCapacity)) {
            LOG_ERROR("Container at maximum capacity");
            return false;
        }
        items.push_back(item);
        return true;
    }
    
    T* getItem(size_t index) const {
        if (index >= items.size()) return NULL;
        return items[index];
    }
    
    bool removeItem(size_t index) {
        if (index >= items.size()) return false;
        delete items[index];
        items.erase(items.begin() + index);
        return true;
    }
    
    size_t getCount() const { return items.size(); }
    
    std::vector<T*> getItems() const { return items; }
    
    void clearAll() {
        for (typename std::vector<T*>::iterator it = items.begin(); 
             it != items.end(); ++it) {
            delete *it;
        }
        items.clear();
    }
    
    double getTotalValue() const {
        double total = 0.0;
        for (typename std::vector<T*>::const_iterator it = items.begin(); 
             it != items.end(); ++it) {
            if (*it) total += (*it)->calculatePrice();
        }
        return total;
    }
};

#endif // ITEM_H
