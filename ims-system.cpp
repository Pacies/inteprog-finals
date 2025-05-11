#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

// Function to check credentials against a file
string checkCredentials(const string& username, const string& password, const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Warning: " << filename << " file not found." << endl;
        return "";
    }

    string line;
    while (getline(file, line)) {
        // Parse line assuming format: username,password
        stringstream ss(line);
        string storedUsername, storedPassword;
        
        // Parse using comma as delimiter
        getline(ss, storedUsername, ',');
        getline(ss, storedPassword, ',');
        
        if (username == storedUsername && password == storedPassword) {
            // If filename contains "admin", return "admin", otherwise "employee"
            if (filename.find("admin") != string::npos) {
                return "admin";
            } else {
                return "employee";
            }
        }
    }
    
    return "";
}

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
        cout << "\n             ----- Raw Material Inventory ----- " << endl;
        cout << setw(5) << "ID" << "\t"
             << setw(20) << left << "Name" << right << "\t"
             << setw(6) << "Quantity" << "\t"
             << "Cost/Unit ($)" << endl;
        cout << "----------------------------------------------------------" << endl;
        for (const auto& item : rawMatInventory) {
            item.display();
        }
        cout << "----------------------------------------------------------" << endl;
    }

    void displayProductInventory() const {
        cout << "\n             ----- Product Inventory ----- " << endl;
        cout << setw(5) << "ID" << "\t"
             << setw(20) << left << "Name" << right << "\t"
             << setw(6) << "Quantity" << "\t"
             << "Price ($)" << endl;
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
    
    void runInventoryMenu() {
        int choice;
        bool menu = true;

        while (menu) {
            cout << "\n ----- Inventory Management Menu ----- " << endl;
            cout << "1. Raw Material Inventory" << endl;
            cout << "2. Product Inventory" << endl;
            cout << "3. Return to Main Menu" << endl;

            choice = getValidIntInput("Enter your choice (1-3): ", 1);

            switch (choice) {
                case 1:
                    displayRawMaterialInventory();
                    break;
                case 2:
                    displayProductInventory();
                    break;
                case 3:
                    if (getConfirmation("Are you sure you want to return to the main menu?")) {
                        menu = false;
                    }
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
                    break;
            }
        }
    }
};

// Function for admin menu
void adminMenu() {
    InventoryManager inventoryManager;
    int adminChoice;
    bool adminSession = true;
    
    while (adminSession) {
        cout << "\n--------------------------------" << endl;
        cout << "|       ADMIN DASHBOARD        |" << endl;
        cout << "--------------------------------" << endl;
        cout << "1. Manage Inventory" << endl;
        cout << "2. Manage Users" << endl;
        cout << "3. System Settings" << endl;
        cout << "4. Reports" << endl;
        cout << "5. Logout" << endl;
        
        adminChoice = inventoryManager.getValidIntInput("Enter your choice (1-5): ", 1);
        
        switch (adminChoice) {
            case 1:
                inventoryManager.runInventoryMenu();
                break;
            case 2:
                cout << "User management module - Coming soon!" << endl;
                break;
            case 3:
                cout << "System settings module - Coming soon!" << endl;
                break;
            case 4:
                cout << "Reports module - Coming soon!" << endl;
                break;
            case 5:
                if (inventoryManager.getConfirmation("Are you sure you want to logout?")) {
                    cout << "Logging out from admin account..." << endl;
                    adminSession = false;
                }
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }
}

// Function for employee menu
void employeeMenu() {
    InventoryManager inventoryManager;
    int empChoice;
    bool empSession = true;
    
    while (empSession) {
        cout << "\n--------------------------------" << endl;
        cout << "|      EMPLOYEE DASHBOARD      |" << endl;
        cout << "-------------------------------" << endl;
        cout << "1. View Inventory" << endl;
        cout << "2. Logout" << endl;
        
        empChoice = inventoryManager.getValidIntInput("Enter your choice (1 or 2): ", 1);
        
        switch (empChoice) {
            case 1:
                int viewChoice;
                cout << "\nView Inventory:" << endl;
                cout << "1. Raw Materials" << endl;
                cout << "2. Products" << endl;
                viewChoice = inventoryManager.getValidIntInput("Enter your choice (1 or 2): ", 1);
                
                if (viewChoice == 1) {
                    inventoryManager.displayRawMaterialInventory();
                } else if (viewChoice == 2) {
                    inventoryManager.displayProductInventory();
                } else {
                    cout << "Invalid choice." << endl;
                }
                break;
            case 2:
                if (inventoryManager.getConfirmation("Are you sure you want to logout?")) {
                    cout << "Logging out from employee account..." << endl;
                    empSession = false;
                }
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }
}

int main() {
    string username, password;
    string userType;
    bool runProgram = true;
    
    while (runProgram) {
        cout << "\n--------------------------------" << endl;
        cout << "|        LOGIN SYSTEM          |" << endl;
        cout << "--------------------------------" << endl;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        
        // Check admin credentials
        userType = checkCredentials(username, password, "admin.txt");
        
        // If not found in admin.txt, check employee.txt
        if (userType.empty()) {
            userType = checkCredentials(username, password, "employee.txt");
        }
        
        if (!userType.empty()) {
            cout << "\nLogin successful! You are logged in as " << userType << "." << endl;
            
            if (userType == "admin") {
                adminMenu();
            } else {
                employeeMenu();
            }
        } else {
            cout << "Login failed. Invalid username or password." << endl;
            InventoryManager tempManager; // Just to use the getConfirmation method
            if (!tempManager.getConfirmation("Do you want to try again?")) {
                runProgram = false;
                cout << "Exiting program. Goodbye!" << endl;
            }
        }
    }
    
    return 0;
}