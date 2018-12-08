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
    {}

    AstarPos(Position pos, int fscore)
        : pos(pos)
        , fScore(fscore)
    {}
    
    Position pos;
    int fScore;
};

bool operator<(const AstarPos& lhs, const AstarPos& rhs)
{
    return lhs.fScore > rhs.fScore;
}

int calcFScore(Position & start, Position & finish)
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
    Tile tile;
    string tileItem;
};

struct Board
{
    array<array<Tile, 7>, 7> grid;
    map<string, Position> items;
    Player hero, villain;

    vector<string> aStar(Player& player, array<int,2> & target)
    {
        vector<string> answer;

        map<Position, Position> cameFrom;
        set<Position> closedSet;
        priority_queue<AstarPos> openSet;
        openSet.emplace(player.position, calcFScore(player.position, target));

        while (!openSet.empty())
        {
            const AstarPos & current = openSet.top;
            if (current.pos == target)
            {
                // return best path
                return answer;
            }

            openSet.pop();
            closedSet.insert(current.pos);

            // TODO: determine the neighbors of the position

        }


        return answer;
    }
};




int main()
{
    // game loop
    while (1) {
        Board board;
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
            else
            {
                board.items[itemName] = { {itemY, itemX} };
            }

            // Don't think I need to use itemPlayerID
        }

        // Read in quest information
        int numQuests; // the total number of revealed quests for both players
        cin >> numQuests; cin.ignore();
        for (int i = 0; i < numQuests; i++) {
            string questItemName;
            int questPlayerId;
            cin >> questItemName >> questPlayerId; cin.ignore();

            if (questPlayerId = 0)
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

        if (!moveTurn)
        {
            cout << "PUSH 3 RIGHT" << endl; // PUSH <id> <direction> | MOVE <direction> | PASS
        }
        else
        {
            // Start by identifying where I need to go
            string& goal = board.hero.quests[0];
            auto res = board.items.find(goal);

            // If it isn't on the board, I can't get there, just pass for now
            if (res != board.items.end())
            {
                auto path = board.aStar(board.hero, res->second);

                string move = "MOVE";
                for (auto & dir : path)
                {
                    move += " ";
                    move += dir;
                }

                cout << move << endl;
            }
            else
            {
                cout << "PASS" << endl;
            }
        }
    }
}