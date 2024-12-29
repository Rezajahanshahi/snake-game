#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

const int height = 26, width = 32;
enum direction { Stop = 0, Up, Down, Left, Right } dir;

class Snake;
class Snakes_Food;
void input();
bool Game_over(Snake& s);
void show(Snake& s, Snakes_Food& f);
void High_Score(int);
void clearScreen(HANDLE hConsole);
void drawInitialState(HANDLE hConsole);
void updateScreen(HANDLE hConsole, Snake& s, Snakes_Food& f);


class Snake
{
protected:
    int score;
    int pos[2];
    vector<vector<int>> tail;

public:
    Snake()
    {
        score = 0;
        pos[0] = height / 2;
        pos[1] = width / 2;
    }

    void movement();
    void updateTail();
    friend void show(Snake& s, Snakes_Food& f);
    friend bool Game_over(Snake& s);

    int* getPos()
    {
        return pos;
    }

    void growTail()
    {
        tail.push_back({ pos[0], pos[1] });
    }

    int getScore() const
    {
        return score;
    }

    void increaseScore(int points)
    {
        score += points;
    }
};

class Snakes_Food
{
public:
    int pos[2];

    Snakes_Food()
    {
        generate_new_food();
    }

    void generate_new_food()
    {
        pos[0] = rand() % (height - 2) + 1;
        pos[1] = rand() % (width - 2) + 1;
    }

    bool Eat(Snake& s)
    {
        return (s.getPos()[0] == pos[0] && s.getPos()[1] == pos[1]);
    }
};

void Snake::movement()
{
    updateTail();

    switch (dir)
    {
    case Up:
        pos[0] = (pos[0] - 1) % height;
        break;
    case Down:
        pos[0] = (pos[0] + 1) % height;
        break;
    case Left:
        pos[1] = (pos[1] - 1) % width;
        break;
    case Right:
       pos[1] = (pos[1] + 1) % width;
        break;
    default:
        break;
    }
}

void Snake::updateTail()
{
    if (tail.empty()) return;

    for (int i = tail.size() - 1; i > 0; --i)
    {
        tail[i] = tail[i - 1];
    }

    tail[0] = { pos[0], pos[1] };
}

int main()
{
    srand(time(NULL));
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    cout << "Press any key to start the game!";
    while (!_kbhit()) {};
    _getche();
    drawInitialState(hConsole);

    Snake s;
    Snakes_Food f;
    int initialSleepTime = 200;
    double decayRate = 0.9;

    int sleepTime = initialSleepTime;

    while (!Game_over(s))
    {
        updateScreen(hConsole, s, f);
        input();
        s.movement();

        if (f.Eat(s))
        {
            f.generate_new_food();
            s.growTail();
            s.increaseScore(10);

            sleepTime = max(50, static_cast<int>(initialSleepTime * pow(decayRate, s.getScore() / 10)));
        }

        Sleep(sleepTime);
    }
    system("CLS");
    High_Score(s.getScore());
    cout << "Game Over!" << endl;

    return 0;
}


void input()
{
    if (kbhit())
    {
        switch (tolower(_getch()))
        {
        case 'w':
            dir = Up;
            break;
        case 's':
            dir = Down;
            break;
        case 'a':
            dir = Left;
            break;
        case 'd':
            dir = Right;
            break;
        default:
            break;
        }
    }
}

bool Game_over(Snake& s)
{
    //if (s.pos[0] == 0 || s.pos[0] == height - 1 || s.pos[1] == 0 || s.pos[1] == width - 1)
        //return true;
    return false;
}

void show(Snake& s, Snakes_Food& f)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i == s.getPos()[0] && j == s.getPos()[1])
            {
                cout << "O";
            }
            else if (i == f.pos[0] && j == f.pos[1])
            {
                cout << "*";
            }
            else if (i == 0 || i == height - 1)
            {
                cout << "-";
            }
            else if (j == 0 || j == width - 1)
            {
                cout << "|";
            }
            else
            {
                bool isTailPart = false;
                for (const auto& segment : s.tail)
                {
                    if (segment[0] == i && segment[1] == j)
                    {
                        isTailPart = true;
                        break;
                    }
                }

                if (isTailPart)
                {
                    cout << "o";
                }
                else
                    cout << " ";
            }
        }
        cout << endl;
    }
}

void High_Score(int score)
{
    int Highscore;
    fstream file("high.txt", ios::in);
    if (file >> Highscore)
    {
        if (score > Highscore)
        {
            cout << "You've reached a new high score!" << endl << "Your score is: " << score << ". High score was: " << Highscore << endl;
            file.close();
            file.open("high.txt", ios::out | ios::trunc);
            file << score;
        }
        else
        {
            cout << "Your score is: " << score <<endl<< "High score is: " << Highscore << endl;
        }
    }
    else
    {
        cout << "Your score is: " << score << ". No previous high score found." << endl;
        file.close();
        file.open("high.txt", ios::out);
        file << score;
    }
    file.close();
}
void clearScreen(HANDLE hConsole)
{
    COORD coord = {0, 0};
    SetConsoleCursorPosition(hConsole, coord);
}

void drawInitialState(HANDLE hConsole)
{
    clearScreen(hConsole);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i == 0 || i == height - 1)
                cout << "-";
            else if (j == 0 || j == width - 1)
                cout << "|";
            else
                cout << " ";
        }
        cout << endl;
    }
}

void updateScreen(HANDLE hConsole, Snake& s, Snakes_Food& f)
{
    clearScreen(hConsole);
    show(s, f);
    cout << endl << "Your current score is: " << s.getScore() << endl;
}
