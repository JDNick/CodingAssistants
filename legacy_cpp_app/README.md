# Legacy C++ Inventory Management System

## Overview

This is a complex legacy C++ application designed to serve as a test base for evaluating the performance of various AI coding models. The application simulates a comprehensive inventory management system with approximately 5,000 lines of code.

## Purpose

This codebase is intentionally designed to challenge frontier AI models with:
- Complex class hierarchies and inheritance
- Multiple design patterns (Singleton, Factory, Observer)
- Template programming
- Legacy C++ patterns and conventions
- Raw pointer management
- Complex business logic
- Inter-dependent subsystems

## Architecture

The system consists of several interconnected modules:

### Core Components

1. **Item Management** (`Item.h/cpp`)
   - Base `Item` class with polymorphic behavior
   - Specialized item types: `ElectronicsItem`, `PerishableItem`, `FragileItem`
   - Template-based `ItemContainer` class
   - Complex pricing and shipping calculations

2. **Category System** (`Category.h/cpp`)
   - Hierarchical category structure
   - CategoryManager singleton
   - Category-item relationships

3. **Supplier Management** (`Supplier.h/cpp`)
   - Supplier data management
   - Credit and payment tracking
   - SupplierManager singleton with factory pattern

4. **Warehouse Network** (`Warehouse.h/cpp`)
   - Multi-warehouse inventory tracking
   - Location-based item storage
   - Inventory transfers between warehouses
   - Capacity management

5. **Customer System** (`Customer.h/cpp`)
   - Customer profiles
   - Loyalty tier system
   - Order history tracking
   - CustomerManager singleton

6. **Order Processing** (`Order.h/cpp`)
   - Order lifecycle management
   - OrderItem composition
   - Observer pattern for notifications
   - Complex order calculations

## Design Patterns Used

- **Singleton Pattern**: CategoryManager, SupplierManager, WarehouseNetwork, CustomerManager, OrderManager
- **Factory Pattern**: Object creation in manager classes
- **Observer Pattern**: Order notification system
- **Template Pattern**: ItemContainer<T>

## Building the Application

### Prerequisites
- C++ compiler with C++98 support (g++, clang++)
- Make utility

### Build Instructions

```bash
# Navigate to the legacy_cpp_app directory
cd legacy_cpp_app

# Build the application
make

# Run the application
make run

# Clean build artifacts
make clean

# Build with debug symbols
make debug

# Build optimized release
make release
```

## Features

The application provides:
- Interactive menu-driven interface
- Category hierarchy management
- Supplier relationship tracking
- Multi-warehouse inventory system
- Customer loyalty program
- Order processing workflow
- Comprehensive reporting

## Code Complexity Features

This codebase includes various complexity elements:
- Multiple levels of inheritance
- Virtual functions and polymorphism
- Template classes
- Operator overloading
- Complex pointer management
- Cross-module dependencies
- Legacy C++ idioms (C++98 style)
- Manual memory management
- Observer pattern implementation
- Singleton pattern with proper cleanup

## Testing with AI Models

This application can be used to test AI coding assistants on:
- Understanding complex class hierarchies
- Navigating multi-file codebases
- Refactoring legacy code
- Adding new features to existing systems
- Debugging complex interactions
- Memory management issues
- Design pattern recognition and modification

## File Structure

```
legacy_cpp_app/
├── include/           # Header files
│   ├── Common.h
│   ├── Item.h
│   ├── Category.h
│   ├── Supplier.h
│   ├── Warehouse.h
│   ├── Order.h
│   └── Customer.h
├── src/              # Implementation files
│   ├── Item.cpp
│   ├── Category.cpp
│   ├── Supplier.cpp
│   ├── Warehouse.cpp
│   ├── Order.cpp
│   ├── Customer.cpp
│   └── main.cpp
├── Makefile          # Build configuration
└── README.md         # This file
```

## License

This code is provided for testing and educational purposes.

## Notes

- This is intentionally legacy-style C++ code (C++98)
- Some patterns may not reflect modern C++ best practices
- The complexity is by design to challenge AI models
- Functionality is simulated and not production-ready
