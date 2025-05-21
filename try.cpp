#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cctype>
#include <ctime>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>

using namespace std;

// ================= UTILITY FUNCTIONS =================

void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return (tolower(response) == 'y');
}

// ================= USER MANAGEMENT SECTION (SINGLETON) =================

class UserManager {
private:
    struct User {
        string username;
        string password;
    };

    vector<User> adminUsers;
    vector<User> employeeUsers;
    
    // Singleton instance
    static UserManager* instance;
    
    // Private constructor for Singleton
    UserManager() {
        loadUsers("admin.txt", adminUsers);
        loadUsers("employee.txt", employeeUsers);
    }

    void loadUsers(const string& filename, vector<User>& users) {
        users.clear();
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string username, password;
            if (getline(ss, username, ',') && getline(ss, password)) {
                users.push_back({username, password});
            }
        }
    }

    void saveUsers(const vector<User>& users, const string& filename) {
        ofstream file(filename);
        for (const auto& user : users) {
            file << user.username << "," << user.password << endl;
        }
    }

    bool usernameExists(const vector<User>& users, const string& username) {
        return find_if(users.begin(), users.end(), 
            [&](const User& u) { return u.username == username; }) != users.end();
    }

public:
    // Delete copy constructor and assignment operator
    UserManager(const UserManager&) = delete;
    UserManager& operator=(const UserManager&) = delete;
    
    // Get singleton instance
    static UserManager* getInstance() {
        if (!instance) {
            instance = new UserManager();
        }
        return instance;
    }
    
    // Cleanup singleton
    static void destroyInstance() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }

    string checkCredentials(const string& username, const string& password) {
        // Check admin credentials
        for (const auto& user : adminUsers) {
            if (user.username == username && user.password == password) {
                return "admin";
            }
        }
        
        // Check employee credentials
        for (const auto& user : employeeUsers) {
            if (user.username == username && user.password == password) {
                return "employee";
            }
        }
        
        return "";
    }

    void addUser(bool isAdmin) {
        vector<User>& users = isAdmin ? adminUsers : employeeUsers;
        string filename = isAdmin ? "admin.txt" : "employee.txt";
        string userType = isAdmin ? "admin" : "employee";
        
        string username, password;
        cout << "Enter new " << userType << " username: ";
        getline(cin, username);
        
        if (usernameExists(adminUsers, username) || usernameExists(employeeUsers, username)) {
            cout << "Username already exists!" << endl;
            return;
        }
        
        cout << "Enter password: ";
        getline(cin, password);
        
        users.push_back({username, password});
        saveUsers(users, filename);
        cout << "User added successfully." << endl;
    }

    void editUser(bool isAdmin) {
        vector<User>& users = isAdmin ? adminUsers : employeeUsers;
        string filename = isAdmin ? "admin.txt" : "employee.txt";
        string userType = isAdmin ? "admin" : "employee";
        
        string username;
        cout << "Enter " << userType << " username to edit: ";
        getline(cin, username);
        
        auto it = find_if(users.begin(), users.end(), 
            [&](const User& u) { return u.username == username; });
            
        if (it == users.end()) {
            cout << "User not found." << endl;
            return;
        }
        
        cout << "Enter new password: ";
        getline(cin, it->password);
        
        saveUsers(users, filename);
        cout << "Password updated successfully." << endl;
    }

    void deleteUser(bool isAdmin) {
        vector<User>& users = isAdmin ? adminUsers : employeeUsers;
        string filename = isAdmin ? "admin.txt" : "employee.txt";
        string userType = isAdmin ? "admin" : "employee";
        
        string username;
        cout << "Enter " << userType << " username to delete: ";
        getline(cin, username);
        
        auto it = remove_if(users.begin(), users.end(), 
            [&](const User& u) { return u.username == username; });
            
        if (it == users.end()) {
            cout << "User not found." << endl;
            return;
        }
        
        users.erase(it, users.end());
        saveUsers(users, filename);
        cout << "User deleted successfully." << endl;
    }

    void listUsers(bool isAdmin) {
        vector<User>& users = isAdmin ? adminUsers : employeeUsers;
        string userType = isAdmin ? "admin" : "employee";
        
        cout << userType << " users:" << endl;
        for (const auto& user : users) {
            cout << " - " << user.username << endl;
        }
    }

    void createDefaultCredentialsIfNeeded() {
        if (adminUsers.empty()) {
            adminUsers.push_back({"admin", "admin123"});
            saveUsers(adminUsers, "admin.txt");
            cout << "Created default admin credentials." << endl;
        }
        
        if (employeeUsers.empty()) {
            employeeUsers.push_back({"employee", "emp123"});
            saveUsers(employeeUsers, "employee.txt");
            cout << "Created default employee credentials." << endl;
        }
    }
};

// Initialize static member
UserManager* UserManager::instance = nullptr;

// ================= INVENTORY SECTION (STRATEGY PATTERN) =================

// Record structure used by all inventory types
struct Record {
    int id;
    string name;
    int quantity;
    double price;
    Record* next;
    
    Record(int _id, const string& _name, int _qty, double _price) 
        : id(_id), name(_name), quantity(_qty), price(_price), next(nullptr) {}
};

// Strategy interface for inventory operations
class InventoryType {
public:
    std::function<void()> onModified; // Callback to notify modifications
    virtual ~InventoryType() = default;
    virtual void addRecord(Record*& head, int& nextId, bool isAdmin) = 0;
    virtual void editRecord(Record* head, bool isAdmin) = 0;
    virtual void deleteRecord(Record*& head, bool isAdmin) = 0;
    virtual void displayInventory(Record* head) = 0;
    virtual void displayMenu(Record*& head, int& nextId, bool isAdmin) = 0;
};

// Concrete class for Raw Material inventory
class RawMaterialInventory : public InventoryType {
public:
    void addRecord(Record*& head, int& nextId, bool isAdmin) override {
        if (!isAdmin) {
            cout << "Access denied. Only administrators can add new raw materials." << endl;
            return;
        }
        
        string name;
        cout << "Enter raw material name: ";
        cin.ignore();
        getline(cin, name);
        
        // Check for duplicate name
        Record* current = head;
        while (current != nullptr) {
            if (current->name == name) {
                cout << "A raw material with this name already exists!" << endl;
                return;
            }
            current = current->next;
        }
        
        bool isValidName = true;
        for (char c : name) {
            if (isdigit(c)) {
                isValidName = false;
                break;
            }
        }
        
        if (!isValidName) {
            cout << "Invalid name. Name should not contain numbers." << endl;
            return;
        }
        
        int quantity = getValidIntInput("Enter quantity: ", 1);
        
        double price = 0;
        bool isValidPrice = false;
        do {
            cout << "Enter unit price: ";
            if (cin >> price) {
                if (price > 0) {
                    isValidPrice = true;
                } else {
                    cout << "Invalid price. Price must be positive." << endl;
                }
            } else {
                cout << "Invalid input. Please enter a number." << endl;
                cin.clear();
                clearInputBuffer();
            }
        } while (!isValidPrice);
        
        if (!getConfirmation("Are you sure you want to add this raw material?")) {
            cout << "Operation cancelled." << endl;
            return;
        }
        
        Record* newRecord = new Record(nextId++, name, quantity, price);
        
        if (head == nullptr) {
            head = newRecord;
        } else {
            Record* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newRecord;
        }
        
        cout << "Raw material added successfully." << endl;
        if (onModified) onModified();
    }

    void editRecord(Record* head, bool isAdmin) override {
        if (head == nullptr) {
            cout << "No raw materials available to edit." << endl;
            return;
        }
        
        displayInventory(head);
        int idToEdit = getValidIntInput("Enter ID of raw material to edit: ", 1);
        
        Record* current = head;
        bool found = false;
        
        while (current != nullptr && !found) {
            if (current->id == idToEdit) {
                found = true;
                break;
            }
            current = current->next;
        }
        
        if (!found) {
            cout << "Raw material with ID " << idToEdit << " not found." << endl;
            return;
        }
        
        cout << "Editing raw material with ID: " << idToEdit << endl;
        cout << "Current name: " << current->name << endl;
        cout << "Current quantity: " << current->quantity << endl;
        cout << "Current unit price: $" << fixed << setprecision(2) << current->price << endl;
        
        if (isAdmin) {
            cout << "Enter new name (or press Enter to keep current): ";
            string newName;
            cin.ignore();
            getline(cin, newName);
            
            if (!newName.empty()) {
                bool isValidName = true;
                for (char c : newName) {
                    if (isdigit(c)) {
                        isValidName = false;
                        break;
                    }
                }
                
                if (!isValidName) {
                    cout << "Invalid name. Name should not contain numbers. Name not updated." << endl;
                } else {
                    current->name = newName;
                }
            }
        }
        
        cout << "Enter new quantity (or 0 to keep current): ";
        int newQuantity;
        cin >> newQuantity;
        
        if (newQuantity > 0) {
            current->quantity = newQuantity;
        } else if (newQuantity < 0) {
            cout << "Invalid quantity. Quantity must be positive. Quantity not updated." << endl;
        }
        
        if (isAdmin) {
            cout << "Enter new unit price (or 0 to keep current): ";
            double newPrice;
            cin >> newPrice;
            
            if (newPrice > 0) {
                current->price = newPrice;
            } else if (newPrice < 0) {
                cout << "Invalid price. Price must be positive. Price not updated." << endl;
            }
        }
        
        if (!getConfirmation("Save these changes?")) {
            cout << "Operation cancelled. No changes saved." << endl;
            return;
        }
        
        cout << "Raw material updated successfully." << endl;
        if (onModified) onModified();
    }

    void deleteRecord(Record*& head, bool isAdmin) override {
        if (!isAdmin) {
            cout << "Access denied. Only administrators can delete raw materials." << endl;
            return;
        }
        
        if (head == nullptr) {
            cout << "No raw materials available to delete." << endl;
            return;
        }
        
        displayInventory(head);
        int idToDelete = getValidIntInput("Enter ID of raw material to delete: ", 1);
        
        if (!getConfirmation("Are you sure you want to delete this raw material?")) {
            cout << "Operation cancelled." << endl;
            return;
        }
        
        if (head->id == idToDelete) {
            Record* temp = head;
            head = head->next;
            delete temp;
            cout << "Raw material deleted successfully." << endl;
            if (onModified) onModified();
            return;
        }
        
        Record* current = head;
        Record* previous = nullptr;
        bool found = false;
        
        while (current != nullptr && !found) {
            if (current->id == idToDelete) {
                found = true;
                break;
            }
            previous = current;
            current = current->next;
        }
        
        if (!found) {
            cout << "Raw material with ID " << idToDelete << " not found." << endl;
            return;
        }
        
        previous->next = current->next;
        delete current;
        cout << "Raw material deleted successfully." << endl;
        if (onModified) onModified();
    }

    void displayInventory(Record* head) override {
        if (head == nullptr) {
            cout << "No raw materials available." << endl;
            return;
        }
        
        cout << "\n------ Raw Material Inventory ------" << endl;
        cout << left << setw(5) << "ID"
             << setw(20) << "Name"
             << setw(10) << "Quantity"
             << "Price" << endl;
        cout << string(50, '-') << endl;
        
        Record* current = head;
        while (current != nullptr) {
            cout << left << setw(5) << current->id
                 << setw(20) << current->name
                 << setw(10) << current->quantity
                 << "$" << fixed << setprecision(2) << current->price << endl;
            current = current->next;
        }
        
        cout << string(50, '-') << endl;
    }

    void displayMenu(Record*& head, int& nextId, bool isAdmin) override {
        bool running = true;
        while (running) {
            cout << "\n" << string(30, '=') << endl;
            cout << setw(25) << "Raw Material Inventory Menu" << endl;
            cout << string(30, '=') << endl;
            
            if (isAdmin) {
                cout << "1. Add Raw Material" << endl;
                cout << "2. Edit Raw Material" << endl;
                cout << "3. Delete Raw Material" << endl;
                cout << "4. Display Raw Materials" << endl;
                cout << "5. Return to Previous Menu" << endl;
                
                int choice = getValidIntInput("Enter your choice (1-5): ", 1);
                switch (choice) {
                    case 1: addRecord(head, nextId, isAdmin); break;
                    case 2: editRecord(head, isAdmin); break;
                    case 3: deleteRecord(head, isAdmin); break;
                    case 4: displayInventory(head); break;
                    case 5:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) 
                            running = false;
                        break;
                    default: cout << "Invalid choice. Please try again." << endl;
                }
            } else {
                cout << "1. Edit Raw Material Quantity" << endl;
                cout << "2. Display Raw Materials" << endl;
                cout << "3. Return to Previous Menu" << endl;
                
                int choice = getValidIntInput("Enter your choice (1-3): ", 1);
                switch (choice) {
                    case 1: editRecord(head, isAdmin); break;
                    case 2: displayInventory(head); break;
                    case 3:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) 
                            running = false;
                        break;
                    default: cout << "Invalid choice. Please try again." << endl;
                }
            }
        }
    }
};

// Concrete class for Product inventory
class ProductInventory : public InventoryType {
public:
    void addRecord(Record*& head, int& nextId, bool isAdmin) override {
        if (!isAdmin) {
            cout << "Access denied. Only administrators can add new products." << endl;
            return;
        }
        
        string name;
        cout << "Enter product name: ";
        cin.ignore();
        getline(cin, name);
        
        bool isValidName = true;
        for (char c : name) {
            if (isdigit(c)) {
                isValidName = false;
                break;
            }
        }
        
        if (!isValidName) {
            cout << "Invalid name. Name should not contain numbers." << endl;
            return;
        }
        
        int quantity = getValidIntInput("Enter quantity: ", 1);
        
        double price = 0;
        bool isValidPrice = false;
        do {
            cout << "Enter unit price: ";
            if (cin >> price) {
                if (price > 0) {
                    isValidPrice = true;
                } else {
                    cout << "Invalid price. Price must be positive." << endl;
                }
            } else {
                cout << "Invalid input. Please enter a number." << endl;
                cin.clear();
                clearInputBuffer();
            }
        } while (!isValidPrice);
        
        if (!getConfirmation("Are you sure you want to add this product?")) {
            cout << "Operation cancelled." << endl;
            return;
        }
        
        Record* newRecord = new Record(nextId++, name, quantity, price);
        
        if (head == nullptr) {
            head = newRecord;
        } else {
            Record* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newRecord;
        }
        
        cout << "Product added successfully." << endl;
        if (onModified) onModified();
    }

    void editRecord(Record* head, bool isAdmin) override {
        if (head == nullptr) {
            cout << "No products available to edit." << endl;
            return;
        }
        
        displayInventory(head);
        int idToEdit = getValidIntInput("Enter ID of product to edit: ", 1);
        
        Record* current = head;
        bool found = false;
        
        while (current != nullptr && !found) {
            if (current->id == idToEdit) {
                found = true;
                break;
            }
            current = current->next;
        }
        
        if (!found) {
            cout << "Product with ID " << idToEdit << " not found." << endl;
            return;
        }
        
        cout << "Editing product with ID: " << idToEdit << endl;
        cout << "Current name: " << current->name << endl;
        cout << "Current quantity: " << current->quantity << endl;
        cout << "Current unit price: $" << fixed << setprecision(2) << current->price << endl;
        
        if (isAdmin) {
            cout << "Enter new name (or press Enter to keep current): ";
            string newName;
            cin.ignore();
            getline(cin, newName);
            
            if (!newName.empty()) {
                bool isValidName = true;
                for (char c : newName) {
                    if (isdigit(c)) {
                        isValidName = false;
                        break;
                    }
                }
                
                if (!isValidName) {
                    cout << "Invalid name. Name should not contain numbers. Name not updated." << endl;
                } else {
                    current->name = newName;
                }
            }
        }
        
        cout << "Enter new quantity (or 0 to keep current): ";
        int newQuantity;
        cin >> newQuantity;
        
        if (newQuantity > 0) {
            current->quantity = newQuantity;
        } else if (newQuantity < 0) {
            cout << "Invalid quantity. Quantity must be positive. Quantity not updated." << endl;
        }
        
        if (isAdmin) {
            cout << "Enter new unit price (or 0 to keep current): ";
            double newPrice;
            cin >> newPrice;
            
            if (newPrice > 0) {
                current->price = newPrice;
            } else if (newPrice < 0) {
                cout << "Invalid price. Price must be positive. Price not updated." << endl;
            }
        }
        
        if (!getConfirmation("Save these changes?")) {
            cout << "Operation cancelled. No changes saved." << endl;
            return;
        }
        
        cout << "Product updated successfully." << endl;
        if (onModified) onModified();
    }

    void deleteRecord(Record*& head, bool isAdmin) override {
        if (!isAdmin) {
            cout << "Access denied. Only administrators can delete products." << endl;
            return;
        }
        
        if (head == nullptr) {
            cout << "No products available to delete." << endl;
            return;
        }
        
        displayInventory(head);
        int idToDelete = getValidIntInput("Enter ID of product to delete: ", 1);
        
        if (!getConfirmation("Are you sure you want to delete this product?")) {
            cout << "Operation cancelled." << endl;
            return;
        }
        
        if (head->id == idToDelete) {
            Record* temp = head;
            head = head->next;
            delete temp;
            cout << "Product deleted successfully." << endl;
            if (onModified) onModified();
            return;
        }
        
        Record* current = head;
        Record* previous = nullptr;
        bool found = false;
        
        while (current != nullptr && !found) {
            if (current->id == idToDelete) {
                found = true;
                break;
            }
            previous = current;
            current = current->next;
        }
        
        if (!found) {
            cout << "Product with ID " << idToDelete << " not found." << endl;
            return;
        }
        
        previous->next = current->next;
        delete current;
        cout << "Product deleted successfully." << endl;
        if (onModified) onModified();
    }

    void displayInventory(Record* head) override {
        if (head == nullptr) {
            cout << "No products available." << endl;
            return;
        }
        
        cout << "\n------ Product Inventory ------" << endl;
        cout << left << setw(5) << "ID"
             << setw(20) << "Name"
             << setw(10) << "Quantity"
             << "Price" << endl;
        cout << string(50, '-') << endl;
        
        Record* current = head;
        while (current != nullptr) {
            cout << left << setw(5) << current->id
                 << setw(20) << current->name
                 << setw(10) << current->quantity
                 << "$" << fixed << setprecision(2) << current->price << endl;
            current = current->next;
        }
        
        cout << string(50, '-') << endl;
    }

    void displayMenu(Record*& head, int& nextId, bool isAdmin) override {
        bool running = true;
        while (running) {
            cout << "\n" << string(30, '=') << endl;
            cout << setw(22) << "Product Inventory Menu" << endl;
            cout << string(30, '=') << endl;
            
            if (isAdmin) {
                cout << "1. Add Product" << endl;
                cout << "2. Edit Product" << endl;
                cout << "3. Delete Product" << endl;
                cout << "4. Display Products" << endl;
                cout << "5. Return to Previous Menu" << endl;
                
                int choice = getValidIntInput("Enter your choice (1-5): ", 1);
                switch (choice) {
                    case 1: addRecord(head, nextId, isAdmin); break;
                    case 2: editRecord(head, isAdmin); break;
                    case 3: deleteRecord(head, isAdmin); break;
                    case 4: displayInventory(head); break;
                    case 5:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) 
                            running = false;
                        break;
                    default: cout << "Invalid choice. Please try again." << endl;
                }
            } else {
                cout << "1. Edit Product Quantity" << endl;
                cout << "2. Display Products" << endl;
                cout << "3. Return to Previous Menu" << endl;
                
                int choice = getValidIntInput("Enter your choice (1-3): ", 1);
                switch (choice) {
                    case 1: editRecord(head, isAdmin); break;
                    case 2: displayInventory(head); break;
                    case 3:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) 
                            running = false;
                        break;
                    default: cout << "Invalid choice. Please try again." << endl;
                }
            }
        }
    }
};

// Inventory class that uses Strategy pattern
class Inventory {
private:
    Record* head;
    int nextId;
    string filename;
    bool isAdmin;
    unique_ptr<InventoryType> strategy;

    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) return;
        
        // Clear existing records
        while (head != nullptr) {
            Record* temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr;
        nextId = 1;
        
        int id;
        string name;
        int quantity;
        double price;
        string line;
        
        while (getline(file, line)) {
            stringstream ss(line);
            ss >> id;
            ss.ignore();
            getline(ss, name, '|');
            ss >> quantity >> price;
            
            Record* newRecord = new Record(id, name, quantity, price);
            
            if (id >= nextId) nextId = id + 1;
            
            if (head == nullptr) {
                head = newRecord;
            } else {
                Record* current = head;
                while (current->next != nullptr) {
                    current = current->next;
                }
                current->next = newRecord;
            }
        }
        file.close();
    }

    void saveToFile() {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open file for saving." << endl;
            return;
        }
        
        Record* current = head;
        while (current != nullptr) {
            file << current->id << " " << current->name << "|"
                 << current->quantity << " " << current->price << endl;
            current = current->next;
        }
        file.close();
    }

public:
    Inventory(const string& file, unique_ptr<InventoryType> strat, bool admin = false) 
        : head(nullptr), nextId(1), filename(file), isAdmin(admin), strategy(move(strat)) {
        strategy->onModified = [this]() { this->saveToFile(); };
        loadFromFile();
    }

    ~Inventory() {
        saveToFile();
        Record* current = head;
        while (current != nullptr) {
            Record* temp = current;
            current = current->next;
            delete temp;
        }
    }

    void setAdminStatus(bool admin) {
        isAdmin = admin;
    }

    void displayMenu() {
        strategy->displayMenu(head, nextId, isAdmin);
        saveToFile();
    }
};

// ================= REPORT MANAGER (SINGLETON) =================

class ReportManager {
private:
    static ReportManager* instance;
    
    // Private constructor for Singleton
    ReportManager() {}
    
public:
    // Delete copy constructor and assignment operator
    ReportManager(const ReportManager&) = delete;
    ReportManager& operator=(const ReportManager&) = delete;
    
    // Get singleton instance
    static ReportManager* getInstance() {
        if (!instance) {
            instance = new ReportManager();
        }
        return instance;
    }
    
    // Cleanup singleton
    static void destroyInstance() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }

    void displayRawMatReport() {
        struct ReportRecord {
            int id;
            string name;
            int quantity;
            double price;
        };
        
        ifstream file("rawmaterial.txt");
        if (!file.is_open()) {
            cout << "Error: Could not open rawmaterial.txt for reading." << endl;
            return;
        }
        
        time_t now = time(0);
        char* dt = ctime(&now);
        
        cout << "\n" << string(70, '=') << endl;
        cout << setw(45) << "RAW MATERIAL INVENTORY REPORT" << endl;
        cout << "Generated on: " << dt;
        cout << string(70, '=') << endl;
        cout << left << setw(5) << "ID"
             << setw(25) << "Product Name"
             << setw(10) << "Quantity"
             << setw(15) << "Unit Price"
             << setw(15) << "Value" << endl;
        cout << string(70, '-') << endl;
        
        int totalQuantity = 0;
        double totalValue = 0.0;
        string line;
        
        while (getline(file, line)) {
            stringstream ss(line);
            ReportRecord record;
            ss >> record.id;
            ss.ignore();
            getline(ss, record.name, '|');
            ss >> record.quantity >> record.price;
            
            double value = record.quantity * record.price;
            totalQuantity += record.quantity;
            totalValue += value;
            
            cout << left << setw(5) << record.id
                 << setw(25) << record.name
                 << setw(10) << record.quantity
                 << "$" << setw(14) << fixed << setprecision(2) << record.price
                 << "$" << setw(14) << fixed << setprecision(2) << value << endl;
        }
        
        file.close();
        
        cout << string(70, '-') << endl;
        cout << left << setw(30) << "TOTAL:"
             << setw(10) << totalQuantity
             << setw(15) << ""
             << "$" << fixed << setprecision(2) << totalValue << endl;
        cout << string(70, '=') << endl;
    }

    void displayProductReport() {
        struct ReportRecord {
            int id;
            string name;
            int quantity;
            double price;
        };
        
        ifstream file("product.txt");
        if (!file.is_open()) {
            cout << "Error: Could not open product.txt for reading." << endl;
            return;
        }
        
        time_t now = time(0);
        char* dt = ctime(&now);
        
        cout << "\n" << string(70, '=') << endl;
        cout << setw(45) << "PRODUCT INVENTORY REPORT" << endl;
        cout << "Generated on: " << dt;
        cout << string(70, '=') << endl;
        cout << left << setw(5) << "ID"
             << setw(25) << "Product Name"
             << setw(10) << "Quantity"
             << setw(15) << "Unit Price"
             << setw(15) << "Value" << endl;
        cout << string(70, '-') << endl;
        
        int totalQuantity = 0;
        double totalValue = 0.0;
        string line;
        
        while (getline(file, line)) {
            stringstream ss(line);
            ReportRecord record;
            ss >> record.id;
            ss.ignore();
            getline(ss, record.name, '|');
            ss >> record.quantity >> record.price;
            
            double value = record.quantity * record.price;
            totalQuantity += record.quantity;
            totalValue += value;
            
            cout << left << setw(5) << record.id
                 << setw(25) << record.name
                 << setw(10) << record.quantity
                 << "$" << setw(14) << fixed << setprecision(2) << record.price
                 << "$" << setw(14) << fixed << setprecision(2) << value << endl;
        }
        
        file.close();
        
        cout << string(70, '-') << endl;
        cout << left << setw(30) << "TOTAL:"
             << setw(10) << totalQuantity
             << setw(15) << ""
             << "$" << fixed << setprecision(2) << totalValue << endl;
        cout << string(70, '=') << endl;
    }

    void reportUI() {
        cout << "\n--------------------------------" << endl;
        cout << "|       REPORTS DASHBOARD      |" << endl;
        cout << "--------------------------------" << endl;
        cout << "1. Product Inventory Report" << endl;
        cout << "2. Raw Material Inventory Report" << endl;
        cout << "3. Return to Previous Menu" << endl;
        
        int choice = getValidIntInput("Enter your choice (1-3): ", 1);
        switch (choice) {
            case 1: displayProductReport(); break;
            case 2: displayRawMatReport(); break;
            case 3: cout << "Returning to previous menu..." << endl; break;
            default: cout << "Invalid choice. Please try again." << endl; break;
        }
    }
};

// Initialize static member
ReportManager* ReportManager::instance = nullptr;

// ================= INVENTORY MANAGER (SINGLETON) =================

class InventoryManager {
private:
    unique_ptr<Inventory> rawMaterials;
    unique_ptr<Inventory> products;
    bool isAdmin;
    
    static InventoryManager* instance;
    
    // Private constructor for Singleton
    InventoryManager(bool admin = false) : isAdmin(admin) {
        rawMaterials = make_unique<Inventory>("rawmaterial.txt", make_unique<RawMaterialInventory>(), admin);
        products = make_unique<Inventory>("product.txt", make_unique<ProductInventory>(), admin);
        initializeSampleData();
    }
    
    void initializeSampleData() {
        ifstream rmCheck("rawmaterial.txt");
        bool rmFileEmpty = !rmCheck || rmCheck.peek() == ifstream::traits_type::eof();
        rmCheck.close();
        
        ifstream prodCheck("product.txt");
        bool prodFileEmpty = !prodCheck || prodCheck.peek() == ifstream::traits_type::eof();
        prodCheck.close();
        
        if (rmFileEmpty) {
            ofstream rmFile("rawmaterial.txt");
            if (rmFile) {
                rmFile << "1 Cotton Fabric|500 150.00" << endl;
                rmFile << "2 Polyester Fabric|450 120.00" << endl;
                rmFile << "3 Denim|300 200.00" << endl;
                rmFile << "4 Silk Fabric|150 350.00" << endl;
                rmFile << "5 Wool Fabric|200 250.00" << endl;
                rmFile << "6 Thread|1000 10.00" << endl;
                rmFile << "7 Buttons|2500 5.00" << endl;
                rmFile << "8 Zippers|1200 15.00" << endl;
                rmFile << "9 Elastic Bands|800 8.00" << endl;
                rmFile << "10 Labels/Tags|1200 3.00" << endl;
                rmFile.close();
                cout << "Initialized sample raw materials data." << endl;
            }
        }
        
        if (prodFileEmpty) {
            ofstream prodFile("product.txt");
            if (prodFile) {
                prodFile << "1 Men's T-Shirts|200 220.00" << endl;
                prodFile << "2 Women's Blouses|180 320.00" << endl;
                prodFile << "3 Denim Jeans|150 650.00" << endl;
                prodFile << "4 Hoodies|120 750.00" << endl;
                prodFile << "5 Jackets|100 750.00" << endl;
                prodFile << "6 Formal Trousers|90 340.00" << endl;
                prodFile << "7 Skirts|110 490.00" << endl;
                prodFile << "8 Dresses|85 300.00" << endl;
                prodFile << "9 Shorts|95 270.00" << endl;
                prodFile << "10 Polo Shirts|130 270.00" << endl;
                prodFile.close();
                cout << "Initialized sample products data." << endl;
            }
        }
    }
    
public:
    // Delete copy constructor and assignment operator
    InventoryManager(const InventoryManager&) = delete;
    InventoryManager& operator=(const InventoryManager&) = delete;
    
    // Get singleton instance
    static InventoryManager* getInstance(bool admin = false) {
        if (!instance) {
            instance = new InventoryManager(admin);
        } else {
            // Update admin status if needed
            instance->setAdminStatus(admin);
        }
        return instance;
    }
    
    // Cleanup singleton
    static void destroyInstance() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }
    
    void setAdminStatus(bool admin) {
        isAdmin = admin;
        rawMaterials->setAdminStatus(admin);
        products->setAdminStatus(admin);
    }
    
    void runInventoryMenu() {
        bool menu = true;
        while (menu) {
            cout << "\n ----- Inventory Management Menu ----- " << endl;
            cout << "1. Raw Material Inventory" << endl;
            cout << "2. Product Inventory" << endl;
            cout << "3. Return to Main Menu" << endl;
            
            int choice = getValidIntInput("Enter your choice (1-3): ", 1);
            switch (choice) {
                case 1: rawMaterials->displayMenu(); break;
                case 2: products->displayMenu(); break;
                case 3:
                    if (getConfirmation("Are you sure you want to return to the main menu?")) 
                        menu = false;
                    break;
                default: cout << "Invalid choice. Please try again." << endl;
            }
        }
    }
};

// Initialize static member
InventoryManager* InventoryManager::instance = nullptr;

// ================= MENU SYSTEM =================

void adminUserManagementMenu() {
    UserManager* userManager = UserManager::getInstance();
    
    while (true) {
        cout << "\n--- Admin User Management ---\n";
        cout << "1. List Admins\n2. List Employees\n3. Add Admin\n4. Add Employee\n";
        cout << "5. Edit Admin\n6. Edit Employee\n7. Delete Admin\n8. Delete Employee\n0. Back\nChoice: ";
        
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: userManager->listUsers(true); break;
            case 2: userManager->listUsers(false); break;
            case 3: userManager->addUser(true); break;
            case 4: userManager->addUser(false); break;
            case 5: userManager->editUser(true); break;
            case 6: userManager->editUser(false); break;
            case 7: userManager->deleteUser(true); break;
            case 8: userManager->deleteUser(false); break;
            case 0: return;
            default: cout << "Invalid choice.\n";
        }
    }
}

void adminMenu() {
    InventoryManager* inventoryManager = InventoryManager::getInstance(true);
    ReportManager* reportManager = ReportManager::getInstance();
    
    bool adminSession = true;
    while (adminSession) {
        cout << "\n--------------------------------" << endl;
        cout << "|       ADMIN DASHBOARD        |" << endl;
        cout << "--------------------------------" << endl;
        cout << "1. Manage Inventory" << endl;
        cout << "2. Manage Users" << endl;
        cout << "3. Reports" << endl;
        cout << "4. Logout" << endl;
        
        int adminChoice = getValidIntInput("Enter your choice (1-4): ", 1);
        switch (adminChoice) {
            case 1: inventoryManager->runInventoryMenu(); break;
            case 2: adminUserManagementMenu(); break;
            case 3: reportManager->reportUI(); break;
            case 4:
                if (getConfirmation("Are you sure you want to logout?")) {
                    cout << "Logging out from admin account..." << endl;
                    adminSession = false;
                }
                break;
            default: cout << "Invalid choice. Please try again." << endl; break;
        }
    }
}

void employeeMenu() {
    InventoryManager* inventoryManager = InventoryManager::getInstance(false);
    
    bool empSession = true;
    while (empSession) {
        cout << "\n--------------------------------" << endl;
        cout << "|      EMPLOYEE DASHBOARD      |" << endl;
        cout << "--------------------------------" << endl;
        cout << "1. Manage Inventory" << endl;
        cout << "2. Logout" << endl;
        
        int empChoice = getValidIntInput("Enter your choice (1-2): ", 1);
        switch (empChoice) {
            case 1: inventoryManager->runInventoryMenu(); break;
            case 2:
                if (getConfirmation("Are you sure you want to logout?")) {
                    cout << "Logging out from employee account..." << endl;
                    empSession = false;
                }
                break;
            default: cout << "Invalid choice. Please try again." << endl; break;
        }
    }
}

// ================= MAIN FUNCTION =================

int main() {
    string username, password;
    string userType;
    bool runProgram = true;
    
    // Initialize singletons
    UserManager* userManager = UserManager::getInstance();
    userManager->createDefaultCredentialsIfNeeded();
    
    while (runProgram) {
        cout << "\n--------------------------------" << endl;
        cout << "|        LOGIN SYSTEM          |" << endl;
        cout << "--------------------------------" << endl;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        
        userType = userManager->checkCredentials(username, password);
        
        if (!userType.empty()) {
            cout << "\nLogin successful! You are logged in as " << userType << "." << endl;
            if (userType == "admin") {
                adminMenu();
            } else {
                employeeMenu();
            }
        } else {
            cout << "Login failed. Invalid username or password." << endl;
            if (!getConfirmation("Do you want to try again?")) {
                runProgram = false;
                cout << "Exiting program. Goodbye!" << endl;
            }
        }
    }
    
    // Clean up singletons
    UserManager::destroyInstance();
    InventoryManager::destroyInstance();
    ReportManager::destroyInstance();
    
    return 0;
}