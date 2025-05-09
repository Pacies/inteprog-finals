#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

int main() {
    string username, password;
    string userType;
    
    cout << "\n===== LOGIN SYSTEM =====" << endl;
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
        cout << "Login successful! You are logged in as " << userType << "." << endl;
        
        // Add specific functionality based on user type
        if (userType == "admin") {
            cout << "Welcome to the Admin Panel. You have full access." << endl;
            // Admin-specific functionality would go here
        } else {
            cout << "Welcome to the Employee Panel. You have limited access." << endl;
            // Employee-specific functionality would go here
        }
        
        return 0;
    } else {
        cout << "Login failed. Invalid username or password." << endl;
        return 1;
    }
}