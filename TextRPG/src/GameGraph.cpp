// Authors: Cade Haley, Mitch Block
// Date: Apr. 19, 2015
// Description: A text-based game that makes use of graphs to control the map

#include "GameGraph.h"
#include <iostream>
#include <queue>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <random>
#include <time.h>

using namespace std;

GameGraph::GameGraph()
{
    //ctor
    skill = 0;
    currentLocation = NULL;
    quit = false;

    timeleft = 20; // Time limit for room traversals
    skill_threshold = 200; // The skill level that must be reached in order to defeat the boss

    boss_room_random = false;
    bossroom = NULL;
    hasBuilt = false;
    ending_called = false;
}

GameGraph::~GameGraph()
{
    delete currentLocation;
    areas.clear();
}

/*
    void readAndBuild(string filename);
    Method reads in a map file and calls addArea, addPathToArea, and connectRemainingEdges to assemble the graph from a properly formatted text file.
    ex. readAndBuild(mapfile.txt)
    Pre-conditions: Instance of graph created and a proper map file has been made
    Post-conditions: All edges are added along with respective vertices and all information
    Dependencies: addArea, addPathToArea, connectRemainingEdges
*/
void GameGraph::readAndBuild(string filename)
{
    ifstream infile;
    infile.open(filename.c_str());
    string line;

    bool duplicate_found = false;
    int numberize = 0;
    // Infile Modes, for telling if a line is
    bool adding_adjacent = false;
    bool adjacent_added = false;
    int rowmode = 0;
    int adjacentmode = 0;
    string adjTitle;
    string adjDesc;
    string adjKey;

    if (infile.is_open())
    {
        while ( getline (infile, line))
        {
            rowmode++; // Increment the row mode
            stringstream one(line); // Establish for case 3
            if (line == "@@" || line[0] == '#' || line == "")  // Reset the action to 0 (do nothing) if ** or # is used
            {
                rowmode = 0;
                duplicate_found = false;
            }
            if (line == "&&")
            {
                rowmode = -100; //Reset rowmode
                adjacentmode = 0;
                adding_adjacent = true; // Go into "loop" that adds adjacent verts
            }
            switch (rowmode)
            {
            case 1: // For the first line, create the vertex
                duplicate_found = addArea(line);
                if (duplicate_found) // If there's already an existing vertex, skip this vertex entry entirely
                    rowmode = -100;
                else
                    cout << line << " created" << endl;
                break;
            case 2: // For the second line, associate the description
                areas[areas.size()-1]->description = line;
                break;
            case 3: // 3rd line, convert to int, associate skill points
                if (!(one >> numberize))
                {
                    numberize = 999;   // Convert the line to a number like this (using sstream), because "stoi" tends to be finnicky
                }
                areas[areas.size()-1]->skill = numberize;
                break;
            case 4: // 4th line, associate an action for a newly visited room
                areas[areas.size()-1]->action = line;
                break;
            case 5: // 5th line, room type
                if (line == "s") // If it's the starting room, set the current room to this vert
                    currentLocation = areas[areas.size()-1];
                if (line == "b")
                    bossroom = areas[areas.size()-1]; // If it's the boss room, save this vertex for later
                break;
            // Otherwise, just a regular room
            default:
                break;
            }
            if (adding_adjacent && !duplicate_found)  // If you ran across && and it's not a duplicate (that's supposed to be skipped)
            {
                switch (adjacentmode)
                {
                case 1: // Create variables before loading into function
                    adjTitle = line;
                    break;
                case 2:
                    adjDesc = line;
                    break;
                case 3:
                    adjKey = line;
                    cout << "Adding adjacent -----" << endl;
                    //cout << areas[areas.size()-1]->title << " + " << adjTitle << " + " << adjDesc << " + " << adjKey << endl;
                    adjacent_added = addPathToArea(areas[areas.size()-1]->title, adjTitle, adjDesc, adjKey);
                    if (!adjacent_added)
                    {
                        cout << "   Destination non-existent, adding path later" << endl;
                        // If no ending area exists yet (or at all), add to "unconnected"
                        placeholder orphan;
                        orphan.starting = areas[areas.size()-1]->title;
                        orphan.destination = adjTitle;
                        orphan.description = adjDesc;
                        orphan.key = adjKey;
                        unconnected.push_back(orphan);
                    }
                    adding_adjacent = false;
                    break;
                default:
                    break;
                }
                adjacentmode++;
            }
        }
        infile.close();
        cout << " -- Connecting remaining paths -- " << endl;
        connectRemainingPaths(); // After all vertices have been made, connect the rest of the edges
        cout << " ...done\n\n";
        if (currentLocation == NULL)
        {
            cout << "Warning: No starting room found in file" << endl;
            currentLocation = areas[0];
        }
        else
            cout << "Starting room:  " << currentLocation->title << endl;

        // Attach the boss room
        if (bossroom != NULL && boss_room_random)
        {
            cout <<"    ... Assigning boss room RANDOMLY..." << endl;
            int randindex = rand() % areas.size();
            string boss_desc = "Your final enemy is very near, at the " + areas[randindex]->title + ". If you are ready to fight, type 'boss'";
            string boss_key = "boss";
            addPathToArea(areas[randindex]->title, bossroom->title, boss_desc, boss_key);
        }
        if (bossroom == NULL)
        {
            cout << "Warning: No boss room found in file" << endl;
            bossroom = areas[areas.size()-1];
        }
        hasBuilt = true;
    }
    else
    {
        cout << "File not open" << endl;
    }

}



void GameGraph::reBuildMap(string fileName)
{

    if(!hasBuilt)
    {
        cout << "Build map first" << endl;
        cout << "\nPress any key to continue" << endl;
        cin.get();
        return;
    }

    //
    timeleft = 20; // Time limit for room traversals
    skill = 0;
    boss_room_random = false;
    bossroom = NULL;
    hasBuilt = false;
    ending_called = false;

    areas.clear(); //Clears Areas so we can rebuild
    unconnected.clear();
    readAndBuild(fileName);

    cout << "Map Reset" << endl;
    cout << "\nPress any key to continue" << endl;
    cin.get();
}


/*
    bool addArea(string name), returns true if an existing vertex is already found
    Method adds an area to the graph, using the string passed in as the name.
    ex. addArea("Forest")
    Pre-conditions: instance of graph
    Post-conditions: area added to graph
*/
bool GameGraph::addArea(std::string name)
{
    room * newRoom = NULL;
    bool found = false;
    for (int i = 0; i < areas.size(); i++) //search collection of areas to see if area has already been added
    {
        if(areas[i]->title == name)
        {
            found = true;
            std::cout << "ERROR: " << areas[i]->title << " found." << std::endl;
        }
    }
    if(found == false) //add the new area to vector
    {
        newRoom = new room;
        newRoom->title = name;
        areas.push_back(newRoom);
        return false;
    }
    else
    {
        return true;
    }
}

/*
    bool addPathToArea(string startingArea, string endingArea, string description, string key) - returns true if path added
    Method connects two areas with an edge
    ex. addPathToArea("Forest", "Mountains", "Press m to go to the mountains.", "m")
    Pre-conditions: at least two different areas, an instance of the graph
    Post-conditions: An adjacent vert is added to the current vert's list of adjacent vertices
*/
bool GameGraph::addPathToArea(std::string beginningArea, std::string endingArea, std::string description, std::string key)
{
    bool added = false;
    for(int i = 0; i < areas.size(); i++)
    {
        if(areas[i]->title == beginningArea) //if beginning area is found
        {
            for(int j = 0; j < areas.size(); j++)
            {
                if(areas[j]->title == endingArea && i != j) //if ending area is found, add path between the two areas
                {
                    adjRoom aR;
                    //aR.r = &areas[j];
                    aR.r = areas[j];
                    aR.description = description; // Add the adjacency description to the adjVert
                    aR.key = key; // Add the required keystroke too
                    areas[i]->adj.push_back(aR);
                    added = true;
                    // DEBUG //
                    cout << areas[i]->title << " -> " << areas[j]->title << endl;
                    //another area for edge in other direction
                    // REMOVED, no need for doubly-connecting vertices as a proper file already will do this
                    /*
                    adjRoom aR2;
                    aR2.r = &areas[i];
                    areas[j].adj.push_back(aR2);
                    */
                }
            }
        }
    }
    return added;
}

/*
    void lookAtMap ()
    Method serves as a GUI and prints to the screen the contents of the current area's adjRoom vector
    ex. lookAtMap()
    Pre-conditions: an instance of the graph, player's location assigned
    Post-conditions: Player location will be changed based on user input
    Dependencies: movePlayer
*/
void GameGraph::lookAtMap()
{
    string input;



    cout << currentLocation->description << " ";
    if (!currentLocation->action_done)
    {
        cout << currentLocation->action << endl;
        currentLocation->action_done = true;
    }
    else
        cout << endl;

    if (ending_called)
    {
        cout << "\nPress any key to continue" << endl;
        cin.get();
        quit = true;
        return;
    }

    for (int i = 0; i < currentLocation->adj.size(); i++)
    {
        std::cout << currentLocation->adj[i].description << std::endl;
    }
    // Display skill and time remaining
    cout << "                           " << "Time remaining: " << timeleft << "    Skill: " << skill << endl;
    endGameSequences();
    if (!quit && !ending_called)
    {
        howFarToBoss();
        getline(cin,input);
        if (input != "q")
            movePlayer(input);
        else
            quit = true;
    }
}

/*
    routeToLocation shortestRouteToDestination(string startingArea, string endLocation)
    Method calculates the shortest unweighted distance between the starting area and end location
    ex. shortestRouteToDestination("Forest", "Graveyard")
    Pre-conditions: instance of graph, at least two different areas
    Post-conditions: return shortest path between starting area and end location
*/
routeToLocation GameGraph::shortestRouteToDestination(std::string startArea, std::string endLocation)
{
    room areaCheck;
    for (int i = 0; i < areas.size(); i++) //loop through areas, setting visited to false
    {
        areas[i]->visited = false;
        if (areas[i]->title == startArea)
        {
            areaCheck = *areas[i];
        }
    }
    areaCheck.visited = true;
    routeToLocation currentPath;
    currentPath.distance = 0;
    currentPath.path.push_back(areaCheck);
    std::queue<routeToLocation> pathQueue; //set up queue to store paths taken
    pathQueue.push(currentPath);
    while (!pathQueue.empty())
    {
        currentPath = pathQueue.front(); //look at current path taken
        pathQueue.pop();
        room roomCheck = currentPath.path.back();
        for (int i = 0; i < roomCheck.adj.size(); i++) //look at all adjacent areas to last area in current path
        {
            if (roomCheck.adj[i].r->visited == false)
            {
                routeToLocation newRoute;
                newRoute.path = currentPath.path;
                newRoute.distance = currentPath.distance;
                newRoute.path.push_back(*roomCheck.adj[i].r); //add area to path
                newRoute.distance++;
                if (roomCheck.adj[i].r->title == endLocation) //return once destination found
                {
                    return newRoute;
                }
                else
                {
                    pathQueue.push(newRoute); //add new path to queue
                }
            }
        }
    }
    return currentPath;
}

/*
    void movePlayer(string input)
    Moves player to a new space based on their keyboard input, adjusting skill points and remaining time accordingly
    ex. shortestRouteToDestination("w")
    Pre-conditions: Graph with paths for each area already established
    Post-conditions: Moves player's current location, updates skill points and time left
*/
void GameGraph::movePlayer(string input)
{
    bool moved = false;
    // Loop through adjacent verts, seeing if the input matches
    for (int i = 0; i<currentLocation->adj.size(); i++)
    {
        if (input == currentLocation->adj[i].key)
        {
            currentLocation = currentLocation->adj[i].r;
            moved = true;
            break;
        }
    }
    if (moved)
    {
        // update skill points
        if (currentLocation->action_done == false)
        {
            skill += currentLocation->skill;
        }
        // Update time limit
        timeleft -= 1;
    }
}

/*
    void connectRemainingPaths()
    Goes back and connects paths that were made before the area existed
    ex. shortestRouteToDestination("w")
    Pre-conditions: Vector "unconnected" is already filled with placeholder edge information
    Post-conditions: Adds all edges from the "unconnected" vector to the adjVert vectors of respective areas
    Dependencies: addPathToArea
*/
void GameGraph::connectRemainingPaths()
{
    bool reconnected;
    // For each entry in the vector of placeholders
    for (int i = 0; i<unconnected.size(); i++)
    {
        // Connect an edge
        reconnected = addPathToArea(unconnected[i].starting, unconnected[i].destination, unconnected[i].description, unconnected[i].key);
        if (!reconnected)
            cout << "   Unable to connect edge: " << unconnected[i].starting << " -> " << unconnected[i].destination << endl;
    }
}

/*
    void endGameSequences();
    Prints the correct statement for the given end game scenarios
    ex. endGameSequences();
    Pre-conditions: instance of graph, boss room and time limit have been set
    Post-conditions: returns player to the main menu
*/
void GameGraph::endGameSequences()
{
    bool won = true;

    room * success = NULL;
    room * defeat = NULL;
    room * timeout = NULL;

    // Attach pointers to the 3 scenarios
    for (int i = 0; i<areas.size(); i++)
    {
        if (areas[i]->title == "Success")
            success = areas[i];
        if (areas[i]->title == "Defeat")
            defeat = areas[i];
        if (areas[i]->title == "Timeout")
            timeout = areas[i];
    }
    // Error checking
    if (success == NULL || defeat == NULL || timeout == NULL)
    {
        cout << "WARNING: Your game is hella broken (one or more of your ending scenarios is missing), so by default, you win." << endl;
        //Gave me hella luls
        quit = true;
        return;
    }

    if (timeleft == 0)
    {
        ending_called = true;
        if (currentLocation == bossroom) //no time left and reached the boss room
        {
            // Wait for second 'if' in this function
        }
        else //run out of time and haven't found the boss
        {
            currentLocation = timeout;
        }
    }
    if (currentLocation == bossroom) // Win or lose sequences
    {
        ending_called = true;
        //cout << "Congratulations! You beat the game!" << endl;
        if (skill < skill_threshold)
        {
            // Defeat ending
            cout << "\nPress enter to continue..." << endl;
            cin.get();
            currentLocation = defeat;
            /*
            won = bossFight(); // Initiate the smackdown
            */
        }
        else
        {
            // Success ending
            cout << "\nPress enter to continue..." << endl;
            cin.get();
            /*
            won = bossFight();
            */
            if (won)
                currentLocation = success;
            else
                currentLocation = defeat;
        }
    }
}

/*
    void howFarToBoss()
    Calculates how far it is from player's current location to the boss
    Pre-conditions: boss room set, currentLocation is known
    Post-conditions: distance to boss printed if player is close enough
*/
void GameGraph::howFarToBoss()
{
    routeToLocation routeToBoss;
    routeToBoss = shortestRouteToDestination(currentLocation->title, "Well");
    if (routeToBoss.distance < 3 && routeToBoss.distance != 0)
    {
        cout << "                           " << "Boss is " << routeToBoss.distance;
        if (routeToBoss.distance ==1)
        {
            cout << " move away." << endl;
        }
        else
        {
            cout << " moves away." << endl;
        }
    }
}
/*

Post-conditions: returns whether the player won or lost
*/

bool GameGraph::bossFight()
{
    bool won;
    int timelimit = 10; // Modify for boss fight length
    int actionthreshold = 12; // Modify for minimum actions

    clock_t time;
    string input;
    int actionCount = 0;
    int seconds_elapsed;
    bool tryAgain = true;
    bossFightActions tempAction;

    bossFightActions dodge;
            dodge.bossAction = "The Spirit swings his dangerous, translucent fist at your face. DODGE";
            dodge.responseToAction = "d";  // "d" to dodge

    bossFightActions attack;
            attack.bossAction = "You see an opening to strike back against the monster. ATTACK";
            attack.responseToAction = "f";      // "a" to attack

    bossFightActions jump;
            jump.bossAction = "The Spirit swings at your knee caps, you should JUMP";
            jump.responseToAction = " ";    //spacebar to jump

    bossFightActions actionsArray[3] = {dodge, attack, jump};

    cout << "\n \n" << "The stage is set for a man vs. spirit battle of epic proportions." << endl;
    cout << "The battle will be ferocious but quick. Time is of the essence and speed is a factor." << endl;
    cout << "The faster you respond, the greater the chance of victory." << endl;
    cout << "\n" <<   "The rules are simple: Press spacebar to jump, d to dodge, and f to attack. You must also press enter once after each key is hit.  \n \t inaccurate entries will not count!!!" << endl;
    cout << "\n" << "Press Enter if you are READY TO FIGHT" << "\n \n " << endl;

    getline(cin, input);

    while(tryAgain == true)
    {

    time = clock();

    do{

            tempAction = actionsArray[randomNumberGenerator(2)]; //passing 2 as a parameter will generate one of three numbers randomly; 0, 1, 2. The indices of the array.
            cout << tempAction.bossAction << "\n" << endl;

            getline(cin, input);

            cout << "\n" << endl;

            if(input == tempAction.responseToAction) actionCount++;

            seconds_elapsed = (clock() - time) / CLOCKS_PER_SEC;
            cout << "BOSS DAMAGE: " << actionCount << "/" << actionthreshold << endl;
            cout << "Time remaining: " << timelimit - seconds_elapsed << endl;

    }   while (seconds_elapsed <= timelimit);




    if(actionCount > actionthreshold)
    {
        cout << "YOU HAVE WON THE BATTLE!" << endl;
        won = true;
    }
    else
    {

        cout << "You may have had what it takes but your speed and precision were found lacking. You are no hero today. \n" << endl;
        cout << "Most heroes do not get to go back in time to fight a lost battle again. Today luck is on your side. " << endl;
        won = false;
    }

    cout << "Would you like to try the Boss Fight Again? \n" << endl;
    cout << "y for yes" << "\n" << "n for no" << endl;
    getline(cin, input);

    if(input == "y");
    else tryAgain = false;

    }

    return won;
}

int GameGraph::randomNumberGenerator(int rangeMax)
{

    static default_random_engine generator;     // include <random> used for this functionality. rand and srand not used because a modulo operation would be needed to get the random number
    uniform_int_distribution <int> distribution (0, rangeMax);              // in the range desired. RAND implementation favors lower indices, aka not as random
    int randNum = distribution(generator);                  //random engine is the seed generator with distribution declaring all values in the range are equally as likely to be picked. upper limit taken as parameter

    return randNum;
}

