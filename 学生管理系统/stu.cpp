#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
 
// ---------------------------
// 学生信息结构体定义
// ---------------------------
typedef struct Student {
    char id[20];         // 学号
    char name[50];       // 姓名
    char gender[10];     // 性别，例如 "男" 或 "女"
    int age;             // 年龄
    float score;         // 成绩
    struct Student *next; // 指向下一个学生记录的指针
} Student;

// ---------------------------
// 函数原型声明
// ---------------------------
Student* add_student(Student *head);
Student* delete_student(Student *head, const char *id);
Student* modify_student(Student *head, const char *id);
Student* search_student(Student *head, const char *id_or_name);
void display_all_students(Student *head);
void free_all_students(Student *head);
void print_menu();
 
// ---------------------------
// 主函数：学生管理系统入口
// ---------------------------
int main() {
    Student *head = NULL; // 初始化学生链表为空
    int choice;
    char id_or_name[50];
 
    while (true) {
        print_menu();
        printf("请输入操作选项: ");
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "输入错误，请输入数字选项！\n");
            // 清空输入缓冲区
            while(getchar() != '\n');
            continue;
        }
        // 清空输入缓冲区
        while(getchar() != '\n');
 
        switch (choice) {
            case 1:
                head = add_student(head);
                break;
            case 2:
                printf("请输入要删除的学生学号: ");
                fgets(id_or_name, sizeof(id_or_name), stdin);
                id_or_name[strcspn(id_or_name, "\n")] = '\0'; // 去除换行符
                head = delete_student(head, id_or_name);
                break;
            case 3:
                printf("请输入要修改的学生学号: ");
                fgets(id_or_name, sizeof(id_or_name), stdin);
                id_or_name[strcspn(id_or_name, "\n")] = '\0';
                head = modify_student(head, id_or_name);
                break;
            case 4:
                printf("请输入要查询的学生学号或姓名: ");
                fgets(id_or_name, sizeof(id_or_name), stdin);
                id_or_name[strcspn(id_or_name, "\n")] = '\0';
                {
                    Student *found = search_student(head, id_or_name);
                    if (found) {
                        printf("查找到的学生信息:\n");
                        printf("学号: %s\n", found->id);
                        printf("姓名: %s\n", found->name);
                        printf("性别: %s\n", found->gender);
                        printf("年龄: %d\n", found->age);
                        printf("成绩: %.2f\n", found->score);
                    } else {
                        printf("未找到匹配的学生记录。\n");
                    }
                }
                break;
            case 5:
                display_all_students(head);
                break;
            case 0:
                free_all_students(head);
                printf("退出学生管理系统！\n");
                exit(EXIT_SUCCESS);
                break;
            default:
                printf("无效的选项，请重新输入！\n");
                break;
        }
    }
    return 0;
}
 
// ---------------------------
// 函数实现：打印操作菜单
// ---------------------------
void print_menu() {
    printf("\n========== 学生管理系统 ==========\n");
    printf("1. 添加学生\n");
    printf("2. 删除学生\n");
    printf("3. 修改学生信息\n");
    printf("4. 查询学生信息\n");
    printf("5. 显示所有学生\n");
    printf("0. 退出系统\n");
    printf("==================================\n");
}
 
// ---------------------------
// 函数实现：添加学生
// ---------------------------
/*
 * 函数名称: add_student
 * 功能: 从用户输入中获取学生信息，创建新学生节点并插入链表中
 * 参数:
 *    head - 当前学生链表头指针
 * 返回值:
 *    更新后的学生链表头指针
 *
 * 说明:
 *    1. 提示用户输入学号、姓名、性别、年龄和成绩。
 *    2. 创建新节点并将信息存入，插入到链表尾部（或头部）。
 */
Student* add_student(Student *head) {
    Student *new_student = (Student *)malloc(sizeof(Student));
    if (new_student == NULL) {
        fprintf(stderr, "错误：内存分配失败，无法添加新学生！\n");
        return head;
    }
 
    printf("请输入学号: ");
    fgets(new_student->id, sizeof(new_student->id), stdin);
    new_student->id[strcspn(new_student->id, "\n")] = '\0';
 
    printf("请输入姓名: ");
    fgets(new_student->name, sizeof(new_student->name), stdin);
    new_student->name[strcspn(new_student->name, "\n")] = '\0';
 
    printf("请输入性别 (男/女): ");
    fgets(new_student->gender, sizeof(new_student->gender), stdin);
    new_student->gender[strcspn(new_student->gender, "\n")] = '\0';
 
    printf("请输入年龄: ");
    scanf("%d", &new_student->age);
    while(getchar() != '\n');  // 清空输入缓冲区
 
    printf("请输入成绩: ");
    scanf("%f", &new_student->score);
    while(getchar() != '\n');
 
    new_student->next = NULL;
 
    // 将新节点插入到链表末尾
    if (head == NULL) {
        head = new_student;
    } else {
        Student *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_student;
    }
    printf("添加学生成功！\n");
    return head;
}
 
// ---------------------------
// 函数实现：删除学生
// ---------------------------
/*
 * 函数名称: delete_student
 * 功能: 根据输入的学号删除对应的学生记录
 * 参数:
 *    head - 当前学生链表头指针
 *    id   - 待删除学生的学号
 * 返回值:
 *    更新后的学生链表头指针
 *
 * 说明:
 *    1. 遍历链表查找学号匹配的节点。
 *    2. 若找到，将其从链表中删除，并释放内存；若未找到，则提示用户。
 */
Student* delete_student(Student *head, const char *id) {
    if (head == NULL) {
        printf("学生记录为空，无法删除！\n");
        return head;
    }
 
    Student *temp = head;
    Student *prev = NULL;
 
    // 检查头节点是否为目标学生
    if (strcmp(head->id, id) == 0) {
        head = head->next;
        free(temp);
        printf("删除学号 %s 的学生成功！\n", id);
        return head;
    }
 
    // 遍历链表查找目标节点
    while (temp != NULL && strcmp(temp->id, id) != 0) {
        prev = temp;
        temp = temp->next;
    }
 
    if (temp == NULL) {
        printf("未找到学号为 %s 的学生记录。\n", id);
    } else {
        prev->next = temp->next;
        free(temp);
        printf("删除学号 %s 的学生成功！\n", id);
    }
    return head;
}
 
// ---------------------------
// 函数实现：修改学生信息
// ---------------------------
/*
 * 函数名称: modify_student
 * 功能: 根据输入的学号查找学生记录，并允许用户修改该学生的信息
 * 参数:
 *    head - 当前学生链表头指针
 *    id   - 待修改学生的学号
 * 返回值:
 *    更新后的学生链表头指针
 *
 * 说明:
 *    1. 遍历链表查找目标学生记录。
 *    2. 若找到，提示用户选择需要修改的项（姓名、性别、年龄、成绩），并更新相应数据。
 *    3. 若未找到，则提示用户未找到该学生记录。
 */
Student* modify_student(Student *head, const char *id) {
    Student *temp = head;
    while (temp != NULL) {
        if (strcmp(temp->id, id) == 0) {
            printf("找到学生记录：\n");
            printf("学号: %s\n", temp->id);
            printf("姓名: %s\n", temp->name);
            printf("性别: %s\n", temp->gender);
            printf("年龄: %d\n", temp->age);
            printf("成绩: %.2f\n", temp->score);
            printf("请选择需要修改的项：\n");
            printf("1. 姓名\n");
            printf("2. 性别\n");
            printf("3. 年龄\n");
            printf("4. 成绩\n");
            printf("请输入选项（1-4）： ");
            int choice;
            scanf("%d", &choice);
            while(getchar() != '\n');
            switch (choice) {
                case 1:
                    printf("请输入新的姓名: ");
                    fgets(temp->name, sizeof(temp->name), stdin);
                    temp->name[strcspn(temp->name, "\n")] = '\0';
                    break;
                case 2:
                    printf("请输入新的性别 (男/女): ");
                    fgets(temp->gender, sizeof(temp->gender), stdin);
                    temp->gender[strcspn(temp->gender, "\n")] = '\0';
                    break;
                case 3:
                    printf("请输入新的年龄: ");
                    scanf("%d", &temp->age);
                    while(getchar() != '\n');
                    break;
                case 4:
                    printf("请输入新的成绩: ");
                    scanf("%f", &temp->score);
                    while(getchar() != '\n');
                    break;
                default:
                    printf("无效选项，不修改记录。\n");
                    break;
            }
            printf("修改成功！\n");
            return head;
        }
        temp = temp->next;
    }
    printf("未找到学号为 %s 的学生记录。\n", id);
    return head;
}
 
// ---------------------------
// 函数实现：查询学生信息
// ---------------------------
/*
 * 函数名称: search_student
 * 功能: 根据输入的学号或姓名查找学生记录
 * 参数:
 *    head       - 当前学生链表头指针
 *    id_or_name - 待查询的学号或姓名
 * 返回值:
 *    若找到，返回指向该学生记录的指针；若未找到，则返回 NULL
 */
Student* search_student(Student *head, const char *id_or_name) {
    Student *temp = head;
    while (temp != NULL) {
        if (strcmp(temp->id, id_or_name) == 0 || strcmp(temp->name, id_or_name) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}
 
// ---------------------------
// 函数实现：显示所有学生信息
// ---------------------------
/*
 * 函数名称: display_all_students
 * 功能: 遍历学生链表，按列表形式打印所有学生的详细信息
 * 参数:
 *    head - 当前学生链表头指针
 * 返回值:
 *    无
 */
void display_all_students(Student *head) {
    if (head == NULL) {
        printf("学生记录为空。\n");
        return;
    }
    printf("\n所有学生信息：\n");
    printf("-----------------------------------------------------\n");
    printf("学号\t\t姓名\t\t性别\t年龄\t成绩\n");
    printf("-----------------------------------------------------\n");
    Student *temp = head;
    while (temp != NULL) {
        printf("%-10s\t%-10s\t%-4s\t%d\t%.2f\n", temp->id, temp->name, temp->gender, temp->age, temp->score);
        temp = temp->next;
    }
    printf("-----------------------------------------------------\n");
}
 
// ---------------------------
// 函数实现：释放所有学生记录的内存
// ---------------------------
/*
 * 函数名称: free_all_students
 * 功能: 遍历学生链表并释放所有节点占用的内存
 * 参数:
 *    head - 当前学生链表头指针
 * 返回值:
 *    无
 */
void free_all_students(Student *head) {
    Student *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
