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

using namespace std;

// ================= USER MANAGEMENT SECTION =================

struct User {
    string username;
    string password;
};

vector<User> loadUsers(const string& filename) {
    vector<User> users;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string username, password;
        if (getline(ss, username, ',') && getline(ss, password)) {
            users.push_back({username, password});
        }
    }
    return users;
}

void saveUsers(const vector<User>& users, const string& filename) {
    ofstream file(filename);
    for (const auto& user : users) {
        file << user.username << "," << user.password << endl;
    }
}

bool usernameExists(const vector<User>& users, const string& username) {
    for (const auto& user : users) {
        if (user.username == username) return true;
    }
    return false;
}

// ...existing code...

void addUser(const string& filename) {
    vector<User> users = loadUsers(filename);
    string username, password;
    cout << "Enter new username: ";
    getline(cin, username); // Removed cin.ignore()
    if (usernameExists(users, username)) {
        cout << "Username already exists!" << endl;
        return;
    }
    cout << "Enter password: ";
    getline(cin, password);
    users.push_back({username, password});
    saveUsers(users, filename);
    cout << "User added successfully." << endl;
}

void editUser(const string& filename) {
    vector<User> users = loadUsers(filename);
    string username;
    cout << "Enter username to edit: ";
    getline(cin, username); // Removed cin.ignore()
    auto it = find_if(users.begin(), users.end(), [&](const User& u){ return u.username == username; });
    if (it == users.end()) {
        cout << "User not found." << endl;
        return;
    }
    cout << "Enter new password: ";
    getline(cin, it->password);
    saveUsers(users, filename);
    cout << "Password updated successfully." << endl;
}

void deleteUser(const string& filename) {
    vector<User> users = loadUsers(filename);
    string username;
    cout << "Enter username to delete: ";
    getline(cin, username); // Removed cin.ignore()
    auto it = remove_if(users.begin(), users.end(), [&](const User& u){ return u.username == username; });
    if (it == users.end()) {
        cout << "User not found." << endl;
        return;
    }
    users.erase(it, users.end());
    saveUsers(users, filename);
    cout << "User deleted successfully." << endl;
}

void listUsers(const string& filename) {
    vector<User> users = loadUsers(filename);
    cout << "Users in " << filename << ":" << endl;
    for (const auto& user : users) {
        cout << " - " << user.username << endl;
    }
}

void adminUserManagementMenu() {
    while (true) {
        cout << "\n--- Admin User Management ---\n";
        cout << "1. List Admins\n2. List Employees\n3. Add Admin\n4. Add Employee\n5. Edit Admin\n6. Edit Employee\n7. Delete Admin\n8. Delete Employee\n0. Back\nChoice: ";
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Only here!
        switch (choice) {
            case 1: listUsers("admin.txt"); break;
            case 2: listUsers("employee.txt"); break;
            case 3: addUser("admin.txt"); break;
            case 4: addUser("employee.txt"); break;
            case 5: editUser("admin.txt"); break;
            case 6: editUser("employee.txt"); break;
            case 7: deleteUser("admin.txt"); break;
            case 8: deleteUser("employee.txt"); break;
            case 0: return;
            default: cout << "Invalid choice.\n";
        }
    }
}

// ================= INVENTORY SECTION =================

string checkCredentials(const string& username, const string& password, const string& filename) {
    ifstream file(filename);
    if (!file) return "";
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string storedUsername, storedPassword;
        getline(ss, storedUsername, ',');
        getline(ss, storedPassword, ',');
        if (username == storedUsername && password == storedPassword) {
            if (filename.find("admin") != string::npos) return "admin";
            else return "employee";
        }
    }
    return "";
}

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

// ================= INVENTORY CLASSES =================

class Inventory {
protected:
    struct Record {
        int id;
        string name;
        int quantity;
        double price;
        Record* next;
    };

    Record* head;
    int nextId;
    string filename;
    bool isAdmin;

public:
    Inventory(const string& file, bool admin = false) {
        head = nullptr;
        nextId = 1;
        filename = file;
        isAdmin = admin;
        loadFromFile();
    }

    virtual ~Inventory() {
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

    virtual void displayMenu() = 0;

    void addRecord() {
        if (!isAdmin) {
            cout << "Access denied. Only administrators can add new records." << endl;
            return;
        }
        Record* newRecord = new Record;
        newRecord->id = nextId++;
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, newRecord->name);
        bool isValidName = true;
        for (char c : newRecord->name) {
            if (isdigit(c)) {
                isValidName = false;
                break;
            }
        }
        if (!isValidName) {
            cout << "Invalid name. Name should not contain numbers." << endl;
            delete newRecord;
            return;
        }
        newRecord->quantity = getValidIntInput("Enter quantity: ", 1);
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
        newRecord->price = price;
        if (!getConfirmation("Are you sure you want to add this record?")) {
            cout << "Operation cancelled." << endl;
            delete newRecord;
            return;
        }
        newRecord->next = nullptr;
        if (head == nullptr) {
            head = newRecord;
        } else {
            Record* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newRecord;
        }
        cout << "Record added successfully." << endl;
        saveToFile();
    }

    void editRecord() {
        if (head == nullptr) {
            cout << "No records available to edit." << endl;
            return;
        }
        displayInventory();
        int idToEdit = getValidIntInput("Enter ID of record to edit: ", 1);
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
            cout << "Record with ID " << idToEdit << " not found." << endl;
            return;
        }
        cout << "Editing record with ID: " << idToEdit << endl;
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
        cout << "Record updated successfully." << endl;
        saveToFile();
    }

    void deleteRecord() {
        if (!isAdmin) {
            cout << "Access denied. Only administrators can delete records." << endl;
            return;
        }
        if (head == nullptr) {
            cout << "No records available to delete." << endl;
            return;
        }
        displayInventory();
        int idToDelete = getValidIntInput("Enter ID of record to delete: ", 1);
        if (!getConfirmation("Are you sure you want to delete this record?")) {
            cout << "Operation cancelled." << endl;
            return;
        }
        if (head->id == idToDelete) {
            Record* temp = head;
            head = head->next;
            delete temp;
            cout << "Record deleted successfully." << endl;
            saveToFile();
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
            cout << "Record with ID " << idToDelete << " not found." << endl;
            return;
        }
        previous->next = current->next;
        delete current;
        cout << "Record deleted successfully." << endl;
        saveToFile();
    }

    void displayInventory() {
        if (head == nullptr) {
            cout << "No records available." << endl;
            return;
        }
        cout << "\n------ Inventory Records ------" << endl;
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

protected:
    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) return;
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
            Record* newRecord = new Record;
            newRecord->id = id;
            newRecord->name = name;
            newRecord->quantity = quantity;
            newRecord->price = price;
            newRecord->next = nullptr;
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
};

class RawMaterialInventory : public Inventory {
public:
    RawMaterialInventory(bool admin = false) : Inventory("rawmaterial.txt", admin) {}
    void displayMenu() override {
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
                    case 1: addRecord(); break;
                    case 2: editRecord(); break;
                    case 3: deleteRecord(); break;
                    case 4: displayInventory(); break;
                    case 5:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) running = false;
                        break;
                    default: cout << "Invalid choice. Please try again." << endl;
                }
            } else {
                cout << "1. Edit Raw Material Quantity" << endl;
                cout << "2. Display Raw Materials" << endl;
                cout << "3. Return to Previous Menu" << endl;
                int choice = getValidIntInput("Enter your choice (1-3): ", 1);
                switch (choice) {
                    case 1: editRecord(); break;
                    case 2: displayInventory(); break;
                    case 3:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) running = false;
                        break;
                    default: cout << "Invalid choice. Please try again." << endl;
                }
            }
        }
    }
};

class ProductInventory : public Inventory {
public:
    ProductInventory(bool admin = false) : Inventory("product.txt", admin) {}
    void displayMenu() override {
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
                    case 1: addRecord(); break;
                    case 2: editRecord(); break;
                    case 3: deleteRecord(); break;
                    case 4: displayInventory(); break;
                    case 5:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) running = false;
                        break;
                    default: cout << "Invalid choice. Please try again." << endl;
                }
            } else {
                cout << "1. Edit Product Quantity" << endl;
                cout << "2. Display Products" << endl;
                cout << "3. Return to Previous Menu" << endl;
                int choice = getValidIntInput("Enter your choice (1-3): ", 1);
                switch (choice) {
                    case 1: editRecord(); break;
                    case 2: displayInventory(); break;
                    case 3:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) running = false;
                        break;
                    default: cout << "Invalid choice. Please try again." << endl;
                }
            }
        }
    }
};

class InventoryManager {
private:
    RawMaterialInventory rawMaterials;
    ProductInventory products;
    bool isAdmin;
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
    InventoryManager(bool admin = false) : rawMaterials(admin), products(admin), isAdmin(admin) {
        initializeSampleData();
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
                case 1: rawMaterials.displayMenu(); break;
                case 2: products.displayMenu(); break;
                case 3:
                    if (getConfirmation("Are you sure you want to return to the main menu?")) menu = false;
                    break;
                default: cout << "Invalid choice. Please try again." << endl;
            }
        }
    }
};

// ========== Inventory Reports ==========

void displayRawMatReport() {
    struct Record {
        int id;
        string name;
        int quantity;
        double price;
    };
    ifstream file("rawmaterial.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open product.txt for reading." << endl;
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
        Record record;
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
    struct Record {
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
        Record record;
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

// ========== MENUS ==========

void adminMenu() {
    InventoryManager inventoryManager(true);
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
            case 1: inventoryManager.runInventoryMenu(); break;
            case 2: adminUserManagementMenu(); break;
            case 3: reportUI(); break;
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
    InventoryManager inventoryManager(false);
    bool empSession = true;
    while (empSession) {
        cout << "\n--------------------------------" << endl;
        cout << "|      EMPLOYEE DASHBOARD      |" << endl;
        cout << "--------------------------------" << endl;
        cout << "1. Manage Inventory" << endl;
        cout << "2. Logout" << endl;
        int empChoice = getValidIntInput("Enter your choice (1-2): ", 1);
        switch (empChoice) {
            case 1: inventoryManager.runInventoryMenu(); break;
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

void createDefaultCredentialsIfNeeded() {
    ifstream adminCheck("admin.txt");
    if (!adminCheck) {
        ofstream adminFile("admin.txt");
        if (adminFile) {
            adminFile << "admin,admin123" << endl;
            adminFile.close();
            cout << "Created default admin credentials." << endl;
        }
    } else {
        adminCheck.close();
    }
    ifstream empCheck("employee.txt");
    if (!empCheck) {
        ofstream empFile("employee.txt");
        if (empFile) {
            empFile << "employee,emp123" << endl;
            empFile.close();
            cout << "Created default employee credentials." << endl;
        }
    } else {
        empCheck.close();
    }
}

int main() {
    string username, password;
    string userType;
    bool runProgram = true;
    createDefaultCredentialsIfNeeded();
    while (runProgram) {
        cout << "\n--------------------------------" << endl;
        cout << "|        LOGIN SYSTEM          |" << endl;
        cout << "--------------------------------" << endl;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        userType = checkCredentials(username, password, "admin.txt");
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
            if (!getConfirmation("Do you want to try again?")) {
                runProgram = false;
                cout << "Exiting program. Goodbye!" << endl;
            }
        }
    }
    return 0;
}