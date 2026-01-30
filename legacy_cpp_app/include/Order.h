#ifndef ORDER_H
#define ORDER_H

#include "Common.h"

// Forward declarations
class Item;
class Customer;

// OrderItem - represents an item in an order
class OrderItem {
private:
    Item* item;
    int quantity;
    double unitPrice;
    double discount;
    
public:
    OrderItem();
    OrderItem(Item* itm, int qty, double price);
    ~OrderItem();
    
    Item* getItem() const { return item; }
    void setItem(Item* itm) { item = itm; }
    
    int getQuantity() const { return quantity; }
    void setQuantity(int qty);
    
    double getUnitPrice() const { return unitPrice; }
    void setUnitPrice(double price) { unitPrice = price; }
    
    double getDiscount() const { return discount; }
    void setDiscount(double disc);
    
    double calculateSubtotal() const;
    double calculateTax() const;
    double calculateTotal() const;
    
    void displayInfo() const;
};

// Order class
class Order {
private:
    OrderID id;
    Customer* customer;
    std::vector<OrderItem*> items;
    OrderStatus status;
    time_t orderDate;
    time_t shipDate;
    time_t deliveryDate;
    std::string shippingAddress;
    std::string billingAddress;
    PaymentMethod paymentMethod;
    std::string trackingNumber;
    double shippingCost;
    std::string orderNotes;
    
public:
    Order();
    Order(OrderID orderId, Customer* cust);
    ~Order();
    
    // Item management
    void addItem(Item* item, int quantity);
    bool removeItem(ItemID itemId);
    OrderItem* findOrderItem(ItemID itemId) const;
    std::vector<OrderItem*> getItems() const { return items; }
    int getItemCount() const { return static_cast<int>(items.size()); }
    
    // Order calculations
    double calculateSubtotal() const;
    double calculateTax() const;
    double calculateTotal() const;
    double calculateTotalWithShipping() const;
    
    // Getters and setters
    OrderID getId() const { return id; }
    void setId(OrderID newId) { id = newId; }
    
    Customer* getCustomer() const { return customer; }
    void setCustomer(Customer* cust) { customer = cust; }
    
    OrderStatus getStatus() const { return status; }
    void setStatus(OrderStatus newStatus);
    
    time_t getOrderDate() const { return orderDate; }
    void setOrderDate(time_t date) { orderDate = date; }
    
    time_t getShipDate() const { return shipDate; }
    void setShipDate(time_t date) { shipDate = date; }
    
    time_t getDeliveryDate() const { return deliveryDate; }
    void setDeliveryDate(time_t date) { deliveryDate = date; }
    
    std::string getShippingAddress() const { return shippingAddress; }
    void setShippingAddress(const std::string& addr) { shippingAddress = addr; }
    
    std::string getBillingAddress() const { return billingAddress; }
    void setBillingAddress(const std::string& addr) { billingAddress = addr; }
    
    PaymentMethod getPaymentMethod() const { return paymentMethod; }
    void setPaymentMethod(PaymentMethod method) { paymentMethod = method; }
    
    std::string getTrackingNumber() const { return trackingNumber; }
    void setTrackingNumber(const std::string& tracking) { trackingNumber = tracking; }
    
    double getShippingCost() const { return shippingCost; }
    void setShippingCost(double cost) { shippingCost = cost; }
    
    // Order processing methods
    bool validateOrder() const;
    bool processOrder();
    bool shipOrder();
    bool cancelOrder();
    bool returnOrder();
    
    // Utility methods
    void displayOrderSummary() const;
    void displayDetailedOrder() const;
    std::string getStatusString() const;
    int getDaysSinceOrder() const;
};

// Observer pattern for order notifications
class OrderObserver {
public:
    virtual ~OrderObserver() {}
    virtual void onOrderCreated(Order* order) = 0;
    virtual void onOrderStatusChanged(Order* order, OrderStatus oldStatus, OrderStatus newStatus) = 0;
    virtual void onOrderCancelled(Order* order) = 0;
};

// OrderManager - manages all orders with Observer pattern
class OrderManager {
private:
    static OrderManager* instance;
    std::map<OrderID, Order*> orders;
    OrderID nextOrderId;
    std::vector<OrderObserver*> observers;
    
    // Private constructor
    OrderManager();
    ~OrderManager();
    
    // Prevent copying
    OrderManager(const OrderManager&);
    OrderManager& operator=(const OrderManager&);
    
    void notifyOrderCreated(Order* order);
    void notifyOrderStatusChanged(Order* order, OrderStatus oldStatus, OrderStatus newStatus);
    void notifyOrderCancelled(Order* order);
    
public:
    static OrderManager* getInstance();
    static void destroyInstance();
    
    // Order operations
    Order* createOrder(Customer* customer);
    bool deleteOrder(OrderID orderId);
    Order* getOrder(OrderID orderId) const;
    
    std::vector<Order*> getAllOrders() const;
    std::vector<Order*> getOrdersByCustomer(CustomerID custId) const;
    std::vector<Order*> getOrdersByStatus(OrderStatus status) const;
    std::vector<Order*> getOrdersByDateRange(time_t startDate, time_t endDate) const;
    
    // Observer management
    void addObserver(OrderObserver* observer);
    void removeObserver(OrderObserver* observer);
    
    // Statistics
    double getTotalRevenue() const;
    int getTotalOrderCount() const { return static_cast<int>(orders.size()); }
    double getAverageOrderValue() const;
    
    // Utility
    void displayAllOrders() const;
    void clearAllOrders();
};

#endif // ORDER_H
