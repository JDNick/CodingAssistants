#include "Category.h"
#include "Item.h"

// Category implementation
Category::Category() 
    : categoryId(0), name(""), description(""), parentCategory(NULL),
      taxRate(TAX_RATE), isActive(true) {
}

Category::Category(unsigned int id, const std::string& catName)
    : categoryId(id), name(catName), description(""), parentCategory(NULL),
      taxRate(TAX_RATE), isActive(true) {
}

Category::Category(unsigned int id, const std::string& catName, Category* parent)
    : categoryId(id), name(catName), description(""), parentCategory(parent),
      taxRate(TAX_RATE), isActive(true) {
    if (parent != NULL) {
        parent->addSubCategory(this);
    }
}

Category::~Category() {
    // Clean up subcategories
    for (std::vector<Category*>::iterator it = subCategories.begin();
         it != subCategories.end(); ++it) {
        delete *it;
    }
    subCategories.clear();
    
    // Note: We don't delete items as they're managed by inventory
    items.clear();
}

void Category::addSubCategory(Category* subCat) {
    if (subCat == NULL) {
        LOG_ERROR("Attempted to add NULL subcategory");
        return;
    }
    
    // Check if already exists
    for (std::vector<Category*>::iterator it = subCategories.begin();
         it != subCategories.end(); ++it) {
        if (*it == subCat) {
            LOG_INFO("Subcategory already exists");
            return;
        }
    }
    
    subCategories.push_back(subCat);
    subCat->setParentCategory(this);
}

bool Category::removeSubCategory(unsigned int catId) {
    for (std::vector<Category*>::iterator it = subCategories.begin();
         it != subCategories.end(); ++it) {
        if ((*it)->getCategoryId() == catId) {
            delete *it;
            subCategories.erase(it);
            return true;
        }
    }
    return false;
}

Category* Category::findSubCategory(unsigned int catId) {
    for (std::vector<Category*>::iterator it = subCategories.begin();
         it != subCategories.end(); ++it) {
        if ((*it)->getCategoryId() == catId) {
            return *it;
        }
    }
    return NULL;
}

std::vector<Category*> Category::getAllSubCategories() const {
    return subCategories;
}

void Category::addItem(Item* item) {
    if (item == NULL) {
        LOG_ERROR("Attempted to add NULL item to category");
        return;
    }
    items.push_back(item);
    item->setCategory(this);
}

bool Category::removeItem(ItemID itemId) {
    for (std::vector<Item*>::iterator it = items.begin();
         it != items.end(); ++it) {
        if ((*it)->getId() == itemId) {
            items.erase(it);
            return true;
        }
    }
    return false;
}

Item* Category::findItem(ItemID itemId) {
    for (std::vector<Item*>::iterator it = items.begin();
         it != items.end(); ++it) {
        if ((*it)->getId() == itemId) {
            return *it;
        }
    }
    return NULL;
}

std::vector<Item*> Category::getAllItems() const {
    return items;
}

int Category::getItemCount() const {
    return static_cast<int>(items.size());
}

void Category::displayCategoryTree(int depth) const {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    std::cout << "|- " << name << " (" << items.size() << " items)\n";
    
    for (std::vector<Category*>::const_iterator it = subCategories.begin();
         it != subCategories.end(); ++it) {
        (*it)->displayCategoryTree(depth + 1);
    }
}

int Category::getTotalItemCount() const {
    int count = static_cast<int>(items.size());
    
    for (std::vector<Category*>::const_iterator it = subCategories.begin();
         it != subCategories.end(); ++it) {
        count += (*it)->getTotalItemCount();
    }
    
    return count;
}

double Category::getTotalValue() const {
    double total = 0.0;
    
    for (std::vector<Item*>::const_iterator it = items.begin();
         it != items.end(); ++it) {
        if (*it != NULL) {
            total += (*it)->calculatePrice() * (*it)->getStockQuantity();
        }
    }
    
    for (std::vector<Category*>::const_iterator it = subCategories.begin();
         it != subCategories.end(); ++it) {
        total += (*it)->getTotalValue();
    }
    
    return total;
}

std::string Category::getFullPath() const {
    if (parentCategory == NULL) {
        return name;
    }
    return parentCategory->getFullPath() + " > " + name;
}

// CategoryManager implementation (Singleton)
CategoryManager* CategoryManager::instance = NULL;

CategoryManager::CategoryManager() : nextCategoryId(1) {
    rootCategory = new Category(0, "Root");
    categories[0] = rootCategory;
}

CategoryManager::~CategoryManager() {
    clearAllCategories();
    delete rootCategory;
}

CategoryManager* CategoryManager::getInstance() {
    if (instance == NULL) {
        instance = new CategoryManager();
    }
    return instance;
}

void CategoryManager::destroyInstance() {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

Category* CategoryManager::createCategory(const std::string& name, Category* parent) {
    unsigned int id = nextCategoryId++;
    Category* newCategory = new Category(id, name, parent);
    categories[id] = newCategory;
    
    if (parent == NULL) {
        rootCategory->addSubCategory(newCategory);
    }
    
    LOG_INFO("Created category: " + name + " (ID: " + Utils::intToString(id) + ")");
    return newCategory;
}

bool CategoryManager::deleteCategory(unsigned int catId) {
    if (catId == 0) {
        LOG_ERROR("Cannot delete root category");
        return false;
    }
    
    std::map<unsigned int, Category*>::iterator it = categories.find(catId);
    if (it == categories.end()) {
        LOG_ERROR("Category not found");
        return false;
    }
    
    Category* cat = it->second;
    Category* parent = cat->getParentCategory();
    
    if (parent != NULL) {
        parent->removeSubCategory(catId);
    }
    
    categories.erase(it);
    return true;
}

Category* CategoryManager::getCategory(unsigned int catId) const {
    std::map<unsigned int, Category*>::const_iterator it = categories.find(catId);
    if (it == categories.end()) {
        return NULL;
    }
    return it->second;
}

std::vector<Category*> CategoryManager::getAllCategories() const {
    std::vector<Category*> allCats;
    for (std::map<unsigned int, Category*>::const_iterator it = categories.begin();
         it != categories.end(); ++it) {
        allCats.push_back(it->second);
    }
    return allCats;
}

std::vector<Category*> CategoryManager::searchCategories(const std::string& keyword) const {
    std::vector<Category*> results;
    
    for (std::map<unsigned int, Category*>::const_iterator it = categories.begin();
         it != categories.end(); ++it) {
        if (it->second->getName().find(keyword) != std::string::npos ||
            it->second->getDescription().find(keyword) != std::string::npos) {
            results.push_back(it->second);
        }
    }
    
    return results;
}

void CategoryManager::displayAllCategories() const {
    std::cout << "\n========== Category Hierarchy ==========\n";
    rootCategory->displayCategoryTree(0);
    std::cout << "========================================\n";
    std::cout << "Total categories: " << categories.size() << "\n";
    std::cout << "Total items: " << rootCategory->getTotalItemCount() << "\n";
}

int CategoryManager::getTotalCategoryCount() const {
    return static_cast<int>(categories.size());
}

void CategoryManager::clearAllCategories() {
    for (std::map<unsigned int, Category*>::iterator it = categories.begin();
         it != categories.end(); ++it) {
        if (it->first != 0) { // Don't delete root here
            delete it->second;
        }
    }
    categories.clear();
    categories[0] = rootCategory;
    nextCategoryId = 1;
}
