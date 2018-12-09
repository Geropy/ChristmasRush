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

    pair<vector<string>, int> aStar(array<int, 2> & start, array<int,2> & target, int maxMoves = 20) const
    {
        map<Position, Position> cameFrom;
        set<Position> closedSet;
        priority_queue<AstarPos> openSet;
        openSet.emplace(start, calcHScore(start, target), 0, vector<string>());

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
            // No neightbors if move limit reached
            if (current.cameFrom.size() >= maxMoves)
            {
                continue;
            }
       
            const Tile& currentTile = grid[currentPos[0]][currentPos[1]];

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
            // Try to get make a move that enables a quest (assuming opponent doesn't mess it up)

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
                    distance = board.aStar(board.hero.position, res->second).second;
                    if (distance < minDist)
                    {
                        minDist = distance;
                        bestIndex = i;
                        direction = LEFT;
                    }
                    board.shiftRowRight(i);

                    //Right
                    board.shiftRowRight(i);
                    distance = board.aStar(board.hero.position, res->second).second;
                    if (distance < minDist)
                    {
                        minDist = distance;
                        bestIndex = i;
                        direction = RIGHT;
                    }
                    board.shiftRowLeft(i);

                    //Up 
                    board.shiftColUp(i);
                    distance = board.aStar(board.hero.position, res->second).second;
                    if (distance < minDist)
                    {
                        minDist = distance;
                        bestIndex = i;
                        direction = UP;
                    }
                    board.shiftColDown(i);

                    //Down 
                    board.shiftColDown(i);
                    distance = board.aStar(board.hero.position, res->second).second;
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
            // Go through every quest item, see what I can get closest too
            // In ties, use the shorter path
            pair<vector<string>, int> bestPath(vector<string>(), 50);
            string questPersued = "";
            Position questPosition;
            string move = "PASS";
            for (auto& quest : board.hero.quests)
            {
                // Start by identifying where I need to go
                auto res = board.hero.items.find(quest);

                // If it isn't on the board, I can't get there, just pass for now
                if (res->second[0] != -1)
                {
                    auto path = board.aStar(board.hero.position, res->second);

                    if (path.first.empty())
                    {
                        continue;
                    }
                    else if (path.second < bestPath.second || (path.second == bestPath.second && path.first.size() < bestPath.first.size()))
                    {
                        bestPath = path;
                        questPersued = quest;
                        questPosition = res->second;
                    }
                }
                else
                {
                    continue;
                }
            }

            if (!bestPath.first.empty())
            {
                move = "MOVE";
                for (auto & dir : bestPath.first)
                {
                    move += " ";
                    move += dir;
                }
            }

            // At this point, I have single efficient path
            // However, if this is a successful path, I have leftover useful moves
            if (bestPath.second == 0)
            {
                int moves = 20 - bestPath.first.size();
                bestPath = make_pair(vector<string>(), 50);

                for (auto& quest : board.hero.quests)
                {
                    if (quest == questPersued)
                    {
                        continue;
                    }
                    
                    // Start by identifying where I need to go
                    auto res = board.hero.items.find(quest);

                    // If it isn't on the board, I can't get there, just pass for now
                    if (res->second[0] != -1)
                    {
                        auto path = board.aStar(questPosition, res->second, moves);

                        if (path.first.empty())
                        {
                            continue;
                        }
                        else if (path.second < bestPath.second || (path.second == bestPath.second && path.first.size() < bestPath.first.size()))
                        {
                            bestPath = path;
                            questPersued = quest;
                            questPosition = res->second;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }

                if (!bestPath.first.empty())
                {
                    for (auto & dir : bestPath.first)
                    {
                        move += " ";
                        move += dir;
                    }
                }
            }

            cout << move << endl;
           
        }
    }
}