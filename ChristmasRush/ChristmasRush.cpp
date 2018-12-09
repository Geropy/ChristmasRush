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

enum DIRECTION {UP, DOWN, LEFT, RIGHT};

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

    pair<vector<string>, int> aStar(Player& player, array<int,2> & target) const
    {
        map<Position, Position> cameFrom;
        set<Position> closedSet;
        priority_queue<AstarPos> openSet;
        openSet.emplace(player.position, calcHScore(player.position, target), 0, vector<string>());

        AstarPos bestSoFar = openSet.top();

        //cerr << "path" << player.position[0] << player.position[1] << target[0] << target[1] << endl;

        while (!openSet.empty())
        {
            AstarPos current = openSet.top();
            Position & currentPos = current.pos;
            if (current.pos == target)
            {
                // return best path by going through the cameFroms
                return make_pair(current.cameFrom, 0);
            }

            openSet.pop();
            closedSet.insert(currentPos);

            if (calcHScore(currentPos, target) < calcHScore(bestSoFar.pos, target))
            {
                bestSoFar = current;
            }

            // Determine the neighbors of the position
            const Tile& currentTile = grid[currentPos[0]][currentPos[1]];
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

        return make_pair(bestSoFar.cameFrom, calcHScore(bestSoFar.pos, target));
    }

    void clear()
    {
        hero.clear();
        villain.clear();
    }

    void shiftRowLeft(int row)
    {
        Tile temp = hero.tile;
        hero.tile = grid[row][0];
        for (int i = 0; i < 6; i++)
        {
            grid[row][i] = grid[row][i + 1];
        }

        grid[row][6] = temp;

        if (hero.position[0] == row)
        {
            hero.position[1]--;
            if (hero.position[1] == -1)
            {
                hero.position[1] = 6;
            }
        }

        for (auto& item : hero.items)
        {
            if (item.second[0] == row)
            {
                item.second[1]--;
                if (item.second[1] == -1)
                {
                    item.second = { {-1, -1} };
                }
            }

            else if (item.second[0] == -1)
            {
                item.second = { {row, 6} };
            }
        }
    }

    void shiftRowRight(int row)
    {
        Tile temp = hero.tile;
        hero.tile = grid[row][6];
        for (int i = 6; i > 0; i--)
        {
            grid[row][i] = grid[row][i - 1];
        }

        grid[row][0] = temp;

        if (hero.position[0] == row)
        {
            hero.position[1]++;
            if (hero.position[1] == 7)
            {
                hero.position[1] = 0;
            }
        }

        for (auto& item : hero.items)
        {
            if (item.second[0] == row)
            {
                item.second[1]++;
                if (item.second[1] == 7)
                {
                    item.second = { { -1, -1 } };
                }
            }

            else if (item.second[0] == -1)
            {
                item.second = { { row, 0 } };
            }
        }
    }

    void shiftColUp(int col)
    {
        Tile temp = hero.tile;
        hero.tile = grid[0][col];
        for (int i = 0; i < 6; i++)
        {
            grid[i][col] = grid[i + 1][col];
        }

        grid[6][col] = temp;

        if (hero.position[1] == col)
        {
            hero.position[0]--;
            if (hero.position[0] == -1)
            {
                hero.position[0] = 6;
            }
        }

        for (auto& item : hero.items)
        {
            if (item.second[1] == col)
            {
                item.second[0]--;
                if (item.second[0] == -1)
                {
                    item.second = { { -1, -1 } };
                }
            }

            else if (item.second[1] == -1)
            {
                item.second = { { 6, col } };
            }
        }
    }

    void shiftColDown(int col)
    {
        Tile temp = hero.tile;
        hero.tile = grid[6][col];
        for (int i = 6; i > 0; i--)
        {
            grid[i][col] = grid[i - 1][col];
        }

        grid[0][col] = temp;

        if (hero.position[1] == col)
        {
            hero.position[0]++;
            if (hero.position[0] == 7)
            {
                hero.position[0] = 0;
            }
        }

        for (auto& item : hero.items)
        {
            if (item.second[1] == col)
            {
                item.second[0]++;
                if (item.second[0] == 7)
                {
                    item.second = { { -1, -1 } };
                }
            }

            else if (item.second[1] == -1)
            {
                item.second = { { 0, col } };
            }
        }
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
                board.hero.items[itemName] = { {-1, -1} };
            }
            else if (itemX == -2)
            {
                board.villain.items[itemName] = { { -1, -1 } };
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


        if (!moveTurn)
        {
            // Try to get close to the goal

            // Start by identifying where I need to go
            string& goal = board.hero.quests.front();
            auto res = board.hero.items.find(goal);

            // If it isn't on the board, I can't get there, just random move
            if (res->second[0] == -1)
            {
                cout << "PUSH 3 UP" << endl; 
            }

            else
            {
                int minDist = 999;
                int bestIndex = 0;
                DIRECTION direction = LEFT;
                int distance;

                // Try every row
                for (int i = 0; i < 7; i++)
                {
                    //Left 
                    board.shiftRowLeft(i);
                    distance = board.aStar(board.hero, res->second).second;
                    if (distance < minDist)
                    {
                        minDist = distance;
                        bestIndex = i;
                        direction = LEFT;
                    }
                    board.shiftRowRight(i);

                    //Right
                    board.shiftRowRight(i);
                    distance = board.aStar(board.hero, res->second).second;
                    if (distance < minDist)
                    {
                        minDist = distance;
                        bestIndex = i;
                        direction = RIGHT;
                    }
                    board.shiftRowLeft(i);

                    //Up 
                    board.shiftColUp(i);
                    distance = board.aStar(board.hero, res->second).second;
                    if (distance < minDist)
                    {
                        minDist = distance;
                        bestIndex = i;
                        direction = UP;
                    }
                    board.shiftColDown(i);

                    //Down 
                    board.shiftColDown(i);
                    distance = board.aStar(board.hero, res->second).second;
                    if (distance < minDist)
                    {
                        minDist = distance;
                        bestIndex = i;
                        direction = DOWN;
                    }
                    board.shiftColUp(i);
                }

                string dir = "";
                switch (direction)
                {
                case UP:
                    dir = " UP";
                    break;
                case DOWN:
                    dir = " DOWN";
                    break;
                case LEFT:
                    dir = " LEFT";
                    break;
                case RIGHT:
                    dir = " RIGHT";
                    break;
                default:
                    break;
                }



                cout << "PUSH " << bestIndex << dir << endl;
            }
            
        }
        else
        {
            // Start by identifying where I need to go
            string& goal = board.hero.quests.front();
            //cerr << goal << endl;
            auto res = board.hero.items.find(goal);

            // If it isn't on the board, I can't get there, just pass for now
            if (res->second[0] != -1)
            {
                //cerr << res->first << endl;
                auto path = board.aStar(board.hero, res->second);

                if (path.first.empty())
                {
                    cout << "PASS" << endl;
                }
                else
                {
                    string move = "MOVE";
                    for (auto & dir : path.first)
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