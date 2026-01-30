#include "Order.h"
#include "Item.h"
#include "Customer.h"

// OrderItem implementation
OrderItem::OrderItem()
    : item(NULL), quantity(0), unitPrice(0.0), discount(0.0) {
}

OrderItem::OrderItem(Item* itm, int qty, double price)
    : item(itm), quantity(qty), unitPrice(price), discount(0.0) {
}

OrderItem::~OrderItem() {
    // Note: We don't delete item as it's managed by inventory
}

void OrderItem::setQuantity(int qty) {
    if (qty < 0) {
        LOG_ERROR("Cannot set negative quantity");
        return;
    }
    quantity = qty;
}

void OrderItem::setDiscount(double disc) {
    if (disc < 0.0 || disc > 1.0) {
        LOG_ERROR("Discount must be between 0 and 1");
        return;
    }
    discount = disc;
}

double OrderItem::calculateSubtotal() const {
    return unitPrice * quantity * (1.0 - discount);
}

double OrderItem::calculateTax() const {
    return calculateSubtotal() * TAX_RATE;
}

double OrderItem::calculateTotal() const {
    return calculateSubtotal() + calculateTax();
}

void OrderItem::displayInfo() const {
    std::cout << "  ";
    if (item != NULL) {
        std::cout << item->getName() << " (ID: " << item->getId() << ")";
    }
    std::cout << "\n";
    std::cout << "    Quantity: " << quantity << "\n";
    std::cout << "    Unit Price: $" << unitPrice << "\n";
    if (discount > 0.0) {
        std::cout << "    Discount: " << (discount * 100) << "%\n";
    }
    std::cout << "    Subtotal: $" << calculateSubtotal() << "\n";
    std::cout << "    Tax: $" << calculateTax() << "\n";
    std::cout << "    Total: $" << calculateTotal() << "\n";
}

// Order implementation
Order::Order()
    : id(0), customer(NULL), status(ORDER_PENDING), 
      orderDate(time(0)), shipDate(0), deliveryDate(0),
      shippingAddress(""), billingAddress(""), 
      paymentMethod(PAYMENT_CREDIT_CARD), trackingNumber(""),
      shippingCost(0.0), orderNotes("") {
}

Order::Order(OrderID orderId, Customer* cust)
    : id(orderId), customer(cust), status(ORDER_PENDING),
      orderDate(time(0)), shipDate(0), deliveryDate(0),
      shippingAddress(""), billingAddress(""),
      paymentMethod(PAYMENT_CREDIT_CARD), trackingNumber(""),
      shippingCost(0.0), orderNotes("") {
}

Order::~Order() {
    for (std::vector<OrderItem*>::iterator it = items.begin();
         it != items.end(); ++it) {
        delete *it;
    }
    items.clear();
}

void Order::addItem(Item* item, int quantity) {
    if (item == NULL) {
        LOG_ERROR("Cannot add NULL item to order");
        return;
    }
    
    if (quantity <= 0) {
        LOG_ERROR("Invalid quantity");
        return;
    }
    
    // Check if item already in order
    for (std::vector<OrderItem*>::iterator it = items.begin();
         it != items.end(); ++it) {
        if ((*it)->getItem() == item) {
            (*it)->setQuantity((*it)->getQuantity() + quantity);
            LOG_INFO("Updated quantity for existing item in order");
            return;
        }
    }
    
    // Add new order item
    double price = item->calculatePrice();
    OrderItem* orderItem = new OrderItem(item, quantity, price);
    
    // Apply quantity-based discount
    double discount = Utils::calculateDiscount(price * quantity);
    orderItem->setDiscount(discount);
    
    items.push_back(orderItem);
}

bool Order::removeItem(ItemID itemId) {
    for (std::vector<OrderItem*>::iterator it = items.begin();
         it != items.end(); ++it) {
        if ((*it)->getItem() != NULL && (*it)->getItem()->getId() == itemId) {
            delete *it;
            items.erase(it);
            return true;
        }
    }
    return false;
}

OrderItem* Order::findOrderItem(ItemID itemId) const {
    for (std::vector<OrderItem*>::const_iterator it = items.begin();
         it != items.end(); ++it) {
        if ((*it)->getItem() != NULL && (*it)->getItem()->getId() == itemId) {
            return *it;
        }
    }
    return NULL;
}

double Order::calculateSubtotal() const {
    double subtotal = 0.0;
    for (std::vector<OrderItem*>::const_iterator it = items.begin();
         it != items.end(); ++it) {
        subtotal += (*it)->calculateSubtotal();
    }
    return subtotal;
}

double Order::calculateTax() const {
    double tax = 0.0;
    for (std::vector<OrderItem*>::const_iterator it = items.begin();
         it != items.end(); ++it) {
        tax += (*it)->calculateTax();
    }
    return tax;
}

double Order::calculateTotal() const {
    return calculateSubtotal() + calculateTax();
}

double Order::calculateTotalWithShipping() const {
    return calculateTotal() + shippingCost;
}

void Order::setStatus(OrderStatus newStatus) {
    OrderStatus oldStatus = status;
    status = newStatus;
    
    // Update dates based on status
    if (newStatus == ORDER_SHIPPED && shipDate == 0) {
        shipDate = time(0);
    } else if (newStatus == ORDER_DELIVERED && deliveryDate == 0) {
        deliveryDate = time(0);
    }
    
    OrderManager* manager = OrderManager::getInstance();
    // Notify observers through manager would go here
    // manager->notifyOrderStatusChanged(this, oldStatus, newStatus);
}

bool Order::validateOrder() const {
    if (customer == NULL) {
        LOG_ERROR("Order has no customer");
        return false;
    }
    
    if (items.empty()) {
        LOG_ERROR("Order has no items");
        return false;
    }
    
    if (shippingAddress.empty()) {
        LOG_ERROR("No shipping address specified");
        return false;
    }
    
    if (billingAddress.empty()) {
        LOG_ERROR("No billing address specified");
        return false;
    }
    
    return true;
}

bool Order::processOrder() {
    if (!validateOrder()) {
        return false;
    }
    
    if (status != ORDER_PENDING) {
        LOG_ERROR("Order is not in pending status");
        return false;
    }
    
    // Calculate shipping cost
    shippingCost = SHIPPING_BASE_COST;
    for (std::vector<OrderItem*>::const_iterator it = items.begin();
         it != items.end(); ++it) {
        if ((*it)->getItem() != NULL) {
            shippingCost += (*it)->getItem()->calculateShippingCost() * 0.5;
        }
    }
    
    setStatus(ORDER_PROCESSING);
    LOG_INFO("Order processed successfully");
    return true;
}

bool Order::shipOrder() {
    if (status != ORDER_PROCESSING) {
        LOG_ERROR("Order must be in processing status to ship");
        return false;
    }
    
    trackingNumber = Utils::generateOrderNumber();
    setStatus(ORDER_SHIPPED);
    LOG_INFO("Order shipped with tracking: " + trackingNumber);
    return true;
}

bool Order::cancelOrder() {
    if (status == ORDER_SHIPPED || status == ORDER_DELIVERED) {
        LOG_ERROR("Cannot cancel shipped or delivered order");
        return false;
    }
    
    setStatus(ORDER_CANCELLED);
    LOG_INFO("Order cancelled");
    return true;
}

bool Order::returnOrder() {
    if (status != ORDER_DELIVERED) {
        LOG_ERROR("Can only return delivered orders");
        return false;
    }
    
    setStatus(ORDER_RETURNED);
    LOG_INFO("Order marked as returned");
    return true;
}

void Order::displayOrderSummary() const {
    std::cout << "==========================================\n";
    std::cout << "Order #" << id << "\n";
    if (customer != NULL) {
        std::cout << "Customer: [Customer ID " << customer->getId() << "]\n";
    }
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&orderDate));
    std::cout << "Order Date: " << buffer << "\n";
    
    std::cout << "Status: " << getStatusString() << "\n";
    std::cout << "Items: " << items.size() << "\n";
    std::cout << "Subtotal: $" << calculateSubtotal() << "\n";
    std::cout << "Tax: $" << calculateTax() << "\n";
    std::cout << "Shipping: $" << shippingCost << "\n";
    std::cout << "Total: $" << calculateTotalWithShipping() << "\n";
    std::cout << "==========================================\n";
}

void Order::displayDetailedOrder() const {
    displayOrderSummary();
    
    std::cout << "\nOrder Items:\n";
    for (std::vector<OrderItem*>::const_iterator it = items.begin();
         it != items.end(); ++it) {
        (*it)->displayInfo();
    }
    
    std::cout << "\nShipping Address:\n" << shippingAddress << "\n";
    std::cout << "\nBilling Address:\n" << billingAddress << "\n";
    
    if (!trackingNumber.empty()) {
        std::cout << "\nTracking Number: " << trackingNumber << "\n";
    }
    
    if (!orderNotes.empty()) {
        std::cout << "\nNotes: " << orderNotes << "\n";
    }
}

std::string Order::getStatusString() const {
    switch (status) {
        case ORDER_PENDING: return "Pending";
        case ORDER_PROCESSING: return "Processing";
        case ORDER_SHIPPED: return "Shipped";
        case ORDER_DELIVERED: return "Delivered";
        case ORDER_CANCELLED: return "Cancelled";
        case ORDER_RETURNED: return "Returned";
        default: return "Unknown";
    }
}

int Order::getDaysSinceOrder() const {
    time_t now = time(0);
    double seconds = difftime(now, orderDate);
    return static_cast<int>(seconds / (60 * 60 * 24));
}

// OrderManager implementation (Singleton + Observer)
OrderManager* OrderManager::instance = NULL;

OrderManager::OrderManager() : nextOrderId(1) {
}

OrderManager::~OrderManager() {
    clearAllOrders();
    observers.clear();
}

OrderManager* OrderManager::getInstance() {
    if (instance == NULL) {
        instance = new OrderManager();
    }
    return instance;
}

void OrderManager::destroyInstance() {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

Order* OrderManager::createOrder(Customer* customer) {
    if (customer == NULL) {
        LOG_ERROR("Cannot create order without customer");
        return NULL;
    }
    
    OrderID id = nextOrderId++;
    Order* newOrder = new Order(id, customer);
    orders[id] = newOrder;
    
    notifyOrderCreated(newOrder);
    LOG_INFO("Created order ID: " + Utils::intToString(id));
    return newOrder;
}

bool OrderManager::deleteOrder(OrderID orderId) {
    std::map<OrderID, Order*>::iterator it = orders.find(orderId);
    if (it == orders.end()) {
        LOG_ERROR("Order not found");
        return false;
    }
    
    delete it->second;
    orders.erase(it);
    return true;
}

Order* OrderManager::getOrder(OrderID orderId) const {
    std::map<OrderID, Order*>::const_iterator it = orders.find(orderId);
    if (it == orders.end()) {
        return NULL;
    }
    return it->second;
}

std::vector<Order*> OrderManager::getAllOrders() const {
    std::vector<Order*> allOrders;
    for (std::map<OrderID, Order*>::const_iterator it = orders.begin();
         it != orders.end(); ++it) {
        allOrders.push_back(it->second);
    }
    return allOrders;
}

std::vector<Order*> OrderManager::getOrdersByCustomer(CustomerID custId) const {
    std::vector<Order*> customerOrders;
    for (std::map<OrderID, Order*>::const_iterator it = orders.begin();
         it != orders.end(); ++it) {
        if (it->second->getCustomer() != NULL && 
            it->second->getCustomer()->getId() == custId) {
            customerOrders.push_back(it->second);
        }
    }
    return customerOrders;
}

std::vector<Order*> OrderManager::getOrdersByStatus(OrderStatus status) const {
    std::vector<Order*> statusOrders;
    for (std::map<OrderID, Order*>::const_iterator it = orders.begin();
         it != orders.end(); ++it) {
        if (it->second->getStatus() == status) {
            statusOrders.push_back(it->second);
        }
    }
    return statusOrders;
}

std::vector<Order*> OrderManager::getOrdersByDateRange(time_t startDate, time_t endDate) const {
    std::vector<Order*> rangeOrders;
    for (std::map<OrderID, Order*>::const_iterator it = orders.begin();
         it != orders.end(); ++it) {
        time_t orderDate = it->second->getOrderDate();
        if (orderDate >= startDate && orderDate <= endDate) {
            rangeOrders.push_back(it->second);
        }
    }
    return rangeOrders;
}

void OrderManager::addObserver(OrderObserver* observer) {
    if (observer != NULL) {
        observers.push_back(observer);
    }
}

void OrderManager::removeObserver(OrderObserver* observer) {
    for (std::vector<OrderObserver*>::iterator it = observers.begin();
         it != observers.end(); ++it) {
        if (*it == observer) {
            observers.erase(it);
            return;
        }
    }
}

void OrderManager::notifyOrderCreated(Order* order) {
    for (std::vector<OrderObserver*>::iterator it = observers.begin();
         it != observers.end(); ++it) {
        (*it)->onOrderCreated(order);
    }
}

void OrderManager::notifyOrderStatusChanged(Order* order, OrderStatus oldStatus, OrderStatus newStatus) {
    for (std::vector<OrderObserver*>::iterator it = observers.begin();
         it != observers.end(); ++it) {
        (*it)->onOrderStatusChanged(order, oldStatus, newStatus);
    }
}

void OrderManager::notifyOrderCancelled(Order* order) {
    for (std::vector<OrderObserver*>::iterator it = observers.begin();
         it != observers.end(); ++it) {
        (*it)->onOrderCancelled(order);
    }
}

double OrderManager::getTotalRevenue() const {
    double revenue = 0.0;
    for (std::map<OrderID, Order*>::const_iterator it = orders.begin();
         it != orders.end(); ++it) {
        if (it->second->getStatus() != ORDER_CANCELLED && 
            it->second->getStatus() != ORDER_RETURNED) {
            revenue += it->second->calculateTotalWithShipping();
        }
    }
    return revenue;
}

double OrderManager::getAverageOrderValue() const {
    if (orders.empty()) return 0.0;
    return getTotalRevenue() / orders.size();
}

void OrderManager::displayAllOrders() const {
    std::cout << "\n========== All Orders ==========\n";
    for (std::map<OrderID, Order*>::const_iterator it = orders.begin();
         it != orders.end(); ++it) {
        it->second->displayOrderSummary();
    }
    std::cout << "Total orders: " << orders.size() << "\n";
    std::cout << "Total revenue: $" << getTotalRevenue() << "\n";
}

void OrderManager::clearAllOrders() {
    for (std::map<OrderID, Order*>::iterator it = orders.begin();
         it != orders.end(); ++it) {
        delete it->second;
    }
    orders.clear();
    nextOrderId = 1;
}
