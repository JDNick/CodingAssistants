#include "Common.h"
#include "Item.h"
#include "Category.h"
#include "Supplier.h"
#include "Warehouse.h"
#include "Order.h"
#include "Customer.h"

// Concrete OrderObserver implementation for notifications
class NotificationSystem : public OrderObserver {
private:
    std::string systemName;
    
public:
    NotificationSystem(const std::string& name) : systemName(name) {}
    
    virtual void onOrderCreated(Order* order) {
        std::cout << "[" << systemName << "] New order created: #" << order->getId() << "\n";
    }
    
    virtual void onOrderStatusChanged(Order* order, OrderStatus oldStatus, OrderStatus newStatus) {
        std::cout << "[" << systemName << "] Order #" << order->getId() 
                  << " status changed from " << oldStatus << " to " << newStatus << "\n";
    }
    
    virtual void onOrderCancelled(Order* order) {
        std::cout << "[" << systemName << "] Order #" << order->getId() << " was cancelled\n";
    }
};

// Helper class for data generation
class DataGenerator {
public:
    static void generateSampleData() {
        LOG_INFO("Starting sample data generation...");
        
        // Create categories
        CategoryManager* catMgr = CategoryManager::getInstance();
        Category* electronics = catMgr->createCategory("Electronics");
        Category* computers = catMgr->createCategory("Computers", electronics);
        Category* phones = catMgr->createCategory("Phones", electronics);
        Category* food = catMgr->createCategory("Food");
        Category* dairy = catMgr->createCategory("Dairy", food);
        Category* produce = catMgr->createCategory("Produce", food);
        
        // Create suppliers
        SupplierManager* suppMgr = SupplierManager::getInstance();
        Supplier* supplier1 = suppMgr->createSupplier("TechSupply Inc.");
        supplier1->setContactPerson("John Smith");
        supplier1->setEmail("john@techsupply.com");
        supplier1->setPhone("555-0100");
        supplier1->setAddress("123 Tech Street");
        supplier1->setCreditLimit(50000.0);
        supplier1->setReliabilityRating(9);
        
        Supplier* supplier2 = suppMgr->createSupplier("Global Electronics");
        supplier2->setContactPerson("Jane Doe");
        supplier2->setEmail("jane@globalelectronics.com");
        supplier2->setPhone("555-0200");
        supplier2->setAddress("456 Global Ave");
        supplier2->setCreditLimit(100000.0);
        supplier2->setReliabilityRating(8);
        
        Supplier* supplier3 = suppMgr->createSupplier("Fresh Foods Co.");
        supplier3->setContactPerson("Bob Johnson");
        supplier3->setEmail("bob@freshfoods.com");
        supplier3->setPhone("555-0300");
        supplier3->setAddress("789 Food Lane");
        supplier3->setCreditLimit(30000.0);
        supplier3->setReliabilityRating(7);
        
        // Create items
        ElectronicsItem* laptop = new ElectronicsItem(1, "Gaming Laptop", 1299.99, "Dell", 24);
        laptop->setSku("DELL-LAP-001");
        laptop->setWeight(6.5);
        laptop->setStockQuantity(50);
        laptop->setCategory(computers);
        laptop->setSupplier(supplier1);
        laptop->addTag("gaming");
        laptop->addTag("high-performance");
        
        ElectronicsItem* phone = new ElectronicsItem(2, "Smartphone Pro", 899.99, "Samsung", 12);
        phone->setSku("SAM-PHN-001");
        phone->setWeight(0.4);
        phone->setStockQuantity(100);
        phone->setCategory(phones);
        phone->setSupplier(supplier2);
        phone->addTag("5G");
        phone->addTag("flagship");
        
        ElectronicsItem* tablet = new ElectronicsItem(3, "Tablet Ultra", 599.99, "Apple", 12);
        tablet->setSku("APL-TAB-001");
        tablet->setWeight(1.2);
        tablet->setStockQuantity(75);
        tablet->setCategory(electronics);
        tablet->setSupplier(supplier2);
        tablet->addTag("portable");
        
        time_t expDate1 = time(0) + (30 * 24 * 60 * 60); // 30 days from now
        PerishableItem* milk = new PerishableItem(4, "Organic Milk", 4.99, expDate1);
        milk->setSku("MLK-ORG-001");
        milk->setWeight(8.6);
        milk->setStockQuantity(200);
        milk->setCategory(dairy);
        milk->setSupplier(supplier3);
        
        time_t expDate2 = time(0) + (7 * 24 * 60 * 60); // 7 days from now
        PerishableItem* cheese = new PerishableItem(5, "Aged Cheddar", 12.99, expDate2);
        cheese->setSku("CHS-CHD-001");
        cheese->setWeight(1.0);
        cheese->setStockQuantity(150);
        cheese->setCategory(dairy);
        cheese->setSupplier(supplier3);
        
        time_t expDate3 = time(0) + (14 * 24 * 60 * 60); // 14 days from now
        PerishableItem* apples = new PerishableItem(6, "Fresh Apples", 3.99, expDate3);
        apples->setSku("APL-FRS-001");
        apples->setWeight(5.0);
        apples->setStockQuantity(300);
        apples->setCategory(produce);
        apples->setSupplier(supplier3);
        
        FragileItem* vase = new FragileItem(7, "Crystal Vase", 149.99);
        vase->setSku("VAS-CRY-001");
        vase->setWeight(3.0);
        vase->setStockQuantity(25);
        vase->setInsuranceValue(200.0);
        
        FragileItem* mirror = new FragileItem(8, "Antique Mirror", 299.99);
        mirror->setSku("MIR-ANT-001");
        mirror->setWeight(15.0);
        mirror->setStockQuantity(10);
        mirror->setInsuranceValue(350.0);
        
        // Add items to categories
        computers->addItem(laptop);
        phones->addItem(phone);
        electronics->addItem(tablet);
        dairy->addItem(milk);
        dairy->addItem(cheese);
        produce->addItem(apples);
        
        // Add items to suppliers
        supplier1->addSuppliedItem(laptop);
        supplier2->addSuppliedItem(phone);
        supplier2->addSuppliedItem(tablet);
        supplier3->addSuppliedItem(milk);
        supplier3->addSuppliedItem(cheese);
        supplier3->addSuppliedItem(apples);
        
        // Create warehouses
        WarehouseNetwork* whNetwork = WarehouseNetwork::getInstance();
        Warehouse* warehouse1 = whNetwork->createWarehouse("Main Distribution Center");
        warehouse1->setAddress("1000 Warehouse Blvd");
        warehouse1->setMaxCapacity(100000);
        
        Warehouse* warehouse2 = whNetwork->createWarehouse("Regional Hub East");
        warehouse2->setAddress("2000 Hub Street");
        warehouse2->setMaxCapacity(50000);
        
        Warehouse* warehouse3 = whNetwork->createWarehouse("Regional Hub West");
        warehouse3->setAddress("3000 Distribution Ave");
        warehouse3->setMaxCapacity(50000);
        
        // Add inventory to warehouses
        Location loc1(ZONE_A, 1, 1, 1);
        warehouse1->addInventory(laptop, loc1, 30);
        
        Location loc2(ZONE_A, 1, 2, 1);
        warehouse1->addInventory(phone, loc2, 60);
        
        Location loc3(ZONE_B, 2, 1, 1);
        warehouse1->addInventory(tablet, loc3, 40);
        
        Location loc4(ZONE_C, 3, 1, 1);
        warehouse1->addInventory(milk, loc4, 100);
        
        Location loc5(ZONE_A, 1, 1, 2);
        warehouse2->addInventory(laptop, loc5, 20);
        
        Location loc6(ZONE_A, 1, 2, 2);
        warehouse2->addInventory(phone, loc6, 40);
        
        Location loc7(ZONE_B, 2, 1, 2);
        warehouse3->addInventory(cheese, loc7, 80);
        
        Location loc8(ZONE_B, 2, 2, 1);
        warehouse3->addInventory(apples, loc8, 150);
        
        // Create customers
        CustomerManager* custMgr = CustomerManager::getInstance();
        
        Customer* customer1 = custMgr->createCustomer("Alice", "Johnson");
        customer1->setEmail("alice@email.com");
        customer1->setPhone("555-1001");
        customer1->setAddress("100 Main St, Springfield, IL 62701");
        customer1->addLoyaltyPoints(500);
        customer1->addToLifetimeValue(2500.0);
        
        Customer* customer2 = custMgr->createCustomer("Bob", "Smith");
        customer2->setEmail("bob@email.com");
        customer2->setPhone("555-1002");
        customer2->setAddress("200 Oak Ave, Portland, OR 97201");
        customer2->addLoyaltyPoints(1500);
        customer2->addToLifetimeValue(6000.0);
        
        Customer* customer3 = custMgr->createCustomer("Carol", "Davis");
        customer3->setEmail("carol@email.com");
        customer3->setPhone("555-1003");
        customer3->setAddress("300 Pine Rd, Boston, MA 02101");
        customer3->addLoyaltyPoints(3000);
        customer3->addToLifetimeValue(12000.0);
        
        Customer* customer4 = custMgr->createCustomer("David", "Wilson");
        customer4->setEmail("david@email.com");
        customer4->setPhone("555-1004");
        customer4->setAddress("400 Elm St, Austin, TX 78701");
        customer4->addLoyaltyPoints(200);
        customer4->addToLifetimeValue(800.0);
        
        // Create orders
        OrderManager* orderMgr = OrderManager::getInstance();
        
        Order* order1 = orderMgr->createOrder(customer1);
        order1->setShippingAddress(customer1->getAddress());
        order1->setBillingAddress(customer1->getAddress());
        order1->addItem(laptop, 1);
        order1->addItem(phone, 2);
        order1->processOrder();
        
        Order* order2 = orderMgr->createOrder(customer2);
        order2->setShippingAddress(customer2->getAddress());
        order2->setBillingAddress(customer2->getAddress());
        order2->addItem(tablet, 1);
        order2->addItem(milk, 3);
        order2->processOrder();
        order2->shipOrder();
        
        Order* order3 = orderMgr->createOrder(customer3);
        order3->setShippingAddress(customer3->getAddress());
        order3->setBillingAddress(customer3->getAddress());
        order3->addItem(laptop, 2);
        order3->addItem(tablet, 1);
        order3->addItem(phone, 1);
        order3->processOrder();
        order3->shipOrder();
        
        Order* order4 = orderMgr->createOrder(customer1);
        order4->setShippingAddress(customer1->getAddress());
        order4->setBillingAddress(customer1->getAddress());
        order4->addItem(cheese, 2);
        order4->addItem(apples, 5);
        order4->processOrder();
        
        LOG_INFO("Sample data generation complete!");
    }
    
    static void cleanupData() {
        LOG_INFO("Cleaning up all data...");
        
        // Clean up singletons in reverse dependency order
        // Note: In production, would need proper cleanup with reference counting
        // For now, let OS cleanup to avoid segfault
        
        LOG_INFO("Cleanup complete!");
    }
};

// Main menu system
class MenuSystem {
private:
    bool running;
    
    void displayMainMenu() {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════╗\n";
        std::cout << "║   INVENTORY MANAGEMENT SYSTEM v2.0     ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        std::cout << "\n";
        std::cout << "1.  Category Management\n";
        std::cout << "2.  Supplier Management\n";
        std::cout << "3.  Item Management\n";
        std::cout << "4.  Warehouse Management\n";
        std::cout << "5.  Customer Management\n";
        std::cout << "6.  Order Management\n";
        std::cout << "7.  Generate Reports\n";
        std::cout << "8.  Generate Sample Data\n";
        std::cout << "9.  System Information\n";
        std::cout << "0.  Exit\n";
        std::cout << "\nEnter choice: ";
    }
    
    void categoryMenu() {
        CategoryManager* catMgr = CategoryManager::getInstance();
        
        std::cout << "\n--- Category Management ---\n";
        std::cout << "1. Display all categories\n";
        std::cout << "2. Create new category\n";
        std::cout << "3. Search categories\n";
        std::cout << "0. Back\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                catMgr->displayAllCategories();
                break;
            case 2: {
                std::string name;
                std::cout << "Enter category name: ";
                std::getline(std::cin, name);
                catMgr->createCategory(name);
                break;
            }
            case 3: {
                std::string keyword;
                std::cout << "Enter search keyword: ";
                std::getline(std::cin, keyword);
                std::vector<Category*> results = catMgr->searchCategories(keyword);
                std::cout << "Found " << results.size() << " categories\n";
                break;
            }
        }
    }
    
    void supplierMenu() {
        SupplierManager* suppMgr = SupplierManager::getInstance();
        
        std::cout << "\n--- Supplier Management ---\n";
        std::cout << "1. Display all suppliers\n";
        std::cout << "2. Create new supplier\n";
        std::cout << "3. Search suppliers\n";
        std::cout << "4. Display by rating\n";
        std::cout << "0. Back\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                suppMgr->displayAllSuppliers();
                break;
            case 2: {
                std::string name;
                std::cout << "Enter supplier name: ";
                std::getline(std::cin, name);
                suppMgr->createSupplier(name);
                break;
            }
            case 3: {
                std::string keyword;
                std::cout << "Enter search keyword: ";
                std::getline(std::cin, keyword);
                std::vector<Supplier*> results = suppMgr->searchSuppliers(keyword);
                std::cout << "Found " << results.size() << " suppliers\n";
                break;
            }
            case 4: {
                int rating;
                std::cout << "Enter minimum rating (1-10): ";
                std::cin >> rating;
                std::vector<Supplier*> results = suppMgr->getSuppliersByRating(rating);
                std::cout << "Found " << results.size() << " suppliers with rating >= " << rating << "\n";
                break;
            }
        }
    }
    
    void warehouseMenu() {
        WarehouseNetwork* whNetwork = WarehouseNetwork::getInstance();
        
        std::cout << "\n--- Warehouse Management ---\n";
        std::cout << "1. Display network status\n";
        std::cout << "2. Create new warehouse\n";
        std::cout << "3. Display warehouse inventory\n";
        std::cout << "0. Back\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                whNetwork->displayNetworkStatus();
                break;
            case 2: {
                std::string name;
                std::cout << "Enter warehouse name: ";
                std::getline(std::cin, name);
                whNetwork->createWarehouse(name);
                break;
            }
            case 3: {
                unsigned int whId;
                std::cout << "Enter warehouse ID: ";
                std::cin >> whId;
                Warehouse* wh = whNetwork->getWarehouse(whId);
                if (wh != NULL) {
                    wh->displayInventory();
                } else {
                    LOG_ERROR("Warehouse not found");
                }
                break;
            }
        }
    }
    
    void customerMenu() {
        CustomerManager* custMgr = CustomerManager::getInstance();
        
        std::cout << "\n--- Customer Management ---\n";
        std::cout << "1. Display all customers\n";
        std::cout << "2. Create new customer\n";
        std::cout << "3. Search customers\n";
        std::cout << "4. Display top customers\n";
        std::cout << "0. Back\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                custMgr->displayAllCustomers();
                break;
            case 2: {
                std::string firstName, lastName;
                std::cout << "Enter first name: ";
                std::getline(std::cin, firstName);
                std::cout << "Enter last name: ";
                std::getline(std::cin, lastName);
                custMgr->createCustomer(firstName, lastName);
                break;
            }
            case 3: {
                std::string keyword;
                std::cout << "Enter search keyword: ";
                std::getline(std::cin, keyword);
                std::vector<Customer*> results = custMgr->searchCustomers(keyword);
                std::cout << "Found " << results.size() << " customers\n";
                break;
            }
            case 4: {
                std::vector<Customer*> topCustomers = custMgr->getTopCustomersByValue(10);
                std::cout << "\nTop " << topCustomers.size() << " Customers by Value:\n";
                for (size_t i = 0; i < topCustomers.size(); ++i) {
                    topCustomers[i]->displayInfo();
                }
                break;
            }
        }
    }
    
    void orderMenu() {
        OrderManager* orderMgr = OrderManager::getInstance();
        
        std::cout << "\n--- Order Management ---\n";
        std::cout << "1. Display all orders\n";
        std::cout << "2. Display order details\n";
        std::cout << "3. Display orders by status\n";
        std::cout << "4. Display revenue statistics\n";
        std::cout << "0. Back\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                orderMgr->displayAllOrders();
                break;
            case 2: {
                unsigned int orderId;
                std::cout << "Enter order ID: ";
                std::cin >> orderId;
                Order* order = orderMgr->getOrder(orderId);
                if (order != NULL) {
                    order->displayDetailedOrder();
                } else {
                    LOG_ERROR("Order not found");
                }
                break;
            }
            case 3: {
                int status;
                std::cout << "Enter status (0=Pending, 1=Processing, 2=Shipped, 3=Delivered, 4=Cancelled): ";
                std::cin >> status;
                std::vector<Order*> orders = orderMgr->getOrdersByStatus(static_cast<OrderStatus>(status));
                std::cout << "Found " << orders.size() << " orders with specified status\n";
                break;
            }
            case 4:
                std::cout << "\n--- Revenue Statistics ---\n";
                std::cout << "Total Revenue: $" << orderMgr->getTotalRevenue() << "\n";
                std::cout << "Total Orders: " << orderMgr->getTotalOrderCount() << "\n";
                std::cout << "Average Order Value: $" << orderMgr->getAverageOrderValue() << "\n";
                break;
        }
    }
    
    void generateReports() {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════╗\n";
        std::cout << "║         SYSTEM REPORTS                 ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        
        CategoryManager* catMgr = CategoryManager::getInstance();
        SupplierManager* suppMgr = SupplierManager::getInstance();
        WarehouseNetwork* whNetwork = WarehouseNetwork::getInstance();
        CustomerManager* custMgr = CustomerManager::getInstance();
        OrderManager* orderMgr = OrderManager::getInstance();
        
        std::cout << "\nCategories: " << catMgr->getTotalCategoryCount() << "\n";
        std::cout << "Suppliers: " << suppMgr->getTotalSupplierCount() << "\n";
        std::cout << "Warehouses: " << whNetwork->getTotalWarehouseCount() << "\n";
        std::cout << "Customers: " << custMgr->getTotalCustomerCount() << "\n";
        std::cout << "Orders: " << orderMgr->getTotalOrderCount() << "\n";
        
        std::cout << "\nFinancial Summary:\n";
        std::cout << "Total Inventory Value: $" << whNetwork->getTotalNetworkValue() << "\n";
        std::cout << "Total Customer Lifetime Value: $" << custMgr->getTotalCustomerLifetimeValue() << "\n";
        std::cout << "Total Revenue: $" << orderMgr->getTotalRevenue() << "\n";
        std::cout << "Supplier Outstanding Balance: $" << suppMgr->getTotalOutstandingBalance() << "\n";
    }
    
public:
    MenuSystem() : running(true) {}
    
    void run() {
        std::cout << "Initializing Inventory Management System...\n";
        
        // Seed random number generator
        srand(static_cast<unsigned int>(time(0)));
        
        while (running) {
            displayMainMenu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1:
                    categoryMenu();
                    break;
                case 2:
                    supplierMenu();
                    break;
                case 3:
                    std::cout << "Item Management - Not implemented in menu\n";
                    break;
                case 4:
                    warehouseMenu();
                    break;
                case 5:
                    customerMenu();
                    break;
                case 6:
                    orderMenu();
                    break;
                case 7:
                    generateReports();
                    break;
                case 8:
                    DataGenerator::generateSampleData();
                    break;
                case 9:
                    std::cout << "\nSystem: Inventory Management System v2.0\n";
                    std::cout << "Compiled: " << __DATE__ << " " << __TIME__ << "\n";
                    std::cout << "Platform: C++ Legacy Application\n";
                    break;
                case 0:
                    running = false;
                    std::cout << "Exiting system...\n";
                    break;
                default:
                    std::cout << "Invalid choice!\n";
            }
        }
        
        // Cleanup
        DataGenerator::cleanupData();
    }
};

// Main entry point
int main(int argc, char* argv[]) {
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║   LEGACY INVENTORY MANAGEMENT SYSTEM   ║\n";
    std::cout << "║              Version 2.0               ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    try {
        MenuSystem menu;
        menu.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 2;
    }
    
    std::cout << "\nThank you for using the Inventory Management System!\n";
    return 0;
}
