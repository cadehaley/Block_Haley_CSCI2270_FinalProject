// Authors: Cade Haley, Mitch Block
// Date: Apr. 19, 2015
// Description: A text-based game that makes use of graphs to control the map

#include <cstdlib>
#include <iostream>
#include "GameGraph.h"
#include <string>
#include <sstream>

using namespace std;

void clearscreen();
void displayMenu();
void subMenu1(GameGraph * g);

int main(int argc, const char * argv[]) {
    if (argc != 2){
        cout << "Please input the map file name as the argument." << endl;
        exit(1);
    }

    GameGraph g; // Establish GameGraph object
    string filename = argv[1];
    string userInput;
    char letter_input;
    int numberized_input;
    bool graph_built = false;
    bool quit = false;
    bool menu_back = false;

    while (!quit){ // Main menu
        menu_back = false;
        clearscreen();
        displayMenu();
        getline(cin, userInput);
        stringstream one(userInput);
        if (!(one >> numberized_input)){ numberized_input = 999;} // Using string-to-int stringstream method
        switch(numberized_input){ // Note: Don't use stoi or else it will core dump when user input has letters (isn't valid).
            case 1: // Build the map
                if (!graph_built){
                    clearscreen();
                    // Run member function that builds the map
                    g.readAndBuild(filename);
                    graph_built = true;
                    cout << "\nPress ENTER to continue " << endl;
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
                    cout << "\nA wise man once said, '" << g.skill_threshold << " skill points be the mark of a true warrior. Else what is he more than a noob awaiting defeat?'\n" << endl;
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
            case 3: // Options menu
                while (!menu_back){
                    clearscreen();
                    subMenu1(&g);
                    getline(cin, userInput);
                    // Convert input to int
                    stringstream one(userInput);
                    if (!(one >> numberized_input)){ numberized_input = 999;}
                    switch (numberized_input){
                        case 1:
                            cout << "Set the boss room to be randomly assigned? y/n " << endl;
                            //cin.clear();
                            getline(cin, userInput);
                            if (userInput == "y"){
                                g.boss_room_random = true;
                                cout << "Boss room set to random. ENTER to continue" << endl;
                                cin.get();
                                break;
                            }
                            else{
                                cout << "Boss room not random. ENTER to continue" << endl;
                                cin.get();
                            }
                            break;
                        case 2:
                            cout << "Enter skill threshold: " << endl;
                            one.str( std::string() );
                            one.clear();
                            getline(cin, userInput);
                            one << userInput;
                            one.clear();
                            if (!(one >> numberized_input)){
                                    cout << "Invalid. Setting to 200. ENTER to continue" << endl;
                                    numberized_input = 200;
                                    cin.get();
                            }
                            g.skill_threshold = numberized_input;
                            break;
                        case 3:
                            cout << "Enter time limit: " << endl;
                            one.str( std::string() );
                            one.clear();
                            getline(cin, userInput);
                            one << userInput;
                            if (!(one >> numberized_input)){
                                    cout << "Invalid. Setting to 20. ENTER to continue" << endl;
                                    numberized_input = 20;
                                    cin.get();
                            }
                            g.timeleft = numberized_input;
                            break;
                        case 4:
                            menu_back = true;
                            break;
                        default:
                            cout << "Invalid Input" << endl;
                            cin.get();
                            break;
                    }
                }
                break;
            case 4: //

                break;

            case 5: // Quit
                quit = true;
                break;
            default:
                cout << "Invalid Input. ENTER to continue" << endl;
                cin.get();
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
    cout << "3. Go to options" << endl;
    cout << "4. Function 4 (Empty)" << endl;
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

void subMenu1(GameGraph * g){
    cout << "======Options Menu======" << endl;
    cout << "1. Set boss room to be randomly attached" << endl;
    cout << "2. Required skills to defeat boss: " << g->skill_threshold << endl;
    cout << "3. Time limit (in extralunar miridas cycles): " << g->timeleft << endl;
    cout << "4. Go back" << endl;
}
