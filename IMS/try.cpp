#include <iostream>
#include <cstring>
#include <cctype>

using namespace std;

// ABSTRACTION & ENCAPSULATION 
// Base class for inventory items
class InventoryItem {
protected:
    // Base attributes - keeping these generic since child classes will specify exact names
    int id;
    char name[50];
    int quantity;

public:
    // Constructor
    InventoryItem(int _id = 0, const char* _name = "", int _qty = 0) {
        id = _id;
        strncpy(name, _name, 49);
        name[49] = '\0'; 
        quantity = _qty;
    }

    // Getters
    int getId() const { return id; }
    const char* getName() const { return name; }
    int getQuantity() const { return quantity; }

    virtual void display() const = 0; 

    int getNameLength() const {
        return strlen(name);
    }
};

// ===== INHERITANCE =====
class RawMaterial : public InventoryItem {
private:
    // Following class diagram exactly
    float costPerUnit;  // From class diagram

public:
    // Constructor using diagram variable names internally
    RawMaterial(int rawMatID = 0, const char* rawMatName = "", int rawMatQuantity = 0, float _cost = 0.0)
        : InventoryItem(rawMatID, rawMatName, rawMatQuantity), costPerUnit(_cost) {}

    float getCostPerUnit() const { return costPerUnit; }

    // POLYMORPHISM 
    void display() const override {
        cout << id << "\t";
        
        cout << name;
        if (getNameLength() < 8)
            cout << "\t\t\t";
        else if (getNameLength() < 16)
            cout << "\t\t";
        else
            cout << "\t";
            
        cout << quantity << "\t\t";
        
        cout.precision(2);
        cout << fixed << costPerUnit << endl;
    }
};

class Product : public InventoryItem {
private:
    // Following class diagram exactly
    float prodPrice;  // From class diagram

public:
    // Constructor using diagram variable names internally
    Product(int productID = 0, const char* productName = "", int prodQuantity = 0, float _price = 0.0)
        : InventoryItem(productID, productName, prodQuantity), prodPrice(_price) {}

    float getProdPrice() const { return prodPrice; }

    // POLYMORPHISM
    void display() const override {
        cout << id << "\t";
        
        cout << name;
        if (getNameLength() < 8)
            cout << "\t\t\t";
        else if (getNameLength() < 16)
            cout << "\t\t";
        else
            cout << "\t";
        
        cout << quantity << "\t\t";
        
        cout.precision(2);
        cout << fixed << prodPrice << endl;
    }
};

class InventoryManager {
private:
    // Using exact variable names from class diagram
    RawMaterial rawMatInventory[10];  // Named exactly as per diagram implication
    Product productInventory[10];     // Named exactly as per diagram implication
    int rawMatCount;                  // Counter for raw materials
    int productCount;                 // Counter for products

    void clearInputBuffer() {
        cin.clear();
        while (cin.get() != '\n');
    }

    char toLowerChar(char c) {
        if (c >= 'A' && c <= 'Z') {
            return c + 32;
        }
        return c;
    }

public:
    InventoryManager() : rawMatCount(0), productCount(0) {
        // Added cost parameter for raw materials
        addRawMaterial(1, "Cotton Fabric", 500, 150.0);
        addRawMaterial(2, "Polyester Fabric", 450, 120.0);
        addRawMaterial(3, "Denim", 300, 200.0);
        addRawMaterial(4, "Silk Fabric", 150, 350.0);
        addRawMaterial(5, "Wool Fabric", 200, 250.0);
        addRawMaterial(6, "Thread", 1000, 10.0);
        addRawMaterial(7, "Buttons", 2500, 5.0);
        addRawMaterial(8, "Zippers", 1200, 15.0);
        addRawMaterial(9, "Elastic Bands", 800, 8.0);
        addRawMaterial(10, "Labels/Tags", 1200, 3.0);

        addProduct(1, "Men's T-Shirts", 200, 220.0);
        addProduct(2, "Women's Blouses", 180, 320.0);
        addProduct(3, "Denim Jeans", 150, 650.0);
        addProduct(4, "Hoodies", 120, 750.0);
        addProduct(5, "Jackets", 100, 750.0);
        addProduct(6, "Formal Trousers", 90, 340.0);
        addProduct(7, "Skirts", 110, 490.0);
        addProduct(8, "Dresses", 85, 300.0);
        addProduct(9, "Shorts", 95, 270.0);
        addProduct(10, "Polo Shirts", 130, 270.0);
    }

    // Using exact variable names from class diagram
    void addRawMaterial(int rawMatID, const char* rawMatName, int rawMatQuantity, float costPerUnit) {
        if (rawMatCount < 10) {
            rawMatInventory[rawMatCount++] = RawMaterial(rawMatID, rawMatName, rawMatQuantity, costPerUnit);
        }
    }

    // Using exact variable names from class diagram
    void addProduct(int productID, const char* productName, int prodQuantity, float prodPrice) {
        if (productCount < 10) {
            productInventory[productCount++] = Product(productID, productName, prodQuantity, prodPrice);
        }
    }

    void displayRawMaterialInventory() const {
        cout << "\n--- Raw Material Inventory ---" << endl;
        cout << "ID\tName\t\t\tQuantity\tCost/Unit (₱)" << endl;
        cout << "----------------------------------------------------------" << endl;
        
        for (int i = 0; i < rawMatCount; i++) {
            rawMatInventory[i].display();
        }
        
        cout << "----------------------------------------------------------" << endl;
    }

    // Display products inventory
    void displayProductInventory() const {
        cout << "\n--- Product Inventory ---" << endl;
        cout << "ID\tName\t\t\tQuantity\tPrice (₱)" << endl;
        cout << "----------------------------------------------------------" << endl;
        
        for (int i = 0; i < productCount; i++) {
            productInventory[i].display();
        }
        
        cout << "----------------------------------------------------------" << endl;
    }

    int getValidIntInput(const char* prompt, int minValue) {
        int input;
        bool isValid = false;
        
        do {
            cout << prompt;
            if (cin >> input) {
                if (input >= minValue) {
                    isValid = true;
                } else {
                    cout << "Invalid input. Value must be " << minValue << " or greater." << endl;
                }
            } else {
                cout << "Invalid input. Please enter a number." << endl;
                cin.clear();
                while (cin.get() != '\n');
            }
        } while (!isValid);
        
        clearInputBuffer();
        return input;
    }

    bool getConfirmation(const char* prompt) {
        char response[10];
        
        cout << prompt << " (y/n): ";
        cin.getline(response, 10);
        
        return (toLowerChar(response[0]) == 'y');
    }
};

int main() {
    InventoryManager inventoryManager;
    int choice;
    bool menu = true;
    
    while (menu) {
        cout << "\n--Inventory Management Menu--" << endl;
        cout << "1. Raw Material Inventory" << endl;
        cout << "2. Product Inventory" << endl;
        cout << "3. Exit" << endl;
        
        choice = inventoryManager.getValidIntInput("Enter your choice (1-3): ", 1);
        
        switch (choice) {
            case 1:
                inventoryManager.displayRawMaterialInventory();
                break;
                
            case 2:
                inventoryManager.displayProductInventory();
                break;
                
            case 3:
                if (inventoryManager.getConfirmation("Are you sure you want to exit?")) {
                    cout << "Byebye." << endl;
                    menu = false;
                }
                break;
                
            default:
                if (choice > 3) {
                    cout << "Invalid choice. Please try again." << endl;
                }
                break;
        }
    }
    
    return 0;
}