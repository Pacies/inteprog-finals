#include <iostream> 
#include <fstream>
#include <string>
#include <limits>
#include <cctype> // for tolower
#include <cstdlib> // for system()
#include <algorithm> // for find_if

using namespace std;

#define MAX_USERS 1000

enum UserRole {
    ROLE_ADMIN,
    ROLE_EMPLOYEE
};
struct User {
    int id;
    string username;
    string password;
    string fullName;
    string email;
};

struct LoginCredential {
    string username;
    string password;
};

// Function prototypes
void loadUsers(User users[], int& count, const string& filename);
void saveUsers(User users[], int count, const string& filename);
void addUser(User users[], int& count, const string& filename);
void deleteUser(User users[], int& count, const string& filename);
void editUser(User users[], int count, const string& filename);
int getNextId(User users[], int count);
void clearScreen();
void waitForKeypress();
bool usernameExists(User users[], int count, const string& username);
bool isValidUsername(const string& username);
bool login(UserRole& role);
void initializeCredentials();
void adminMenu(User users[], int& userCount);
void employeeMenu(User users[], int& userCount);
void manageAdminCredentials();
void manageEmployeeCredentials();

int main() {
    // Initialize admin and employee credentials if they don't exist
    initializeCredentials();
    
    UserRole currentRole;
    
    // Login screen
    while (!login(currentRole)) {
        // Keep trying until successful login
    }
    
    User users[MAX_USERS];
    int userCount = 0;
    string filename = "users.txt";
    
    loadUsers(users, userCount, filename);
    
    if (currentRole == ROLE_ADMIN) {
        adminMenu(users, userCount);
    } else {
        employeeMenu(users, userCount);
    }
    
    cout << "Thank you for using the System. Goodbye!" << endl;
    cout << "Press Enter to exit..." << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    return 0;
}

void initializeCredentials() {
    // Initialize admin credentials
    ofstream adminFile("admin.txt", ios::app);
    if (adminFile) {
        ifstream checkFile("admin.txt");
        string line;
        if (!getline(checkFile, line)) {
            // File is empty, write default admin credentials
            adminFile << "admin1" << endl;
            adminFile << "87654321" << endl;
        }
        adminFile.close();
    }
    
    // Initialize employee credentials
    ofstream empFile("employee.txt", ios::app);
    if (empFile) {
        ifstream checkFile("employee.txt");
        string line;
        if (!getline(checkFile, line)) {
            // File is empty, write default employee credentials
            empFile << "employee1" << endl;
            empFile << "12345678" << endl;
        }
        empFile.close();
    }
}

bool login(UserRole& role) {
    clearScreen();
    cout << "===== USER MANAGEMENT SYSTEM LOGIN =====" << endl;
    
    string username, password;
    int loginType;
    
    cout << "Login as:" << endl;
    cout << "1. Admin" << endl;
    cout << "2. Employee" << endl;
    cout << "Enter choice (1-2): ";
    
    if (!(cin >> loginType) || (loginType != 1 && loginType != 2)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice. Press Enter to try again..." << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    cout << "Username: ";
    getline(cin, username);
    
    cout << "Password: ";
    getline(cin, password);
    
    string filename = (loginType == 1) ? "admin.txt" : "employee.txt";
    ifstream file(filename);
    
    if (!file) {
        cout << "Error: Could not open credentials file." << endl;
        waitForKeypress();
        return false;
    }
    
    string storedUsername, storedPassword;
    getline(file, storedUsername);
    getline(file, storedPassword);
    file.close();
    
    if (username == storedUsername && password == storedPassword) {
        role = (loginType == 1) ? ROLE_ADMIN : ROLE_EMPLOYEE;
        cout << "Login successful!" << endl;
        waitForKeypress();
        return true;
    } else {
        cout << "Invalid username or password. Press Enter to try again..." << endl;
        waitForKeypress();
        return false;
    }
}

void adminMenu(User users[], int& userCount) {
    string filename = "users.txt";
    int choice;

    while (true) {
        clearScreen();
        cout << "===== ADMIN MENU =====" << endl;
        cout << "1. Manage Username & Password" << endl;
        cout << "2. Add New User" << endl;
        cout << "3. Delete User" << endl;
        cout << "4. Edit User" << endl;
        cout << "0. Exit" << endl;

        string input;
        cout << "Enter your choice: ";
        getline(cin, input);

        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);

        bool isValid = true;
        if (input.empty()) {
            isValid = false;
        } else {
            for (char c : input) {
                if (!isdigit(c)) {
                    isValid = false;
                    break;
                }
            }
        }

        if (!isValid) {
            cout << "Invalid input. Please enter a valid number only." << endl;
            waitForKeypress();
            continue;
        }

        choice = stoi(input);

        switch (choice) {
            case 1: manageAdminCredentials(); break;
            case 2: addUser(users, userCount, filename); break;
            case 3: deleteUser(users, userCount, filename); break;
            case 4: editUser(users, userCount, filename); break;
            case 0: 
                cout << "Are you sure you want to exit? Press Enter to confirm..." << endl; 
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
                waitForKeypress();
        }
    }
}

void employeeMenu(User users[], int& userCount) {
    int choice;

    while (true) {
        clearScreen();
        cout << "===== EMPLOYEE MENU =====" << endl;
        cout << "1. Manage Username & Password" << endl;
        cout << "0. Exit" << endl;

        string input;
        cout << "Enter your choice: ";
        getline(cin, input);

        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);

        bool isValid = true;
        if (input.empty()) {
            isValid = false;
        } else {
            for (char c : input) {
                if (!isdigit(c)) {
                    isValid = false;
                    break;
                }
            }
        }

        if (!isValid) {
            cout << "Invalid input. Please enter a valid number only." << endl;
            waitForKeypress();
            continue;
        }

        choice = stoi(input);

        switch (choice) {
            case 1: manageEmployeeCredentials(); break;
            case 0: 
                cout << "Are you sure you want to exit? Press Enter to confirm..." << endl; 
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
                waitForKeypress();
        }
    }
}

void loadUsers(User users[], int& count, const string& filename) {
    ifstream file(filename);
    if (!file) return;

    string line;
    while (getline(file, line)) {
        if (count >= MAX_USERS) break;
        users[count].id = stoi(line);
        getline(file, users[count].username);
        getline(file, users[count].password);
        getline(file, users[count].fullName);
        getline(file, users[count].email);
        count++;
    }

    file.close();
}

void saveUsers(User users[], int count, const string& filename) {
    ofstream file(filename);
    if (!file) {
        cout << "Error opening file for writing!" << endl;
        return;
    }

    for (int i = 0; i < count; i++) {
        file << users[i].id << endl;
        file << users[i].username << endl;
        file << users[i].password << endl;
        file << users[i].fullName << endl;
        file << users[i].email << endl;
    }

    file.close();
}

// Function to check if username contains only allowed characters
bool isValidUsernameFormat(const string& username) {
    if (username.empty()) {
        return false;
    }
    
    // Allow alphanumeric characters and '@' symbol only
    for (char c : username) {
        if (!isalnum(c) && c != '@') {
            return false;
        }
    }
    
    return true;
}

void manageAdminCredentials() {
    clearScreen();
    cout << "===== MANAGE ADMIN CREDENTIALS =====" << endl;

    // Read current credentials
    ifstream inFile("admin.txt");
    if (!inFile) {
        cout << "Error: Could not open admin credentials file." << endl;
        waitForKeypress();
        return;
    }

    string currentUsername, currentPassword;
    getline(inFile, currentUsername);
    getline(inFile, currentPassword);
    inFile.close();

    cout << "Current Username: " << currentUsername << endl;
    
    string newUsername, newPassword, confirmPassword;
    
    // Ask for new username with validation
    do {
        cout << "Enter new username (alphanumeric and '@' only, leave blank to keep current): ";
        getline(cin, newUsername);
        
        if (newUsername.empty()) {
            newUsername = currentUsername;
            break;
        } else if (!isValidUsernameFormat(newUsername)) {
            cout << "Invalid username. Only letters, numbers, and '@' are allowed." << endl;
        } else {
            break;
        }
    } while (true);
    
    // Ask for new password
    cout << "Enter new password (leave blank to keep current): ";
    getline(cin, newPassword);
    
    if (!newPassword.empty()) {
        cout << "Confirm new password: ";
        getline(cin, confirmPassword);
        
        if (newPassword != confirmPassword) {
            cout << "Passwords do not match. Credentials not updated." << endl;
            waitForKeypress();
            return;
        }
    } else {
        newPassword = currentPassword;
    }
    
    // Save new credentials
    ofstream outFile("admin.txt");
    if (!outFile) {
        cout << "Error: Could not update admin credentials file." << endl;
        waitForKeypress();
        return;
    }
    
    outFile << newUsername << endl;
    outFile << newPassword << endl;
    outFile.close();
    
    cout << "Admin credentials updated successfully!" << endl;
    waitForKeypress();
}

void manageEmployeeCredentials() {
    clearScreen();
    cout << "===== MANAGE EMPLOYEE CREDENTIALS =====" << endl;

    // Read current credentials
    ifstream inFile("employee.txt");
    if (!inFile) {
        cout << "Error: Could not open employee credentials file." << endl;
        waitForKeypress();
        return;
    }

    string currentUsername, currentPassword;
    getline(inFile, currentUsername);
    getline(inFile, currentPassword);
    inFile.close();

    cout << "Current Username: " << currentUsername << endl;
    
    string newUsername, newPassword, confirmPassword;
    
    // Ask for new username with validation
    do {
        cout << "Enter new username (alphanumeric and '@' only, leave blank to keep current): ";
        getline(cin, newUsername);
        
        if (newUsername.empty()) {
            newUsername = currentUsername;
            break;
        } else if (!isValidUsernameFormat(newUsername)) {
            cout << "Invalid username. Only letters, numbers, and '@' are allowed." << endl;
        } else {
            break;
        }
    } while (true);
    
    // Ask for new password
    cout << "Enter new password (leave blank to keep current): ";
    getline(cin, newPassword);
    
    if (!newPassword.empty()) {
        cout << "Confirm new password: ";
        getline(cin, confirmPassword);
        
        if (newPassword != confirmPassword) {
            cout << "Passwords do not match. Credentials not updated." << endl;
            waitForKeypress();
            return;
        }
    } else {
        newPassword = currentPassword;
    }
    
    // Save new credentials
    ofstream outFile("employee.txt");
    if (!outFile) {
        cout << "Error: Could not update employee credentials file." << endl;
        waitForKeypress();
        return;
    }
    
    outFile << newUsername << endl;
    outFile << newPassword << endl;
    outFile.close();
    
    cout << "Employee credentials updated successfully!" << endl;
    waitForKeypress();
}

bool usernameExists(User users[], int count, const string& username) {
    for (int i = 0; i < count; i++) {
        if (users[i].username == username)
            return true;
    }
    return false;
}

// Function to validate username (no spaces allowed)
bool isValidUsername(const string& username) {
    // Check if username is empty
    if (username.empty()) {
        return false;
    }
    
    // Check if username contains any spaces
    if (username.find(' ') != string::npos) {
        return false;
    }
    
    // Check if username contains only allowed characters (alphanumeric and '@')
    for (char c : username) {
        if (!isalnum(c) && c != '@') {
            return false;
        }
    }
    
    return true;
}

void addUser(User users[], int& count, const string& filename) {
    clearScreen();
    if (count >= MAX_USERS) {
        cout << "User limit reached. Cannot add more users." << endl;
        waitForKeypress();
        return;
    }

    User newUser;
    cout << "===== Add New Employee =====" << endl;
    newUser.id = getNextId(users, count);

    while (true) {
        cout << "Enter username (alphanumeric and '@' only): ";
        string tempUsername;
        getline(cin, tempUsername);

        // Trim leading and trailing whitespace
        size_t first = tempUsername.find_first_not_of(" \t");
        size_t last = tempUsername.find_last_not_of(" \t");
        
        if (first == string::npos || last == string::npos) {
            cout << "Username cannot be empty." << endl;
            continue;
        }

        newUser.username = tempUsername.substr(first, last - first + 1);

        // Validate username
        if (!isValidUsername(newUser.username)) {
            cout << "Invalid username. Only letters, numbers, and '@' are allowed." << endl;
            continue;
        }

        if (usernameExists(users, count, newUser.username)) {
            cout << "Username already exists. Please choose another." << endl;
            continue;
        }

        break;
    }

    cout << "Enter password: ";
    getline(cin, newUser.password);

    cout << "Enter full name: ";
    getline(cin, newUser.fullName);

    cout << "Enter email: ";
    getline(cin, newUser.email);

    users[count++] = newUser;
    saveUsers(users, count, filename);
    cout << "User added successfully!" << endl;
    waitForKeypress();
}

void deleteUser(User users[], int& count, const string& filename) {
    clearScreen();
    int idToDelete;
    cout << "===== DELETE USER =====" << endl;

    if (count == 0) {
        cout << "No users found." << endl;
        waitForKeypress();
        return;
    }

    cout << "Current users:\nID\tUsername\n----------------" << endl;
    for (int i = 0; i < count; i++) {
        cout << users[i].id << "\t" << users[i].username << endl;
    }

    cout << "\nEnter ID of user to delete: ";
    if (!(cin >> idToDelete)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input." << endl;
        waitForKeypress();
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int index = -1;
    for (int i = 0; i < count; i++) {
        if (users[i].id == idToDelete) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        char confirm;
        cout << "Are you sure you want to delete user '" << users[index].username << "'? (y/n): ";
        cin >> confirm;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (tolower(confirm) == 'y') {
            for (int i = index; i < count - 1; i++) {
                users[i] = users[i + 1];
            }
            count--;
            saveUsers(users, count, filename);
            cout << "User deleted successfully!" << endl;
        } else {
            cout << "Deletion cancelled." << endl;
        }
    } else {
        cout << "User ID not found." << endl;
    }

    waitForKeypress();
}

void editUser(User users[], int count, const string& filename) {
    clearScreen();
    int idToEdit;
    cout << "===== EDIT USER =====" << endl;

    if (count == 0) {
        cout << "No users found." << endl;
        waitForKeypress();
        return;
    }

    cout << "Current users:\nID\tUsername\n----------------" << endl;
    for (int i = 0; i < count; i++) {
        cout << users[i].id << "\t" << users[i].username << endl;
    }

    cout << "\nEnter ID of user to edit: ";
    if (!(cin >> idToEdit)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input." << endl;
        waitForKeypress();
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int index = -1;
    for (int i = 0; i < count; i++) {
        if (users[i].id == idToEdit) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        int field;
        string newValue;
        cout << "1. Username: " << users[index].username << endl;
        cout << "2. Password: " << users[index].password << endl;
        cout << "3. Full Name: " << users[index].fullName << endl;
        cout << "4. Email: " << users[index].email << endl;

        cout << "Enter field number to edit (1-4): ";
        if (!(cin >> field)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input." << endl;
            waitForKeypress();
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (field) {
            case 1:
                do {
                    cout << "Enter new username (alphanumeric and '@' only): ";
                    getline(cin, newValue);
                    
                    // Validate username
                    if (!isValidUsername(newValue)) {
                        cout << "Invalid username. Only letters, numbers, and '@' are allowed." << endl;
                    } else if (usernameExists(users, count, newValue) && newValue != users[index].username) {
                        cout << "Username already exists. Please choose another." << endl;
                    } else {
                        users[index].username = newValue;
                        break;
                    }
                } while (true);
                break;
            case 2: 
                cout << "Enter new value: ";
                getline(cin, newValue);
                if (newValue == users[index].password) { 
                    cout << "PLEASE? New password cannot be the same as the current password. ^^" << endl; 
                    waitForKeypress(); 
                    return;
                }
                users[index].password = newValue; 
                break; 
            case 3: 
                cout << "Enter new value: ";
                getline(cin, newValue);
                users[index].fullName = newValue; 
                break;
            case 4: 
                cout << "Enter new value: ";
                getline(cin, newValue);
                users[index].email = newValue; 
                break;
            default:
                cout << "Invalid field." << endl;
                waitForKeypress();
                return;
        }

        saveUsers(users, count, filename);
        cout << "User updated successfully!" << endl;
    } else {
        cout << "User ID not found." << endl;
    }

    waitForKeypress();
}

void searchUser(User users[], int count) {
    clearScreen();
    cout << "===== SEARCH USER =====" << endl;

    if (count == 0) {
        cout << "No users found." << endl;
        waitForKeypress();
        return;
    }

    string term;
    cout << "Enter username, full name, or email to search: ";
    getline(cin, term);

    bool found = false;
    cout << "ID\tUsername\tFull Name\tEmail\n";
    cout << "------------------------------------------------\n";

    for (int i = 0; i < count; i++) {
        if (users[i].username.find(term) != string::npos ||
            users[i].fullName.find(term) != string::npos ||
            users[i].email.find(term) != string::npos) {
            cout << users[i].id << "\t" << users[i].username << "\t\t"
                 << users[i].fullName << "\t\t" << users[i].email << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No users found matching '" << term << "'." << endl;
    }

    waitForKeypress();
}

void displayAllUsers(User users[], int count) {
    clearScreen();
    cout << "===== ALL USERS =====" << endl;
    if (count == 0) {
        cout << "No users found." << endl;
        waitForKeypress();
        return;
    }

    cout << "ID\tUsername\tFull Name\tEmail" << endl;
    cout << "------------------------------------------------" << endl;
    for (int i = 0; i < count; i++) {
        cout << users[i].id << "\t" 
             << users[i].username << "\t\t" 
             << users[i].fullName << "\t\t" 
             << users[i].email << endl;
    }
    waitForKeypress();
}

int getNextId(User users[], int count) {
    int maxId = 0;
    for (int i = 0; i < count; i++) {
        if (users[i].id > maxId)
            maxId = users[i].id;
    }
    return maxId + 1;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void waitForKeypress() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}