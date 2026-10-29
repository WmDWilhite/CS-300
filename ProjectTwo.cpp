// ProjectTwo.cpp
// ABCU Course Planner - Hash Table Implementation
// William Wilhite - CS-300

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <direct.h>   // needed for _getcwd()



using namespace std;

// Course Structure

struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};


// Hash Table Structure

struct HashTable {
    int tableSize;
    vector<list<Course>> table;
};


// Helper: Trim whitespace

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}


// Hash Function

int hashFunction(const string& courseNumber, int tableSize) {
    int sum = 0;
    for (char ch : courseNumber) {
        sum += static_cast<int>(ch);
    }
    return sum % tableSize;
}

// Insert Course into Hash Table

void insertCourse(HashTable& hashTable, const Course& course) {
    int index = hashFunction(course.courseNumber, hashTable.tableSize);
    hashTable.table[index].push_back(course);
}

// Load Courses from CSV

vector<Course> loadCourses(const string& filename) {
    vector<Course> rawCourses;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return rawCourses;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }

        if (tokens.size() < 2) continue;

        Course c;
        c.courseNumber = tokens[0];
        c.courseName = tokens[1];

        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                c.prerequisites.push_back(tokens[i]);
            }
        }

        rawCourses.push_back(c);
    }

    file.close();
    return rawCourses;
}


// Validate Prerequisites

void validatePrerequisites(const vector<Course>& rawCourses) {
    for (const auto& course : rawCourses) {
        for (const auto& prereq : course.prerequisites) {
            bool found = false;

            for (const auto& c : rawCourses) {
                if (c.courseNumber == prereq) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Error: prerequisite " << prereq
                    << " for course " << course.courseNumber
                    << " does not exist" << endl;
            }
        }
    }
}


// Build Hash Table

void buildHashTable(const vector<Course>& rawCourses, HashTable& hashTable) {
    for (const auto& course : rawCourses) {
        insertCourse(hashTable, course);
    }
}


// Search for a Course

Course* searchCourse(HashTable& hashTable, const string& courseNumber) {
    int index = hashFunction(courseNumber, hashTable.tableSize);
    for (auto& course : hashTable.table[index]) {
        if (course.courseNumber == courseNumber) {
            return &course;
        }
    }
    return nullptr;
}


// Print Course Information

void printCourseInfo(HashTable& hashTable, const string& courseNumber) {
    Course* course = searchCourse(hashTable, courseNumber);

    if (course == nullptr) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseName << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            cout << course->prerequisites[i];
            if (i < course->prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}


// Print All Courses (Sorted)

void printAllCourses(HashTable& hashTable) {
    vector<string> keys;

    for (int i = 0; i < hashTable.tableSize; ++i) {
        for (const auto& course : hashTable.table[i]) {
            keys.push_back(course.courseNumber);
        }
    }

    sort(keys.begin(), keys.end());

    cout << "Here is a sample schedule:" << endl;

    for (const auto& key : keys) {
        Course* c = searchCourse(hashTable, key);
        if (c != nullptr) {
            cout << c->courseNumber << ", " << c->courseName << endl;
        }
    }
}


// Convert string to uppercase

void toUpperCase(string& s) {
    for (char& c : s) {
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }
}


// Main Menu

int main() {
    HashTable hashTable;
    hashTable.tableSize = 10;
    hashTable.table = vector<list<Course>>(hashTable.tableSize);

    vector<Course> rawCourses;
    bool dataLoaded = false;

    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";

        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input." << endl;
            continue;
        }

        if (choice == 1) {
            string filename;
            cout << "Enter file name: ";
            cin >> filename;

            rawCourses = loadCourses(filename);
            validatePrerequisites(rawCourses);
            buildHashTable(rawCourses, hashTable);
            dataLoaded = true;
            cout << "Data loaded successfully" << endl;
        }

        else if (choice == 2) {
            if (!dataLoaded) {
                cout << "No data loaded." << endl;
            }
            else {
                printAllCourses(hashTable);
            }
        }
        else if (choice == 3) {
            if (!dataLoaded) {
                cout << "No data loaded." << endl;
            }
            else {
                string courseNum;
                cout << "What course do you want to know about? ";
                cin >> courseNum;
                toUpperCase(courseNum);
                printCourseInfo(hashTable, courseNum);
            }
        }
        else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
        }
        else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
