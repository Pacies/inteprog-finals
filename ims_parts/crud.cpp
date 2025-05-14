#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>
#include <cctype>

using namespace std;

// Inventory Class 
class Inventory {
protected:
    struct Record {
        int id;
        char name[50];
        int quantity;
        double unitPrice;
        Record* next;
    };

    Record* head;
    int nextId;
    char filename[20];

public:
    Inventory(const char* file) {
        head = nullptr;
        nextId = 1;
        strcpy(filename, file);
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

    virtual void displayMenu() = 0;
    
    void addRecord() {
        Record* newRecord = new Record;
        newRecord->id = nextId++;
        
        cout << "Enter name: ";
        cin.ignore();
        cin.getline(newRecord->name, 50);
        
        // Pang-validate ng name (no numbers)
        bool isValidName = true;
        for (int i = 0; newRecord->name[i] != '\0'; i++) {
            if (isdigit(newRecord->name[i])) {
                isValidName = false;
                break;
            }
        }
        
        if (!isValidName) {
            cout << "Invalid name. Name should not contain numbers." << endl;
            delete newRecord;
            return;
        }
        
        cout << "Enter quantity: ";
        cin >> newRecord->quantity;
        
        // Pang-validate ng quantity
        if (newRecord->quantity <= 0) {
            cout << "Invalid quantity. Quantity must be positive." << endl;
            delete newRecord;
            return;
        }
        
        cout << "Enter unit price: ";
        cin >> newRecord->unitPrice;
        
        // Pang-validate ng price
        if (newRecord->unitPrice <= 0) {
            cout << "Invalid unit price. Dapat tama." << endl;
            delete newRecord;
            return;
        }
        
        // Confirmation ng prompt
        char confirm;
        cout << "Are you sure you want to proceed? (Y/N): ";
        cin >> confirm;
        
        if (toupper(confirm) != 'Y') {
            cout << "Cancelled na po." << endl;
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
        
        cout << "Na-add na siya." << endl;
        saveToFile();
    }
    
    void editRecord() {
        if (head == nullptr) {
            cout << "Wala naman laman. Ano e-edit mo?" << endl;
            return;
        }
        
        displayInventory();
        
        int idToEdit;
        cout << "Enter ID of record to edit: ";
        cin >> idToEdit;
        
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
            cout << "Record with ID " << idToEdit << " Wala raw." << endl;
            return;
        }
        
        cout << "Editing record with ID: " << idToEdit << endl;
        cout << "Current name: " << current->name << endl;
        cout << "Current quantity: " << current->quantity << endl;
        cout << "Current unit price: ₱" << fixed << setprecision(2) << current->unitPrice << endl;
        
        cout << "Enter new name (or press Enter to keep current): ";
        cin.ignore();
        char newName[50];
        cin.getline(newName, 50);
        
        if (strlen(newName) > 0) {
            // Validate name (no numbers)
            bool isValidName = true;
            for (int i = 0; newName[i] != '\0'; i++) {
                if (isdigit(newName[i])) {
                    isValidName = false;
                    break;
                }
            }
            
            if (!isValidName) {
                cout << "Invalid name. Name should not contain numbers. Name not updated." << endl;
            } else {
                strcpy(current->name, newName);
            }
        }
        
        cout << "Enter new quantity (or 0 to keep current): ";
        int newQuantity;
        cin >> newQuantity;
        
        if (newQuantity > 0) {
            current->quantity = newQuantity;
        } else if (newQuantity < 0) {
            cout << "Invalid quantity. Positive lang pwede. Quantity not updated." << endl;
        }
        
        cout << "Enter new unit price (or 0 to keep current): ";
        double newUnitPrice;
        cin >> newUnitPrice;
        
        if (newUnitPrice > 0) {
            current->unitPrice = newUnitPrice;
        } else if (newUnitPrice < 0) {
            cout << "Invalid price. pwede ba yun? Price not updated." << endl;
        }
        
        // Confirmation prompt
        char confirm;
        cout << "Sure na ba? (Y/N): ";
        cin >> confirm;
        
        if (toupper(confirm) != 'Y') {
            cout << "CANCELLED!." << endl;
            return;
        }
        
        cout << "Naka-save na!" << endl;
        saveToFile();
    }
    
    void deleteRecord() {
        if (head == nullptr) {
            cout << "Anong id-delete mo? Wala naman laman." << endl;
            return;
        }
        
        displayInventory();
        
        int idToDelete;
        cout << "Enter ID of record to delete: ";
        cin >> idToDelete;
        
        // Confirmation prompt
        char confirm;
        cout << "Are you sure you want to proceed? (Y/N): ";
        cin >> confirm;
        
        if (toupper(confirm) != 'Y') {
            cout << "Cancelled na raw po." << endl;
            return;
        }
        
        if (head->id == idToDelete) {
            Record* temp = head;
            head = head->next;
            delete temp;
            cout << "Bakit mo binura? Husay ka sa amo mo!" << endl;
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
        cout << "Bakit mo binura? Husay ka sa amo mo!" << endl;
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
                 << "₱" << fixed << setprecision(2) << current->unitPrice << endl;
            current = current->next;
        }
        cout << string(50, '-') << endl;
    }
    
protected:
    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) {
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
        char name[50];
        int quantity;
        double unitPrice;
        
        while (file >> id) {
            file.ignore();
            file.getline(name, 50, '|');
            file >> quantity;
            file.ignore();
            file >> unitPrice;
            
            Record* newRecord = new Record;
            newRecord->id = id;
            strcpy(newRecord->name, name);
            newRecord->quantity = quantity;
            newRecord->unitPrice = unitPrice;
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
                 << current->quantity << " " << current->unitPrice << endl;
            current = current->next;
        }
        
        file.close();
    }
};

class RawMaterialInventory : public Inventory {
public:
    RawMaterialInventory() : Inventory("rawmaterial.txt") {}
    
    void displayMenu() {
        bool running = true;
        
        while (running) {
            cout << "\n" << string(30, '=') << endl;
            cout << setw(25) << "Raw Material Inventory Menu" << endl;
            cout << string(30, '=') << endl;
            cout << "1. Add Record" << endl;
            cout << "2. Edit Record" << endl;
            cout << "3. Delete Record" << endl;
            cout << "4. Display Inventory" << endl;
            cout << "5. Back to Main Menu" << endl;
            cout << "Enter your choice: ";
            
            int choice;
            cin >> choice;
            
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
                    running = false;
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        }
    }
};

class ProductInventory : public Inventory {
public:
    ProductInventory() : Inventory("product.txt") {}
    
    void displayMenu() {
        bool running = true;
        
        while (running) {
            cout << "\n" << string(30, '=') << endl;
            cout << setw(22) << "Product Inventory Menu" << endl;
            cout << string(30, '=') << endl;
            cout << "1. Add Record" << endl;
            cout << "2. Edit Record" << endl;
            cout << "3. Delete Record" << endl;
            cout << "4. Display Inventory" << endl;
            cout << "5. Back to Main Menu" << endl;
            cout << "Enter your choice: ";
            
            int choice;
            cin >> choice;
            
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
                    running = false;
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        }
    }
};

int main() {
    RawMaterialInventory rawMaterials;
    ProductInventory products;
    bool running = true;
    
    cout << fixed << setprecision(2);
    
    while (running) {
        cout << "\n" << string(30, '=') << endl;
        cout << setw(25) << "Inventory Management Menu" << endl;
        cout << string(30, '=') << endl;
        cout << "1. Raw Material Inventory" << endl;
        cout << "2. Product Inventory" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                rawMaterials.displayMenu();
                break;
            case 2:
                products.displayMenu();
                break;
            case 3:
                running = false;
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
    
    return 0;
}
