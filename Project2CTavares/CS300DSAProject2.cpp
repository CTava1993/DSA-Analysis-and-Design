//============================================================================
// Name        : CS300DSAProject2.cpp
// Author      : Christian Tavares
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <algorithm>
#include <iostream>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold course information
struct Course {
    string courseId; // unique identifier
    string title;
    string prerequisite1;
    string prerequisite2;
    Course() {}
};

//============================================================================
// Hash Table class definition
//============================================================================

class HashTable {

private:
    // Define structures to hold courses
    struct Node {
        Course course;
        unsigned int key;
        Node *next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a course
        Node(Course aCourse) : Node() {
        	course = aCourse;
        }

        // initialize with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
        	key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    void Remove(string courseId);
    Course Search(string courseId);
};

string getPrerequisiteText(string prereq1, string prereq2) { // A function that creates a string of text from prerequisites
	string middleText = ", ";
	string finalText;

	if (prereq1 == "none") { // if either column 3 or 4 is empty, comma is removed
		prereq1 = "";
		middleText = "";
	}

	if (prereq2 == "none") { // if either prerequisite = none (identifier in file), replace it with spaces
		prereq2 = "";
		middleText = "";
	}

	finalText = prereq1 + middleText + prereq2; // create text string
	return finalText;
}

void displayCourse(Course course) { // Prints a single course
	cout << course.courseId << ": " << course.title << " | Prerequisites: " << getPrerequisiteText(course.prerequisite1, course.prerequisite2) << endl;
	return;
}



HashTable::HashTable() {
	nodes.resize(tableSize); // Initialize node structure by resizing tableSize
}

HashTable::HashTable(unsigned int size) {
	this->tableSize = size; // invoke local tableSize to size with this->
	nodes.resize(size); // resize nodes size
}

HashTable::~HashTable() { // **Destructor**
	nodes.erase(nodes.begin()); // erase nodes beginning
}

unsigned int HashTable::hash(int key) {
	return key % tableSize; // return key tableSize
}

void HashTable::Insert(Course course) {

	unsigned key = hash(stoi(course.courseId));

	Node* prevNode = &(nodes.at(key)); //search for existing nodes

	if (prevNode == nullptr) { //insert node if previous node is empty
		Node* nextNode = new Node(course, key);
		nodes.insert(nodes.begin() + key, (*nextNode));
	}
	else { //else
		if (prevNode->key == UINT_MAX) {
			prevNode->key = key;
			prevNode->course = course;
			prevNode->next = nullptr;
		}
		else {
			while (prevNode->next != nullptr) {
				prevNode = prevNode->next;
			}
		}
	}
}

void HashTable::PrintAll() {
	for (unsigned int i = 0; i < tableSize; i++) { //loop until end

		Node* currNode = &nodes.at(i); //set pointer

		if (currNode->key != UINT_MAX) {
			i++; //increment
			cout << currNode->course.courseId << " | " << currNode->course.title
			<< " | " << getPrerequisiteText(currNode->course.prerequisite1, currNode->course.prerequisite2) << endl;

			while (currNode->next != nullptr) { //continue until empty
				i++; //increment
				currNode = currNode->next;
				cout << ": " << currNode->course.courseId << " | " << currNode->course.title
				<< " | " << getPrerequisiteText(currNode->course.prerequisite1, currNode->course.prerequisite2) << endl;
			}
		}
	}

	return; //return
}

void HashTable::Remove(string courseId) {
	unsigned int keyToRemove = hash(stoi(courseId)); //declare designated key
	nodes.erase(nodes.begin() + keyToRemove); //remove designated node
}

Course HashTable::Search(string courseId) {
    Course course;

    unsigned key = hash(atoi(courseId.c_str())); //set search key

    Node* node = &(nodes.at(key));

    if (node != nullptr && node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
    	return node->course; //if it matches, return said node
    }

    if (node == nullptr || node->key == UINT_MAX) {
    	return course; //if empty, return course
    }

    while (node != nullptr) { //while node isn't empty
    	if (node->key != UINT_MAX && node->course.courseId.compare(courseId) == 0) {
    		return node->course; //return said node
    	}
    }

    return course; //return
}

void displayCourse(Course course);

double strToDouble(string str, char ch) {
	str.erase(remove(str.begin(), str.end(), ch), str.end());
	return atof(str.c_str());
}



string getPrerequisiteText(string prereq1, string prereq2);

void loadCourses(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of courses
            Course course;
            course.courseId = file[i][0];
            course.title = file[i][1];
            course.prerequisite1 = file[i][2];
            course.prerequisite2 = file[i][3];

            hashTable->Insert(course); // add course to hash table
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
	// process command line arguments
	string csvPath;
	switch (argc) {
	case 2:
		csvPath = argv[1];
		break;
	case 3:
		csvPath = argv[1];
		break;
	default:
		csvPath = "U:/ABCU_Advising_Program_Input.csv";
	}

	// Define a hash table to hold all the bids
	HashTable* courseTable;

	Course course;
	courseTable = new HashTable();

	int choice = 0;
	string course_ID;

	while (choice != 9) {
		cout << "Menu:" << endl;
		cout << "  1. Load Courses" << endl;
		cout << "  2. Display All Courses" << endl;
		cout << "  3. Display Course" << endl;
		cout << "  9. Exit" << endl;
		cout << "Enter choice: ";
		cin >> choice;

		switch (choice) {

		case 1:
			loadCourses(csvPath, courseTable);
			break;

		case 2:
			courseTable->PrintAll();
			break;

		case 3:
			cout << "Enter course ID: ";
			cin >> course_ID;

			course = courseTable->Search(course_ID);

			if (!course.courseId.empty()) {
				displayCourse(course);
			}
			else {
				cout << "Course ID " << course_ID << " not found." << endl;
			}

			break;
		}
	}
	cout << "Good bye." << endl;

	return 0;
}
