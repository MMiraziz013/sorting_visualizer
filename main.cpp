/*
 * ============================================================================
 * PROJECT: Graphical Sorting Visualizer
 * ============================================================================
 * 
 * Authors: 
 *   - Miraziz Mirvaliyev
 *   - Ulugbek Suleymonov
 * 
 * Subject: Data Structures
 * 
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <iomanip>
#include <stack>
#include <queue>
#include <algorithm>
#include <sstream>
#include <limits>

// Windows-specific includes for UTF-8 console support
#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif

using namespace std;

const int MAX_ARRAY_SIZE = 20;
const int MIN_ARRAY_SIZE = 3;
const int MAX_VALUE = 99;
const int MIN_VALUE = 1;
const int ANIMATION_DELAY_MS = 500;
const int BAR_WIDTH = 4;

// ANSI Color codes for terminal visualization
namespace Color {
    const string RESET = "\033[0m";
    const string RED = "\033[91m";
    const string GREEN = "\033[92m";
    const string YELLOW = "\033[93m";
    const string BLUE = "\033[94m";
    const string MAGENTA = "\033[95m";
    const string CYAN = "\033[96m";
    const string WHITE = "\033[97m";
    const string BOLD = "\033[1m";
    const string BG_RED = "\033[41m";
    const string BG_GREEN = "\033[42m";
    const string BG_YELLOW = "\033[43m";
    const string BG_BLUE = "\033[44m";
}

namespace Key {
    const int UP = 1000;
    const int DOWN = 1001;
    const int ENTER = 1002;
    const int ESCAPE = 1003;
}

void printSeparator(int width, char ch);
void printCentered(const string& text, int width);
void drawBars(const vector<int>& arr, int highlight1, int highlight2,
              const string& highlightType, int sortedUntil);
void printArray(const vector<int>& arr, int highlight1, int highlight2, const string& highlightType);
void displaySplashScreen();
void displayExitScreen();
void displayFrame(const vector<int>& arr, int h1, int h2, 
                  const string& message, const string& highlightType,
                  int sortedUntil, int stepNum, int totalSteps);
vector<int> getArrayInput();
vector<int> generateRandomArray(int size);
void animationDelay(int ms);
void bubbleSortVisualized(vector<int>& arr);
int displayMainMenu();


// Initialize console
void initConsole() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    #endif
}

// Clear screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        cout << "\033[2J\033[1;1H";
    #endif
}

void printSeparator(int width = 60, char ch = '=') {
    cout << Color::CYAN;
    for (int i = 0; i < width; i++) cout << ch;
    cout << Color::RESET << endl;
}

void printCentered(const string& text, int width = 60) {
    int padding = (width - text.length()) / 2;
    if (padding > 0) {
        cout << string(padding, ' ');
    }
    cout << text << endl;
}

int getKeyPress() {
    #ifdef _WIN32
        int ch = _getch();
        if (ch == 0 || ch == 224) {  // Arrow key prefix on Windows
            ch = _getch();
            switch (ch) {
                case 72: return Key::UP;
                case 80: return Key::DOWN;
                default: return ch;
            }
        }
        if (ch == 13) return Key::ENTER;
        if (ch == 27) return Key::ESCAPE;
        return ch;
    #else
        struct termios oldt, newt;
        int ch;
        
        // Save terminal settings
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        
        // Disable canonical mode and echo
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        
        ch = getchar();
        
        if (ch == 27) {  // Escape sequence
            int next = getchar();
            if (next == '[') {
                int arrow = getchar();
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                switch (arrow) {
                    case 'A': return Key::UP;
                    case 'B': return Key::DOWN;
                    default: return arrow;
                }
            }
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return Key::ESCAPE;
        }
        
        // Restore terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        
        if (ch == 10 || ch == 13) return Key::ENTER;
        return ch;
    #endif
}


// Wait for keypress
void waitForKey() {
    #ifdef _WIN32
        _getch();
    #else
        cin.get();
    #endif
}

vector<int> getArrayInput() {
    clearScreen();
    
    cout << Color::CYAN;
    printSeparator(60);
    printCentered(Color::BOLD + "INPUT MODULE" + Color::RESET + Color::CYAN, 60);
    printSeparator(60);
    cout << Color::RESET << endl;
    
    int size;
    cout << "    Enter array size (" << MIN_ARRAY_SIZE << "-" << MAX_ARRAY_SIZE << "): ";
    cin >> size;
    
    while (cin.fail() || size < MIN_ARRAY_SIZE || size > MAX_ARRAY_SIZE) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << Color::RED << "    Invalid! Enter size between " << MIN_ARRAY_SIZE 
             << " and " << MAX_ARRAY_SIZE << ": " << Color::RESET;
        cin >> size;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    // Interactive method selection
    int selectedMethod = 0;
    string methods[] = {"Generate random values", "Enter values manually"};
    
    while (true) {
        clearScreen();
        
        cout << Color::CYAN;
        printSeparator(60);
        printCentered(Color::BOLD + "INPUT MODULE" + Color::RESET + Color::CYAN, 60);
        printSeparator(60);
        cout << Color::RESET << endl;
        
        cout << "    Array size: " << Color::GREEN << size << Color::RESET << endl << endl;
        cout << "    How would you like to enter the array?" << endl << endl;
        
        for (int i = 0; i < 2; i++) {
            cout << "    ";
            if (i == selectedMethod) {
                cout << Color::BG_BLUE << Color::WHITE << Color::BOLD;
                cout << " >> " << methods[i] << " ";
                cout << Color::RESET;
            } else {
                cout << "    " << methods[i];
            }
            cout << endl << endl;
        }
        
        cout << endl;
        cout << "    " << Color::YELLOW << "Use [UP]/[DOWN] arrows to navigate, [ENTER] to select" << Color::RESET << endl;
        
        int key = getKeyPress();
        
        if (key == Key::UP || key == Key::DOWN) {
            selectedMethod = 1 - selectedMethod;  // Toggle between 0 and 1
        } else if (key == Key::ENTER) {
            break;
        }
    }
    
    vector<int> arr;
    
    if (selectedMethod == 0) {
        arr = generateRandomArray(size);
        clearScreen();
        cout << Color::CYAN;
        printSeparator(60);
        printCentered(Color::BOLD + "INPUT MODULE" + Color::RESET + Color::CYAN, 60);
        printSeparator(60);
        cout << Color::RESET << endl;
        
        cout << "    " << Color::GREEN << "Generated random array:" << Color::RESET << endl << endl;
        cout << "    ";
        for (int val : arr) cout << val << " ";
        cout << endl;
    } else {
        arr.resize(size);
        clearScreen();
        cout << Color::CYAN;
        printSeparator(60);
        printCentered(Color::BOLD + "INPUT MODULE" + Color::RESET + Color::CYAN, 60);
        printSeparator(60);
        cout << Color::RESET << endl;
        
        cout << "    Enter " << size << " values (" << MIN_VALUE << "-" << MAX_VALUE << "):" << endl << endl;
        for (int i = 0; i < size; i++) {
            cout << "    arr[" << i << "] = ";
            cin >> arr[i];
            
            while (cin.fail() || arr[i] < MIN_VALUE || arr[i] > MAX_VALUE) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << Color::RED << "    Invalid! Enter value between " << MIN_VALUE 
                     << " and " << MAX_VALUE << ": " << Color::RESET;
                cin >> arr[i];
            }
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    cout << endl << "    " << Color::YELLOW << "Press any key to start visualization..." << Color::RESET;
    getKeyPress();
    
    return arr;
}

vector<int> generateRandomArray(int size) {
    vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        arr[i] = MIN_VALUE + rand() % (MAX_VALUE - MIN_VALUE + 1);
    }
    return arr;
}

void animationDelay(int ms = ANIMATION_DELAY_MS) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void drawBars(const vector<int>& arr, int highlight1 = -1, int highlight2 = -1,
              const string& highlightType = "compare", int sortedUntil = -1) {
    
    // Find maximum value for scaling
    int maxVal = *max_element(arr.begin(), arr.end());
    int maxHeight = 15;  // Maximum bar height
    
    // Use ASCII characters for maximum compatibility
    const char BAR_CHAR = '#';      // Bar fill character
    const char BASE_CHAR = '-';     // Baseline character
    
    cout << endl;
    
    // Draw bars from top to bottom
    for (int level = maxHeight; level >= 1; level--) {
        cout << "  ";
        for (size_t i = 0; i < arr.size(); i++) {
            int barHeight = (arr[i] * maxHeight) / maxVal;
            
            if (barHeight >= level) {
                // Determine bar color
                string barColor = Color::WHITE;
                if ((int)i == highlight1 || (int)i == highlight2) {
                    if (highlightType == "compare") {
                        barColor = Color::YELLOW;
                    } else if (highlightType == "swap") {
                        barColor = Color::RED;
                    } else if (highlightType == "pivot") {
                        barColor = Color::MAGENTA;
                    }
                } else if (sortedUntil >= 0 && (int)i >= sortedUntil) {
                    barColor = Color::GREEN;
                }
                
                cout << barColor;
                for (int w = 0; w < BAR_WIDTH; w++) cout << BAR_CHAR;
                cout << Color::RESET;
            } else {
                for (int w = 0; w < BAR_WIDTH; w++) cout << " ";
            }
            cout << " ";
        }
        cout << endl;
    }
    
    // Draw baseline
    cout << "  ";
    for (size_t i = 0; i < arr.size(); i++) {
        for (int w = 0; w < BAR_WIDTH; w++) cout << BASE_CHAR;
        cout << " ";
    }
    cout << endl;
    
    // Draw values below bars
    cout << "  ";
    for (size_t i = 0; i < arr.size(); i++) {
        string valStr = to_string(arr[i]);
        int padding = (BAR_WIDTH - valStr.length()) / 2;
        
        // Color based on state
        if ((int)i == highlight1 || (int)i == highlight2) {
            if (highlightType == "compare") {
                cout << Color::YELLOW << Color::BOLD;
            } else if (highlightType == "swap") {
                cout << Color::RED << Color::BOLD;
            } else if (highlightType == "pivot") {
                cout << Color::MAGENTA << Color::BOLD;
            }
        } else if (sortedUntil >= 0 && (int)i >= sortedUntil) {
            cout << Color::GREEN;
        }
        
        cout << string(padding, ' ') << valStr 
             << string(BAR_WIDTH - padding - valStr.length(), ' ') << Color::RESET << " ";
    }
    cout << endl;
    
    // Draw indices
    cout << Color::CYAN << "  ";
    for (size_t i = 0; i < arr.size(); i++) {
        string idxStr = "[" + to_string(i) + "]";
        int padding = (BAR_WIDTH - idxStr.length()) / 2;
        cout << string(padding, ' ') << idxStr 
             << string(BAR_WIDTH - padding - idxStr.length(), ' ') << " ";
    }
    cout << Color::RESET << endl << endl;
}

void printArray(const vector<int>& arr, int highlight1 = -1, int highlight2 = -1, 
                const string& highlightType = "compare") {
    cout << "  Array: [ ";
    for (size_t i = 0; i < arr.size(); i++) {
        if ((int)i == highlight1 || (int)i == highlight2) {
            if (highlightType == "compare") {
                cout << Color::BG_YELLOW << Color::BOLD;
            } else if (highlightType == "swap") {
                cout << Color::BG_RED << Color::BOLD;
            } else if (highlightType == "sorted") {
                cout << Color::BG_GREEN << Color::BOLD;
            } else if (highlightType == "pivot") {
                cout << Color::BG_BLUE << Color::BOLD;
            }
        }
        cout << setw(3) << arr[i] << Color::RESET << " ";
    }
    cout << "]" << endl;
}

// Splash Screen
void displaySplashScreen() {
    clearScreen();
    
    cout << Color::CYAN;
    cout << R"(
    +==============================================================+
    |                                                              |
    |   ____   ___  ____ _____ ___ _   _  ____                     |
    |  / ___| / _ \|  _ \_   _|_ _| \ | |/ ___|                    |
    |  \___ \| | | | |_) || |  | ||  \| | |  _                     |
    |   ___) | |_| |  _ < | |  | || |\  | |_| |                    |
    |  |____/ \___/|_| \_\|_| |___|_| \_|\____|                    |
    |                                                              |
    |  __     _____ ____  _   _   _    _     ___ _________ ____    |
    |  \ \   / /_ _/ ___|| | | | / \  | |   |_ _|__  / ____|  _ \  |
    |   \ \ / / | |\___ \| | | |/ _ \ | |    | |  / /|  _| | |_) | |
    |    \ V /  | | ___) | |_| / ___ \| |___ | | / /_| |___|  _ <  |
    |     \_/  |___|____/ \___/_/   \_\_____|___/____|_____|_| \_\ |
    |                                                              |
    +==============================================================+
    |                                                              |
    |              GRAPHICAL SORTING VISUALIZER                    |
    |                                                              |
    |   Algorithms: Bubble Sort | Insertion Sort | Quick Sort      |
    |                                                              |
    +==============================================================+
    |                                                              |
    |   Authors:                                                   |
    |     * Miraziz  Mirvaliyev                                    |
    |     * Ulugbek  Suleymonov                                    |
    |                                                              |
    |   Subject: Data Structures                                   |
    |                                                              |
    +==============================================================+
    )" << Color::RESET;
    
    cout << endl;
    cout << Color::YELLOW << "    Press any key to continue..." << Color::RESET;
    waitForKey();
}

// Exit Screen
void displayExitScreen() {
    clearScreen();
    
    cout << Color::CYAN;
    cout << R"(
    +==============================================================+
    |                                                              |
    |                    THANK YOU FOR USING                       |
    |                                                              |
    |             GRAPHICAL SORTING VISUALIZER                     |
    |                                                              |
    +==============================================================+
    |                                                              |
    |   Project by: Miraziz & Ulugbek                              |
    |   Subject: Data Structures                                   |
    |                                                              |
    +==============================================================+
    )" << Color::RESET << endl;
}

void displayFrame(const vector<int>& arr, int h1, int h2, 
                  const string& message, const string& highlightType,
                  int sortedUntil = -1, int stepNum = 0, int totalSteps = 0) {
    clearScreen();
    
    printSeparator(60);
    printCentered(Color::BOLD + "SORTING VISUALIZATION" + Color::RESET, 60);
    printSeparator(60);
    
    if (totalSteps > 0) {
        cout << Color::CYAN << "  Step " << stepNum << " of " << totalSteps << Color::RESET << endl;
    }
    
    cout << endl << "  " << Color::YELLOW << message << Color::RESET << endl;
    
    drawBars(arr, h1, h2, highlightType, sortedUntil);
    printArray(arr, h1, h2, highlightType);
    
    cout << endl;
    printSeparator(60, '-');
    cout << "  " << Color::CYAN << "Legend: ";
    cout << Color::YELLOW << "# Comparing  ";
    cout << Color::RED << "# Swapping  ";
    cout << Color::GREEN << "# Sorted  ";
    cout << Color::MAGENTA << "# Pivot" << Color::RESET << endl;
    printSeparator(60, '-');
}

void bubbleSortVisualized(vector<int>& arr) {
    int n = arr.size();
    int totalComparisons = 0;
    int totalSwaps = 0;
    int stepCount = 0;
    
    // Display initial state
    displayFrame(arr, -1, -1, "Initial array - Starting Bubble Sort", "compare", n);
    animationDelay(1000);
    
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        
        for (int j = 0; j < n - i - 1; j++) {
            stepCount++;
            
            // Show comparison
            string msg = "Pass " + to_string(i + 1) + ": Comparing arr[" + 
                        to_string(j) + "]=" + to_string(arr[j]) + 
                        " with arr[" + to_string(j + 1) + "]=" + to_string(arr[j + 1]);
            displayFrame(arr, j, j + 1, msg, "compare", n - i);
            totalComparisons++;
            animationDelay();
            
            if (arr[j] > arr[j + 1]) {
                // Show swap
                msg = "Swapping " + to_string(arr[j]) + " and " + to_string(arr[j + 1]);
                displayFrame(arr, j, j + 1, msg, "swap", n - i);
                
                swap(arr[j], arr[j + 1]);
                swapped = true;
                totalSwaps++;
                animationDelay();
            }
        }
        
        // Show element is now sorted
        string msg = "Element " + to_string(arr[n - i - 1]) + " is now in its correct position";
        displayFrame(arr, n - i - 1, -1, msg, "sorted", n - i - 1);
        animationDelay(800);
        
        // Early termination if no swaps
        if (!swapped) {
            displayFrame(arr, -1, -1, "No swaps in this pass - Array is sorted!", "sorted", 0);
            animationDelay(1000);
            break;
        }
    }
    
    // Final display
    displayFrame(arr, -1, -1, "Bubble Sort Complete!", "sorted", 0);
    cout << endl;
    cout << "  " << Color::GREEN << "Statistics:" << Color::RESET << endl;
    cout << "  - Total Comparisons: " << totalComparisons << endl;
    cout << "  - Total Swaps: " << totalSwaps << endl;
    cout << "  - Total Steps: " << stepCount << endl;
}

int partitionVisualized(vector<int>& arr, int low, int high, 
                        int& totalComparisons, int& totalSwaps) {
    int pivot = arr[high];
    
    string msg = "Selecting pivot = " + to_string(pivot) + " at index " + to_string(high);
    displayFrame(arr, high, -1, msg, "pivot");
    animationDelay(800);
    
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        totalComparisons++;
        
        msg = "Comparing arr[" + to_string(j) + "]=" + to_string(arr[j]) + 
              " with pivot=" + to_string(pivot);
        displayFrame(arr, j, high, msg, "compare");
        animationDelay();
        
        if (arr[j] <= pivot) {
            i++;
            if (i != j) {
                msg = "Swapping arr[" + to_string(i) + "]=" + to_string(arr[i]) + 
                      " with arr[" + to_string(j) + "]=" + to_string(arr[j]);
                displayFrame(arr, i, j, msg, "swap");
                swap(arr[i], arr[j]);
                totalSwaps++;
                animationDelay();
            }
        }
    }
    
    // place pivot in correct position
    if (i + 1 != high) {
        msg = "Placing pivot " + to_string(pivot) + " at its correct position " + to_string(i + 1);
        displayFrame(arr, i + 1, high, msg, "swap");
        swap(arr[i + 1], arr[high]);
        totalSwaps++;
        animationDelay();
    }
    
    msg = "Pivot " + to_string(arr[i + 1]) + " is now at correct position " + to_string(i + 1);
    displayFrame(arr, i + 1, -1, msg, "sorted");
    animationDelay(800);
    
    return i + 1;
}

void quickSortVisualized(vector<int>& arr) {
    int n = arr.size();
    int totalComparisons = 0;
    int totalSwaps = 0;
    
    displayFrame(arr, -1, -1, "Initial array - Starting Quick Sort (Iterative with Stack)", "compare");
    animationDelay(1000);
    
    stack<pair<int, int>> stk;
    
    stk.push({0, n - 1});
    
    cout << "  " << Color::CYAN << "Using Stack to track subarrays to sort" << Color::RESET << endl;
    animationDelay(500);
    
    while (!stk.empty()) {
        pair<int, int> range = stk.top();
        stk.pop();
        
        int low = range.first;
        int high = range.second;
        
        if (low < high) {
            string msg = "Processing subarray from index " + to_string(low) + 
                        " to " + to_string(high);
            displayFrame(arr, low, high, msg, "compare");
            animationDelay(600);
            
            int pi = partitionVisualized(arr, low, high, totalComparisons, totalSwaps);
            
            // push subarrays to stack
            if (pi - 1 > low) {
                stk.push({low, pi - 1});
            }
            if (pi + 1 < high) {
                stk.push({pi + 1, high});
            }
        }
    }
    
    displayFrame(arr, -1, -1, "Quick Sort Complete!", "sorted", 0);
    cout << endl;
    cout << "  " << Color::GREEN << "Statistics:" << Color::RESET << endl;
    cout << "  - Total Comparisons: " << totalComparisons << endl;
    cout << "  - Total Swaps: " << totalSwaps << endl;
}

int displayMainMenu() {
    int selectedOption = 0;
    const int numOptions = 4;
    string options[] = {
        "Bubble Sort Visualization",
        "Insertion Sort Visualization", 
        "Quick Sort Visualization",
        "Exit"
    };
    
    while (true) {
        clearScreen();
        
        cout << Color::CYAN;
        printSeparator(60);
        printCentered(Color::BOLD + "MAIN MENU" + Color::RESET + Color::CYAN, 60);
        printSeparator(60);
        cout << Color::RESET << endl;
        
        cout << "    " << Color::CYAN << "+========================================+" << Color::RESET << endl;
        cout << "    " << Color::CYAN << "|" << Color::RESET << "                                        " << Color::CYAN << "|" << Color::RESET << endl;
        
        for (int i = 0; i < numOptions; i++) {
            cout << "    " << Color::CYAN << "|" << Color::RESET << "   ";
            
            if (i == selectedOption) {
                // Highlighted option
                cout << Color::BG_BLUE << Color::WHITE << Color::BOLD;
                cout << " >> " << options[i];
                // Pad to align
                int padding = 32 - options[i].length();
                for (int p = 0; p < padding; p++) cout << " ";
                cout << Color::RESET;
            } else {
                // Normal option
                if (i == 3) {
                    cout << Color::RED;  // Exit in red
                } else {
                    cout << Color::WHITE;
                }
                cout << "    " << options[i];
                int padding = 32 - options[i].length();
                for (int p = 0; p < padding; p++) cout << " ";
                cout << Color::RESET;
            }
            
            cout << Color::CYAN << "|" << Color::RESET << endl;
            cout << "    " << Color::CYAN << "|" << Color::RESET << "                                        " << Color::CYAN << "|" << Color::RESET << endl;
        }
        
        cout << "    " << Color::CYAN << "+========================================+" << Color::RESET << endl;
        
        cout << endl;
        cout << "    " << Color::YELLOW << "Use [UP]/[DOWN] arrows to navigate" << Color::RESET << endl;
        cout << "    " << Color::YELLOW << "Press [ENTER] to select" << Color::RESET << endl;
        
        // Get key press
        int key = getKeyPress();
        
        if (key == Key::UP) {
            selectedOption = (selectedOption - 1 + numOptions) % numOptions;
        } else if (key == Key::DOWN) {
            selectedOption = (selectedOption + 1) % numOptions;
        } else if (key == Key::ENTER) {
            return selectedOption + 1;  // Return 1-4
        } else if (key == Key::ESCAPE) {
            return 4;  // Exit
        } else if (key >= '1' && key <= '4') {
            return key - '0';  // Also allow number keys
        }
    }
}


int main() {
    initConsole();

    
    // TODO: Add sorting algorithms
    // TODO: Add visualization


    displaySplashScreen();

    bool running = true;

    while (running) {
        int choice = displayMainMenu();
        
        switch (choice) {
            case 1: {
                // Bubble Sort
                vector<int> arr = getArrayInput();
                bubbleSortVisualized(arr);
                cout << endl << "    " << Color::YELLOW << "Press any key to return to menu..." << Color::RESET;
                getKeyPress();
                break;
            }
            case 2: {
                // Insertion Sort
                break;
            }
            case 3: {
                // Quick Sort
                break;
            }
            case 4: {
                // Exit
                running = false;
                displayExitScreen();
                break;
            }
        }
    }
    
    return 0;
}