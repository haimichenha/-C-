#define _CRT_SECURE_NO_WARNINGS
#include "room.h"
#include <cstdio>
#include <cctype>

Room::Position ren(Room& room) {    // 改函数名 man 为 ren
    char c;
    do {
        printf("Please enter your choose %c: ", Room::Player::ren);  
        scanf("%c", &c);
        while (getchar() != '\n');
        printf("\n");
    } while (!((c >= '1' && c <= '9') && room.isblank((c - '1') / 3, (c - '1') % 3)));
    Room::Position point;
    point.i = (c - '1') / 3;
    point.j = (c - '1') % 3;
    return point;
}

int main() {
    char m;
    printf("Please choose game mode (E for easy, H for hard, O for special mode 2.5): \n");
    do {
        scanf("%c", &m);
        while (getchar() != '\n');
        printf("\n");
    } while (!(tolower(m) == 'e' || tolower(m) == 'h' || tolower(m) == 'o'));
    
    Room::Model mode;
    if (tolower(m) == 'e')
        mode = Room::Model::easy;
    else if (tolower(m) == 'h')
        mode = Room::Model::hard;
    else
        mode = Room::Model::model2_5;
    
    char c;
    printf("Please enter computer first(enter C or c) or Player first(enter R or r)?\n"); 
    do {
        scanf("%c", &c);
        while (getchar() != '\n');
        printf("\n");
    } while (!(tolower(c) == 'r' || tolower(c) == 'c'));  


    Room room(c, mode);              // 对输入的字符创建对象，对象传到Room.cpp Room函数

    room.display();           // 生成棋盘

    while (true) {
        if (room.getter() == Room::Turn::ren) {    
            Room::Position point = ren(room);       
            room.move(point);  // 恢复原始的move调用
        } else {
            Room::Position bestpoint = room.BestMove();
            room.move(bestpoint);
            printf("Computer moves to: %d\n\n", bestpoint.i * 3 + bestpoint.j + 1);
        }
        
        room.display();
        
        
                switch (room.evaluate()) {
        case Room::Condition::computer:
            if (mode == Room::Model::model2_5) {
                printf("li bu li hai ni kun ge!!\n");
            } else {
                printf("Computer wins!!\n");
            }
            return 0;

        case Room::Condition::ren:                 
            if (mode == Room::Model::model2_5) {
                printf("ni gan ma ai yo~\n");
            } else {
                printf("Ren wins!!\n");                
            }
            return 0;

        case Room::Condition::draw:
            printf("Same Power!!\n");
            return 0;

        default:
            break;
        }
    }
}
