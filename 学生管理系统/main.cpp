//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>
//
//// 学生结构体定义
//typedef struct Student {
//    char id[20];
//    char name[50];
//    char gender[10];
//    int age;
//    float score;
//    struct Student *next;
//} Student;
//
//typedef struct Teacher {
//    char id[20];
//    char course[50]; // 教授课程
//    Student *students; // 关联学生
//} Teacher;
//
//// 全局变量
//Student *head = NULL;
//const char *FILENAME = "students.dat"; // 数据存储文件
//
//// 函数声明
//void admin_menu();
//void student_menu();
//void add_student();
//void delete_student(char *id);
//void modify_student(char *id);
//void search_student(char *id);
//void display_all();
//void save_to_file();
//void load_from_file();
//float calculate_avg();
//
//// ========== 主函数 ==========
//int main() {
//    load_from_file(); // 启动时加载数据
//    int role;
//    printf("\n=== 学生管理系统 ===\n");
//    printf("1. 管理员登录\n2. 学生登录\n选择身份: ");
//    scanf("%d", &role);
//    
//    if (role == 1) admin_menu();
//    else student_menu();
//    
//    return 0;
//}
//
//// ========== 管理员菜单 ==========
//void admin_menu() {
//    int choice;
//    char target_id[20];
//    do {
//        printf("\n----- 管理员菜单 -----\n");
//        printf("1. 添加学生\n2. 删除学生\n3. 修改信息\n4. 查询学生\n5. 显示所有\n6. 成绩统计\n7. 保存退出\n选择: ");
//        scanf("%d", &choice);
//        
//        switch(choice) {
//            case 1: add_student(); break;
//            case 2: 
//                printf("输入要删除的学号: ");
//                scanf("%s", target_id);
//                delete_student(target_id); 
//                break;
//            case 3: 
//                printf("输入要修改的学号: ");
//                scanf("%s", target_id);
//                modify_student(target_id); 
//                break;
//            case 4: 
//                printf("输入查询学号: ");
//                scanf("%s", target_id);
//                search_student(target_id); 
//                break;
//            case 5: display_all(); break;
//            case 6: 
//                printf("班级平均分: %.2f\n", calculate_avg()); 
//                break;
//            case 7: 
//                save_to_file(); 
//                printf("数据已保存！\n");
//                exit(0);
//            default: printf("无效选项！\n");
//        }
//    } while(1);
//}
//
//// ========== 核心功能实现 ==========
//// 添加学生
//void add_student() {
//    Student *new_stu = (Student*)malloc(sizeof(Student));
//    printf("输入学号: "); scanf("%s", new_stu->id);
//    printf("姓名: ");    scanf("%s", new_stu->name);
//    printf("性别: ");    scanf("%s", new_stu->gender);
//    printf("年龄: ");    scanf("%d", &new_stu->age);
//    printf("成绩: ");    scanf("%f", &new_stu->score);
//    
//    // 链表插入
//    new_stu->next = head;
//    head = new_stu;
//    printf("【成功】学生 %s 已添加！\n", new_stu->name);
//}
//
//// 删除学生
//void delete_student(char *id) {
//    Student *current = head, *prev = NULL;
//    while(current != NULL) {
//        if(strcmp(current->id, id) == 0) {
//            if(prev) prev->next = current->next;
//            else head = current->next;
//            free(current);
//            printf("【成功】学号 %s 已删除\n", id);
//            return;
//        }
//        prev = current;
//        current = current->next;
//    }
//    printf("【错误】未找到该学号！\n");
//}
//
//// 文件存储（二进制格式）
//void save_to_file() {
//    FILE *fp = fopen(FILENAME, "wb");
//    Student *current = head;
//    while(current != NULL) {
//        fwrite(current, sizeof(Student), 1, fp);
//        current = current->next;
//    }
//    fclose(fp);
//}
//
//void load_from_file() {
//    FILE *fp = fopen(FILENAME, "rb");
//    if(!fp) return;
//    
//    Student temp, *new_stu;
//    while(fread(&temp, sizeof(Student), 1, fp)) {
//        new_stu = (Student*)malloc(sizeof(Student));
//        *new_stu = temp;
//        new_stu->next = head;
//        head = new_stu;
//    }
//    fclose(fp);
//}
//
//void student_menu() {
//    char my_id[20];
//    printf("输入您的学号: ");
//    scanf("%s", my_id);
//    search_student(my_id); // 只能查看自己
//}
//
//void sort_by_score() {
//    // 冒泡排序链表节点[9](@ref)
//    Student *i, *j;
//    for(i = head; i != NULL; i = i->next) {
//        for(j = i->next; j != NULL; j = j->next) {
//            if(i->score < j->score) {
//                swap_student_data(i, j); // 交换数据函数
//            }
//        }
//    }
//}

