#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include "Common.h"

// Forward declarations
class Item;

// Location within warehouse
struct Location {
    WarehouseZone zone;
    int aisle;
    int rack;
    int shelf;
    
    Location() : zone(ZONE_A), aisle(0), rack(0), shelf(0) {}
    Location(WarehouseZone z, int a, int r, int s) 
        : zone(z), aisle(a), rack(r), shelf(s) {}
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Zone-" << static_cast<char>('A' + zone) 
           << "-" << aisle << "-" << rack << "-" << shelf;
        return ss.str();
    }
};

// Inventory record
class InventoryRecord {
private:
    Item* item;
    Location location;
    int quantity;
    time_t lastUpdated;
    time_t lastCountDate;
    int reorderPoint;
    int reorderQuantity;
    
public:
    InventoryRecord();
    InventoryRecord(Item* itm, const Location& loc, int qty);
    ~InventoryRecord();
    
    Item* getItem() const { return item; }
    void setItem(Item* itm) { item = itm; }
    
    Location getLocation() const { return location; }
    void setLocation(const Location& loc) { location = loc; }
    
    int getQuantity() const { return quantity; }
    void setQuantity(int qty);
    
    void adjustQuantity(int adjustment);
    
    time_t getLastUpdated() const { return lastUpdated; }
    time_t getLastCountDate() const { return lastCountDate; }
    void updateCountDate() { lastCountDate = time(0); }
    
    int getReorderPoint() const { return reorderPoint; }
    void setReorderPoint(int point) { reorderPoint = point; }
    
    int getReorderQuantity() const { return reorderQuantity; }
    void setReorderQuantity(int qty) { reorderQuantity = qty; }
    
    bool needsReorder() const;
    void displayInfo() const;
};

// Warehouse class
class Warehouse {
private:
    WarehouseID id;
    std::string name;
    std::string address;
    std::string city;
    std::string state;
    int maxCapacity;
    int currentCapacity;
    std::map<ItemID, InventoryRecord*> inventory;
    std::vector<std::string> zones;
    bool isOperational;
    
public:
    Warehouse();
    Warehouse(WarehouseID whId, const std::string& whName);
    ~Warehouse();
    
    // Inventory operations
    bool addInventory(Item* item, const Location& loc, int quantity);
    bool removeInventory(ItemID itemId, int quantity);
    bool transferInventory(ItemID itemId, Warehouse* targetWarehouse, int quantity);
    InventoryRecord* getInventoryRecord(ItemID itemId) const;
    
    int getItemQuantity(ItemID itemId) const;
    bool hasItem(ItemID itemId) const;
    std::vector<InventoryRecord*> getAllInventoryRecords() const;
    std::vector<InventoryRecord*> getItemsNeedingReorder() const;
    
    // Capacity management
    bool hasCapacity(int requiredSpace) const;
    int getAvailableCapacity() const { return maxCapacity - currentCapacity; }
    double getCapacityUtilization() const;
    
    // Getters and setters
    WarehouseID getId() const { return id; }
    void setId(WarehouseID newId) { id = newId; }
    
    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }
    
    std::string getAddress() const { return address; }
    void setAddress(const std::string& addr) { address = addr; }
    
    int getMaxCapacity() const { return maxCapacity; }
    void setMaxCapacity(int capacity) { maxCapacity = capacity; }
    
    int getCurrentCapacity() const { return currentCapacity; }
    
    bool getIsOperational() const { return isOperational; }
    void setIsOperational(bool operational) { isOperational = operational; }
    
    // Utility methods
    void displayWarehouseInfo() const;
    void displayInventory() const;
    int getTotalItemCount() const;
    double getTotalInventoryValue() const;
    void performInventoryAudit();
};

// WarehouseNetwork - manages multiple warehouses
class WarehouseNetwork {
private:
    static WarehouseNetwork* instance;
    std::map<WarehouseID, Warehouse*> warehouses;
    WarehouseID nextWarehouseId;
    
    // Private constructor for singleton
    WarehouseNetwork();
    ~WarehouseNetwork();
    
    // Prevent copying
    WarehouseNetwork(const WarehouseNetwork&);
    WarehouseNetwork& operator=(const WarehouseNetwork&);
    
public:
    static WarehouseNetwork* getInstance();
    static void destroyInstance();
    
    // Warehouse operations
    Warehouse* createWarehouse(const std::string& name);
    bool deleteWarehouse(WarehouseID whId);
    Warehouse* getWarehouse(WarehouseID whId) const;
    std::vector<Warehouse*> getAllWarehouses() const;
    
    // Network-wide operations
    int getTotalItemQuantity(ItemID itemId) const;
    Warehouse* findWarehouseWithItem(ItemID itemId, int requiredQuantity) const;
    std::vector<Warehouse*> findAllWarehousesWithItem(ItemID itemId) const;
    bool transferBetweenWarehouses(WarehouseID fromId, WarehouseID toId, 
                                   ItemID itemId, int quantity);
    
    // Reporting
    void displayNetworkStatus() const;
    double getTotalNetworkValue() const;
    int getTotalWarehouseCount() const { return static_cast<int>(warehouses.size()); }
    
    void clearAllWarehouses();
};

#endif // WAREHOUSE_H
