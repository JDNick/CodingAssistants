#include "Item.h"
#include "Category.h"
#include "Supplier.h"
#include <cmath>

// Base Item class implementation
Item::Item() : id(0), name(""), description(""), basePrice(0.0), 
               stockQuantity(0), status(STATUS_AVAILABLE), 
               category(NULL), supplier(NULL), sku(""), 
               weight(0.0), dimensions(""), lastUpdated(time(0)) {
}

Item::Item(ItemID itemId, const std::string& itemName, double price) 
    : id(itemId), name(itemName), description(""), basePrice(price),
      stockQuantity(0), status(STATUS_AVAILABLE), 
      category(NULL), supplier(NULL), sku(""),
      weight(0.0), dimensions(""), lastUpdated(time(0)) {
}

Item::Item(const Item& other) 
    : id(other.id), name(other.name), description(other.description),
      basePrice(other.basePrice), stockQuantity(other.stockQuantity),
      status(other.status), category(other.category), supplier(other.supplier),
      sku(other.sku), weight(other.weight), dimensions(other.dimensions),
      lastUpdated(other.lastUpdated), tags(other.tags) {
}

Item::~Item() {
    // Note: We don't delete category or supplier as they're managed elsewhere
}

double Item::calculatePrice() const {
    double price = basePrice;
    
    // Apply quantity-based discount
    if (stockQuantity > 100) {
        price *= 0.95; // 5% discount for bulk
    }
    
    // Apply status-based adjustments
    if (status == STATUS_DISCONTINUED) {
        price *= 0.80; // 20% discount for discontinued items
    } else if (status == STATUS_ON_ORDER) {
        price *= 1.05; // 5% premium for items on order
    }
    
    return price;
}

double Item::calculateShippingCost() const {
    double cost = SHIPPING_BASE_COST;
    
    // Weight-based calculation
    if (weight > 50.0) {
        cost += (weight - 50.0) * 0.50;
    } else if (weight > 20.0) {
        cost += (weight - 20.0) * 0.30;
    }
    
    // Volume-based surcharge
    if (dimensions.length() > 0) {
        cost += 5.0; // Oversized item surcharge
    }
    
    return cost;
}

std::string Item::getDescription() const {
    std::stringstream ss;
    ss << "Item #" << id << ": " << name << "\n";
    ss << "Price: $" << basePrice << "\n";
    ss << "Stock: " << stockQuantity << " units\n";
    ss << "SKU: " << sku << "\n";
    if (!description.empty()) {
        ss << "Description: " << description << "\n";
    }
    return ss.str();
}

bool Item::isAvailable() const {
    return (status == STATUS_AVAILABLE || status == STATUS_RESERVED) 
           && stockQuantity > 0;
}

void Item::updateStock(int quantity) {
    stockQuantity += quantity;
    lastUpdated = time(0);
    
    if (stockQuantity <= 0) {
        status = STATUS_OUT_OF_STOCK;
        stockQuantity = 0;
    } else if (stockQuantity < MIN_STOCK_LEVEL) {
        LOG_INFO("Low stock warning for item: " + name);
    } else {
        if (status == STATUS_OUT_OF_STOCK) {
            status = STATUS_AVAILABLE;
        }
    }
}

Item* Item::clone() const {
    return new Item(*this);
}

void Item::addTag(const std::string& tag) {
    tags.push_back(tag);
}

Item& Item::operator=(const Item& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        description = other.description;
        basePrice = other.basePrice;
        stockQuantity = other.stockQuantity;
        status = other.status;
        category = other.category;
        supplier = other.supplier;
        sku = other.sku;
        weight = other.weight;
        dimensions = other.dimensions;
        lastUpdated = other.lastUpdated;
        tags = other.tags;
    }
    return *this;
}

bool Item::operator==(const Item& other) const {
    return id == other.id;
}

bool Item::operator<(const Item& other) const {
    return basePrice < other.basePrice;
}

void Item::displayInfo() const {
    std::cout << "==========================================\n";
    std::cout << "Item ID: " << id << "\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "SKU: " << sku << "\n";
    std::cout << "Base Price: $" << basePrice << "\n";
    std::cout << "Current Price: $" << calculatePrice() << "\n";
    std::cout << "Stock Quantity: " << stockQuantity << "\n";
    std::cout << "Status: " << status << "\n";
    std::cout << "Weight: " << weight << " lbs\n";
    if (!description.empty()) {
        std::cout << "Description: " << description << "\n";
    }
    if (category != NULL) {
        std::cout << "Category: [Set]\n";
    }
    if (supplier != NULL) {
        std::cout << "Supplier: [Set]\n";
    }
    if (!tags.empty()) {
        std::cout << "Tags: ";
        for (size_t i = 0; i < tags.size(); ++i) {
            std::cout << tags[i];
            if (i < tags.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
    }
    std::cout << "==========================================\n";
}

bool Item::needsRestock() const {
    return stockQuantity < MIN_STOCK_LEVEL && status != STATUS_DISCONTINUED;
}

void Item::markAsDiscontinued() {
    status = STATUS_DISCONTINUED;
    lastUpdated = time(0);
}

// ElectronicsItem implementation
ElectronicsItem::ElectronicsItem() 
    : Item(), manufacturer(""), warrantyMonths(0), model(""),
      hasExtendedWarranty(false), powerConsumption(0.0) {
}

ElectronicsItem::ElectronicsItem(ItemID id, const std::string& name, 
                                 double price, const std::string& mfr, 
                                 int warranty)
    : Item(id, name, price), manufacturer(mfr), warrantyMonths(warranty),
      model(""), hasExtendedWarranty(false), powerConsumption(0.0) {
}

ElectronicsItem::~ElectronicsItem() {
}

double ElectronicsItem::calculatePrice() const {
    double price = Item::calculatePrice();
    
    // Add warranty cost
    if (hasExtendedWarranty) {
        price += warrantyMonths * 2.99;
    }
    
    // Premium for well-known manufacturers
    if (manufacturer == "Sony" || manufacturer == "Samsung" || 
        manufacturer == "Apple") {
        price *= 1.10;
    }
    
    return price;
}

double ElectronicsItem::calculateShippingCost() const {
    double cost = Item::calculateShippingCost();
    
    // Electronics require special handling
    cost += 10.0;
    
    // High-value items require insurance
    if (basePrice > 500.0) {
        cost += basePrice * 0.02; // 2% insurance
    }
    
    return cost;
}

std::string ElectronicsItem::getDescription() const {
    std::stringstream ss;
    ss << Item::getDescription();
    ss << "Manufacturer: " << manufacturer << "\n";
    ss << "Model: " << model << "\n";
    ss << "Warranty: " << warrantyMonths << " months\n";
    if (hasExtendedWarranty) {
        ss << "Extended warranty included\n";
    }
    ss << "Power Consumption: " << powerConsumption << "W\n";
    return ss.str();
}

Item* ElectronicsItem::clone() const {
    return new ElectronicsItem(*this);
}

bool ElectronicsItem::extendWarranty(int additionalMonths) {
    if (additionalMonths <= 0 || additionalMonths > 60) {
        return false;
    }
    warrantyMonths += additionalMonths;
    hasExtendedWarranty = true;
    return true;
}

// PerishableItem implementation
PerishableItem::PerishableItem()
    : Item(), expirationDate(0), temperature(0.0), 
      requiresRefrigeration(false), storageInstructions("") {
}

PerishableItem::PerishableItem(ItemID id, const std::string& name, 
                               double price, time_t expDate)
    : Item(id, name, price), expirationDate(expDate), temperature(0.0),
      requiresRefrigeration(false), storageInstructions("") {
}

PerishableItem::~PerishableItem() {
}

double PerishableItem::calculatePrice() const {
    double price = Item::calculatePrice();
    
    // Apply expiration-based discount
    double discount = getDiscountBasedOnExpiration();
    price *= (1.0 - discount);
    
    return price;
}

bool PerishableItem::isAvailable() const {
    return Item::isAvailable() && !isExpired();
}

std::string PerishableItem::getDescription() const {
    std::stringstream ss;
    ss << Item::getDescription();
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtime(&expirationDate));
    ss << "Expiration Date: " << buffer << "\n";
    
    if (requiresRefrigeration) {
        ss << "Requires refrigeration at " << temperature << "°F\n";
    }
    
    if (!storageInstructions.empty()) {
        ss << "Storage: " << storageInstructions << "\n";
    }
    
    int days = daysUntilExpiration();
    if (days <= 7) {
        ss << "WARNING: Expires in " << days << " days!\n";
    }
    
    return ss.str();
}

Item* PerishableItem::clone() const {
    return new PerishableItem(*this);
}

bool PerishableItem::isExpired() const {
    return time(0) > expirationDate;
}

int PerishableItem::daysUntilExpiration() const {
    time_t now = time(0);
    double seconds = difftime(expirationDate, now);
    return static_cast<int>(seconds / (60 * 60 * 24));
}

double PerishableItem::getDiscountBasedOnExpiration() const {
    int days = daysUntilExpiration();
    
    if (days <= 1) return 0.50; // 50% off expires tomorrow
    if (days <= 3) return 0.30; // 30% off expires in 3 days
    if (days <= 7) return 0.15; // 15% off expires in a week
    
    return 0.0;
}

// FragileItem implementation
FragileItem::FragileItem()
    : Item(), packagingRequirements(""), requiresInsurance(false),
      insuranceValue(0.0), handlingInstructions("") {
}

FragileItem::FragileItem(ItemID id, const std::string& name, double price)
    : Item(id, name, price), packagingRequirements("Extra padding required"),
      requiresInsurance(false), insuranceValue(0.0), 
      handlingInstructions("Handle with care") {
}

FragileItem::~FragileItem() {
}

double FragileItem::calculateShippingCost() const {
    double cost = Item::calculateShippingCost();
    
    // Extra packaging cost
    cost += 12.50;
    
    // Insurance cost
    if (requiresInsurance) {
        cost += insuranceValue * 0.03; // 3% of insured value
    }
    
    // Special handling fee
    cost += 8.00;
    
    return cost;
}

std::string FragileItem::getDescription() const {
    std::stringstream ss;
    ss << Item::getDescription();
    ss << "FRAGILE ITEM - SPECIAL HANDLING REQUIRED\n";
    ss << "Packaging: " << packagingRequirements << "\n";
    ss << "Handling: " << handlingInstructions << "\n";
    if (requiresInsurance) {
        ss << "Insured for: $" << insuranceValue << "\n";
    }
    return ss.str();
}

Item* FragileItem::clone() const {
    return new FragileItem(*this);
}
