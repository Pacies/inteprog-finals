#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class User {
protected:
    string username;
    string password;
public:
    User(const string& uname, const string& pwd) : username(uname), password(pwd) {}

    virtual bool login() {
        cout << "Base login (should be overridden)." << endl;
        return false;
    }

    virtual ~User() {}
};

class Employee : public User {
public:
    Employee(const string& uname, const string& pwd) : User(uname, pwd) {}

    bool login() override {
        cout << "Logging in as Employee..." << endl;
        // Add employee features here.
        return true;
    }
};

class Admin : public User {
public:
    Admin(const string& uname, const string& pwd) : User(uname, pwd) {}

    bool login() override {
        cout << "Logging in as Admin..." << endl;
        // Add admin features here.
        return true;
    }
};

// Function to check credentials in a given file
bool checkCredentials(const string& filename, const string& username, const string& password) {
    ifstream file(filename.c_str());
    if (!file) {
        cout << "Error: Could not open " << filename << endl;
        return false;
    }

    string fileUsername, filePassword;
    while (file >> fileUsername >> filePassword) {
        if (username == fileUsername && password == filePassword) {
            return true;
        }
    }
    return false;
}

int main() {
    string username, password;

    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    // Check in employee.txt
    if (checkCredentials("employee.txt", username, password)) {
        Employee emp(username, password);
        emp.login();
    }
    // Check in admin.txt
    else if (checkCredentials("admin.txt", username, password)) {
        Admin admin(username, password);
        admin.login();
    }
    // Not found in either file
    else {
        cout << "Invalid username or password." << endl;
    }

    return 0;
}
