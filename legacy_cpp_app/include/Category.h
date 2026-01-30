#ifndef CATEGORY_H
#define CATEGORY_H

#include "Common.h"

class Item;

// Category class for organizing items
class Category {
private:
    unsigned int categoryId;
    std::string name;
    std::string description;
    Category* parentCategory;
    std::vector<Category*> subCategories;
    std::vector<Item*> items;
    double taxRate;
    bool isActive;
    
public:
    Category();
    Category(unsigned int id, const std::string& catName);
    Category(unsigned int id, const std::string& catName, Category* parent);
    ~Category();
    
    // Category hierarchy management
    void addSubCategory(Category* subCat);
    bool removeSubCategory(unsigned int catId);
    Category* findSubCategory(unsigned int catId);
    std::vector<Category*> getAllSubCategories() const;
    
    // Item management
    void addItem(Item* item);
    bool removeItem(ItemID itemId);
    Item* findItem(ItemID itemId);
    std::vector<Item*> getAllItems() const;
    int getItemCount() const;
    
    // Getters and setters
    unsigned int getCategoryId() const { return categoryId; }
    void setCategoryId(unsigned int id) { categoryId = id; }
    
    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }
    
    std::string getDescription() const { return description; }
    void setDescription(const std::string& desc) { description = desc; }
    
    Category* getParentCategory() const { return parentCategory; }
    void setParentCategory(Category* parent) { parentCategory = parent; }
    
    double getTaxRate() const { return taxRate; }
    void setTaxRate(double rate) { taxRate = rate; }
    
    bool getIsActive() const { return isActive; }
    void setIsActive(bool active) { isActive = active; }
    
    // Utility methods
    void displayCategoryTree(int depth = 0) const;
    int getTotalItemCount() const;
    double getTotalValue() const;
    std::string getFullPath() const;
};

// CategoryManager - Singleton pattern
class CategoryManager {
private:
    static CategoryManager* instance;
    std::map<unsigned int, Category*> categories;
    Category* rootCategory;
    unsigned int nextCategoryId;
    
    // Private constructor for singleton
    CategoryManager();
    ~CategoryManager();
    
    // Prevent copying
    CategoryManager(const CategoryManager&);
    CategoryManager& operator=(const CategoryManager&);
    
public:
    static CategoryManager* getInstance();
    static void destroyInstance();
    
    // Category operations
    Category* createCategory(const std::string& name, Category* parent = NULL);
    bool deleteCategory(unsigned int catId);
    Category* getCategory(unsigned int catId) const;
    Category* getRootCategory() const { return rootCategory; }
    
    std::vector<Category*> getAllCategories() const;
    std::vector<Category*> searchCategories(const std::string& keyword) const;
    
    // Utility
    void displayAllCategories() const;
    int getTotalCategoryCount() const;
    void clearAllCategories();
};

#endif // CATEGORY_H
