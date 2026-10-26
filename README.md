# CS 300 Portfolio: Data Structures and Algorithms

**Fernando Rivas | Southern New Hampshire University**

This repository contains my ABCU Advising Assistance Program and the analysis that guided its design. The projects focus on choosing a data structure, loading course information from a CSV file, and helping academic advisors find and display courses.

## Portfolio Artifacts

| File | Description |
| --- | --- |
| [ProjectOne_Runtime_and_Memory_Analysis.docx](ProjectOne_Runtime_and_Memory_Analysis.docx) | Project One pseudocode and comparison of runtime, memory use, and tradeoffs for a vector, hash table, and binary search tree. |
| [ProjectTwo.cpp](ProjectTwo.cpp) | Working C++ program that loads courses, prints them in alphanumeric order, and displays course details and prerequisites. |
| [ABCU_Advising_Program_Input.csv](ABCU_Advising_Program_Input.csv) | Sample input containing eight courses and their prerequisites. |

## What was the problem you were solving in the projects for this course?

The problem was creating a program that would help ABCU academic advisors access information about computer science courses. The program needed to load course data, print the courses in alphanumeric order, and show the title and prerequisites for a selected course. In Project One, I compared the runtime and memory requirements of vectors, hash tables, and binary search trees. In Project Two, I used that analysis to build the advising program in C++.

## How did you approach the problem? Consider why data structures are important to understand.

I started by breaking the requirements into file loading, course storage, searching, and displaying information. I then wrote pseudocode and compared how each data structure would handle those operations. I chose a binary search tree because an in-order traversal prints the courses in order without a separate sorting step. Understanding data structures helped me consider both performance and memory use instead of choosing a structure only because it was familiar. I also learned to consider worst-case behavior, since an unbalanced tree can have slow searches and insertions when the input produces a skewed tree.

## How did you overcome any roadblocks you encountered while going through the activities or project?

The parts that required the most care were processing the CSV file and keeping prerequisite information consistent. I handled these tasks in separate functions so I could focus on parsing, validation, and tree operations individually. The program checks for duplicate course numbers and validates prerequisite references after all courses have been loaded, since a prerequisite might appear later in the file. It also loads records into a temporary tree before replacing the active data. This keeps a failed file load from erasing courses that were already available.

## How has your work on this project expanded your approach to designing software and developing programs?

This project encouraged me to think about how a program should work before starting the implementation. Writing pseudocode helped me connect the requirements to specific operations, and the runtime analysis showed how those choices could affect performance as the number of courses grows. I now pay more attention to the operations a program will perform most often and to how invalid input should be handled. I also see the value of considering tradeoffs early, because a structure that is useful for fast lookups might still require extra work to produce sorted output.

## How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?

I put more thought into descriptive names and functions with clear responsibilities. The program separates the course record, tree operations, CSV parsing, file loading, and menu behavior, which makes it easier to understand where a change belongs. Comments explain decisions such as validating prerequisites after loading and deleting tree nodes to release memory. This organization would make it easier to change the input handling or add another menu option without rewriting the entire program.

## Build and Run

With a C++17-capable compiler, run these commands from the repository folder:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic ProjectTwo.cpp -o CoursePlanner
./CoursePlanner
```

On Windows, run the resulting executable as `CoursePlanner.exe`. The source can also be built as a C++ console application in Visual Studio.

1. Select **1** and enter `ABCU_Advising_Program_Input.csv`, or enter its full path.
2. Select **2** to print all courses in alphanumeric order.
3. Select **3** and enter a course number, such as `CSCI400`, to display its title and prerequisites.
4. Select **9** to exit.

The CSV format is `course number,title,prerequisite number,...`, with one course per line and no header row. Prerequisites are optional.

## Verification

The program compiled with C++17 and the warning flags shown above. Checks with the provided input confirmed that all eight courses print in alphanumeric order, course details include prerequisite titles, and lowercase course queries work. Additional checks confirmed that the program asks users to load data before viewing courses, reports an unavailable course, and preserves loaded data when a subsequent file cannot be opened.
