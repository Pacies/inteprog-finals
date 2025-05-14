#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cctype>
#include <ctime>

using namespace std;

// Function to check credentials against a file
string checkCredentials(const string& username, const string& password, const string& filename) {
    ifstream file(filename);
    if (!file) {
        // Instead of showing a warning, we'll create default credentials files later
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

// Function to clear input buffer
void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Function to get valid integer input
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

// Function to get confirmation
bool getConfirmation(const string& prompt) {
    char response;
    cout << prompt << " (y/n): ";
    cin >> response;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return (tolower(response) == 'y');
}

// Base Inventory Class for persistent storage and management of inventory items
class Inventory {
protected:
    struct Record {
        int id;
        string name;
        int quantity;
        double price;  // Unit price or cost
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
        
        // Validate name (no numbers)
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
        
        // Get and validate price
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
        
        // Edit name - only admins can change the name
        if (isAdmin) {
            cout << "Enter new name (or press Enter to keep current): ";
            string newName;
            cin.ignore();
            getline(cin, newName);
            
            if (!newName.empty()) {
                // Validate name (no numbers)
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
        
        // Edit quantity - both admin and employee
        cout << "Enter new quantity (or 0 to keep current): ";
        int newQuantity;
        cin >> newQuantity;
        
        if (newQuantity > 0) {
            current->quantity = newQuantity;
        } else if (newQuantity < 0) {
            cout << "Invalid quantity. Quantity must be positive. Quantity not updated." << endl;
        }
        
        // Edit price - only admins can change the price
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
        if (!file.is_open()) {
            // File doesn't exist yet, just return
            return;
        }
        
        // Clean existing list
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
            ss.ignore(); // Skip space
            
            // Read name up to the pipe
            getline(ss, name, '|');
            
            ss >> quantity >> price;
            
            Record* newRecord = new Record;
            newRecord->id = id;
            newRecord->name = name;
            newRecord->quantity = quantity;
            newRecord->price = price;
            newRecord->next = nullptr;
            
            if (id >= nextId) {
                nextId = id + 1;
            }
            
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

// Raw Material Inventory Class
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
                    case 1:
                        addRecord();
                        break;
                    case 2:
                        editRecord();
                        break;
                    case 3:
                        deleteRecord();
                        break;
                    case 4:
                        displayInventory();
                        break;
                    case 5:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) {
                            running = false;
                        }
                        break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                }
            } else {
                // Limited menu for employees
                cout << "1. Edit Raw Material Quantity" << endl;
                cout << "2. Display Raw Materials" << endl;
                cout << "3. Return to Previous Menu" << endl;
                
                int choice = getValidIntInput("Enter your choice (1-3): ", 1);
                
                switch (choice) {
                    case 1:
                        editRecord(); // This will only allow editing quantity
                        break;
                    case 2:
                        displayInventory();
                        break;
                    case 3:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) {
                            running = false;
                        }
                        break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                }
            }
        }
    }
};

// Product Inventory Class
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
                    case 1:
                        addRecord();
                        break;
                    case 2:
                        editRecord();
                        break;
                    case 3:
                        deleteRecord();
                        break;
                    case 4:
                        displayInventory();
                        break;
                    case 5:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) {
                            running = false;
                        }
                        break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                }
            } else {
                // Limited menu for employees
                cout << "1. Edit Product Quantity" << endl;
                cout << "2. Display Products" << endl;
                cout << "3. Return to Previous Menu" << endl;
                
                int choice = getValidIntInput("Enter your choice (1-3): ", 1);
                
                switch (choice) {
                    case 1:
                        editRecord(); // This will only allow editing quantity
                        break;
                    case 2:
                        displayInventory();
                        break;
                    case 3:
                        if (getConfirmation("Are you sure you want to return to the previous menu?")) {
                            running = false;
                        }
                        break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                }
            }
        }
    }
};

// Inventory Manager Class that will be used by both admin and employee
class InventoryManager {
private:
    RawMaterialInventory rawMaterials;
    ProductInventory products;
    bool isAdmin;

    // Method to initialize sample data if inventory files are empty
    void initializeSampleData() {
        // Check if raw material file exists and has content
        ifstream rmCheck("rawmaterial.txt");
        bool rmFileEmpty = !rmCheck || rmCheck.peek() == ifstream::traits_type::eof();
        rmCheck.close();
        
        // Check if product file exists and has content
        ifstream prodCheck("product.txt");
        bool prodFileEmpty = !prodCheck || prodCheck.peek() == ifstream::traits_type::eof();
        prodCheck.close();
        
        // Initialize raw materials if file is empty
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
        
        // Initialize products if file is empty
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
        // Initialize sample data if needed
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
                case 1:
                    rawMaterials.displayMenu();
                    break;
                case 2:
                    products.displayMenu();
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

void reportUI(){

    cout << "\n--------------------------------" << endl;
    cout << "|       REPORTS DASHBOARD      |" << endl;
    cout << "--------------------------------" << endl;
    cout << "1. Product Inventory Report" << endl;
    cout << "2. Raw Material Inventory Report" << endl;
    cout << "3. Return to Previous Menu" << endl;

    int choice = getValidIntInput("Enter your choice (1-3): ", 1);

    switch (choice) {
        case 1:
            displayProductReport();
            break;
        case 2:
            displayRawMatReport();
            break;
        case 3:
            cout << "Returning to previous menu..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
    }

}

// Function for admin menu
void adminMenu() {
    InventoryManager inventoryManager(true); // Pass true to indicate admin privileges
    bool adminSession = true;
    
    while (adminSession) {
        cout << "\n--------------------------------" << endl;
        cout << "|       ADMIN DASHBOARD        |" << endl;
        cout << "--------------------------------" << endl;
        cout << "1. Manage Inventory" << endl;
        cout << "2. Manage Users" << endl;
        cout << "3. Reports" << endl;
        cout << "4. Logout" << endl;
        
        int adminChoice = getValidIntInput("Enter your choice (1-5): ", 1);
        
        switch (adminChoice) {
            case 1:
                inventoryManager.runInventoryMenu();
                break;
            case 2:
                cout << "User management module - Coming soon!" << endl;
                break;
            case 3:
                reportUI();
                break;
            case 4:
                if (getConfirmation("Are you sure you want to logout?")) {
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
    InventoryManager inventoryManager(false); // Pass false to indicate employee privileges
    bool empSession = true;
    
    while (empSession) {
        cout << "\n--------------------------------" << endl;
        cout << "|      EMPLOYEE DASHBOARD      |" << endl;
        cout << "--------------------------------" << endl;
        cout << "1. Manage Inventory" << endl;
        cout << "2. Logout" << endl;
        
        int empChoice = getValidIntInput("Enter your choice (1-2): ", 1);
        
        switch (empChoice) {
            case 1:
                inventoryManager.runInventoryMenu();
                break;
            case 2:
                if (getConfirmation("Are you sure you want to logout?")) {
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

// Function to write default user credentials if files don't exist
void createDefaultCredentialsIfNeeded() {
    // Check and create admin file if it doesn't exist
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

    // Check and create employee file if it doesn't exist
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
    
    // Create default credential files if they don't exist
    createDefaultCredentialsIfNeeded();
    
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
            if (!getConfirmation("Do you want to try again?")) {
                runProgram = false;
                cout << "Exiting program. Goodbye!" << endl;
            }
        }
    }
    
    return 0;
}