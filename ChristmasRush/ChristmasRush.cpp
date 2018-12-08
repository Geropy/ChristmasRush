#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <map>
#include <unordered_set>
#include <queue>
#include <set>

using namespace std;

using Position = array<int, 2>;


struct AstarPos
{
    AstarPos()
        : pos({{0,0}})
        , fScore(0)
        , gScore(0)
    {}

    AstarPos(Position pos, int fscore, int gscore, vector<string> path)
        : pos(pos)
        , fScore(fscore)
        , gScore(gscore)
        , cameFrom(path)
    {}
    
    Position pos;
    vector<string> cameFrom;
    int fScore;
    int gScore;
};

bool operator<(const AstarPos& lhs, const AstarPos& rhs)
{
    return lhs.fScore > rhs.fScore;
}

int calcHScore(Position & start, Position & finish)
{
    return abs(start[0] - finish[0]) + abs(start[1] - finish[1]);
}


struct Tile
{
    bool leftPath;
    bool rightPath;
    bool upPath;
    bool downPath;

    void readType(string type)
    {
        upPath = type[0] == '1';
        rightPath = type[1] == '1';
        downPath = type[2] == '1';
        leftPath = type[3] == '1';
    }
};

struct Player
{
    Position position;
    vector<string> quests;
    map<string, Position> items;
    Tile tile;
    string tileItem;

    void clear()
    {
        quests.clear();
        items.clear();
    }
};

struct Board
{
    array<array<Tile, 7>, 7> grid;
    Player hero, villain;

    vector<string> aStar(Player& player, array<int,2> & target)
    {
        map<Position, Position> cameFrom;
        set<Position> closedSet;
        priority_queue<AstarPos> openSet;
        openSet.emplace(player.position, calcHScore(player.position, target), 0, vector<string>());

        //cerr << "path" << player.position[0] << player.position[1] << target[0] << target[1] << endl;

        while (!openSet.empty())
        {
            AstarPos current = openSet.top();
            Position & currentPos = current.pos;
            if (current.pos == target)
            {
                // return best path by going through the cameFroms

                return current.cameFrom;
            }

            openSet.pop();
            closedSet.insert(currentPos);

            // Determine the neighbors of the position
            Tile& currentTile = grid[currentPos[0]][currentPos[1]];
            //cerr << "tile " << currentTile.upPath << currentTile.downPath << currentTile.leftPath << currentTile.rightPath << endl;

            if (currentTile.upPath && currentPos[0] != 0 && grid[currentPos[0] - 1][currentPos[1]].downPath)
            { 
                Position newPos = { {currentPos[0] - 1, currentPos[1]} };
                if (closedSet.find(newPos) == closedSet.end())
                {
                    int gScore = current.gScore + 1;
                    vector<string> path = current.cameFrom;
                    path.push_back("UP");

                    openSet.emplace(newPos, gScore + calcHScore(newPos, target), gScore, path);
                }
            }

            if (currentTile.downPath && currentPos[0] != 6 && grid[currentPos[0] + 1][currentPos[1]].upPath)
            {
                Position newPos = { { currentPos[0] + 1, currentPos[1] } };
                if (closedSet.find(newPos) == closedSet.end())
                {
                    int gScore = current.gScore + 1;
                    vector<string> path = current.cameFrom;
                    path.push_back("DOWN");

                    openSet.emplace(newPos, gScore + calcHScore(newPos, target), gScore, path);
                }
            }

            if (currentTile.leftPath && currentPos[1] != 0 && grid[currentPos[0]][currentPos[1] - 1].rightPath)
            {
                Position newPos = { { currentPos[0], currentPos[1] - 1} };
                if (closedSet.find(newPos) == closedSet.end())
                {
                    int gScore = current.gScore + 1;
                    vector<string> path = current.cameFrom;
                    path.push_back("LEFT");

                    openSet.emplace(newPos, gScore + calcHScore(newPos, target), gScore, path);
                }
            }

            if (currentTile.rightPath && currentPos[1] != 6 && grid[currentPos[0]][currentPos[1] + 1].leftPath)
            {
                Position newPos = { { currentPos[0], currentPos[1] + 1 } };
                if (closedSet.find(newPos) == closedSet.end())
                {
                    int gScore = current.gScore + 1;
                    vector<string> path = current.cameFrom;
                    path.push_back("RIGHT");

                    openSet.emplace(newPos, gScore + calcHScore(newPos, target), gScore, path);
                }
            }
        }

        return vector<string>();
    }

    void clear()
    {
        hero.clear();
        villain.clear();
    }
};




int main()
{
    Board board;
    
    // game loop
    while (1) {
        board.clear();
        bool moveTurn;
        cin >> moveTurn; cin.ignore();

        // Read in current board tiles
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                string tile;
                cin >> tile; cin.ignore();

                board.grid[i][j].readType(tile);
            }
        }

        cerr << "check" << endl;

        // Read in player information
        for (int i = 0; i < 2; i++) {
            int numPlayerCards; // the total number of quests for a player (hidden and revealed)
            int playerX;
            int playerY;
            string playerTile;
            cin >> numPlayerCards >> playerX >> playerY >> playerTile; cin.ignore();

            Player& player = i == 0 ? board.hero : board.villain;
            player.position = { {playerY, playerX} };
            player.tile.readType(playerTile);
        }

        // Read in item information
        int numItems; // the total number of items available on board and on player tiles
        cin >> numItems; cin.ignore();
        for (int i = 0; i < numItems; i++) {
            string itemName;
            int itemX;
            int itemY;
            int itemPlayerId;
            cin >> itemName >> itemX >> itemY >> itemPlayerId; cin.ignore();

            if (itemX == -1)
            {
                board.hero.tileItem = itemName;
            }
            else if (itemX == -2)
            {
                board.villain.tileItem = itemName;
            }
            else if (itemPlayerId == 0)
            {
                board.hero.items[itemName] = { {itemY, itemX} };
            }
            else
            {
                board.villain.items[itemName] = { { itemY, itemX } };
            }

        }

        // Read in quest information
        int numQuests; // the total number of revealed quests for both players
        cin >> numQuests; cin.ignore();
        for (int i = 0; i < numQuests; i++) {
            string questItemName;
            int questPlayerId;
            cin >> questItemName >> questPlayerId; cin.ignore();

            if (questPlayerId == 0)
            {
                board.hero.quests.push_back(questItemName);
            }
            else
            {
                board.villain.quests.push_back(questItemName);
            }
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cerr << moveTurn << endl;

        if (!moveTurn)
        {
            cout << "PUSH 3 RIGHT" << endl; // PUSH <id> <direction> | MOVE <direction> | PASS
        }
        else
        {
            // Start by identifying where I need to go
            string& goal = board.hero.quests.front();
            //cerr << goal << endl;
            auto res = board.hero.items.find(goal);

            // If it isn't on the board, I can't get there, just pass for now
            if (res != board.hero.items.end())
            {
                //cerr << res->first << endl;
                auto path = board.aStar(board.hero, res->second);

                if (path.empty())
                {
                    cout << "PASS" << endl;
                }
                else
                {
                    string move = "MOVE";
                    for (auto & dir : path)
                    {
                        move += " ";

                        move += dir;
                    }

                    cout << move << endl;
                } 
            }
            else
            {
                cout << "PASS" << endl;
            }
        }
    }
}