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
#include <string>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#endif

using namespace std;

// ANSI Color codes
namespace Color {
    const string RESET = "\033[0m";
    const string CYAN = "\033[96m";
    const string YELLOW = "\033[93m";
}

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

// Wait for keypress
void waitForKey() {
    #ifdef _WIN32
        _getch();
    #else
        cin.get();
    #endif
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

int main() {
    initConsole();
    
    displaySplashScreen();
    
    // TODO: Add main menu
    // TODO: Add sorting algorithms
    // TODO: Add visualization
    
    displayExitScreen();
    
    return 0;
}