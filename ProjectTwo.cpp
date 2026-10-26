// Fernando Rivas
// CS 300 Project Two - Advising Assistance Program
// Uses the binary search tree recommended in Project One.
// File format: course number,title,prerequisite number,prerequisite number,...
// Each course belongs on one line. Do not include a header row.

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// A course stores prerequisite numbers. Their titles are found in the tree.
struct Course {
    string courseNumber;
    string title;
    vector<string> prerequisites;
};

struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(const Course& newCourse) {
        course = newCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Remove whitespace around file fields and keyboard input.
string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

string toUpperCase(string text) {
    for (size_t i = 0; i < text.length(); ++i) {
        text[i] = static_cast<char>(toupper(static_cast<unsigned char>(text[i])));
    }
    return text;
}

// Course numbers contain letters followed by digits, such as CSCI100.
bool validCourseNumber(const string& number) {
    size_t i = 0;
    while (i < number.length() && isalpha(static_cast<unsigned char>(number[i]))) {
        ++i;
    }
    if (i == 0 || i == number.length()) {
        return false;
    }
    while (i < number.length()) {
        if (!isdigit(static_cast<unsigned char>(number[i]))) {
            return false;
        }
        ++i;
    }
    return true;
}

class BinarySearchTree {
private:
    Node* root;

    // Left, current, right visits course numbers in alphanumeric order.
    void printInOrder(const Node* node) const {
        if (node == nullptr) {
            return;
        }
        printInOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.title << endl;
        printInOrder(node->right);
    }

    // Delete children before their parent to release every allocated node.
    void deleteTree(Node* node) {
        if (node == nullptr) {
            return;
        }
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

    bool checkPrerequisites(const Node* node) const {
        if (node == nullptr) {
            return true;
        }
        for (const string& number : node->course.prerequisites) {
            if (search(number) == nullptr) {
                cout << "Error: " << node->course.courseNumber
                     << " lists prerequisite " << number
                     << ", which is missing from the file." << endl;
                return false;
            }
        }
        return checkPrerequisites(node->left) && checkPrerequisites(node->right);
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        deleteTree(root);
    }

    // Prevent two tree objects from owning and deleting the same nodes.
    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    bool empty() const {
        return root == nullptr;
    }

    // Smaller keys go left; larger keys go right. Duplicate keys are rejected.
    bool insert(const Course& course) {
        if (root == nullptr) {
            root = new Node(course);
            return true;
        }

        Node* current = root;
        while (true) {
            if (course.courseNumber == current->course.courseNumber) {
                return false;
            }
            if (course.courseNumber < current->course.courseNumber) {
                if (current->left == nullptr) {
                    current->left = new Node(course);
                    return true;
                }
                current = current->left;
            } else {
                if (current->right == nullptr) {
                    current->right = new Node(course);
                    return true;
                }
                current = current->right;
            }
        }
    }

    const Course* search(const string& number) const {
        const Node* current = root;
        while (current != nullptr) {
            if (number == current->course.courseNumber) {
                return &current->course;
            }
            if (number < current->course.courseNumber) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return nullptr;
    }

    void printAll() const {
        printInOrder(root);
    }

    bool validatePrerequisites() const {
        return checkPrerequisites(root);
    }

    // A successful load replaces the old tree. The temporary tree deletes it.
    void swapContents(BinarySearchTree& other) {
        swap(root, other.root);
    }
};

// Split a CSV row without an external CSVParser. Quoted titles may have commas.
bool parseCsvRow(const string& line, vector<string>& fields) {
    string field;
    bool insideQuotes = false;
    bool quoteClosed = false;
    fields.clear();

    for (size_t i = 0; i < line.length(); ++i) {
        char character = line[i];
        if (insideQuotes) {
            if (character == '"') {
                // Two quotes inside a quoted field represent one literal quote.
                if (i + 1 < line.length() && line[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    insideQuotes = false;
                    quoteClosed = true;
                }
            } else {
                field += character;
            }
        } else if (character == ',') {
            fields.push_back(trim(field));
            field.clear();
            quoteClosed = false;
        } else if (character == '"') {
            if (quoteClosed || !trim(field).empty()) {
                return false;
            }
            field.clear();
            insideQuotes = true;
        } else if (quoteClosed) {
            if (!isspace(static_cast<unsigned char>(character))) {
                return false;
            }
        } else {
            field += character;
        }
    }

    if (insideQuotes) {
        return false;
    }
    fields.push_back(trim(field));
    return true;
}

bool loadCourses(const string& fileName, BinarySearchTree& courses) {
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        cout << "Error: Could not open file: " << fileName << endl;
        return false;
    }

    // Validate a temporary tree first so a bad file cannot erase loaded data.
    BinarySearchTree newCourses;
    string line;
    int lineNumber = 0;
    int courseCount = 0;

    while (getline(inputFile, line)) {
        ++lineNumber;
        // Some UTF-8 files begin with a three-byte encoding marker.
        if (lineNumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }
        if (trim(line).empty()) {
            continue;
        }

        vector<string> fields;
        if (!parseCsvRow(line, fields) || fields.size() < 2 || fields[1].empty()) {
            cout << "Error on line " << lineNumber
                 << ": Expected a course number, title, and optional prerequisites." << endl;
            return false;
        }

        Course course;
        course.courseNumber = toUpperCase(fields[0]);
        course.title = fields[1];
        if (!validCourseNumber(course.courseNumber)) {
            cout << "Error on line " << lineNumber << ": Invalid course number." << endl;
            return false;
        }

        for (size_t i = 2; i < fields.size(); ++i) {
            string prerequisite = toUpperCase(fields[i]);
            // Empty optional cells mean that no prerequisite is listed there.
            if (prerequisite.empty()) {
                continue;
            }
            if (!validCourseNumber(prerequisite) || prerequisite == course.courseNumber) {
                cout << "Error on line " << lineNumber << ": Invalid prerequisite." << endl;
                return false;
            }
            course.prerequisites.push_back(prerequisite);
        }

        if (!newCourses.insert(course)) {
            cout << "Error on line " << lineNumber << ": Duplicate course number "
                 << course.courseNumber << "." << endl;
            return false;
        }
        ++courseCount;
    }

    if (inputFile.bad()) {
        cout << "Error: The file could not be read completely." << endl;
        return false;
    }
    if (courseCount == 0) {
        cout << "Error: The file contains no courses." << endl;
        return false;
    }
    // Check after reading every row because a prerequisite may appear later.
    if (!newCourses.validatePrerequisites()) {
        return false;
    }

    courses.swapContents(newCourses);
    cout << courseCount << " courses loaded successfully." << endl;
    return true;
}

void printCourseInformation(const BinarySearchTree& courses, const string& number) {
    const Course* course = courses.search(number);
    if (course == nullptr) {
        cout << "Course not found: " << number << endl;
        return;
    }

    cout << "\n" << course->courseNumber << ", " << course->title << endl;
    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites:" << endl;
    for (const string& prerequisiteNumber : course->prerequisites) {
        // Loading has already confirmed that each prerequisite exists.
        const Course* prerequisite = courses.search(prerequisiteNumber);
        if (prerequisite != nullptr) {
            cout << "  " << prerequisite->courseNumber << ", " << prerequisite->title << endl;
        }
    }
}

void printMenu() {
    cout << "\n1. Load Data Structure" << endl;
    cout << "2. Print Course List" << endl;
    cout << "3. Print Course Information" << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

int main() {
    BinarySearchTree courses;
    string choice;
    cout << "Welcome to the course planner." << endl;

    while (true) {
        printMenu();
        // Reading whole lines also handles letters and mixed input safely.
        if (!getline(cin, choice)) {
            break;
        }
        choice = trim(choice);

        if (choice == "1") {
            string fileName;
            cout << "Enter the course data file name or full path: ";
            if (!getline(cin, fileName)) {
                break;
            }
            fileName = trim(fileName);
            // Allow a copied file path to have surrounding double quotes.
            if (fileName.length() >= 2 && fileName.front() == '"' && fileName.back() == '"') {
                fileName = fileName.substr(1, fileName.length() - 2);
            }
            if (fileName.empty()) {
                cout << "Error: Please enter a file name." << endl;
            } else if (!loadCourses(fileName, courses)) {
                cout << "Load failed. Previously loaded data, if any, is unchanged." << endl;
            }
        } else if (choice == "2" || choice == "3") {
            if (courses.empty()) {
                cout << "Please load the course data using option 1 first." << endl;
            } else if (choice == "2") {
                cout << "\nHere is the course list:" << endl;
                courses.printAll();
            } else {
                string number;
                cout << "What course do you want to know about? ";
                if (!getline(cin, number)) {
                    break;
                }
                number = toUpperCase(trim(number));
                if (!validCourseNumber(number)) {
                    cout << "Error: Enter a course number such as CSCI100." << endl;
                } else {
                    printCourseInformation(courses, number);
                }
            }
        } else if (choice == "9") {
            break;
        } else {
            cout << "Error: Please choose 1, 2, 3, or 9." << endl;
        }
    }

    cout << "\nThank you for using the course planner!" << endl;
    return 0;
}
