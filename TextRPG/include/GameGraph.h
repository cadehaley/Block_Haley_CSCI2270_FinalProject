// Authors: Cade Haley, Mitch Block
// Date: Apr. 19, 2015
// Description: A text-based game that makes use of graphs to control the map

#ifndef GAMEGRAPH_H
#define GAMEGRAPH_H
#include <string>
#include <vector>

struct adjRoom;

struct room{
    std::string title;
    std::string description;
    std::string action; // For a newly visited room, the player will do this action, e.g. ("slay a dragon")
    bool action_done;
    bool visited;
    int skill; // Skill points
    std::vector<adjRoom> adj; // Adjacent room vector
};

struct routeToLocation  //struct for traversal method (shortestRouteToDestination)
{
    int distance;
    std::vector<room> path;
};

struct adjRoom{
    room * r; // Pointer to connected room
    std::string description; // Adjacency description, not to be confused with regular description
    std::string key; // Character you type to move into the room
};

struct placeholder{ // Forms a placeholder for edges made before their destination vertex existed
    std::string starting;
    std::string destination;
    std::string description;
    std::string key;
};

class GameGraph
{
    public:
        /** Default constructor */
        GameGraph();
        /** Default destructor */
        virtual ~GameGraph();
        void readAndBuild(std::string filename);
        bool addArea(std::string name);     //add area or location to map, returning whether a duplicate was found
        bool addPathToArea(std::string beginningArea, std::string endingArea, std::string description, std::string key);    //add path between two areas or locations
        void lookAtMap();    //looks at map and prints all adjacent areas one can get to
        routeToLocation shortestRouteToDestination(std::string currentArea, std::string endLocation);    //shortest path between current location and boss
        void movePlayer(std::string input); // TBD, changes current location and marks vertex as visited
        void endGameSequences();
        void howFarToBoss();
        void connectRemainingPaths(); // Connects edges that were made before the vertex existed
        // Public variables
        bool quit; // Set to true if the player wants to quit
        bool boss_room_random;
        int skill_threshold;
        int timeleft;
    protected:
    private:
        room * currentLocation; // Pointer to the area where the player is currently located
        room * bossroom;
        int skill;
        bool ending_called;
        std::vector<room*> areas; //vector containing all areas
        std::vector<placeholder> unconnected; // Vector containing edges that have yet to be connected
};

#endif // GAMEGRAPH_H
