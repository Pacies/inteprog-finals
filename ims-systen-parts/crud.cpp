#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits> 
using namespace std;

// Base class for inventory items
class InventoryItem {
protected:
    int id;
    string name;
    int quantity;

public:
    InventoryItem(int _id = 0, const string& _name = "", int _qty = 0) : id(_id), name(_name), quantity(_qty) {}

    int getId() const { return id; }
    const string& getName() const { return name; }
    int getQuantity() const { return quantity; }

    virtual void display() const = 0;
};

class RawMaterial : public InventoryItem {
private:
    float costPerUnit;

public:
    RawMaterial(int rawMatID = 0, const string& rawMatName = "", int rawMatQuantity = 0, float _costPerUnit = 0.0)
        : InventoryItem(rawMatID, rawMatName, rawMatQuantity), costPerUnit(_costPerUnit) {}

    float getCostPerUnit() const { return costPerUnit; }

    void display() const override {
        cout << setw(5) << id << "\t"
             << setw(20) << left << name << right << "\t"
             << setw(6) << quantity << "\t\t"
             << fixed << setprecision(2) << costPerUnit << endl;
    }
};

class Product : public InventoryItem {
private:
    float prodPrice;

public:
    Product(int productID = 0, const string& prodName = "", int prodQuantity = 0, float _prodPrice = 0.0)
        : InventoryItem(productID, prodName, prodQuantity), prodPrice(_prodPrice) {}

    float getProdPrice() const { return prodPrice; }

    void display() const override {
        cout << setw(5) << id << "\t"
             << setw(20) << left << name << right << "\t"
             << setw(6) << quantity << "\t\t"
             << fixed << setprecision(2) << prodPrice << endl;
    }
};

class InventoryManager {
private:
    vector<RawMaterial> rawMatInventory;
    vector<Product> productInventory;

    void clearInputBuffer() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    char toLowerChar(char c) {
        return tolower(c);
    }

public:
    InventoryManager() {
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

    void addRawMaterial(int rawMatID, const string& rawMatName, int rawMatQuantity, float costPerUnit) {
        rawMatInventory.emplace_back(rawMatID, rawMatName, rawMatQuantity, costPerUnit);
    }

    void addProduct(int productID, const string& prodName, int prodQuantity, float prodPrice) {
        productInventory.emplace_back(productID, prodName, prodQuantity, prodPrice);
    }

    void displayRawMaterialInventory() const {
        cout << "\n             ╔═══❖Raw Material Inventory ❖═══╗ " << endl;
        cout << setw(5) << "ID" << "\t"
             << setw(20) << left << "Name" << right << "\t"
             << setw(6) << "Quantity" << "\t"
             << "Cost/Unit (₱)" << endl;
        cout << "----------------------------------------------------------" << endl;
        for (const auto& item : rawMatInventory) {
            item.display();
        }
        cout << "----------------------------------------------------------" << endl;
    }

    void displayProductInventory() const {
        cout << "\n             ╔══❖ Product Inventory ❖═══╗ " << endl;
        cout << setw(5) << "ID" << "\t"
             << setw(20) << left << "Name" << right << "\t"
             << setw(6) << "Quantity" << "\t"
             << "Price (₱)" << endl;
        cout << "----------------------------------------------------------" << endl;
        for (const auto& item : productInventory) {
            item.display();
        }
        cout << "----------------------------------------------------------" << endl;
    }

    int getValidIntInput(const string& prompt, int minValue) {
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
                clearInputBuffer();
            }
        } while (!isValid);
        clearInputBuffer();
        return input;
    }

    bool getConfirmation(const string& prompt) {
        char response;
        cout << prompt << " (y/n): ";
        cin >> response;
        clearInputBuffer();
        return (toLowerChar(response) == 'y');
    }
};

int main() {
    InventoryManager inventoryManager;
    int choice;
    bool menu = true;

    while (menu) {
        cout << "\n ╔══❖ Inventory Management Menu ❖══╗ " << endl;
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
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }

    return 0;
}