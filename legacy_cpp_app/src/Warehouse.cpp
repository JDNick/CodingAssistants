#include "Warehouse.h"
#include "Item.h"

// InventoryRecord implementation
InventoryRecord::InventoryRecord()
    : item(NULL), location(), quantity(0), lastUpdated(time(0)),
      lastCountDate(0), reorderPoint(MIN_STOCK_LEVEL), reorderQuantity(100) {
}

InventoryRecord::InventoryRecord(Item* itm, const Location& loc, int qty)
    : item(itm), location(loc), quantity(qty), lastUpdated(time(0)),
      lastCountDate(time(0)), reorderPoint(MIN_STOCK_LEVEL), reorderQuantity(100) {
}

InventoryRecord::~InventoryRecord() {
    // Note: We don't delete item as it's managed by inventory system
}

void InventoryRecord::setQuantity(int qty) {
    if (qty < 0) {
        LOG_ERROR("Cannot set negative quantity");
        return;
    }
    quantity = qty;
    lastUpdated = time(0);
}

void InventoryRecord::adjustQuantity(int adjustment) {
    int newQty = quantity + adjustment;
    if (newQty < 0) {
        LOG_ERROR("Adjustment would result in negative quantity");
        return;
    }
    quantity = newQty;
    lastUpdated = time(0);
}

bool InventoryRecord::needsReorder() const {
    return quantity <= reorderPoint;
}

void InventoryRecord::displayInfo() const {
    std::cout << "----------------------------------------\n";
    if (item != NULL) {
        std::cout << "Item: " << item->getName() << " (ID: " << item->getId() << ")\n";
    }
    std::cout << "Location: " << location.toString() << "\n";
    std::cout << "Quantity: " << quantity << "\n";
    std::cout << "Reorder Point: " << reorderPoint << "\n";
    if (needsReorder()) {
        std::cout << "*** REORDER NEEDED ***\n";
    }
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&lastUpdated));
    std::cout << "Last Updated: " << buffer << "\n";
    std::cout << "----------------------------------------\n";
}

// Warehouse implementation
Warehouse::Warehouse()
    : id(0), name(""), address(""), city(""), state(""),
      maxCapacity(DEFAULT_WAREHOUSE_CAPACITY), currentCapacity(0),
      isOperational(true) {
    zones.push_back("Zone A");
    zones.push_back("Zone B");
    zones.push_back("Zone C");
    zones.push_back("Zone D");
}

Warehouse::Warehouse(WarehouseID whId, const std::string& whName)
    : id(whId), name(whName), address(""), city(""), state(""),
      maxCapacity(DEFAULT_WAREHOUSE_CAPACITY), currentCapacity(0),
      isOperational(true) {
    zones.push_back("Zone A");
    zones.push_back("Zone B");
    zones.push_back("Zone C");
    zones.push_back("Zone D");
}

Warehouse::~Warehouse() {
    for (std::map<ItemID, InventoryRecord*>::iterator it = inventory.begin();
         it != inventory.end(); ++it) {
        delete it->second;
    }
    inventory.clear();
}

bool Warehouse::addInventory(Item* item, const Location& loc, int quantity) {
    if (item == NULL) {
        LOG_ERROR("Cannot add NULL item to inventory");
        return false;
    }
    
    if (quantity <= 0) {
        LOG_ERROR("Invalid quantity");
        return false;
    }
    
    if (!hasCapacity(quantity)) {
        LOG_ERROR("Insufficient warehouse capacity");
        return false;
    }
    
    ItemID itemId = item->getId();
    std::map<ItemID, InventoryRecord*>::iterator it = inventory.find(itemId);
    
    if (it != inventory.end()) {
        // Item already exists, update quantity
        it->second->adjustQuantity(quantity);
    } else {
        // New item
        InventoryRecord* record = new InventoryRecord(item, loc, quantity);
        inventory[itemId] = record;
    }
    
    currentCapacity += quantity;
    LOG_INFO("Added " + Utils::intToString(quantity) + " units of item " + item->getName());
    return true;
}

bool Warehouse::removeInventory(ItemID itemId, int quantity) {
    std::map<ItemID, InventoryRecord*>::iterator it = inventory.find(itemId);
    
    if (it == inventory.end()) {
        LOG_ERROR("Item not found in inventory");
        return false;
    }
    
    if (it->second->getQuantity() < quantity) {
        LOG_ERROR("Insufficient quantity in inventory");
        return false;
    }
    
    it->second->adjustQuantity(-quantity);
    currentCapacity -= quantity;
    
    // Remove record if quantity is zero
    if (it->second->getQuantity() == 0) {
        delete it->second;
        inventory.erase(it);
    }
    
    return true;
}

bool Warehouse::transferInventory(ItemID itemId, Warehouse* targetWarehouse, int quantity) {
    if (targetWarehouse == NULL) {
        LOG_ERROR("Target warehouse is NULL");
        return false;
    }
    
    std::map<ItemID, InventoryRecord*>::iterator it = inventory.find(itemId);
    if (it == inventory.end()) {
        LOG_ERROR("Item not found in source warehouse");
        return false;
    }
    
    if (it->second->getQuantity() < quantity) {
        LOG_ERROR("Insufficient quantity for transfer");
        return false;
    }
    
    if (!targetWarehouse->hasCapacity(quantity)) {
        LOG_ERROR("Target warehouse has insufficient capacity");
        return false;
    }
    
    Item* item = it->second->getItem();
    Location newLoc; // Default location in target warehouse
    
    if (removeInventory(itemId, quantity)) {
        if (targetWarehouse->addInventory(item, newLoc, quantity)) {
            LOG_INFO("Transfer completed successfully");
            return true;
        } else {
            // Rollback - add back to source
            Location loc = it->second->getLocation();
            addInventory(item, loc, quantity);
            LOG_ERROR("Transfer failed - rolled back");
            return false;
        }
    }
    
    return false;
}

InventoryRecord* Warehouse::getInventoryRecord(ItemID itemId) const {
    std::map<ItemID, InventoryRecord*>::const_iterator it = inventory.find(itemId);
    if (it == inventory.end()) {
        return NULL;
    }
    return it->second;
}

int Warehouse::getItemQuantity(ItemID itemId) const {
    InventoryRecord* record = getInventoryRecord(itemId);
    return record ? record->getQuantity() : 0;
}

bool Warehouse::hasItem(ItemID itemId) const {
    return inventory.find(itemId) != inventory.end();
}

std::vector<InventoryRecord*> Warehouse::getAllInventoryRecords() const {
    std::vector<InventoryRecord*> records;
    for (std::map<ItemID, InventoryRecord*>::const_iterator it = inventory.begin();
         it != inventory.end(); ++it) {
        records.push_back(it->second);
    }
    return records;
}

std::vector<InventoryRecord*> Warehouse::getItemsNeedingReorder() const {
    std::vector<InventoryRecord*> needReorder;
    for (std::map<ItemID, InventoryRecord*>::const_iterator it = inventory.begin();
         it != inventory.end(); ++it) {
        if (it->second->needsReorder()) {
            needReorder.push_back(it->second);
        }
    }
    return needReorder;
}

bool Warehouse::hasCapacity(int requiredSpace) const {
    return (currentCapacity + requiredSpace) <= maxCapacity;
}

double Warehouse::getCapacityUtilization() const {
    if (maxCapacity == 0) return 0.0;
    return (static_cast<double>(currentCapacity) / maxCapacity) * 100.0;
}

void Warehouse::displayWarehouseInfo() const {
    std::cout << "\n==========================================\n";
    std::cout << "Warehouse ID: " << id << "\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "Address: " << address << ", " << city << ", " << state << "\n";
    std::cout << "Capacity: " << currentCapacity << " / " << maxCapacity 
              << " (" << getCapacityUtilization() << "%)\n";
    std::cout << "Status: " << (isOperational ? "Operational" : "Not Operational") << "\n";
    std::cout << "Total Items: " << getTotalItemCount() << "\n";
    std::cout << "Total Value: $" << getTotalInventoryValue() << "\n";
    std::cout << "==========================================\n";
}

void Warehouse::displayInventory() const {
    std::cout << "\n========== Inventory for " << name << " ==========\n";
    for (std::map<ItemID, InventoryRecord*>::const_iterator it = inventory.begin();
         it != inventory.end(); ++it) {
        it->second->displayInfo();
    }
    std::cout << "Total records: " << inventory.size() << "\n";
}

int Warehouse::getTotalItemCount() const {
    return static_cast<int>(inventory.size());
}

double Warehouse::getTotalInventoryValue() const {
    double total = 0.0;
    for (std::map<ItemID, InventoryRecord*>::const_iterator it = inventory.begin();
         it != inventory.end(); ++it) {
        if (it->second->getItem() != NULL) {
            total += it->second->getItem()->calculatePrice() * it->second->getQuantity();
        }
    }
    return total;
}

void Warehouse::performInventoryAudit() {
    LOG_INFO("Starting inventory audit for warehouse: " + name);
    
    std::vector<InventoryRecord*> needsReorder = getItemsNeedingReorder();
    
    std::cout << "\n========== Inventory Audit Report ==========\n";
    std::cout << "Warehouse: " << name << "\n";
    std::cout << "Total Items: " << inventory.size() << "\n";
    std::cout << "Items Needing Reorder: " << needsReorder.size() << "\n";
    
    if (!needsReorder.empty()) {
        std::cout << "\nItems requiring reorder:\n";
        for (size_t i = 0; i < needsReorder.size(); ++i) {
            if (needsReorder[i]->getItem() != NULL) {
                std::cout << "  - " << needsReorder[i]->getItem()->getName() 
                         << " (Qty: " << needsReorder[i]->getQuantity() << ")\n";
            }
        }
    }
    
    std::cout << "==========================================\n";
}

// WarehouseNetwork implementation (Singleton)
WarehouseNetwork* WarehouseNetwork::instance = NULL;

WarehouseNetwork::WarehouseNetwork() : nextWarehouseId(1) {
}

WarehouseNetwork::~WarehouseNetwork() {
    clearAllWarehouses();
}

WarehouseNetwork* WarehouseNetwork::getInstance() {
    if (instance == NULL) {
        instance = new WarehouseNetwork();
    }
    return instance;
}

void WarehouseNetwork::destroyInstance() {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

Warehouse* WarehouseNetwork::createWarehouse(const std::string& name) {
    WarehouseID id = nextWarehouseId++;
    Warehouse* newWarehouse = new Warehouse(id, name);
    warehouses[id] = newWarehouse;
    
    LOG_INFO("Created warehouse: " + name + " (ID: " + Utils::intToString(id) + ")");
    return newWarehouse;
}

bool WarehouseNetwork::deleteWarehouse(WarehouseID whId) {
    std::map<WarehouseID, Warehouse*>::iterator it = warehouses.find(whId);
    if (it == warehouses.end()) {
        LOG_ERROR("Warehouse not found");
        return false;
    }
    
    delete it->second;
    warehouses.erase(it);
    return true;
}

Warehouse* WarehouseNetwork::getWarehouse(WarehouseID whId) const {
    std::map<WarehouseID, Warehouse*>::const_iterator it = warehouses.find(whId);
    if (it == warehouses.end()) {
        return NULL;
    }
    return it->second;
}

std::vector<Warehouse*> WarehouseNetwork::getAllWarehouses() const {
    std::vector<Warehouse*> allWarehouses;
    for (std::map<WarehouseID, Warehouse*>::const_iterator it = warehouses.begin();
         it != warehouses.end(); ++it) {
        allWarehouses.push_back(it->second);
    }
    return allWarehouses;
}

int WarehouseNetwork::getTotalItemQuantity(ItemID itemId) const {
    int total = 0;
    for (std::map<WarehouseID, Warehouse*>::const_iterator it = warehouses.begin();
         it != warehouses.end(); ++it) {
        total += it->second->getItemQuantity(itemId);
    }
    return total;
}

Warehouse* WarehouseNetwork::findWarehouseWithItem(ItemID itemId, int requiredQuantity) const {
    for (std::map<WarehouseID, Warehouse*>::const_iterator it = warehouses.begin();
         it != warehouses.end(); ++it) {
        if (it->second->getItemQuantity(itemId) >= requiredQuantity) {
            return it->second;
        }
    }
    return NULL;
}

std::vector<Warehouse*> WarehouseNetwork::findAllWarehousesWithItem(ItemID itemId) const {
    std::vector<Warehouse*> results;
    for (std::map<WarehouseID, Warehouse*>::const_iterator it = warehouses.begin();
         it != warehouses.end(); ++it) {
        if (it->second->hasItem(itemId)) {
            results.push_back(it->second);
        }
    }
    return results;
}

bool WarehouseNetwork::transferBetweenWarehouses(WarehouseID fromId, WarehouseID toId,
                                                 ItemID itemId, int quantity) {
    Warehouse* fromWarehouse = getWarehouse(fromId);
    Warehouse* toWarehouse = getWarehouse(toId);
    
    if (fromWarehouse == NULL || toWarehouse == NULL) {
        LOG_ERROR("Invalid warehouse IDs");
        return false;
    }
    
    return fromWarehouse->transferInventory(itemId, toWarehouse, quantity);
}

void WarehouseNetwork::displayNetworkStatus() const {
    std::cout << "\n========== Warehouse Network Status ==========\n";
    std::cout << "Total Warehouses: " << warehouses.size() << "\n";
    std::cout << "Total Network Value: $" << getTotalNetworkValue() << "\n\n";
    
    for (std::map<WarehouseID, Warehouse*>::const_iterator it = warehouses.begin();
         it != warehouses.end(); ++it) {
        it->second->displayWarehouseInfo();
    }
}

double WarehouseNetwork::getTotalNetworkValue() const {
    double total = 0.0;
    for (std::map<WarehouseID, Warehouse*>::const_iterator it = warehouses.begin();
         it != warehouses.end(); ++it) {
        total += it->second->getTotalInventoryValue();
    }
    return total;
}

void WarehouseNetwork::clearAllWarehouses() {
    for (std::map<WarehouseID, Warehouse*>::iterator it = warehouses.begin();
         it != warehouses.end(); ++it) {
        delete it->second;
    }
    warehouses.clear();
    nextWarehouseId = 1;
}
