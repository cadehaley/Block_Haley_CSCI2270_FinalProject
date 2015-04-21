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
    int skill; // Skill points
    std::vector<adjRoom> adj; // Adjacent room vector
};

struct adjRoom{
    room * r;
    std::string description; // Adjacency description, not to be confused with regular description
    char action; // Character you type to move into the room
};

class GameGraph
{
    public:
        /** Default constructor */
        GameGraph();
        /** Default destructor */
        virtual ~GameGraph();
        void addArea(std::string name);     //add area or location to map
        void addPathToArea(std::string beginningArea, std::string endingArea);    //add path between two areas or locations
        void lookAtMap(std::string currentArea);    //looks at map and prints all adjacent areas once can get to
    protected:
    private:
};

#endif // GAMEGRAPH_H
