#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <map>
using namespace std;
//Grading
struct Tile {
    int neighborNum = 0;
    bool OnEnd = false;
    bool isRevealed = false;
    bool tagged = false;
    bool hasMine = false;
    bool flagUp = false;
};
vector<string> ldrNames;
string* recentWin = nullptr;

int getWidth() {
    int width;
    ifstream file("files/board_config.cfg");
    if (file.is_open()) {
        string n;
        getline(file, n);
        width = stoi(n);
    }
    return width;
}
int getHeight() {
    int width, height;
    ifstream file("files/board_config.cfg");
    if (file.is_open()) {
        string n;
        getline(file, n);
        width = stoi(n);
        getline(file, n);
        height = stoi(n);
    }
    return height;
}
int getMineCount() {
    int width, height, mineCount;
    ifstream file("files/board_config.cfg");
    if (file.is_open()) {
        string n;
        getline(file, n);
        width = stoi(n);
        getline(file, n);
        height = stoi(n);
        getline(file, n);
        mineCount = stoi(n);
        mineCount = width * height;
    }
    file.close();
    return mineCount;
}

bool debugOn = false;
bool pauseOn = false;
std::vector<std::vector<char> > boards;
std::vector<int> minePos, flagPos, revealedTilePos;
bool winning = true;
bool gameWon = false;
int currentBoard = 0;
Tile gameBoard[25][16];
int timer = 0;


int findNum(int j, int k) {
    int width = getWidth();
    return ((k * width) + j);
}


void setNxtNums() {
    int width = getWidth(); int height = getHeight();
    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            int numOfNeighbors = 0;
            if (!gameBoard[j][i].OnEnd) {
                // all left
                if (gameBoard[j - 1][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j - 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j - 1][i + 1].hasMine)
                    numOfNeighbors++;

                // all right
                if (gameBoard[j + 1][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i + 1].hasMine)
                    numOfNeighbors++;

                // above, below
                if (gameBoard[j][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j][i + 1].hasMine)
                    numOfNeighbors++;

                gameBoard[j][i].neighborNum = numOfNeighbors;
            }
            //do special cases for corners
            else if ((j == 0) && (i == 0)) {
                //top left corner
                if (gameBoard[j + 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i + 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j][i + 1].hasMine)
                    numOfNeighbors++;

                gameBoard[j][i].neighborNum = numOfNeighbors;
            }
            else if ((j == 0) && (i == height - 1)) {
                //bottom left corner
                if (gameBoard[j][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i].hasMine)
                    numOfNeighbors++;

                gameBoard[j][i].neighborNum = numOfNeighbors;
            }
            else if ((j == width - 1) && (i == 0)) {
                // top right corner
                if (gameBoard[j - 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j - 1][i + 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j][i + 1].hasMine)
                    numOfNeighbors++;

                gameBoard[j][i].neighborNum = numOfNeighbors;
            }
            else if ((j == width - 1) && (i == height - 1)) {
                //bottom right corner
                if (gameBoard[j - 1][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j - 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j][i - 1].hasMine)
                    numOfNeighbors++;

                gameBoard[j][i].neighborNum = numOfNeighbors;
            }
            else if (j == 0) {
                //left down
                if (gameBoard[j][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i + 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j][i + 1].hasMine)
                    numOfNeighbors++;

                gameBoard[j][i].neighborNum = numOfNeighbors;
            }
            else if (j == width - 1) {
                //check for right down
                if (gameBoard[j][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j - 1][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j - 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j - 1][i + 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j][i + 1].hasMine)
                    numOfNeighbors++;

                gameBoard[j][i].neighborNum = numOfNeighbors;
            }
            else if (i == 0) {
                //check for top row
                if (gameBoard[j - 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j - 1][i + 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j][i + 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i + 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i].hasMine)
                    numOfNeighbors++;

                gameBoard[j][i].neighborNum = numOfNeighbors;
            }
            else if (i == height - 1) {
                //check for bottom row
                if (gameBoard[j - 1][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j - 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j + 1][i - 1].hasMine)
                    numOfNeighbors++;
                if (gameBoard[j][i - 1].hasMine)
                    numOfNeighbors++;

                gameBoard[j][i].neighborNum = numOfNeighbors;
            }

            if (gameBoard[j][i].hasMine)
                gameBoard[j][i].neighborNum = 0;
        }
    }
}

int rand(int min, int max) {
    static std::mt19937 random;

    std::uniform_int_distribution<int> dist(min, max);
    return dist(random);
}

void createBoard() {
    winning = true;
    gameWon = false;
    std::vector<char> randBoard;


    int mineCount = 0;
    int realCount = getMineCount();
    for (unsigned int i = 0; i < 400; i++) {
        if (mineCount >= 50)
            randBoard.push_back('0');
        else {
            int num = rand(0, 1);
            if (num == 0)
                randBoard.push_back('0');
            else {
                for (unsigned int k = 0; k < (rand() % 20); k++) {
                    randBoard.push_back('0');
                }
                randBoard.push_back('1');
                mineCount++;
            }
        }

    }

    minePos.clear();
    flagPos.clear();
    revealedTilePos.clear();

    //find mine pos
    for (unsigned int i = 0; i < randBoard.size(); i++) {
        if (randBoard[i] == '1')
            minePos.push_back(i);
    }

    boards.push_back(randBoard);
    int width = getWidth(); int height = getHeight();
    int TileCount = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Tile tile;
            if ((i == 0) || (i == height - 1) || (j == 0) || (j == width - 1))
                tile.OnEnd = true;
            if (boards.at(currentBoard).at(TileCount) == '1')
                tile.hasMine = true;
            gameBoard[j][i] = tile;

            TileCount++;
        }
    }
    setNxtNums();
    currentBoard++;
}

void clickTile(int i) {
    int width = getWidth(); int height = getHeight();
    int hiddenIter = 0;

    for (int h = 0; h < height; h++) {
        for (int t = 0; t < width; t++) {

            if (i == hiddenIter) {
                gameBoard[t][h].isRevealed = true;

                //if flag dont reveal
                for (int p = 0; p < flagPos.size(); p++) {
                    if (flagPos[p] == i) {
                        gameBoard[t][h].isRevealed = false;
                        continue;
                    }
                }

                if ((gameBoard[t][h].neighborNum == 0) && (!gameBoard[t][h].hasMine) && (!gameBoard[t][h].tagged)) {
                    gameBoard[t][h].tagged = true;

                    //top left corner
                    if ((t == 0) && (h == 0)) {
                        clickTile(findNum(1, 0));
                        clickTile(findNum(1, 1));
                        clickTile(findNum(0, 1));
                    }
                    //top right corner
                    else if ((t == width - 1) && (h == 0)) {
                        clickTile(findNum(width - 2, 0));
                        clickTile(findNum(width - 2, 1));
                    }
                    //bottom right corner
                    else if ((t == width - 1) && (h == height - 1)) {
                        clickTile(findNum(width - 2, height - 1));
                        clickTile(findNum(width - 2, height - 2));
                        clickTile(findNum(width - 1, height - 2));
                    }
                    //bottom left corner
                    else if ((t == 0) && (h == height - 1)) {
                        clickTile(findNum(0, height - 2));
                        clickTile(findNum(1, height - 2));
                        clickTile(findNum(1, height - 1));
                    }
                    //top row
                    else if (h == 0) {
                        clickTile(findNum(t - 1, h));
                        clickTile(findNum(t + 1, h));
                        clickTile(findNum(t + 1, h + 1));
                        clickTile(findNum(t, h + 1));
                        clickTile(findNum(t - 1, h + 1));
                    }
                    //bottom row
                    else if (h == height - 1) {
                        clickTile(findNum(t - 1, h));
                        clickTile(findNum(t - 1, h - 1));
                        clickTile(findNum(t, h - 1));
                        clickTile(findNum(t + 1, h - 1));
                        clickTile(findNum(t + 1, h));
                    }
                    //right
                    else if (t == 0) {
                        clickTile(findNum(t, h - 1));
                        clickTile(findNum(t + 1, h - 1));
                        clickTile(findNum(t + 1, h));
                        clickTile(findNum(t + 1, h + 1));
                        clickTile(findNum(t, h + 1));
                    }
                    //left
                    else if (t == width - 1) {
                        clickTile(findNum(t - 1, h));
                        clickTile(findNum(t - 1, h - 1));
                        clickTile(findNum(t, h - 1));
                        clickTile(findNum(t, h + 1));
                        clickTile(findNum(t - 1, h + 1));
                    }
                    //middle
                    else {
                        clickTile(findNum(t - 1, h));
                        clickTile(findNum(t - 1, h - 1));
                        clickTile(findNum(t, h - 1));
                        clickTile(findNum(t + 1, h - 1));
                        clickTile(findNum(t + 1, h));
                        clickTile(findNum(t + 1, h + 1));
                        clickTile(findNum(t, h + 1));
                        clickTile(findNum(t - 1, h + 1));
                    }
                }

                //pushback location to revealedPositions vector
                if (gameBoard[t][h].isRevealed) {
                    revealedTilePos.push_back(hiddenIter);
                }
            }
            hiddenIter++;
        }
    }
}
void setPosition(sf::Text& words, const float& x, const float& y)
{
    words.setOrigin(words.getLocalBounds().width / 2, words.getLocalBounds().height / 2);
    words.setPosition(x / 2.0f, y);
}

sf::Text setText(const std::string& txt, const int& size, const float& x, const float& y, const sf::Font& font, bool isUL)
{
    sf::Text text;
    text.setFont(font);
    text.setString(txt);
    text.setCharacterSize(size);
    if (isUL)
        text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    else
        text.setStyle(sf::Text::Bold);
    setPosition(text, x, y);

    return text;
}
void readLeader() {
    std::ifstream file("files/leaderboard.txt");
    if (file.is_open())
    {
        string line;
        while (std::getline(file, line)) {
            ldrNames.push_back(line);
        }
    }
    std::sort(ldrNames.begin(), ldrNames.end());
    file.close();
}
void writeLeader() {
    std::ofstream file;
    file.open("files/leaderboard.txt", std::ofstream::out | std::ofstream::trunc);
    if (file.is_open())
        for (auto& name : ldrNames) {
            file << name;
            if (name != ldrNames.at(ldrNames.size() - 1))
                file << "\n";
        }

    file.close();
    *recentWin += "*";
}
void updateLeader(sf::Text& bt) {
    string output = "";
    sf::Text* winTxt = &bt;
    int width = getWidth(); int height = getHeight();
    for (int i = 0; i < ldrNames.size(); i++)
    {
        output += std::to_string(i + 1) + ".\t" + ldrNames.at(i).substr(0, 5) + "\t" + ldrNames.at(i).substr(6);
        if (i != ldrNames.size()) output += "\n\n";
    }

    winTxt->setString(output);
    sf::FloatRect box = winTxt->getLocalBounds();
    winTxt->setOrigin(box.left + box.width / 2.0f, box.top + box.height / 2.0f);
    winTxt->setPosition(16 * width / 2.0f, 16 * height / 2.0f + 20);
}




int main()
{
    int width, height, mineCount;
    ifstream file("files/board_config.cfg");
    if (file.is_open()) {
        string num;
        getline(file, num);
        width = stoi(num);
        getline(file, num);
        height = stoi(num);
        getline(file, num);
        mineCount = stoi(num);
        mineCount = width * height;
    }
    file.close();

    //load textures
    sf::Texture tile_hidden, tile_revealed, face_happy, face_lose, face_win, debug, pause, play, leader, flag, mine,
        number_1, number_2, number_3, number_4, number_5, number_6, number_7, number_8, digits;
    tile_hidden.loadFromFile("files/images/tile_hidden.png");
    tile_revealed.loadFromFile("files/images/tile_revealed.png");
    face_happy.loadFromFile("files/images/face_happy.png");
    face_lose.loadFromFile("files/images/face_lose.png");
    face_win.loadFromFile("files/images/face_win.png");
    debug.loadFromFile("files/images/debug.png");
    pause.loadFromFile("files/images/pause.png");
    play.loadFromFile("files/images/play.png");
    leader.loadFromFile("files/images/leaderboard.png");
    flag.loadFromFile("files/images/flag.png");
    mine.loadFromFile("files/images/mine.png");
    number_1.loadFromFile("files/images/number_1.png");
    number_2.loadFromFile("files/images/number_2.png");
    number_3.loadFromFile("files/images/number_3.png");
    number_4.loadFromFile("files/images/number_4.png");
    number_5.loadFromFile("files/images/number_5.png");
    number_6.loadFromFile("files/images/number_6.png");
    number_7.loadFromFile("files/images/number_7.png");
    number_8.loadFromFile("files/images/number_8.png");
    digits.loadFromFile("files/images/digits.png");



    createBoard();

    int fillCount = 0;
    for (unsigned int i = 0; i < width; i++) {
        for (unsigned int j = 0; j < height; j++) {
            if (gameBoard[i][j].isRevealed)
                revealedTilePos.push_back(fillCount);
            fillCount++;
        }
    }


    //sprites
    std::vector<sf::Sprite> hiddenTiles;
    sf::Sprite tile_hiddenSprite(tile_hidden);
    for (auto i = 0; i < height; i++) {
        for (auto j = 0; j < width; j++) {
            tile_hiddenSprite.setPosition(sf::Vector2f(j * tile_hiddenSprite.getGlobalBounds().width,
                i * tile_hiddenSprite.getGlobalBounds().height));
            hiddenTiles.push_back(tile_hiddenSprite);
        }
    }
    std::vector<sf::Sprite> revealedTiles;
    sf::Sprite tile_revealedSprite(tile_revealed);
    for (auto i = 0; i < height; i++) {
        for (auto j = 0; j < width; j++) {
            tile_revealedSprite.setPosition(sf::Vector2f(j * tile_revealedSprite.getGlobalBounds().width,
                i * tile_revealedSprite.getGlobalBounds().height));
            revealedTiles.push_back(tile_revealedSprite);
        }
    }
    sf::Sprite face_happySprite(face_happy);
    sf::Sprite face_loseSprite(face_lose);
    sf::Sprite face_winSprite(face_win);
    sf::Sprite debugSprite(debug);
    sf::Sprite pauseSprite(pause);
    sf::Sprite playSprite(play);
    sf::Sprite leaderSprite(leader);
    std::vector<sf::Sprite> flags;
    sf::Sprite flagSprite(flag);
    for (auto i = 0; i < 16; i++) {
        for (auto j = 0; j < 25; j++) {
            flagSprite.setPosition(sf::Vector2f(j * flagSprite.getGlobalBounds().width,
                i * flagSprite.getGlobalBounds().height));
            flags.push_back(flagSprite);
        }
    }
    //sprites create
    std::vector<sf::Sprite> mines, ones, twos, threes, fours, fives, sixes, sevens, eights;
    sf::Sprite mineSprite(mine), number_1Sprite(number_1), number_2Sprite(number_2), number_3Sprite(number_3), number_4Sprite(number_4), number_5Sprite(number_5), number_6Sprite(number_6), number_7Sprite(number_7), number_8Sprite(number_8);
    for (auto i = 0; i < 16; i++) {
        for (auto j = 0; j < 25; j++) {
            mineSprite.setPosition(sf::Vector2f(j * mineSprite.getGlobalBounds().width, i * mineSprite.getGlobalBounds().height));
            mines.push_back(mineSprite);

            number_1Sprite.setPosition(sf::Vector2f(j * number_1Sprite.getGlobalBounds().width, i * number_1Sprite.getGlobalBounds().height));
            ones.push_back(number_1Sprite);
            number_2Sprite.setPosition(sf::Vector2f(j * number_2Sprite.getGlobalBounds().width, i * number_2Sprite.getGlobalBounds().height));
            twos.push_back(number_2Sprite);
            number_3Sprite.setPosition(sf::Vector2f(j * number_3Sprite.getGlobalBounds().width, i * number_3Sprite.getGlobalBounds().height));
            threes.push_back(number_3Sprite);
            number_4Sprite.setPosition(sf::Vector2f(j * number_4Sprite.getGlobalBounds().width, i * number_4Sprite.getGlobalBounds().height));
            fours.push_back(number_4Sprite);
            number_5Sprite.setPosition(sf::Vector2f(j * number_5Sprite.getGlobalBounds().width, i * number_5Sprite.getGlobalBounds().height));
            fives.push_back(number_5Sprite);
            number_6Sprite.setPosition(sf::Vector2f(j * number_6Sprite.getGlobalBounds().width, i * number_6Sprite.getGlobalBounds().height));
            sixes.push_back(number_6Sprite);
            number_7Sprite.setPosition(sf::Vector2f(j * number_7Sprite.getGlobalBounds().width, i * number_7Sprite.getGlobalBounds().height));
            sevens.push_back(number_7Sprite);
            number_8Sprite.setPosition(sf::Vector2f(j * number_8Sprite.getGlobalBounds().width, i * number_8Sprite.getGlobalBounds().height));
            eights.push_back(number_8Sprite);
        }
    }

    std::vector<sf::Sprite> digPos;
    std::vector<sf::Sprite> digNeg;
    sf::Sprite digitsSprite(digits);
    for (auto i = 0; i < 10; i++) {
        if (i == 0) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(51, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

        }
        if (i == 1) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
        }
        if (i == 2) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
        }
        if (i == 3) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
        }
        if (i == 4) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
        }
        if (i == 5) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
        }
        if (i == 6) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
        }
        if (i == 7) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
        }
        if (i == 8) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
        }
        if (i == 9) {
            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect((21 * i), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
        }
    }
    for (auto i = 0; i < 10; i++) {
        if (i == 0) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
        if (i == 1) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
        if (i == 2) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
        if (i == 3) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
        if (i == 4) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
        if (i == 5) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
        if (i == 6) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
        if (i == 7) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
        if (i == 8) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
        if (i == 9) {
            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(33, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(54, 32 * (height + 0.5f) + 16));
            digPos.push_back(digitsSprite);
            digNeg.push_back(digitsSprite);

            digitsSprite.setTextureRect(sf::IntRect(((21 * i) + 21), 0, 21, 32));
            digitsSprite.setPosition(sf::Vector2f(75, 32 * (height + 0.5f) + 16));
            digNeg.push_back(digitsSprite);
        }
    }


    sf::RenderWindow welcome(sf::VideoMode(32 * width, 32 * height + 100), "Minesweeper");
    welcome.setFramerateLimit(60);
    sf::Font font;
    font.loadFromFile("files/font.ttf");

    sf::Text greet = setText("WELCOME TO MINESWEEPER!", 24, 32 * width, 32 * height / 2.0f - 150, font, true);
    sf::Text ask = setText("Enter your name:", 20, 32 * width, 32 * height / 2.0f - 75, font, false);
    sf::Text name = setText("", 18, 32 * width, 32 * height / 2.0f - 45, font, false);
    name.setFillColor(sf::Color::Yellow);
    std::string userName = "";

    while (welcome.isOpen())
    {
        sf::Event event;
        while (welcome.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                welcome.close();
            if (event.type == sf::Event::TextEntered && userName.size() < 10)
            {
                char letter = '0';
                if (event.text.unicode < 128)
                    letter = static_cast<char>(event.text.unicode);

                if (isalpha(letter)) {
                    if (userName.length() == 0) userName += toupper(letter);
                    else  userName += tolower(letter);
                }

            }
            if (event.type == sf::Event::KeyPressed && userName.size() > 0)
            {
                if (event.key.code == sf::Keyboard::Backspace)
                    userName.pop_back();

                else if (event.key.code == sf::Keyboard::Enter) {
                    welcome.close();
                    sf::RenderWindow window(sf::VideoMode(32 * width, 32 * height + 100), "Minesweeper");
                    window.setFramerateLimit(60);

                    while (window.isOpen())
                    {
                        sf::Event event;
                        while (window.pollEvent(event))
                        {
                            if (event.type == sf::Event::Closed)
                                window.close();

                            //if button is pressed
                            if (event.type == sf::Event::MouseButtonPressed) {
                                //if left button
                                if (event.mouseButton.button == sf::Mouse::Left) {
                                    //get position
                                    auto mousePosition = sf::Mouse::getPosition(window);

                                    //if click debug mode
                                    if (debugSprite.getGlobalBounds().contains(sf::Vector2f(mousePosition.x, mousePosition.y))) {
                                        debugOn = !debugOn;
                                    }
                                    // if pause pressed
                                    if (pauseSprite.getGlobalBounds().contains(sf::Vector2f(mousePosition.x, mousePosition.y))) {
                                        pauseOn = !pauseOn;
                                    }
                                    //press ldrboard
                                    if (leaderSprite.getGlobalBounds().contains(sf::Vector2f(mousePosition.x, mousePosition.y))) {
                                        sf::RenderWindow lead(sf::VideoMode(16 * width, 16 * height + 50), "Minesweeper");
                                        lead.setFramerateLimit(60);
                                        sf::Font font;
                                        font.loadFromFile("files/font.ttf");
                                        sf::Text leader = setText("LEADERBOARD", 20, 16 * width, 16 * height / 2.0f - 120, font, true);
                                        sf::Text top = setText("", 18, 16 * width, 16 * height / 2.0f + 20, font, false);

                                        readLeader();
                                        updateLeader(top);


                                        while (lead.isOpen())
                                        {
                                            sf::Event event;
                                            while (lead.pollEvent(event))
                                            {
                                                if (event.type == sf::Event::Closed) {
                                                    lead.close();
                                                }
                                            }
                                            lead.clear(sf::Color(0, 0, 255));
                                            lead.draw(leader);
                                            lead.draw(top);
                                            lead.display();
                                        }
                                    }
                                    if (winning) {
                                        //click on tile (reveal it)
                                        for (int i = 0; i < hiddenTiles.size(); i++) {
                                            if (hiddenTiles[i].getGlobalBounds().contains(sf::Vector2f(mousePosition.x, mousePosition.y))) {
                                                clickTile(i);
                                            }
                                        }
                                    }

                                    //hit a mine
                                    for (unsigned int i = 0; i < revealedTiles.size(); i++) {
                                        if (revealedTiles[i].getGlobalBounds().contains(sf::Vector2f(mousePosition.x, mousePosition.y))) {
                                            for (int j = 0; j < minePos.size(); j++) {
                                                if (minePos[j] == i) {
                                                    winning = false;
                                                }
                                            }
                                        }
                                    }


                                    if (face_happySprite.getGlobalBounds().contains(sf::Vector2f(mousePosition.x, mousePosition.y))) {
                                        createBoard();
                                    }
                                }

                                //if right button
                                else if (event.mouseButton.button == sf::Mouse::Right) {
                                    //get position
                                    auto mousePosition = sf::Mouse::getPosition(window);

                                    if (winning) {
                                        //flag stuff
                                        int sir = 0;
                                        for (unsigned int i = 0; i < hiddenTiles.size(); i++) {
                                            if (hiddenTiles[i].getGlobalBounds().contains(sf::Vector2f(mousePosition.x, mousePosition.y))) {
                                                for (unsigned int o = 0; o < height; o++) {
                                                    for (unsigned int j = 0; j < width; j++) {
                                                        if (gameBoard[j][o].flagUp && sir == i) {
                                                            int addThis = 0;
                                                            int addToIter = 0;
                                                            for (unsigned int u = 0; u < flagPos.size(); u++) {
                                                                if (flagPos[u] == i) {
                                                                    addThis = addToIter;
                                                                }
                                                                addToIter++;
                                                            }
                                                            flagPos.erase(flagPos.begin() + addThis);
                                                            gameBoard[j][o].flagUp = false;
                                                        }
                                                        else if (!gameBoard[j][o].flagUp && sir == i) {
                                                            int countF = 0;
                                                            for (unsigned int k = 0; k < height; k++) {
                                                                for (unsigned int j = 0; j < width; j++) {
                                                                    if ((countF == i) && (!gameBoard[j][k].isRevealed)) {
                                                                        flagPos.push_back(i);
                                                                        gameBoard[j][k].flagUp = true;
                                                                    }
                                                                    countF++;
                                                                }
                                                            }
                                                        }
                                                        sir++;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        //determines if gameWon
                        int finalCounter = 0;
                        for (unsigned int i = 0; i < height; i++) {
                            for (unsigned int j = 0; j < width; j++) {
                                if (gameBoard[j][i].hasMine || gameBoard[j][i].isRevealed)
                                    finalCounter++;
                            }
                        }
                        if (finalCounter == 400) {
                            gameWon = true;
                        }

                        //only do once
                        window.clear();

                        //draw revealed tiles
                        for (unsigned int i = 0; i < revealedTiles.size(); i++) {
                            window.draw(revealedTiles[i]);
                        }

                        //draw mines
                        int mineCountDrawing = 0;
                        for (unsigned int i = 0; i < height; i++) {
                            for (unsigned int j = 0; j < width; j++) {
                                if (gameBoard[j][i].hasMine) {
                                    window.draw(mines[mineCountDrawing]);
                                }
                                mineCountDrawing++;
                            }
                        }

                        //draw neighbor numbers
                        int numCountDrawing = 0;
                        for (unsigned int i = 0; i < height; i++) {
                            for (unsigned int j = 0; j < width; j++) {
                                if (gameBoard[j][i].neighborNum == 1) {
                                    window.draw(ones[numCountDrawing]);
                                }
                                else if (gameBoard[j][i].neighborNum == 2) {
                                    window.draw(twos[numCountDrawing]);
                                }
                                else if (gameBoard[j][i].neighborNum == 3) {
                                    window.draw(threes[numCountDrawing]);
                                }
                                else if (gameBoard[j][i].neighborNum == 4) {
                                    window.draw(fours[numCountDrawing]);
                                }
                                else if (gameBoard[j][i].neighborNum == 5) {
                                    window.draw(fives[numCountDrawing]);
                                }
                                else if (gameBoard[j][i].neighborNum == 6) {
                                    window.draw(sixes[numCountDrawing]);
                                }
                                else if (gameBoard[j][i].neighborNum == 7) {
                                    window.draw(sevens[numCountDrawing]);
                                }
                                else if (gameBoard[j][i].neighborNum == 8) {
                                    window.draw(eights[numCountDrawing]);
                                }
                                numCountDrawing++;
                            }
                        }

                        //draw hidden tiles
                        int tileIter = 0;
                        for (unsigned int i = 0; i < height; i++) {
                            for (unsigned int j = 0; j < width; j++) {
                                if (!gameBoard[j][i].isRevealed)
                                    window.draw(hiddenTiles[tileIter]);
                                tileIter++;
                            }
                        }

                        //draw mines on all, debugOn
                        if (debugOn || !winning) {
                            int mineCountDrawingInner = 0;
                            for (unsigned int i = 0; i < height; i++) {
                                for (unsigned int j = 0; j < width; j++) {
                                    if (gameBoard[j][i].hasMine) {
                                        window.draw(mines[mineCountDrawingInner]);
                                    }
                                    mineCountDrawingInner++;
                                }
                            }
                        }

                        //draw flags
                        int flagCountDrawing = 0;
                        for (unsigned int i = 0; i < height; i++) {
                            for (unsigned int j = 0; j < width; j++) {
                                for (unsigned int k = 0; k < flagPos.size(); k++) {
                                    if (flagPos[k] == flagCountDrawing) {
                                        window.draw(flags[flagCountDrawing]);
                                    }
                                }
                                flagCountDrawing++;
                            }
                        }

                        //draw counter
                        //count mines
                        int mineCount = 0;
                        for (int i = 0; i < height; i++) {
                            for (int j = 0; j < width; j++) {
                                if (gameBoard[j][i].hasMine) {
                                    mineCount++;
                                }
                            }
                        }
                        //pos
                        if ((int)(mineCount - flagPos.size()) >= 0) {
                            //determine each digit
                            int num = (int)(mineCount - flagPos.size());
                            int onesDigit = (num % 10);
                            int tensDigit = ((num / 10) % 10);

                            window.draw(digPos[0]);
                            if (tensDigit == 9) {
                                window.draw(digPos[28]);
                            }
                            if (tensDigit == 8) {
                                window.draw(digPos[25]);
                            }
                            if (tensDigit == 7) {
                                window.draw(digPos[22]);
                            }
                            if (tensDigit == 6) {
                                window.draw(digPos[19]);
                            }
                            if (tensDigit == 5) {
                                window.draw(digPos[16]);
                            }
                            if (tensDigit == 4) {
                                window.draw(digPos[13]);
                            }
                            if (tensDigit == 3) {
                                window.draw(digPos[10]);
                            }
                            if (tensDigit == 2) {
                                window.draw(digPos[7]);
                            }
                            if (tensDigit == 1) {
                                window.draw(digPos[4]);
                            }
                            if (tensDigit == 0) {
                                window.draw(digPos[1]);
                            }
                            if (onesDigit == 9) {
                                window.draw(digPos[29]);
                            }
                            if (onesDigit == 8) {
                                window.draw(digPos[26]);
                            }
                            if (onesDigit == 7) {
                                window.draw(digPos[23]);
                            }
                            if (onesDigit == 6) {
                                window.draw(digPos[20]);
                            }
                            if (onesDigit == 5) {
                                window.draw(digPos[17]);
                            }
                            if (onesDigit == 4) {
                                window.draw(digPos[14]);
                            }
                            if (onesDigit == 3) {
                                window.draw(digPos[11]);
                            }
                            if (onesDigit == 2) {
                                window.draw(digPos[8]);
                            }
                            if (onesDigit == 1) {
                                window.draw(digPos[5]);
                            }
                            if (onesDigit == 0) {
                                window.draw(digPos[2]);
                            }
                        }
                        //negative number
                        else {
                            //determine each digit
                            int num = ((int)(mineCount - flagPos.size()) * (-1));
                            int onesDigit = (num % 10);
                            int tensDigit = ((num / 10) % 10);
                            int hundredsDigit = ((num / 100) % 10);

                            window.draw(digPos[30]);
                            if (hundredsDigit == 9) {
                                digPos[27].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[27]);
                            }
                            if (hundredsDigit == 8) {
                                digPos[24].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[24]);
                            }
                            if (hundredsDigit == 7) {
                                digPos[21].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[21]);
                            }
                            if (hundredsDigit == 6) {
                                digPos[18].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[18]);
                            }
                            if (hundredsDigit == 5) {
                                digPos[15].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[15]);
                            }
                            if (hundredsDigit == 4) {
                                digPos[12].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[12]);
                            }
                            if (hundredsDigit == 3) {
                                digPos[9].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[9]);
                            }
                            if (hundredsDigit == 2) {
                                digPos[6].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[6]);
                            }
                            if (hundredsDigit == 1) {
                                digPos[3].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[3]);
                            }
                            if (hundredsDigit == 0) {
                                digPos[0].setPosition(sf::Vector2f(21, (16 * 32)));
                                window.draw(digPos[0]);
                            }
                            if (tensDigit == 9) {
                                digPos[28].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[28]);
                            }
                            if (tensDigit == 8) {
                                digPos[25].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[25]);
                            }
                            if (tensDigit == 7) {
                                digPos[22].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[22]);
                            }
                            if (tensDigit == 6) {
                                digPos[19].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[19]);
                            }
                            if (tensDigit == 5) {
                                digPos[16].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[16]);
                            }
                            if (tensDigit == 4) {
                                digPos[13].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[13]);
                            }
                            if (tensDigit == 3) {
                                digPos[10].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[10]);
                            }
                            if (tensDigit == 2) {
                                digPos[7].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[7]);
                            }
                            if (tensDigit == 1) {
                                digPos[4].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[4]);
                            }
                            if (tensDigit == 0) {
                                digPos[1].setPosition(sf::Vector2f(42, (16 * 32)));
                                window.draw(digPos[1]);
                            }
                            if (onesDigit == 9) {
                                digPos[29].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[29]);
                            }
                            if (onesDigit == 8) {
                                digPos[26].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[26]);
                            }
                            if (onesDigit == 7) {
                                digPos[23].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[23]);
                            }
                            if (onesDigit == 6) {
                                digPos[20].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[20]);
                            }
                            if (onesDigit == 5) {
                                digPos[17].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[17]);
                            }
                            if (onesDigit == 4) {
                                digPos[14].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[14]);
                            }
                            if (onesDigit == 3) {
                                digPos[11].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[11]);
                            }
                            if (onesDigit == 2) {
                                digPos[8].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[8]);
                            }
                            if (onesDigit == 1) {
                                digPos[5].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[5]);
                            }
                            if (onesDigit == 0) {
                                digPos[2].setPosition(sf::Vector2f(63, (16 * 32)));
                                window.draw(digPos[2]);
                            }
                        }

                        //winning counter
                        if (gameWon) {
                            window.draw(digPos[0]);
                            window.draw(digPos[1]);
                            window.draw(digPos[2]);
                        }

                        //draw smiley face
                        face_happySprite.setPosition(sf::Vector2f((width / 2.0) * 32 - 32, 32 * (height + 0.5f)));
                        window.draw(face_happySprite);

                        //draw lose face (if game lost)
                        if (!winning) {
                            face_loseSprite.setPosition(sf::Vector2f((width / 2.0) * 32 - 32, 32 * (height + 0.5f)));
                            window.draw(face_loseSprite);
                        }
                        int timer = 0;
                        //draw victory face (if game won)
                        if (gameWon) {
                            face_winSprite.setPosition(sf::Vector2f((width / 2.0) * 32 - 32, 32 * (height + 0.5f)));
                            window.draw(face_winSprite);

                            //add leaderboard
                            int min = timer / 60;
                            int sec = timer - (min * 60);
                            string mL = std::to_string(min / 10 % 10);
                            string mR = std::to_string(min % 10);
                            string sL = std::to_string(sec / 10 % 10);
                            string sR = std::to_string(sec % 10);
                            string newTime = mL + mR + ":" + sL + sR + ',' + userName;

                            if (ldrNames.size() == 0) {
                                ldrNames.push_back(newTime);
                                recentWin = &ldrNames.at(0);
                                writeLeader();
                            }
                            else {
                                for (int i = 0; i < ldrNames.size(); i++) {
                                    if (newTime < ldrNames.at(i)) {
                                        if (recentWin != nullptr) recentWin->pop_back();
                                        ldrNames.insert(ldrNames.begin() + i, newTime);
                                        if (ldrNames.size() > 5)ldrNames.pop_back();
                                        recentWin = &ldrNames.at(i);
                                        writeLeader();
                                    }
                                }

                                if (ldrNames.size() < 5) {
                                    if (recentWin != nullptr) recentWin->pop_back();
                                    ldrNames.push_back(newTime);
                                    recentWin = &ldrNames.at(ldrNames.size() - 1);
                                    writeLeader();
                                }
                            }
                        }

                        //UPDATE LEADER


                        //draw debug button
                        debugSprite.setPosition(sf::Vector2f(width * 32 - 304, 32 * (height + 0.5f)));
                        window.draw(debugSprite);

                        //draw play
                        if (pauseOn) {
                            playSprite.setPosition(sf::Vector2f(width * 32 - 240, 32 * (height + 0.5f)));
                            window.draw(playSprite);
                            //reveal all
                            for (unsigned int i = 0; i < revealedTiles.size(); i++) {
                                window.draw(revealedTiles[i]);
                            }

                        }
                        //draw pause
                        if (!pauseOn) {
                            pauseSprite.setPosition(sf::Vector2f(width * 32 - 240, 32 * (height + 0.5f)));
                            window.draw(pauseSprite);
                        }

                        //draw leaderbutton
                        leaderSprite.setPosition(sf::Vector2f(width * 32 - 176, 32 * (height + 0.5f)));
                        window.draw(leaderSprite);

                        //draw clock

                        //only do once
                        window.display();
                    }

                }
            }
        }
        welcome.clear(sf::Color(0, 0, 255));
        welcome.draw(greet);
        welcome.draw(ask);
        name.setString(userName + "|");
        setPosition(name, 32 * width, 32 * height / 2.0f - 45);
        welcome.draw(name);
        welcome.display();
    }

    return 0;
}
