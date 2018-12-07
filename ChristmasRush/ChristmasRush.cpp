#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>

using namespace std;

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
    int row;
    int column;
    vector<string> quests;
    Tile tile;
    string tileItem;
};

struct ItemLocation
{
    ItemLocation(string name, int row, int column)
        : name(name)
        , row(row)
        , column(column)
    {}
    
    string name;
    int row;
    int column;
};

struct Board
{
    array<array<Tile, 7>, 7> grid;
    vector<ItemLocation> items;
    Player hero, villain;
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
            player.column = playerX;
            player.row = playerY;
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
                board.items.emplace_back(itemName, itemY, itemX);
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
            cout << "PASS" << endl;
        }
    }
}