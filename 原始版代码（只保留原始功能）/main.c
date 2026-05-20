 /* mmsystem.h这个库是用于调用C的music库来播放音乐的 (MCI) */
//声音引用之前配置教学：项目 -->项目属性 -->参数 -->连接 -->在后面加入参数-lwinmm
// 或者修改 LIBS行
// 修改前：
//LIBS     = -L"D:/Program Files/Dev-Cpp/MinGW64/lib" -L"D:/Program Files/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib" -static-libgcc

// 修改后：
//LIBS     = -L"D:/Program Files/Dev-Cpp/MinGW64/lib" -L"D:/Program Files/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib" -static-libgcc -lwinmm 

//如果不行就绝对路径 
// LIBS = -L"D:/Program Files/Dev-Cpp/MinGW64/lib" -L"D:/Program Files/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib" -static-libgcc -lwinmm -l"winmm"
//如果还不行就去看看 在Dev-C++安装目录中 winmm.lib 文件是不是没了 
//不想拿这分当我没说 


/* ============================================================
 * 贪吃蛇游戏 - 详细注释版
 * 本注释旨在帮助学生理解代码结构和关键逻辑
 * 请勿直接复制，要理解每个步骤的作用
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <string.h> 
#include <mmsystem.h>

/* ==================== 声音库配置说明 ==================== */
// 需要在项目属性 -> 连接器 -> 附加依赖项中添加 winmm.lib
// 或者在LIBS行添加 -lwinmm
#pragma comment(lib, "winmm.lib") 

/* ==================== 游戏常量定义 ==================== */
// 这些常量定义了游戏的基本参数和状态
#define GameSnake 1        // 主循环控制
#define UP     0           // 方向控制：上
#define DOWN   1           // 下
#define LEFT   2           // 左
#define RIGHT  3           // 右

#define HIGH   20          // 游戏区域高度
#define WIDTH  40          // 游戏区域宽度（必须为偶数）

#define EMPTY  0           // 地图元素：空地
#define SNAKE  1           // 蛇身
#define WALL   2           // 墙壁
#define FOOD   3           // 食物

/* ==================== 游戏全局变量 ==================== */
// 这些变量存储游戏的当前状态
int game_area[HIGH][WIDTH];    // 游戏地图数组
int game_state = RIGHT;        // 当前移动方向
int food_pos;                  // 食物位置
int score = 0;                 // 当前得分
int game_running = 1;          // 游戏运行状态
int snake_length = 1;          // 蛇的长度
int game_paused = 0;           // 暂停状态

/* ==================== 蛇身数据结构 ==================== */
// 使用双向链表存储蛇身，每个节点包含位置和方向信息
struct SnakeNode {
    int pos;                    // 位置（行×宽度+列）
    int direction;              // 移动方向
    struct SnakeNode *prev;     // 前一个节点
    struct SnakeNode *next;     // 后一个节点
};

struct SnakeNode *snake_head = NULL;  // 蛇头指针
struct SnakeNode *snake_tail = NULL;  // 蛇尾指针

/* ==================== 核心游戏函数 ==================== */

/**
 * 初始化游戏 - 步骤分解：
 * 1. 设置随机数种子
 * 2. 初始化游戏地图（边界为墙，内部为空）
 * 3. 创建蛇头节点并设置初始位置
 * 4. 在地图上标记蛇头位置
 * 5. 生成第一个食物
 */
void initialize_game(void) {
    int i, j;
    int head_row, head_col;
    
    srand((unsigned int)time(NULL));  // 设置随机种子
    
    // 步骤1：初始化地图
    for (i = 0; i < HIGH; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HIGH - 1 || j == 0 || j == WIDTH - 1) {
                game_area[i][j] = WALL;    // 边界设为墙
            } else {
                game_area[i][j] = EMPTY;   // 内部设为空地
            }
        }
    }
    
    // 步骤2：创建蛇头
    snake_head = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
    // 注意：这里要检查malloc是否成功（实际代码中有检查）
    
    // 步骤3：设置蛇头位置（地图中心）
    head_col = (WIDTH / 2);
    if (head_col % 2 != 0) head_col++;  // 确保列数为偶数
    
    snake_head->pos = (HIGH / 2) * WIDTH + head_col;
    snake_head->direction = RIGHT;
    snake_head->prev = NULL;
    snake_head->next = NULL;
    snake_tail = snake_head; 
    
    // 步骤4：在地图上标记蛇头
    head_row = snake_head->pos / WIDTH;
    head_col = snake_head->pos % WIDTH;
    game_area[head_row][head_col] = SNAKE;
    
    // 步骤5：生成食物
    generate_food();
}

/**
 * 生成食物 - 关键逻辑：
 * 1. 随机生成行列位置
 * 2. 确保位置是空的（不是墙、蛇身）
 * 3. 确保列数为偶数（与蛇的移动步长匹配）
 * 4. 在地图上标记食物位置
 */
void generate_food(void) {
    int row, col;
    int attempts = 0;
    const int max_attempts = (HIGH - 2) * (WIDTH - 2);  // 最大尝试次数
    
    do {
        // 随机生成位置（避开边界）
        row = rand() % (HIGH - 2) + 1;
        col = rand() % (WIDTH - 2) + 1;
        
        // 确保列数为偶数
        if (col % 2 != 0) {
            col = (col + 1);
            if (col >= WIDTH - 1) {
                col = 2; 
            }
        }
        
        attempts++;
        // 防止无限循环
        if (attempts > max_attempts) {
            game_running = 0;  // 无法生成食物，游戏结束
            return;
        }
    } while (game_area[row][col] != EMPTY);  // 直到找到空位置
    
    // 标记食物位置
    food_pos = row * WIDTH + col;
    game_area[row][col] = FOOD;
}

/**
 * 更新蛇的位置 - 核心算法步骤：
 * 1. 根据当前方向计算新蛇头位置
 * 2. 检查新位置是否有效（是否撞墙或自身）
 * 3. 创建新的蛇头节点
 * 4. 如果吃到食物：增加长度和分数，生成新食物
 * 5. 如果没吃到：移动蛇尾（删除最后一个节点）
 * 6. 更新地图显示
 */
void update_snake_position(void) {
    int new_head_pos;
    int current_direction = game_state;
    int new_head_row, new_head_col;
    struct SnakeNode *new_head;
    
    // 步骤1：计算新蛇头位置
    switch (current_direction) {
        case UP:    new_head_pos = snake_head->pos - WIDTH; break;
        case DOWN:  new_head_pos = snake_head->pos + WIDTH; break;
        case LEFT:  new_head_pos = snake_head->pos - 2; break;  // 横向移动2格
        case RIGHT: new_head_pos = snake_head->pos + 2; break;  // 保持蛇身连贯
    }
    
    // 步骤2：检查位置有效性
    if (!is_valid_position(new_head_pos)) {
        game_running = 0;  // 碰撞，游戏结束
        return;
    }
    
    // 步骤3：创建新蛇头
    new_head = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
    new_head->pos = new_head_pos;
    new_head->direction = current_direction;
    new_head->prev = NULL;
    new_head->next = snake_head;
    
    snake_head->prev = new_head;
    snake_head = new_head;
    
    // 步骤4：检查是否吃到食物
    if (new_head_pos == food_pos) {
        score += 10;           // 增加分数
        snake_length++;        // 增加长度
        generate_food();       // 生成新食物
    } else {
        // 步骤5：没吃到食物，移动蛇尾
        int tail_row = snake_tail->pos / WIDTH;
        int tail_col = snake_tail->pos % WIDTH;
        game_area[tail_row][tail_col] = EMPTY;  // 清除原蛇尾
        
        struct SnakeNode *old_tail = snake_tail;
        snake_tail = snake_tail->prev;
        if (snake_tail != NULL) {
            snake_tail->next = NULL;
        }
        free(old_tail);  // 释放原蛇尾内存
    }
    
    // 步骤6：更新地图
    new_head_row = new_head_pos / WIDTH;
    new_head_col = new_head_pos % WIDTH;
    game_area[new_head_row][new_head_col] = SNAKE;
}

/**
 * 位置有效性检查 - 碰撞检测：
 * 1. 是否超出地图边界
 * 2. 是否撞到墙壁
 * 3. 是否撞到自身（除了蛇尾，因为蛇在移动）
 */
int is_valid_position(int pos) {
    int row = pos / WIDTH;
    int col = pos % WIDTH;
    
    // 边界检查
    if (row < 0 || row >= HIGH || col < 0 || col >= WIDTH) {
        return 0;
    }
    
    // 墙壁碰撞检查
    if (game_area[row][col] == WALL) {
        return 0;
    }
    
    // 自身碰撞检查（允许移动到蛇尾位置，因为蛇在移动）
    if (game_area[row][col] == SNAKE) {
        if (pos == snake_tail->pos) {
            return 1;  // 可以移动到蛇尾位置
        }
        return 0;      // 其他蛇身位置不可移动
    }
    
    return 1;
}

/**
 * 游戏主循环 - 执行流程：
 * 1. 处理用户输入
 * 2. 如果游戏未暂停：更新蛇位置，绘制画面
 * 3. 如果游戏暂停：只绘制画面（显示暂停信息）
 * 4. 根据难度和得分调整游戏速度
 * 5. 延时控制游戏帧率
 */
void game_loop(void) {
    int speed;
    while (game_running) {
        handle_input();          // 处理键盘输入
        
        if (!game_paused) {
            update_snake_position();  // 更新游戏状态
            
            if (!game_running) {
                break;  // 游戏结束，退出循环
            }
            
            draw_game();  // 绘制游戏画面
        } else {
            draw_game();  // 暂停时只绘制（显示暂停界面）
        }
        
        // 动态调整游戏速度：难度越高、得分越高，速度越快
        speed = base_speed - (score / 10) * (5 - game_difficulty);
        if (speed < 40) speed = 40;  // 设置速度上限
        
        Sleep(speed);  // 控制游戏速度
    }
}

/**
 * 处理用户输入 - 控制逻辑：
 * 1. 检测是否有按键输入
 * 2. 处理方向键和WASD控制移动方向
 * 3. 处理功能键：暂停、存档、退出
 * 4. 防止反向移动（比如不能从右直接向左）
 */
void handle_input(void) {
    if (_kbhit()) {  // 检查是否有按键
        int ch = _getch();
        
        // 特殊键（方向键、F5等）需要两次getch
        if (ch == 0 || ch == 224) {
            ch = _getch();
            
            switch (ch) {
                case KEY_UP:    if (game_state != DOWN) game_state = UP; break;
                case KEY_DOWN:  if (game_state != UP) game_state = DOWN; break;
                case KEY_LEFT:  if (game_state != RIGHT) game_state = LEFT; break;
                case KEY_RIGHT: if (game_state != LEFT) game_state = RIGHT; break;
                case KEY_F5:    save_game_state(); break;  // 快速存档
            }
        } else {
            // 普通字母键
            switch (ch) {
                case 'w': case 'W': if (game_state != DOWN) game_state = UP; break;
                case 's': case 'S': if (game_state != UP) game_state = DOWN; break;
                case 'a': case 'A': if (game_state != RIGHT) game_state = LEFT; break;
                case 'd': case 'D': if (game_state != LEFT) game_state = RIGHT; break;
                case 'p': case 'P': game_paused = !game_paused; break;  // 暂停/继续
                case 'q': case 'Q': game_running = 0; break;            // 退出游戏
            }
        }
    }
}

/* ==================== 内存管理重要说明 ==================== */

/**
 * 清理游戏资源 - 重要：
 * 必须释放蛇身链表的所有节点，避免内存泄漏
 * 这在游戏结束或重新开始时调用
 */
void cleanup_game(void) {
    struct SnakeNode *current = snake_head;
    struct SnakeNode *next;
    
    while (current != NULL) {
        next = current->next;  // 保存下一个节点指针
        free(current);         // 释放当前节点
        current = next;        // 移动到下一个节点
    }
    
    snake_head = NULL;  // 重置指针，避免野指针
    snake_tail = NULL;
}

/* ==================== 学习要点总结 ==================== */

/*
 * 关键知识点总结：
 * 
 * 1. 数据结构：双向链表存储蛇身，每个节点包含位置和方向
 * 2. 碰撞检测：边界检查、墙壁检查、自身碰撞检查
 * 3. 内存管理：动态分配节点，游戏结束时必须释放
 * 4. 游戏循环：输入→更新→渲染的基本模式
 * 5. 状态管理：使用全局变量管理游戏各种状态
 * 6. 控制台编程：光标控制、颜色设置、键盘输入处理
 * 
 * 扩展思考：
 * - 如何实现穿墙模式？
 * - 如何添加不同类型的食物？
 * - 如何实现关卡系统？
 * - 如何优化蛇的移动算法？
 */

int main(void) {
    // 程序主入口 - 建议学生从此开始阅读代码
    // 主要流程：初始化→主菜单→游戏循环→清理资源
    
    /* 获取控制台句柄，用于后续的光标控制 */
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    /* 设置窗口标题和颜色 */
    system("title 貪吃蛇遊戲");
    set_console_color(current_color_code);
    
    /* 播放背景音乐 */
    play_background_music();

    /* 主程序循环 */
    while (GameSnake) {
        // 显示主菜单并获取用户选择
        int choice = get_main_menu_choice();

        switch (choice) {
            case '1': start_new_game(); break;      // 新游戏
            case '2': load_and_run_game(); break;   // 加载存档
            case '3': show_menu_rules(); break;     // 游戏规则
            case '4': show_leaderboard(); break;    // 排行榜
            case '5': show_settings_menu(); break;  // 设置
            case '6': return 0;                     // 退出
        }
    }
    
    return 0;
}
