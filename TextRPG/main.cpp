// Authors: Cade Haley, Mitch Block
// Date: Apr. 19, 2015
// Description: A text-based game that makes use of graphs to control the map

#include <iostream>
#include "GameGraph.h"
#include <string>

using namespace std;

void displayMenu();
void subMenu1();

int main(int argc, const char * argv[]) {
    GameGraph g; // Establish GameGraph object
    int input;
    char letter_input;
    bool quit = false;

    while (!quit){ // Main menu
        displayMenu();
        cin >> input;
        cin.clear();
        cin.ignore(10000,'\n');
        switch(input){
            case 1: //
                cout << "Go to submenu? y/n ";
                cin >> letter_input;
                switch(letter_input){
                    case 'y':
                        letter_input = '0';
                        while (letter_input != 'y'){
                            cout << "Success" << endl;
                            subMenu1();
                            cout << "Go back? y/n " << endl;
                            cin >> letter_input;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case 2: //

                break;
            case 3: //

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
    cout << "1. Go to submenu 1" << endl;
    cout << "2. Function 2" << endl;
    cout << "3. Function 3" << endl;
    cout << "4. Function 4" << endl;
    cout << "5. Quit" << endl;
}

void subMenu1(){
    cout << "======Sub Menu 1======" << endl;
    cout << "Test" << endl;
}
