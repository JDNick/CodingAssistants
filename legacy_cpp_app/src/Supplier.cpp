#include "Supplier.h"
#include "Item.h"

// Supplier implementation
Supplier::Supplier()
    : id(0), name(""), contactPerson(""), phone(""), email(""),
      address(""), city(""), state(""), zipCode(""), country("USA"),
      creditLimit(10000.0), currentBalance(0.0), isActive(true),
      reliabilityRating(5), lastOrderDate(0), totalOrdersPlaced(0) {
}

Supplier::Supplier(SupplierID suppId, const std::string& supplierName)
    : id(suppId), name(supplierName), contactPerson(""), phone(""), email(""),
      address(""), city(""), state(""), zipCode(""), country("USA"),
      creditLimit(10000.0), currentBalance(0.0), isActive(true),
      reliabilityRating(5), lastOrderDate(0), totalOrdersPlaced(0) {
}

Supplier::~Supplier() {
    // Note: We don't delete items as they're managed by inventory
    suppliedItems.clear();
}

void Supplier::addSuppliedItem(Item* item) {
    if (item == NULL) {
        LOG_ERROR("Attempted to add NULL item to supplier");
        return;
    }
    
    // Check if item already in list
    for (std::vector<Item*>::iterator it = suppliedItems.begin();
         it != suppliedItems.end(); ++it) {
        if (*it == item) {
            LOG_INFO("Item already associated with supplier");
            return;
        }
    }
    
    suppliedItems.push_back(item);
    item->setSupplier(this);
}

bool Supplier::removeSuppliedItem(ItemID itemId) {
    for (std::vector<Item*>::iterator it = suppliedItems.begin();
         it != suppliedItems.end(); ++it) {
        if ((*it)->getId() == itemId) {
            suppliedItems.erase(it);
            return true;
        }
    }
    return false;
}

bool Supplier::processPayment(double amount) {
    if (amount <= 0) {
        LOG_ERROR("Invalid payment amount");
        return false;
    }
    
    if (amount > currentBalance) {
        LOG_ERROR("Payment amount exceeds current balance");
        return false;
    }
    
    currentBalance -= amount;
    LOG_INFO("Payment processed successfully");
    return true;
}

bool Supplier::addToBalance(double amount) {
    if (amount <= 0) {
        LOG_ERROR("Invalid amount to add to balance");
        return false;
    }
    
    if (currentBalance + amount > creditLimit) {
        LOG_ERROR("Adding amount would exceed credit limit");
        return false;
    }
    
    currentBalance += amount;
    return true;
}

double Supplier::getAvailableCredit() const {
    return creditLimit - currentBalance;
}

bool Supplier::isCreditAvailable(double amount) const {
    return (currentBalance + amount) <= creditLimit;
}

void Supplier::setReliabilityRating(int rating) {
    if (rating < 1) rating = 1;
    if (rating > 10) rating = 10;
    reliabilityRating = rating;
}

void Supplier::displayInfo() const {
    std::cout << "==========================================\n";
    std::cout << "Supplier ID: " << id << "\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "Contact: " << contactPerson << "\n";
    std::cout << "Phone: " << phone << "\n";
    std::cout << "Email: " << email << "\n";
    std::cout << "Address: " << getFullAddress() << "\n";
    std::cout << "Credit Limit: $" << creditLimit << "\n";
    std::cout << "Current Balance: $" << currentBalance << "\n";
    std::cout << "Available Credit: $" << getAvailableCredit() << "\n";
    std::cout << "Reliability Rating: " << reliabilityRating << "/10\n";
    std::cout << "Status: " << (isActive ? "Active" : "Inactive") << "\n";
    std::cout << "Total Orders: " << totalOrdersPlaced << "\n";
    std::cout << "Supplied Items: " << suppliedItems.size() << "\n";
    std::cout << "==========================================\n";
}

std::string Supplier::getFullAddress() const {
    std::stringstream ss;
    if (!address.empty()) ss << address;
    if (!city.empty()) ss << ", " << city;
    if (!state.empty()) ss << ", " << state;
    if (!zipCode.empty()) ss << " " << zipCode;
    if (!country.empty()) ss << ", " << country;
    return ss.str();
}

bool Supplier::validateSupplierData() const {
    if (name.empty()) {
        LOG_ERROR("Supplier name is empty");
        return false;
    }
    
    if (!email.empty() && !Utils::validateEmail(email)) {
        LOG_ERROR("Invalid email address");
        return false;
    }
    
    if (creditLimit < 0) {
        LOG_ERROR("Invalid credit limit");
        return false;
    }
    
    if (currentBalance < 0 || currentBalance > creditLimit) {
        LOG_ERROR("Invalid balance");
        return false;
    }
    
    return true;
}

// SupplierManager implementation (Singleton + Factory)
SupplierManager* SupplierManager::instance = NULL;

SupplierManager::SupplierManager() : nextSupplierId(1) {
}

SupplierManager::~SupplierManager() {
    clearAllSuppliers();
}

SupplierManager* SupplierManager::getInstance() {
    if (instance == NULL) {
        instance = new SupplierManager();
    }
    return instance;
}

void SupplierManager::destroyInstance() {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

Supplier* SupplierManager::createSupplier(const std::string& name) {
    SupplierID id = nextSupplierId++;
    Supplier* newSupplier = new Supplier(id, name);
    suppliers[id] = newSupplier;
    
    LOG_INFO("Created supplier: " + name + " (ID: " + Utils::intToString(id) + ")");
    return newSupplier;
}

bool SupplierManager::deleteSupplier(SupplierID suppId) {
    std::map<SupplierID, Supplier*>::iterator it = suppliers.find(suppId);
    if (it == suppliers.end()) {
        LOG_ERROR("Supplier not found");
        return false;
    }
    
    delete it->second;
    suppliers.erase(it);
    return true;
}

Supplier* SupplierManager::getSupplier(SupplierID suppId) const {
    std::map<SupplierID, Supplier*>::const_iterator it = suppliers.find(suppId);
    if (it == suppliers.end()) {
        return NULL;
    }
    return it->second;
}

std::vector<Supplier*> SupplierManager::getAllSuppliers() const {
    std::vector<Supplier*> allSuppliers;
    for (std::map<SupplierID, Supplier*>::const_iterator it = suppliers.begin();
         it != suppliers.end(); ++it) {
        allSuppliers.push_back(it->second);
    }
    return allSuppliers;
}

std::vector<Supplier*> SupplierManager::getActiveSuppliers() const {
    std::vector<Supplier*> activeSuppliers;
    for (std::map<SupplierID, Supplier*>::const_iterator it = suppliers.begin();
         it != suppliers.end(); ++it) {
        if (it->second->getIsActive()) {
            activeSuppliers.push_back(it->second);
        }
    }
    return activeSuppliers;
}

std::vector<Supplier*> SupplierManager::searchSuppliers(const std::string& keyword) const {
    std::vector<Supplier*> results;
    
    for (std::map<SupplierID, Supplier*>::const_iterator it = suppliers.begin();
         it != suppliers.end(); ++it) {
        if (it->second->getName().find(keyword) != std::string::npos ||
            it->second->getContactPerson().find(keyword) != std::string::npos ||
            it->second->getEmail().find(keyword) != std::string::npos) {
            results.push_back(it->second);
        }
    }
    
    return results;
}

std::vector<Supplier*> SupplierManager::getSuppliersByRating(int minRating) const {
    std::vector<Supplier*> results;
    
    for (std::map<SupplierID, Supplier*>::const_iterator it = suppliers.begin();
         it != suppliers.end(); ++it) {
        if (it->second->getReliabilityRating() >= minRating) {
            results.push_back(it->second);
        }
    }
    
    return results;
}

void SupplierManager::displayAllSuppliers() const {
    std::cout << "\n========== All Suppliers ==========\n";
    for (std::map<SupplierID, Supplier*>::const_iterator it = suppliers.begin();
         it != suppliers.end(); ++it) {
        it->second->displayInfo();
    }
    std::cout << "Total suppliers: " << suppliers.size() << "\n";
}

int SupplierManager::getTotalSupplierCount() const {
    return static_cast<int>(suppliers.size());
}

double SupplierManager::getTotalOutstandingBalance() const {
    double total = 0.0;
    for (std::map<SupplierID, Supplier*>::const_iterator it = suppliers.begin();
         it != suppliers.end(); ++it) {
        total += it->second->getCurrentBalance();
    }
    return total;
}

void SupplierManager::clearAllSuppliers() {
    for (std::map<SupplierID, Supplier*>::iterator it = suppliers.begin();
         it != suppliers.end(); ++it) {
        delete it->second;
    }
    suppliers.clear();
    nextSupplierId = 1;
}
