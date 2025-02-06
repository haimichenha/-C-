#pragma once
#include <cstdio>
#include <algorithm>
#include <cctype>

class Room
{
public:
    enum class Model
    {
        easy = 0,
        hard = 1,
        model2_5 = 2
    };
    
    enum class Player
    {
        none = '-',
        ren = 'X',      
        computer = 'O'
    };

    struct Position {
        int i, j;
    };

    enum class Condition {
        ren = 0,        
        computer = 1,
        draw = 2,
        unsure = 3
    };

    enum class Turn
    {
        ren = 0,      
        computer = 1
    };

    Room(char i);
    Room(char i, Model m); // 添加设置模式的构造函数

    // 打印棋盘
    void display();

    bool isblank(Position point);
    bool isblank(int i, int j);

    int move(Position point);  // 恢复原始的move函数声明

    Condition evaluate();
    Position BestMove();

    Turn getter() {        // 该函数定义时已经是实现。内联成员函数
        return turn;
    }

    Model getModel() { // 添加获取当前模式的函数
        return model;
    }

private:
    Player board[3][3];
    Turn turn;
    int time;
    Model model;
    int moveCount;  // 添加计数器来跟踪玩家移动次数
    int playerMoves;    // 添加玩家步数计数器
    int computerMoves;  // 添加电脑步数计数器

    void init();
    int minimax(int depth, int alpha, int beta);
};
