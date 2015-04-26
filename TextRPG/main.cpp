// Authors: Cade Haley, Mitch Block
// Date: Apr. 19, 2015
// Description: A text-based game that makes use of graphs to control the map

#include <cstdlib>
#include <iostream>
#include "GameGraph.h"
#include <string>

using namespace std;

void clearscreen();
void displayMenu();
void subMenu1();

int main(int argc, const char * argv[]) {
    if (argc != 2){
        cout << "Please input the map file name as the argument." << endl;
        exit(1);
    }

    GameGraph g; // Establish GameGraph object
    string filename = argv[1];
    string userInput;
    char letter_input;
    bool graph_built = false;
    bool quit = false;

    while (!quit){ // Main menu
        clearscreen();
        displayMenu();
        getline(cin, userInput);
        switch(stoi(userInput)){
            case 1: // Build the map
                if (!graph_built){
                    clearscreen();
                    // Run member function that builds the map
                    g.readAndBuild(filename);
                    graph_built = true;
                    cout << "\nPress any key to continue " << endl;
                    cin.get();
                }
                else{
                    cout << "Map already built. ENTER to continue" << endl;
                    cin.get();
                }
                break;
            case 2: // Play the game
                if (graph_built){
                    g.quit = false;
                    cout << "Type 'q' to quit at any time\n" << endl;
                    // Play the game
                    while (!g.quit){
                    g.lookAtMap();
                    clearscreen();
                    }
                }
                else{
                    cout << "Please build the map first. ENTER to continue" << endl;
                    cin.get();
                }

                break;
            case 3: //
                cout << "Go to submenu? y/n ";
                cin >> letter_input;
                switch(letter_input){
                    case 'y':
                        letter_input = '0';
                        while (letter_input != 'y'){
                            clearscreen();
                            cout << "Success" << endl;
                            subMenu1();
                            cout << "Go back? y/n " << endl;
                            cin >> letter_input;
                        }
                        cin.ignore(10000, '\n');
                        break;
                    default:
                        break;
                }
                break;
            case 4: //

                break;

            case 5: // Quit
                quit = true;
                break;
            default:
                cout << "Invalid Input" << endl;
                cin.clear();
                break;

        }
    }
    return 0;
}

void displayMenu(){
    cout << "======Main Menu======" << endl;
    cout << "1. Build game map" << endl;
    cout << "2. Play game" << endl;
    cout << "3. Go to submenu" << endl;
    cout << "4. Function 4" << endl;
    cout << "5. Quit" << endl;
}

void clearscreen()
{
#ifdef WINDOWS
    std::system("cls");
#else
    // Assume POSIX
    std::system ("clear");
#endif
}

void subMenu1(){
    cout << "======Sub Menu 1======" << endl;
    cout << "Test" << endl;
}
