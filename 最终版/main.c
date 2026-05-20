/* ============================================================
 * 贪吃蛇游戏 - 最终版详细注释
 * 本注释详细解释代码的每个关键部分，帮助学生深入理解
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <string.h> 

#include <mmsystem.h> /* 用于播放音乐 (MCI) */
//声音配置教学：项目 -->项目属性 -->参数 -->连接 -->在后面加入参数-lwinmm
// 或者修改 LIBS行
// 修改前：
//LIBS     = -L"D:/Program Files/Dev-Cpp/MinGW64/lib" -L"D:/Program Files/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib" -static-libgcc

// 修改后：
//LIBS     = -L"D:/Program Files/Dev-Cpp/MinGW64/lib" -L"D:/Program Files/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib" -static-libgcc -lwinmm 

//如果不行就绝对路径 
// LIBS = -L"D:/Program Files/Dev-Cpp/MinGW64/lib" -L"D:/Program Files/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib" -static-libgcc -lwinmm -l"winmm"
//如果还不行就去看看 在Dev-C++安装目录中 winmm.lib 文件是不是没了 

//实在不行就不实现这些音乐 
/* --- 链接多媒体库 --- */
#pragma comment(lib, "winmm.lib") 

/* ==================== 游戏常量定义区域 ==================== */
/* 这些常量定义了游戏的基本参数，修改它们可以改变游戏行为 */

#define GameSnake 1
/* 方向常量 - 使用数字表示方向便于计算和比较 */
#define UP     0 //上 
#define DOWN   1 //下 
#define LEFT   2 //左 
#define RIGHT  3 //右 

/* 游戏区域大小 - 决定了游戏地图的尺寸 */
#define HIGH   20    //高 
#define WIDTH  40   //宽 (必须为偶数，因为蛇横向移动步长为2)

/* 游戏区域元素 - 用数字代表不同类型的地图元素 */
#define EMPTY  0  // 空地
#define SNAKE  1  // 蛇身
#define WALL   2  // 墙壁
#define FOOD   3  // 食物

/* 按鍵掃描碼 - Windows系统下特殊键的扫描码 */
#define KEY_UP    72
#define KEY_LEFT  75
#define KEY_RIGHT 77
#define KEY_DOWN  80
#define KEY_F5    63  /* F5 鍵用於存檔 */
#define KEY_ESC   27  /* ESC 键 (用于从暂停返回) */

/* 存檔/得分文件名 - 游戏数据的保存位置 */
#define SAVE_FILENAME "snake_save.dat"
#define SCORE_FILENAME "fraction.txt"
#define MAX_NAME_LEN 50    // 玩家名字最大长度
#define MAX_SCORES 200     // 排行榜最大记录数

/* --- 游戏设置常量 --- */
/* 难度级别 - 影响游戏速度和挑战性 */
#define DIFF_EASY   0
#define DIFF_NORMAL 1
#define DIFF_HARD   2

/* 颜色配置 - 控制台颜色代码，前背景色各1位十六进制数 */
#define COLOR_BLACK_WHITE "0F" /* 默认: 黑底白字 */
#define COLOR_BLUE_GREEN  "1A" /* 蓝底绿字 */
#define COLOR_BLACK_RED   "0C" /* 黑底红字 */
#define COLOR_YELLOW_BLUE "E1" /* 黄底蓝字 */
#define COLOR_BLACK_GREEN "0A" /* 黑底绿字 */

/* 音量控制常量 */
#define VOLUME_MIN 0
#define VOLUME_MAX 100
#define VOLUME_DEFAULT 60

/* ==================== 全局变量声明区域 ==================== */
/* 这些变量存储游戏的当前状态，在整个程序中都可用 */

/* 音效控制变量 */
int sound_effects_enabled = 1;    /* 音效开关 */
int background_music_enabled = 1; /* 背景音乐开关 */
int volume_level = VOLUME_DEFAULT; /* 音量级别 0-100 */

/* 游戏状态变量 */
int game_area[HIGH][WIDTH];  // 核心游戏地图，存储每个位置的状态
int game_state = RIGHT;      // 当前蛇的移动方向
int food_pos;                // 食物位置（一维坐标表示）
int score = 0;               // 当前得分
int game_running = 1;        // 游戏是否正在运行
int snake_length = 1;        // 蛇的当前长度
int game_paused = 0;         // 游戏是否暂停
int save_message_timer = 0;  // 存档提示信息的显示计时器

/* 游戏设置变量 */
int game_difficulty = DIFF_NORMAL; /* 默认难度 */
int base_speed = 150;              /* 基础速度 (ms) - 数值越小速度越快 */
char current_color_code[3] = "0F"; /* 当前颜色代码 */
char current_player_name[MAX_NAME_LEN] = "Guest"; // 当前玩家名字

/* ==================== 数据结构定义 ==================== */

/**
 * @struct SnakeNode
 * @brief 贪吃蛇节点结构体
 * 使用双向链表存储蛇身，每个节点包含：
 * - pos: 在游戏区域中的位置（行×宽度+列）
 * - direction: 该节点移动的方向
 * - prev/next: 前后节点指针，实现双向链表
 */
struct SnakeNode {
    int pos;
    int direction;
    struct SnakeNode *prev;
    struct SnakeNode *next;
};

/* 得分排行榜条目结构体 - 存储玩家成绩信息 */
typedef struct {
    char name[MAX_NAME_LEN];  // 玩家名字
    int score;                // 得分
    char date[11];            /* 日期 YYYY-MM-DD */
    int difficulty;           // 游戏难度
} ScoreEntry;

/* 蛇身链表头尾指针 */
struct SnakeNode *snake_head = NULL;
struct SnakeNode *snake_tail = NULL;
HANDLE hConsole; /* 控制台句柄 - 用于Windows控制台操作 */

/* ==================== 函数声明区域 ==================== */
/* 这里声明了所有函数，让编译器知道它们的存在 */

/* 核心遊戲邏輯 */
void initialize_game(void);
void cleanup_game(void);
void generate_food(void);
int is_valid_position(int pos);
void update_snake_position(void);
void draw_game(void);
void handle_input(void);
void game_loop(void);

/* 遊戲狀態與界面 */
void show_game_rules(void);
void show_game_over(void);
void show_menu_rules(void);
int get_main_menu_choice(void);

/* 界面与动画 */
void set_console_color(const char* color_code);
void draw_pretty_border(int x_offset, int y_offset, int w, int h, const char* title);
void draw_pause_screen(void);
void play_game_over_animation(void);
void hide_cursor(void);
void show_cursor(void);
void set_cursor_position(int x, int y);

/* 程序流程函數 */
void start_new_game(void);
void run_game_instance(void);
void load_and_run_game(void);

/* 存檔/讀檔函數  */
void save_game_state(void);
int load_game_state(void);
void rebuild_game_area_after_load(void);

/* 排行榜和用戶名函數 */
void get_player_name(void);
void generate_guest_name(void); /*生成 GuestX */
void trim_newline(char *s);
void save_score_to_file(void);
void show_leaderboard(void);
static int compare_scores(const void *a, const void *b);
const char* get_difficulty_string(int diff);

/* 游戏设置和音乐 */
void show_new_game_menu(void);
void select_difficulty(void);
void show_settings_menu(void);
void select_color_menu(void);
void play_background_music(void);
void stop_background_music(void);

/* 音量和音效设置函数 */
void show_audio_settings_menu(void);
void adjust_volume_setting(void);
void set_audio_volume(int volume);
void test_sound_effects(void);
void play_sound_effect(int frequency, int duration);

/* ==================== 音效设置函数实现 ==================== */

/**
 * @brief 音量和音效设置菜单
 * 功能：提供音效开关、背景音乐开关、音量调节等设置选项
 * 设计思路：循环显示菜单，根据用户选择调用相应功能
 */
void show_audio_settings_menu(void) {
    int choice = 0;
    char volume_bar[52]; // 50个字符的进度条 + 2个边界
    
    while(choice != '5') {
        system("cls");
        printf("=========================================\n");
        printf("             音量與音效設置\n");
        printf("=========================================\n\n");
        
        // 显示当前设置状态 - 使用图形化进度条增强用户体验
        printf(" 当前设置:\n");
        printf(" ┌─────────────────────────────────────┐\n");
        
        // 音量显示 - 动态生成进度条
        int bar_length = (volume_level * 50) / 100;
        memset(volume_bar, '=', bar_length);
        memset(volume_bar + bar_length, ' ', 50 - bar_length);
        volume_bar[50] = '\0';
        printf(" │ 音量: [%-50s] %3d%% │\n", volume_bar, volume_level);
        
        // 音效开关显示 - 使用[?]和[ ]表示开关状态
        printf(" │ 音效: %-43s │\n", 
               sound_effects_enabled ? "[?] 开启" : "[ ] 关闭");
        
        // 背景音乐开关显示
        printf(" │ 背景音乐: %-39s │\n", 
               background_music_enabled ? "[?] 开启" : "[ ] 关闭");
        
        printf(" └─────────────────────────────────────┘\n\n");
        
        // 菜单选项
        printf(" [1] 调整音量大小\n");
        printf(" [2] %s音效\n", sound_effects_enabled ? "关闭" : "开启");
        printf(" [3] %s背景音乐\n", background_music_enabled ? "关闭" : "开启");
        printf(" [4] 测试音效\n");
        printf(" [5] 返回上级菜单\n");
        printf("\n");
        printf("=========================================\n");
        printf("請選擇 (1-5): ");
        
        choice = _getch();
        
        switch(choice) {
            case '1':
                adjust_volume_setting();
                break;
            case '2':
                sound_effects_enabled = !sound_effects_enabled;
                printf("\n音效已%s", sound_effects_enabled ? "开启" : "关闭");
                if (sound_effects_enabled) {
                    Beep(523, 100); // 测试音效 - 提供即时反馈
                }
                Sleep(800);
                break;
            case '3':
                background_music_enabled = !background_music_enabled;
                printf("\n背景音乐已%s", background_music_enabled ? "开启" : "关闭");
                if (background_music_enabled) {
                    play_background_music();
                } else {
                    stop_background_music();
                }
                Sleep(800);
                break;
            case '4':
                test_sound_effects();
                break;
            case '5':
                return;
        }
    }
}

/**
 * @brief 调整音量大小
 * 功能：使用方向键实时调整音量，Enter键确认
 * 技术要点：实时更新音量进度条，提供视觉反馈
 */
void adjust_volume_setting(void) {
    int ch;
    char volume_bar[52];
    
    system("cls");
    printf("=========================================\n");
    printf("             調整音量大小\n");
    printf("=========================================\n\n");
    printf(" 使用左右方向鍵調整音量，Enter鍵確認\n\n");
    
    while(1) {
        // 显示音量条 - 动态更新显示
        int bar_length = (volume_level * 50) / 100;
        memset(volume_bar, '=', bar_length);
        memset(volume_bar + bar_length, ' ', 50 - bar_length);
        volume_bar[50] = '\0';
        
        printf(" ┌─────────────────────────────────────┐\n");
        printf(" │ 音量: [%-50s] %3d%% │\n", volume_bar, volume_level);
        printf(" └─────────────────────────────────────┘\n\n");
        
        printf(" 按 Enter 鍵確認並返回\n");
        
        set_audio_volume(volume_level); // 实时应用音量 - 立即生效
        
        ch = _getch();
        if (ch == 0 || ch == 224) {  // 检测特殊键（方向键）
            ch = _getch();
            switch(ch) {
                case KEY_LEFT:  // 左键减小音量
                    if (volume_level > VOLUME_MIN) {
                        volume_level -= 5;  // 每次调整5%
                        if (volume_level < VOLUME_MIN) volume_level = VOLUME_MIN;
                    }
                    break;
                case KEY_RIGHT: // 右键增大音量
                    if (volume_level < VOLUME_MAX) {
                        volume_level += 5;
                        if (volume_level > VOLUME_MAX) volume_level = VOLUME_MAX;
                    }
                    break;
            }
        } else if (ch == 13) { // Enter键确认
            break;
        }
        
        system("cls");  // 清屏重新绘制
        printf("=========================================\n");
        printf("             調整音量大小\n");
        printf("=========================================\n\n");
        printf(" 使用左右方向鍵調整音量，Enter鍵確認\n\n");
    }
    
    printf("\n音量已设置为: %d%%", volume_level);
    Sleep(800);
}

/**
 * @brief 设置音频音量
 * @param volume 音量级别 0-100
 * 技术原理：将0-100的音量转换为MCI的0-1000范围
 */
void set_audio_volume(int volume) {
    // MCI 音量范围是 0-1000，所以需要转换
    int mci_volume = (volume * 1000) / 100;
    char command[50];
    
    sprintf(command, "setaudio music volume to %d", mci_volume);
    mciSendString(command, NULL, 0, NULL);
}

/**
 * @brief 测试音效
 * 功能：播放一组音阶，测试音效是否正常工作
 * 音乐原理：播放C大调音阶，从C5到C6
 */
void test_sound_effects(void) {
    if (!sound_effects_enabled) {
        printf("\n音效当前已关闭，无法测试！");
        Sleep(1000);
        return;
    }
    
    printf("\n正在播放测试音效...");
    
    // 播放一系列测试音效 - C大调音阶
    Beep(523, 200);  // C5
    Beep(587, 200);  // D5
    Beep(659, 200);  // E5
    Beep(698, 200);  // F5
    Beep(784, 200);  // G5
    Beep(880, 200);  // A5
    Beep(988, 200);  // B5
    Beep(1047, 400); // C6
    
    printf("测试完成！");
    Sleep(800);
}

/**
 * @brief 播放音效（带开关检查）
 * @param frequency 频率(Hz) - 决定音高
 * @param duration 持续时间(ms) - 决定音长
 * 设计思路：封装Beep函数，增加开关检查
 */
void play_sound_effect(int frequency, int duration) {
    if (sound_effects_enabled) {
        Beep(frequency, duration);
    }
}

/* ==================== 控制台操作函数 ==================== */

/**
 * @brief 设置控制台颜色 
 * @param color_code 颜色代码字符串
 * 技术原理：使用system命令调用Windows的color命令
 */
void set_console_color(const char* color_code) {
    char command[20];
    sprintf(command, "color %s", color_code);
    system(command);
    strcpy(current_color_code, color_code); /* 保存当前颜色 */
}

/**
 * @brief 隐藏光标
 * 功能：在游戏过程中隐藏控制台光标，提升视觉效果
 */
void hide_cursor(void) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = 0;  // 设置为不可见
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

/**
 * @brief 显示光标
 * 功能：恢复光标的显示，用于菜单输入等场景
 */
void show_cursor(void) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = 1;  // 设置为可见
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

/**
 * @brief 设置光标位置
 * @param x 横坐标
 * @param y 纵坐标
 * 功能：精确定位控制台输出位置，实现复杂界面布局
 */
void set_cursor_position(int x, int y) {
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(hConsole, pos);
}

/* ==================== 音乐控制函数 ==================== */

/**
 * @brief 播放背景音乐 (循环)
 * 功能：尝试播放MP3或WAV格式的背景音乐
 * 容错设计：先尝试MP3，失败后尝试WAV格式
 */
void play_background_music(void) {
    if (!background_music_enabled) {
        return; // 如果背景音乐关闭，则不播放
    }
    
    /* 首先尝试关闭任何可能已打开的音乐 - 避免资源冲突 */
    mciSendString("stop music", NULL, 0, NULL);
    mciSendString("close music", NULL, 0, NULL);
    
    /* 尝试打开 MP3 */
    if (mciSendString("open \"SnakeMusic.mp3\" type mpegvideo alias music", NULL, 0, NULL) != 0) {
        /* 如果 MP3 失败，尝试打开 WAV */
        if (mciSendString("open \"SnakeMusic.wav\" type waveaudio alias music", NULL, 0, NULL) != 0) {
            /* 如果两者都失败，不执行任何操作 */
            return;
        }
    }
    
    // 设置音量
    set_audio_volume(volume_level);
    
    /* 播放并循环 - repeat参数实现循环播放 */
    mciSendString("play music repeat", NULL, 0, NULL);
}

/**
 * @brief 停止并关闭背景音乐
 * 功能：释放音乐资源，避免资源泄漏
 */
void stop_background_music(void) {
    mciSendString("stop music", NULL, 0, NULL);
    mciSendString("close music", NULL, 0, NULL);
}

/* ==================== 游戏界面函数 ==================== */

/**
 * @brief 顯示遊戲規則 (核心內容)
 * 功能：向玩家说明游戏操作方法、规则和目标
 */
void show_game_rules(void) {
    printf("遊戲規則說明:\n");
    printf("----------------------------------------\n");
    printf("1. 使用方向鍵(↑←↓→)或WASD鍵控制蛇的移動\n");
    printf("   W/↑ - 向上 | A/← - 向左 | S/↓ - 向下 | D/→ - 向右\n");
    printf("2. 蛇會自動向前移動，你需要控制它的方向\n");
    printf("3. 吃掉食物(★)可以增加長度和得分 (得分+10)\n");
    printf("4. 撞到牆壁(╔ ═ ╗ ║ ╚ ╝)或自己的身體(■)遊戲結束\n");
    printf("5. 游戏速度取决于你选择的难度 (简单/普通/困难)\n");
    printf("\n");
    printf("遊戲控制:\n");
    printf("----------------------------------------\n");
    printf(" P 鍵   : 暫停 / 繼續遊戲\n");
    printf(" F5 鍵  : 快速存檔 (会保存你的名字和难度)\n");
    printf(" Q 鍵   : (在游戏中) 立即退出\n");
    printf(" ESC 鍵 : (在暂停时) 退出暂停\n");
    printf("\n");
    printf("遊戲目標:\n");
    printf("----------------------------------------\n");
    printf("盡可能吃更多的食物，獲得更高的分數！\n");
}

/**
 * @brief 在主菜單顯示規則
 * 功能：在独立界面中显示游戏规则，按任意键返回
 */
void show_menu_rules(void) {
    system("cls");
    printf("==================================================\n");
    printf("                  遊戲規則說明                  \n");
    printf("==================================================\n\n");
    
    show_game_rules();
    
    printf("\n\n==================================================\n");
    printf("按任意鍵返回主菜單...");
    _getch();
}

/**
 * @brief 顯示遊戲結束界面
 * 功能：显示游戏结果、保存分数、提供评价
 * 特色：包含动画效果和音效反馈
 */
void show_game_over(void) {
    int i;
    
    /* 停止背景音乐 - 游戏结束的特殊处理 */
    stop_background_music();
    
    /* 游戏结束动画 (闪烁屏幕) - 增强游戏体验 */
    play_game_over_animation();
    
    /* 恢复到当前颜色 */
    set_console_color(current_color_code);
    
    /* 将光标移动到游戏区域下方 */
    set_cursor_position(0, HIGH + 4);

    printf("\n=========================================\n");
    printf("              遊戲結束！                 \n");
    printf("=========================================\n\n");
    printf(" 玩家: %s\n", current_player_name);
    printf(" 难度: %s\n", get_difficulty_string(game_difficulty));
    printf(" 最終得分: %d\n", score);
    printf(" 蛇的長度: %d\n", snake_length);
    printf("\n");
    
    /* 根据得分提供不同的评价 - 增加游戏成就感 */
    if (score >= 100) {
        printf(" 评价: 太棒了！你是貪吃蛇大師！\n");
    } else if (score >= 50) {
        printf(" 评价: 很不錯！繼續加油！\n");
    } else if (score >= 20) {
        printf(" 评价: 還可以，再多練習一下！\n");
    } else {
        printf(" 评价: 新手水平，多加練習！\n");
    }
    
    /* 保存分数到文件 - 实现数据持久化 */
    save_score_to_file();
    
    printf("\n 分數已保存到排行榜！");
    printf("\n=========================================\n");
    printf(" 按空格鍵返回主菜單...");  // 修改提示信息

    // 等待用户按下空格键
    while (_getch() != ' ') {
        // 循环等待，直到按下空格键
    }
    
    /* 准备返回主菜单，重新开启音乐 */
    play_background_music();
}
/**
 * @brief 绘制美化的边框
 * @param x_offset 左上角X坐标
 * @param y_offset 左上角Y坐标  
 * @param w 宽度
 * @param h 高度
 * @param title 标题文字
 * 功能：使用Unicode字符绘制漂亮的边框，支持标题居中
 */
void draw_pretty_border(int x_offset, int y_offset, int w, int h, const char* title) {
    int i;
    /* 绘制顶部带标题的边框 - 标题自动居中 */
    set_cursor_position(x_offset, y_offset);
    printf("╔");
    for (i = 0; i < (w - 2) / 2 - (strlen(title) / 2) - 1; i++) printf("═");
    printf(" %s ", title);
    for (i = 0; i < (w - 2) / 2 - (strlen(title) / 2) - (strlen(title) % 2); i++) printf("═");
    printf("╗");

    /* 绘制侧面边框 */
    for (i = 1; i < h - 1; i++) {
        set_cursor_position(x_offset, y_offset + i);
        printf("║");
        set_cursor_position(x_offset + w - 1, y_offset + i);
        printf("║");
    }
    
    /* 绘制底部边框 */
    set_cursor_position(x_offset, y_offset + h - 1);
    printf("╚");
    for (i = 0; i < w - 2; i++) printf("═");
    printf("╝");
}

/**
 * @brief  绘制暂停界面
 * 功能：在游戏画面上叠加显示暂停信息
 */
void draw_pause_screen(void) {
    int x = WIDTH / 2 - 10;
    int y = HIGH / 2 - 2;
    draw_pretty_border(x, y, 20, 5, "PAUSED");
    set_cursor_position(x + 2, y + 2);
    printf("按 P 或 ESC 繼續");
}

/**
 * @brief 游戏结束动画
 * 功能：播放音效和屏幕闪烁效果，增强游戏结束的戏剧性
 */
void play_game_over_animation(void) {
    int i;
    /* 播放一串下降的 Beep 音效 - 营造悲伤氛围 */
	play_sound_effect(330, 100); 
	play_sound_effect(311, 100); 
	play_sound_effect(294, 100); 
	play_sound_effect(262, 200);
    
    /* 屏幕闪烁 - 使用不同颜色组合创造视觉效果 */
    for (i = 0; i < 2; i++) {
        system("color 70"); /* 白底黑字 */
        Sleep(80);
        system("color 07"); /* 黑底白字 */
        Sleep(80);
        system("color C0"); /* 红底黑字 */
        Sleep(80);
        system("color 0C"); /* 黑底红字 */
        Sleep(80);
    }
}

/* ==================== 核心游戏逻辑函数 ==================== */

/**
 * @brief 初始化游戏状态和数据结构
 * 功能：准备新游戏所需的所有数据和状态
 * 步骤：1.初始化地图 2.创建蛇 3.生成食物
 */
void initialize_game(void) {
    int i, j;
    int head_row, head_col;
    
    srand((unsigned int)time(NULL));  // 设置随机种子，确保每次游戏不同
    
    /* 初始化遊戲區域: 設置邊界牆和內部空格 */
    for (i = 0; i < HIGH; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HIGH - 1 || j == 0 || j == WIDTH - 1) {
                game_area[i][j] = WALL;    // 边界设为墙壁
            } else {
                game_area[i][j] = EMPTY;   // 内部设为空地
            }
        }
    }
    
    /* 动态分配内存创建蛇头节点 */
    snake_head = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
    if (snake_head == NULL) {
        printf("錯誤: 內存分配失敗!\n");
        exit(1);
    }
    
    /* 初始化蛇頭位置和方向 - 确保起始位置合法 */
    head_col = (WIDTH / 2);
    if (head_col % 2 != 0) head_col++;  // 确保列数为偶数，匹配移动步长
    
    snake_head->pos = (HIGH / 2) * WIDTH + head_col;  // 计算一维坐标
    snake_head->direction = RIGHT;
    snake_head->prev = NULL;
    snake_head->next = NULL;
    snake_tail = snake_head; 
    
    /* 在地图上标记蛇头位置 */
    head_row = snake_head->pos / WIDTH;
    head_col = snake_head->pos % WIDTH;
    game_area[head_row][head_col] = SNAKE;
    
    generate_food();  // 生成第一个食物
}

/**
 * @brief 清理遊戲資源 (釋放蛇的鏈表)
 * 功能：释放动态分配的内存，避免内存泄漏
 * 重要：必须在游戏结束或重新开始时调用
 */
void cleanup_game(void) {
    struct SnakeNode *current = snake_head;
    struct SnakeNode *next;
    
    // 遍历整个链表，逐个释放节点
    while (current != NULL) {
        next = current->next;  // 先保存下一个节点的指针
        free(current);         // 释放当前节点
        current = next;        // 移动到下一个节点
    }
    
    snake_head = NULL;  // 重置指针，避免野指针
    snake_tail = NULL;
}

/**
 * @brief 生成食物
 * 功能：在随机空位置生成食物
 * 算法：使用do-while循环确保生成有效位置
 */
void generate_food(void) {
    int row, col;
    int attempts = 0;
    const int max_attempts = (HIGH - 2) * (WIDTH - 2);  // 最大尝试次数
    
    do {
        // 生成随机位置（避开边界）
        row = rand() % (HIGH - 2) + 1;
        col = rand() % (WIDTH - 2) + 1;
        
        /* 確保列是偶数，与蛇的移动步长一致 */
        if (col % 2 != 0) {
            col = (col + 1);
            if (col >= WIDTH - 1) {
                col = 2; 
            }
        }
        
        attempts++;
        // 防止在极端情况下无限循环
        if (attempts > max_attempts) {
            game_running = 0;  // 无法生成食物，游戏结束
            return;
        }
    } while (game_area[row][col] != EMPTY);  // 直到找到空位置
    
    food_pos = row * WIDTH + col;      // 计算一维坐标
    game_area[row][col] = FOOD;        // 在地图上标记食物
}

/**
 * @brief 檢查新位置是否有效 (是否會導致碰撞)
 * @param pos 要检查的位置（一维坐标）
 * @return 1有效 0无效
 * 碰撞检测逻辑：1.边界检查 2.墙壁检查 3.自身碰撞检查
 */
int is_valid_position(int pos) {
    int row = pos / WIDTH;   // 计算行坐标
    int col = pos % WIDTH;   // 计算列坐标
    
    // 1. 边界检查 - 是否超出地图范围
    if (row < 0 || row >= HIGH || col < 0 || col >= WIDTH) {
        return 0;
    }
    
    // 2. 墙壁碰撞检查
    if (game_area[row][col] == WALL) {
        return 0;
    }
    
    // 3. 自身碰撞检查（特殊处理：允许移动到蛇尾位置）
    if (game_area[row][col] == SNAKE) {
        if (pos == snake_tail->pos) {
            return 1;  // 可以移动到蛇尾（蛇在移动）
        }
        return 0;      // 撞到自身其他部位
    }
    
    return 1;  // 位置有效
}

/**
 * @brief 更新蛇的位置 - 游戏核心算法
 * 步骤：1.计算新位置 2.碰撞检测 3.处理吃食物 4.移动蛇身
 */
void update_snake_position(void) {
    int new_head_pos;
    int current_direction = game_state;
    int new_head_row, new_head_col;
    int tail_row, tail_col;
    struct SnakeNode *new_head;
    struct SnakeNode *old_tail;
    
    // 步骤1：根据当前方向计算新蛇头位置
    switch (current_direction) {
        case UP:    new_head_pos = snake_head->pos - WIDTH; break;     // 上移一行
        case DOWN:  new_head_pos = snake_head->pos + WIDTH; break;     // 下移一行  
        case LEFT:  new_head_pos = snake_head->pos - 2; break;         // 左移两格
        case RIGHT: new_head_pos = snake_head->pos + 2; break;         // 右移两格
        default:    return;
    }
    
    // 步骤2：碰撞检测
    if (!is_valid_position(new_head_pos)) {
        game_running = 0;  // 碰撞发生，游戏结束
        return;
    }
    
    // 步骤3：创建新蛇头节点
    new_head = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
    if (new_head == NULL) {
        printf("錯誤: 內存分配失敗!\n");
        game_running = 0;
        return;
    }
    
    new_head->pos = new_head_pos;
    new_head->direction = current_direction;
    new_head->prev = NULL;
    new_head->next = snake_head;
    
    // 更新链表关系
    if (snake_head != NULL) {
        snake_head->prev = new_head;
    }
    snake_head = new_head;
    
    // 步骤4：检查是否吃到食物
    if (new_head_pos == food_pos) {
        score += 10;                    // 增加分数
        snake_length++;                 // 增加长度（不删除蛇尾）
        play_sound_effect(523, 100);    // 播放吃食物音效
        generate_food();                // 生成新食物
    } else {
        // 没吃到食物，需要移动蛇尾
        tail_row = snake_tail->pos / WIDTH;
        tail_col = snake_tail->pos % WIDTH;
        game_area[tail_row][tail_col] = EMPTY;  // 清除原蛇尾位置
        
        // 删除原蛇尾节点
        old_tail = snake_tail;
        snake_tail = snake_tail->prev;
        if (snake_tail != NULL) {
            snake_tail->next = NULL;
        }
        free(old_tail);  // 释放内存
    }

    // 步骤5：在地图上标记新蛇头位置
    new_head_row = new_head_pos / WIDTH;
    new_head_col = new_head_pos % WIDTH;
    game_area[new_head_row][new_head_col] = SNAKE;
}

/**
 * @brief 繪製遊戲界面
 * 功能：将游戏状态可视化到控制台
 * 布局：信息区 + 游戏地图区 + 状态提示区
 */
void draw_game(void) {
    int i, j;
    char info_line[100];
    
    /* 重置光標到頂部 - 实现画面刷新 */
    set_cursor_position(0, 0);
    
    /* 顯示遊戲信息 - 使用格式化字符串整理信息 */
    sprintf(info_line, " 玩家: %-10s | 得分: %-5d | 長度: %-3d | 难度: %s ", 
            current_player_name, score, snake_length, get_difficulty_string(game_difficulty));
    printf("%-78s\n", info_line);
    printf(" 控制: 方向鍵/WASD | P-暫停 | F5-存檔 | Q-退出 %-30s\n", "");
    
    /* 顯示狀態信息 - 根据游戏状态显示不同提示 */
    if (game_paused) {
        printf(" === 遊戲暫停中 (按 P/ESC 繼續) === %-40s\n", "");
    } else if (save_message_timer > 0) {
        printf(" === 遊戲已存檔! === %-50s\n", "");
        save_message_timer--;  // 倒计时，自动消失
    } else {
        printf(" %-78s\n", "");  // 空行占位
    }
    
    /* 繪製游戏区域 (带边框) - 使用Unicode字符增强视觉效果 */
    
    /* 顶部边框 */
    printf(" ╔");
    for (i = 0; i < WIDTH - 2; i++) printf("═");
    printf("╗\n");

    /* 中间区域 - 根据game_area数组内容显示不同字符 */
    for (i = 1; i < HIGH - 1; i++) {
        printf(" ║"); /* 左边框 */
        for (j = 1; j < WIDTH - 1; j++) {
            switch (game_area[i][j]) {
                case EMPTY: printf(" "); break;     // 空地：空格
                case SNAKE: printf("■"); break;     // 蛇身：实心方块
                case FOOD:  printf("★"); break;     // 食物：星星
                default:    printf("?"); break;     // 未知：问号
            }
        }
        printf("║\n"); /* 右边框 */
    }

    /* 底部边框 */
    printf(" ╚");
    for (i = 0; i < WIDTH - 2; i++) printf("═");
    printf("╝\n");
    
    /* 如果暂停，绘制暂停覆盖窗口 */
    if (game_paused) {
        draw_pause_screen(); 
    }
}

/**
 * @brief 處理用戶輸入
 * 功能：检测并处理键盘输入，控制游戏行为
 * 设计：支持方向键、WASD、功能键，防止反向移动
 */
void handle_input(void) {
    if (_kbhit()) {  // 检查是否有按键输入
        int ch = _getch();
        
        // 处理特殊键（方向键、功能键需要两次getch）
        if (ch == 0 || ch == 224) {
            ch = _getch();
            
            switch (ch) {
                case KEY_UP:
                    if (game_state != DOWN && !game_paused) game_state = UP;
                    break;
                case KEY_DOWN:
                    if (game_state != UP && !game_paused) game_state = DOWN;
                    break;
                case KEY_LEFT:
                    if (game_state != RIGHT && !game_paused) game_state = LEFT;
                    break;
                case KEY_RIGHT:
                    if (game_state != LEFT && !game_paused) game_state = RIGHT;
                    break;
                case KEY_F5:
                    if (!game_paused) {
                        save_game_state();      // 快速存档
                        save_message_timer = 3; // 显示存档提示
                    }
                    break;
            }
        } else {
            // 处理普通字母键
            switch (ch) {
                case 'w': case 'W':
                    if (game_state != DOWN && !game_paused) game_state = UP;
                    break;
                case 's': case 'S':
                    if (game_state != UP && !game_paused) game_state = DOWN;
                    break;
                case 'a': case 'A':
                    if (game_state != RIGHT && !game_paused) game_state = LEFT;
                    break;
                case 'd': case 'D':
                    if (game_state != LEFT && !game_paused) game_state = RIGHT;
                    break;
                case 'q': case 'Q':
                    game_running = 0;  // 立即退出游戏
                    break;
                case 'p': case 'P':
                case KEY_ESC: /* ESC 也可以 暂停/继续 */
                    game_paused = !game_paused;  // 切换暂停状态
                    break;
            }
        }
    }
}

/**
 * @brief 遊戲主循環 - 游戏引擎核心
 * 功能：不断执行输入→更新→渲染的循环
 * 特色：根据难度和得分动态调整游戏速度
 */
void game_loop(void) {
    int speed;
    while (game_running) {
        handle_input();  // 处理用户输入
        
        if (!game_paused) {
            update_snake_position();  // 更新游戏状态
            
            if (!game_running) {
                break;  // 游戏结束，退出循环
            }
            
            draw_game();  // 渲染游戏画面
        } else {
            draw_game();  // 暂停时只渲染（显示暂停界面）
        }
        
        /* 动态速度计算：基础速度 - (得分/10) × (5-难度) */
        /* 原理：得分越高速度越快，难度越高速度越快 */
        speed = base_speed - (score / 10) * (5 - game_difficulty);
        if (speed < 40) speed = 40; /* 设置最快速度限制，避免过快 */
        
        Sleep(speed);  // 控制游戏速度
    }
}

/* ==================== 存档读档系统 ==================== */

/**
 * @brief 存檔 (包含玩家名稱, 难度)
 * 功能：将游戏状态保存到文件，支持断点续玩
 * 存储内容：玩家信息、游戏状态、蛇身数据
 */
void save_game_state(void) {
    FILE *fp;
    struct SnakeNode *current;

    fp = fopen(SAVE_FILENAME, "wb");  // 二进制写入模式
    if (fp == NULL) {
        return;  // 文件打开失败，静默处理
    }

    /* 1. 寫入玩家名稱 */
    fwrite(current_player_name, sizeof(char), MAX_NAME_LEN, fp);
    
    /* 2. 寫入基礎信息 + 难度 */
    fwrite(&score, sizeof(int), 1, fp);
    fwrite(&snake_length, sizeof(int), 1, fp);
    fwrite(&game_state, sizeof(int), 1, fp);
    fwrite(&food_pos, sizeof(int), 1, fp);
    fwrite(&game_difficulty, sizeof(int), 1, fp);

    /* 3. 寫入蛇身鏈表數據 */
    current = snake_head;
    while (current != NULL) {
        fwrite(&(current->pos), sizeof(int), 1, fp);
        fwrite(&(current->direction), sizeof(int), 1, fp);
        current = current->next;
    }

    fclose(fp);
}

/**
 * @brief 讀檔 (包含玩家名稱, 难度)
 * @return 1成功 0失败
 * 功能：从文件恢复游戏状态
 * 步骤：1.读取基本信息 2.重建蛇身链表 3.重建游戏地图
 */
int load_game_state(void) {
    FILE *fp;
    int i;
    struct SnakeNode *new_node, *prev_node;

    fp = fopen(SAVE_FILENAME, "rb");  // 二进制读取模式
    if (fp == NULL) {
        return 0;  // 文件不存在或打开失败
    }

    cleanup_game();  // 清理当前游戏状态

    /* 1. 讀取玩家名稱和基礎信息 */
    fread(current_player_name, sizeof(char), MAX_NAME_LEN, fp);
    fread(&score, sizeof(int), 1, fp);
    fread(&snake_length, sizeof(int), 1, fp);
    fread(&game_state, sizeof(int), 1, fp);
    fread(&food_pos, sizeof(int), 1, fp);
    fread(&game_difficulty, sizeof(int), 1, fp);

    /* 2. 重建蛇身鏈表 - 根据保存的长度逐个创建节点 */
    prev_node = NULL;
    snake_head = NULL;
    snake_tail = NULL;

    for (i = 0; i < snake_length; i++) {
        new_node = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
        if (new_node == NULL) {
            fclose(fp);
            cleanup_game();
            return 0;  // 内存分配失败
        }

        fread(&(new_node->pos), sizeof(int), 1, fp);
        fread(&(new_node->direction), sizeof(int), 1, fp);
        
        new_node->next = NULL;
        new_node->prev = prev_node;

        if (prev_node != NULL) {
            prev_node->next = new_node;
        }
        
        if (i == 0) snake_head = new_node;          // 第一个节点是蛇头
        if (i == snake_length - 1) snake_tail = new_node;  // 最后一个节点是蛇尾

        prev_node = new_node;
    }

    fclose(fp);
    
    /* 根据读档的难度设置游戏速度 */
    switch(game_difficulty) {
        case DIFF_EASY:   base_speed = 200; break;
        case DIFF_NORMAL: base_speed = 150; break;
        case DIFF_HARD:   base_speed = 100; break;
        default:          base_speed = 150;
    }

    rebuild_game_area_after_load();  // 重建游戏地图
    return 1;
}

/**
 * @brief 讀檔後重建遊戲區域
 * 功能：根据加载的蛇身和食物数据重新构建游戏地图
 */
void rebuild_game_area_after_load(void) {
    int i, j;
    struct SnakeNode *current;
    int row, col;

    // 重新初始化地图（墙壁和空地）
    for (i = 0; i < HIGH; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HIGH - 1 || j == 0 || j == WIDTH - 1) {
                game_area[i][j] = WALL;
            } else {
                game_area[i][j] = EMPTY;
            }
        }
    }

    // 标记食物位置
    row = food_pos / WIDTH;
    col = food_pos % WIDTH;
    game_area[row][col] = FOOD;

    // 标记所有蛇身位置
    current = snake_head;
    while (current != NULL) {
        row = current->pos / WIDTH;
        col = current->pos % WIDTH;
        game_area[row][col] = SNAKE;
        current = current->next;
    }
}

/* ==================== 游戏流程控制 ==================== */

/**
 * @brief 啟動新遊戲 
 * 功能：完整的新游戏初始化流程
 * 步骤：设置→清理→初始化→运行
 */
void start_new_game(void) {
    /* 停止主菜单音乐，准备播放游戏音乐 */
    stop_background_music();
    play_background_music();

    /* 调用新游戏设置菜单 - 获取玩家名字和难度 */
    show_new_game_menu();
    
    cleanup_game();  // 清理可能存在的旧游戏状态
    
    /* 重置所有遊戲全局變量 */
    score = 0;
    snake_length = 1;
    game_state = RIGHT;
    game_running = 1;
    game_paused = 0;
    save_message_timer = 0;

    initialize_game();    // 初始化游戏数据
    run_game_instance();  // 进入游戏循环
}

/**
 * @brief 從存檔啟動遊戲
 * 功能：加载存档并继续游戏
 * 流程：加载→验证→恢复→运行
 */
void load_and_run_game(void) {
    system("cls");
    printf("正在裝載存檔...\n");
    if (load_game_state()) {
        /* 停止主菜单音乐，准备播放游戏音乐 */
        stop_background_music();
        play_background_music();

        game_running = 1;
        game_paused = 0;
        save_message_timer = 0;
        
        printf("存檔裝載成功！ 歡迎回來, %s！\n", current_player_name);
        printf("难度: %s\n", get_difficulty_string(game_difficulty));
        printf("按任意鍵開始...\n");
        _getch();
        
        run_game_instance();
    } else {
        printf("未找到存檔文件 (%s)，或文件已損壞。\n", SAVE_FILENAME);
        printf("\n按任意鍵返回主菜單...");
        _getch();
    }
}

/**
 * @brief 運行遊戲實例 
 * 功能：设置游戏环境并启动游戏主循环
 */
void run_game_instance(void) {
    /* 設置控制台窗口大小 - 固定窗口尺寸保证显示效果 */
    system("mode con cols=80 lines=30");
    /* 设置游戏颜色 */
    set_console_color(current_color_code); 
    
    hide_cursor();  // 游戏过程中隐藏光标

    game_loop();    // 进入游戏主循环
    
    draw_game();    // 最后绘制一次（显示最终状态）
    show_game_over();  // 显示游戏结束界面

    show_cursor();  // 恢复光标显示
}

/**
 * @brief 顯示主菜單並獲取選擇 
 * @return 用户选择的菜单项
 * 功能：显示美观的主菜单，处理用户选择
 */
int get_main_menu_choice(void) {
    system("cls");
    set_console_color(current_color_code);
    
    // 使用美化边框绘制菜单
    draw_pretty_border(15, 2, 50, 15, "貪吃蛇遊戲");

    // 菜单选项布局
    set_cursor_position(30, 5);
    printf("[1] 開始新遊戲");
    set_cursor_position(30, 6);
    printf("[2] 裝載存檔");
    set_cursor_position(30, 7);
    printf("[3] 遊戲規則");
    set_cursor_position(30, 8);
    printf("[4] 得分排行榜");
    set_cursor_position(30, 9);
    printf("[5] 遊戲設置");
    set_cursor_position(30, 10);
    printf("[6] 退出遊戲");
    
    set_cursor_position(30, 13);
    printf("請選擇 (1-6): ");
    
    return _getch();  // 获取用户选择
}

/* ==================== 新增辅助函数 ==================== */

/**
 * @brief "新游戏" 子菜单 (选择名字和难度)
 * 功能：新游戏开始前的准备工作
 */
void show_new_game_menu(void) {
    get_player_name();    // 获取玩家名字
    select_difficulty();  // 选择游戏难度
}

/**
 * @brief 獲取玩家名稱
 * 功能：让玩家输入名字，支持默认名字
 */
void get_player_name(void) {
    system("cls");
    printf("=========================================\n");
    printf("             開始新遊戲\n");
    printf("=========================================\n\n");
    printf("請輸入你的名字 (最多 %d 個字符):\n", MAX_NAME_LEN - 1);
    printf("(直接按 Enter 可使用默认名字 GuestX)\n> ");
    
    fgets(current_player_name, MAX_NAME_LEN, stdin);
    trim_newline(current_player_name);
    
    /* 如果用戶名為空，生成 GuestX */
    if (current_player_name[0] == '\0') {
        generate_guest_name();
        printf("\n已为你自动命名: %s\n", current_player_name);
        Sleep(1000);
    }
}

/**
 * @brief  生成唯一的 GuestX 名字
 * 算法：读取排行榜文件，找到最大的Guest编号+1
 */
void generate_guest_name(void) {
    FILE *fp;
    int max_guest_num = -1;
    char name[MAX_NAME_LEN];
    int score, diff;
    char date[11];
    
    fp = fopen(SCORE_FILENAME, "r");
    if (fp != NULL) {
        /* 读取得分文件，查找最大的 Guest 编号 */
        while (fscanf(fp, "%s %d %s %d", name, &score, date, &diff) == 4) {
            if (strncmp(name, "Guest", 5) == 0) {
                int num = 0;
                if (sscanf(name + 5, "%d", &num) == 1) {
                    if (num > max_guest_num) {
                        max_guest_num = num;
                    }
                }
            }
        }
        fclose(fp);
    }
    
    sprintf(current_player_name, "Guest%d", max_guest_num + 1);
}

/**
 * @brief 选择难度
 * 功能：让玩家选择游戏难度，影响游戏速度
 */
void select_difficulty(void) {
    int choice = 0;
    system("cls");
    printf("=========================================\n");
    printf("             選擇難度\n");
    printf("=========================================\n\n");
    printf(" [1] 簡單 (速度: 慢)\n");
    printf(" [2] 普通 (速度: 中)\n");
    printf(" [3] 困難 (速度: 快)\n");
    printf("\n");
    printf("=========================================\n");
    printf("請選擇 (1-3): ");
    
    while(choice < '1' || choice > '3') {
        choice = _getch();
    }
    
    switch(choice) {
        case '1':
            game_difficulty = DIFF_EASY;
            base_speed = 200;  // 速度最慢
            printf("已选择: 簡單\n");
            break;
        case '2':
            game_difficulty = DIFF_NORMAL;
            base_speed = 150;
            printf("已选择: 普通\n");
            break;
        case '3':
            game_difficulty = DIFF_HARD;
            base_speed = 100;  // 速度最快
            printf("已选择: 困難\n");
            break;
    }
    Sleep(500);
}

/**
 * @brief 游戏设置菜单
 * 功能：提供游戏各种设置选项
 */
void show_settings_menu(void) {
    int choice = 0;
    while(choice != '3') {
        system("cls");
        printf("=========================================\n");
        printf("               遊戲設置\n");
        printf("=========================================\n\n");
        printf(" [1] 字体和背景颜色设置\n");
        printf(" [2] 音量与音效设置\n");
        printf(" [3] 返回主菜單\n");
        printf("\n");
        printf("=========================================\n");
        printf("請選擇 (1-3): ");
        
        choice = _getch();
        
        switch(choice) {
            case '1':
                select_color_menu();
                break;
			case '2':
			    show_audio_settings_menu();
			    break;
            case '3':
                return; /* 返回主菜单 */
        }
    }
}

/**
 * @brief  选择颜色子菜单
 * 功能：改变控制台颜色主题
 */
void select_color_menu(void) {
    int choice = 0;
    system("cls");
    printf("=========================================\n");
    printf("               選擇顏色主題\n");
    printf("=========================================\n\n");
    printf(" [1] 黑底白字 (默认)\n");
    printf(" [2] 蓝底绿字\n");
    printf(" [3] 黑底红字\n");
    printf(" [4] 黄底蓝字\n");
    printf(" [5] 黑底绿字\n");
    printf(" [6] 返回\n");
    printf("\n=========================================\n");
    printf("請選擇 (1-6): ");
    
    while(choice < '1' || choice > '5') {
        choice = _getch();
    }
    
    switch(choice) {
        case '1': set_console_color(COLOR_BLACK_WHITE); break;
        case '2': set_console_color(COLOR_BLUE_GREEN);  break;
        case '3': set_console_color(COLOR_BLACK_RED);   break;
        case '4': set_console_color(COLOR_YELLOW_BLUE); break;
        case '5': set_console_color(COLOR_BLACK_GREEN); break;
        case '6': return;
    }
    printf("\n颜色已设置！\n");
    Sleep(500);
}

/**
 * @brief 移除字符串末尾的換行符
 * @param s 要处理的字符串
 * 功能：清理fgets读取的字符串中的换行符
 */
void trim_newline(char *s) {
    char *p = strchr(s, '\n');
    if (p) {
        *p = '\0';
    }
}

/**
 * @brief 保存分數到文件 (增加日期和难度)
 * 功能：将游戏成绩持久化保存，支持排行榜功能
 */
void save_score_to_file(void) {
    FILE *fp;
    char date_str[11];
    time_t t;
    struct tm *tm_info;
    
    /* 获取当前日期 */
    time(&t);
    tm_info = localtime(&t);
    strftime(date_str, 11, "%Y-%m-%d", tm_info);
    
    fp = fopen(SCORE_FILENAME, "a");  // 追加模式
    if (fp == NULL) {
        printf("\n警告: 無法保存分數到 %s\n", SCORE_FILENAME);
        return;
    }
    
    /* 写入格式："名字 分数 日期 难度" */
    fprintf(fp, "%s %d %s %d\n", current_player_name, score, date_str, game_difficulty);
    
    fclose(fp);
}

/**
 * @brief 将难度代码转为字符串
 * @param diff 难度代码
 * @return 难度描述字符串
 */
const char* get_difficulty_string(int diff) {
    switch(diff) {
        case DIFF_EASY:   return "简单";
        case DIFF_NORMAL: return "普通";
        case DIFF_HARD:   return "困难";
        default:          return "未知";
    }
}

/**
 * @brief 用於 qsort 的比較函數 (降序)
 * @param a 第一个比较项
 * @param b 第二个比较项  
 * @return 比较结果
 * 功能：按分数从高到低排序
 */
static int compare_scores(const void *a, const void *b) {
    ScoreEntry *entryA = (ScoreEntry*)a;
    ScoreEntry *entryB = (ScoreEntry*)b;
    return entryB->score - entryA->score;  // 降序排列
}

/**
 * @brief 顯示得分排行榜 (增加日期和难度)
 * 功能：读取并显示游戏排行榜，支持排序和分页显示
 */
void show_leaderboard(void) {
    ScoreEntry scores[MAX_SCORES];
    int score_count = 0;
    FILE *fp;
    int i;
    int display_count;

    system("cls");
    
    fp = fopen(SCORE_FILENAME, "r");
    if (fp == NULL) {
        // 无记录时的友好提示
        draw_pretty_border(15, 5, 50, 8, "得分排行榜");
        set_cursor_position(30, 8);
        printf("還沒有任何得分紀錄！");
        set_cursor_position(32, 10);
        printf("快去玩一局吧！");
        set_cursor_position(28, 15);
        printf("按任意鍵返回主菜單...");
        _getch();
        return;
    }
    
    /* 读取所有得分记录 */
    while (score_count < MAX_SCORES && 
           fscanf(fp, "%s %d %s %d", 
                  scores[score_count].name, 
                  &scores[score_count].score,
                  scores[score_count].date,
                  &scores[score_count].difficulty) == 4) {
        score_count++;
    }
    fclose(fp);
    
    if (score_count == 0) {
        printf("无法读取到有效的得分纪录。\n");
        printf("\n按任意鍵返回主菜單...");
        _getch();
        return;
    }

    qsort(scores, score_count, sizeof(ScoreEntry), compare_scores);
    
    /* 顯示排行榜 - 表格形式显示前10名 */
    printf("===================================================================\n");
    printf("                       得分排行榜 (Top 10)                         \n");
    printf("===================================================================\n");
    printf(" 排名 | 用戶名               | 得分   | 难度   | 日期       \n");
    printf("-------------------------------------------------------------------\n");
    
    display_count = score_count > 10 ? 10 : score_count;  // 最多显示10条
    
    for (i = 0; i < display_count; i++) {
        printf(" #%-3d | %-20s | %-6d | %-6s | %-10s\n", 
               i + 1, 
               scores[i].name, 
               scores[i].score,
               get_difficulty_string(scores[i].difficulty),
               scores[i].date);
    }
    
    printf("===================================================================\n");
    printf("\n按任意鍵返回主菜單...");
    _getch();
}

/* ==================== 主函数 - 程序入口 ==================== */

/**
 * @brief 主函數 (程序入口) 
 * 功能：程序启动入口，初始化系统并进入主循环
 * 流程：初始化→主菜单循环→清理退出
 */
int main(void) {
    int choice;
    
    /* 获取控制台句柄 - 用于后续控制台操作 */
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    /* 设置窗口标题和初始颜色 */
    system("title 貪吃蛇遊戲");
    set_console_color(current_color_code);
    
    /* 启动主菜单音乐 */
    play_background_music();

    /* 主程序循环 - 使用GameSnake常量控制无限循环 */
    while (GameSnake) {
        choice = get_main_menu_choice();

        switch (choice) {
            case '1':
                start_new_game();
                break;
            case '2':
                load_and_run_game();
                break;
            case '3':
                show_menu_rules();
                break;
            case '4': 
                show_leaderboard();
                break;
            case '5': /* 游戏设置 */
                show_settings_menu();
                break;
            case '6': /* 退出 */
                stop_background_music(); /* 退出前停止音乐 */
                system("cls");
                printf("\n感謝遊玩，再見！\n");
                Sleep(500);
                return 0;
            default:
                set_cursor_position(30, 14);
            	printf("无效输入，请重新选择...");
            	Sleep(800);
                break;
        }
    }
    
    stop_background_music();
    return 0;
}

/* ============================================================
 * 代码架构总结：
 * 
 * 1. 数据结构：使用双向链表存储蛇身，结构体存储游戏数据
 * 2. 模块划分：清晰分为初始化、游戏逻辑、界面、音效、存档等模块
 * 3. 状态管理：全局变量管理游戏状态，函数封装具体操作
 * 4. 用户交互：多级菜单系统，支持键盘各种操作
 * 5. 数据持久化：二进制存档和文本排行榜
 * 6. 错误处理：内存分配检查，文件操作容错
 * 7. 用户体验：音效、动画、颜色、进度条等增强功能
 * 
 * 学习要点：
 * - 链表操作：节点的创建、插入、删除、遍历
 * - 游戏循环：输入→更新→渲染的基本模式
 * - 碰撞检测：边界、墙壁、自身的检测算法
 * - 内存管理：malloc/free的正确使用
 * - 文件操作：二进制和文本文件的读写
 * - 控制台编程：光标控制、颜色设置、键盘输入
 * ============================================================ */
