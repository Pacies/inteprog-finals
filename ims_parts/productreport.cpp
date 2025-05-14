#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <ctime>

using namespace std;

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

int main() {
    displayProductReport();
    return 0;
}