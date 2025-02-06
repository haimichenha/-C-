#include "room.h"

Room::Room(char i, Model m) {
    this->init();
    this->model = m;
    if (tolower(i) == 'r')
        this->turn = Turn::ren;    
    else
        this->turn = Turn::computer;
}

void Room::init() {                     
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            this->board[i][j] = Player::none;
    
    this->time = 0;
    this->moveCount = 0;
    this->playerMoves = 0;
    this->computerMoves = 0;
}

int Room::minimax(int depth, int alpha, int beta) {
    Condition current = evaluate();
    if (current == Condition::computer) return 10 - depth;
    else if (current == Condition::ren) return -10 + depth;
    else if (current == Condition::draw) return -depth;
    if (this->turn == Turn::computer) {
        int best = -1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (this->board[i][j] == Player::none) {
                    this->board[i][j] = Player::computer;
                    this->turn = Turn::ren;
                    this->time++;
                    best = std::max(best, minimax(depth + 1, alpha, beta));
                    this->board[i][j] = Player::none;
                    this->turn = Turn::computer;
                    this->time--;
                    alpha = std::max(alpha, best);
                    if (beta <= alpha) break; // 被阻拦后得到的最高分小于没有限制的最低分
                }
            }
            if (beta <= alpha) //当前行
                break; // Beta 剪枝
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (this->board[i][j] == Player::none) {
                    this->board[i][j] = Player::ren;
                    this->turn = Turn::computer;
                    this->time++;
                    best = std::min(best, minimax(depth + 1, alpha, beta));
                    this->board[i][j] = Player::none;
                    this->turn = Turn::ren;
                    this->time--;
                    beta = std::min(beta, best);
                    if (beta <= alpha) break;
                }
            }
            if (beta <= alpha) break;
        }
        return best;
    }
}

Room::Position Room::BestMove() {
    Position bestmove{-1, -1};
    
    // 简单模式和模式2.5：随机选择空位
    if (this->model == Model::easy || this->model == Model::model2_5) {
        int empty_count = 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (this->board[i][j] == Player::none) {
                    empty_count++;
                }
            }
        }
        if (empty_count > 0) {
            int random_pos = rand() % empty_count;
            empty_count = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (this->board[i][j] == Player::none) {
                        if (empty_count == random_pos) {
                            bestmove.i = i;
                            bestmove.j = j;
                            return bestmove;
                        }
                        empty_count++;
                    }
                }
            }
        }
        return bestmove;
    }
    
    // 困难模式：使用minimax算法
    int score = -2000;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (this->board[i][j] == Player::none) {
                this->board[i][j] = Player::computer;
                this->turn = Turn::ren;
                this->time++;
                int temp = minimax(0, -1000, 1000); // 初始调用时设置 alpha=-1000, beta=1000
                if (temp > score) {
                    score = temp;
                    bestmove.i = i;
                    bestmove.j = j;
                }
                this->board[i][j] = Player::none;
                this->turn = Turn::computer;
                this->time--;
            }
        }
    }
    return bestmove;
}

int Room::move(Position point) {
    if (!isblank(point))
        return 0;

    if (this->turn == Turn::computer) {
        this->board[point.i][point.j] = Player::computer;
        this->turn = Turn::ren;    
        this->computerMoves++;
    } else {
        if (this->model == Model::model2_5) {
            switch (moveCount % 3) {
                case 0: this->board[point.i][point.j] = static_cast<Player>('N'); break;
                case 1: this->board[point.i][point.j] = static_cast<Player>('G'); break;
                case 2: this->board[point.i][point.j] = static_cast<Player>('M'); break;
            }
            moveCount++;
        } else {
            this->board[point.i][point.j] = Player::ren;
        }
        this->turn = Turn::computer;
        this->playerMoves++;
    }
    this->time++;
    return 1;
}

Room::Condition Room::evaluate() {
    // 先检查常规胜利条件
    for (int i = 0; i < 3; i++) {
        if (this->board[i][0] != Player::none && this->board[i][0] == this->board[i][1] && this->board[i][0] == this->board[i][2]) {
            if (this->board[i][0] == Player::computer) return Condition::computer;
            else if (this->board[i][0] == Player::ren) return Condition::ren;
        }
        if (this->board[0][i] != Player::none && this->board[0][i] == this->board[1][i] && this->board[0][i] == this->board[2][i]) {
            if (this->board[0][i] == Player::computer) return Condition::computer;
            else if (this->board[0][i] == Player::ren) return Condition::ren;
        }
    }
    if (this->board[0][0] != Player::none && this->board[0][0] == this->board[1][1] && this->board[0][0] == this->board[2][2]) {
        if (this->board[0][0] == Player::computer) return Condition::computer;
        else if (this->board[0][0] == Player::ren) return Condition::ren;
    }
    if (this->board[0][2] != Player::none && this->board[0][2] == this->board[1][1] && this->board[0][2] == this->board[2][0]) {
        if (this->board[0][2] == Player::computer) return Condition::computer;
        else if (this->board[0][2] == Player::ren) return Condition::ren;
    }
    // 检查特殊模式的NGM连线（任意顺序）
    if (this->model == Model::model2_5) {
        // 检查行
        for (int i = 0; i < 3; i++) {
            bool hasN = false, hasG = false, hasM = false;
            for (int j = 0; j < 3; j++) {
                char c = static_cast<char>(this->board[i][j]);
                if (c == 'N') hasN = true;
                if (c == 'G') hasG = true;
                if (c == 'M') hasM = true;
            }
            if (hasN && hasG && hasM) return Condition::ren;
        }
        
        // 检查列
        for (int j = 0; j < 3; j++) {
            bool hasN = false, hasG = false, hasM = false;
            for (int i = 0; i < 3; i++) {
                char c = static_cast<char>(this->board[i][j]);
                if (c == 'N') hasN = true;
                if (c == 'G') hasG = true;
                if (c == 'M') hasM = true;
            }
            if (hasN && hasG && hasM) return Condition::ren;
        }
        
        // 检查主对角线
        {
            bool hasN = false, hasG = false, hasM = false;
            for (int i = 0; i < 3; i++) {
                char c = static_cast<char>(this->board[i][i]);
                if (c == 'N') hasN = true;
                if (c == 'G') hasG = true;
                if (c == 'M') hasM = true;
            }
            if (hasN && hasG && hasM) return Condition::ren;
        }
        
        // 检查副对角线
        {
            bool hasN = false, hasG = false, hasM = false;
            for (int i = 0; i < 3; i++) {
                char c = static_cast<char>(this->board[i][2-i]);
                if (c == 'N') hasN = true;
                if (c == 'G') hasG = true;
                if (c == 'M') hasM = true;
            }
            if (hasN && hasG && hasM) return Condition::ren;
        }
    }

    if (this->time == 9) return Condition::draw;
    else return Condition::unsure;
}

void Room::display() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (this->board[i][j] >= static_cast<Player>('A') && 
                this->board[i][j] <= static_cast<Player>('Z')) {
                printf(" %c ", static_cast<char>(this->board[i][j]));
            } else {
                switch (this->board[i][j]) {
                    case Player::none: printf(" %d ", i * 3 + j + 1); break;
                    case Player::computer: printf(" O "); break;
                    case Player::ren: printf(" X "); break;
                }
            }
            if (j < 2) printf("|"); else printf("\n");
        }
        if (i < 2) printf("---+---+---\n"); else printf("\n");
    }
}

bool Room::isblank(Position point) {
    return this->board[point.i][point.j] == Player::none;
}

bool Room::isblank(int i, int j) {
    Position p{i, j};
    return isblank(p);
}