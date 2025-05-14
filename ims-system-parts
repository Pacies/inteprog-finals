#include <iostream> 
#include <fstream>
#include <string>
#include <limits>
#include <cctype> // for tolower
#include <cstdlib> // for system()
#include <algorithm> // for find_if

using namespace std;

#define MAX_USERS 1000

struct User {
    int id;
    string username;
    string password;
    string fullName;
    string email;
};

// Function prototypes
void loadUsers(User users[], int& count, const string& filename);
void saveUsers(User users[], int count, const string& filename);
void displayAllUsers(User users[], int count);
void addUser(User users[], int& count, const string& filename);
void deleteUser(User users[], int& count, const string& filename);
void editUser(User users[], int count, const string& filename);
void searchUser(User users[], int count);
int getNextId(User users[], int count);
void clearScreen();
void waitForKeypress();
bool usernameExists(User users[], int count, const string& username);
bool isValidUsername(const string& username);

int main() {
    User users[MAX_USERS];
    int userCount = 0;
    string filename = "users.txt";
    int choice;

    loadUsers(users, userCount, filename);

    while (true) {
        clearScreen();
        cout << "===== USER MANAGEMENT SYSTEM =====" << endl;
        cout << "1. Display All Users" << endl;
        cout << "2. Add New User" << endl;
        cout << "3. Delete User" << endl;
        cout << "4. Edit User" << endl;
        cout << "5. Search User" << endl;
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
            case 1: displayAllUsers(users, userCount); break;
            case 2: addUser(users, userCount, filename); break;
            case 3: deleteUser(users, userCount, filename); break;
            case 4: editUser(users, userCount, filename); break;
            case 5: searchUser(users, userCount); break;
            case 0: 
                cout << "Exiting program. Goodbye!" << endl; 
                return 0;
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

bool usernameExists(User users[], int count, const string& username) {
    for (int i = 0; i < count; i++) {
        if (users[i].username == username)
            return true;
    }
    return false;
}

// New function to validate username (no spaces allowed)
bool isValidUsername(const string& username) {
    // Check if username is empty
    if (username.empty()) {
        return false;
    }
    
    // Check if username contains any spaces
    return (username.find(' ') == string::npos);
}

void addUser(User users[], int& count, const string& filename) {
    clearScreen();
    if (count >= MAX_USERS) {
        cout << "User limit reached. Cannot add more users." << endl;
        waitForKeypress();
        return;
    }

    User newUser;
    cout << "===== ADD NEW USER =====" << endl;
    newUser.id = getNextId(users, count);

    while (true) {
        cout << "Enter username (no spaces allowed): ";
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
            cout << "Invalid username. Spaces are not allowed." << endl;
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

        cout << "Enter new value: ";
        getline(cin, newValue);

        switch (field) {
            case 1:
                // Validate username when editing
                if (!isValidUsername(newValue)) {
                    cout << "Invalid username. Spaces are not allowed." << endl;
                    waitForKeypress();
                    return;
                }
                
                if (usernameExists(users, count, newValue) && newValue != users[index].username) {
                    cout << "Username already exists." << endl;
                    waitForKeypress();
                    return;
                }
                users[index].username = newValue;
                break;
            case 2: 
                if (newValue == users[index].password) { 
                    cout << "PLEASE? New password cannot be the same as the current password. ^^" << endl; 
                    waitForKeypress(); 
                    return;
                }
                users[index].password = newValue; 
                break; 
            case 3: 
                users[index].fullName = newValue; 
                break;
            case 4: 
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
