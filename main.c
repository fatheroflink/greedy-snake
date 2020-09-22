#include <stdio.h>
#include <stdlib.h>

#include "my.h"
#include <unistd.h>
#include<pthread.h>
#include <termios.h>

int main() {
    struct snake_base snake;
    init_snake(&snake);  //初始化贪吃蛇
    create_thread(&snake);
    while (1) {
        usleep(snake.sleeptime);
        act(&snake);
    }
    return 0;
}

/*!
 * 初始化整个游戏
 */
void init_snake(struct snake_base* snake) {
    struct node (*map)[MAP_WIDTH] = snake->MAP;

    /*!
     * 初始化地图二维数组，数组中的每一个元素分别为
     * MAP_BORDER：边界
     * MAP_SPACE：空白
     * MAP_SNAKE：蛇身
     * MAP_FOOD: 食物
     */
    for (int i = 0; i < MAP_HEIGTH; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (i == 0 || i == MAP_HEIGTH - 1 || j == 0 || j == MAP_WIDTH - 1) {   // 首行 || 末行 || 首列 || 末列  都初始化为边界
                map[i][j].i = MAP_BORDER;
            } else {    //其他节点被初始化为空白
                map[i][j].i = MAP_SPACE;
            }
            map[i][j].next = NULL;
        }
    }

    /*!
     * 初始化蛇的行进方向，默认向右
     */
    snake->direction = DIR_RIGHT;

    /*!
    * 初始化游戏速度，默认为100000毫秒
    */
    snake->sleeptime = DEFAULT_SLEEP_TIME;

    /*!
     * 初始化蛇身，生成一个随机的节点作为起点,
     * 并且将该节点既作为头节点，也作为尾节点
     */
    int x, y;
    get_rand(map, &x, &y);
    map[x][y].i = MAP_SNAKE;
    snake->header = snake->tail = &(map[x][y]);

    /*!
     * 初始化贪吃蛇长度为1
     */
    snake->len = 1;

    /*!
     * 初始化食物，生成一个随机的节点作为食物
     */
    get_rand(map, &x, &y);
    map[x][y].i = MAP_FOOD;


}

/*!
 * 获取二维数组中一个随机的元素，该元素必须代表空白，否则重新生成（算法比较暴力，凑乎用吧）
 */
void get_rand(struct node  p[][MAP_WIDTH], int* x, int* y) {
    while (1) {
        int rand_x = rand() % MAP_HEIGTH;
        int rand_y = rand() % MAP_WIDTH;
        if (p[rand_x][rand_y].i != MAP_SPACE)
            continue;
        *x = rand_x;
        *y = rand_y;
        break;
    }
}

/*!
 * 创建新线程，用于处理键盘输入，并将snake_base指针作为参数传给新线程
 */
void create_thread(struct snake_base* snake) {
    pthread_t thid;
    if(pthread_create(&thid, NULL, (void *)key_process, snake) != 0) {
        printf("thread creation failed\n");
        exit(1);
    }
}

/*!
 * 自定义从标准输入中读取字符函数，无回显，无缓冲
 */
int getch (void)
{
    int ch;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

/*!
 * 处理键盘输入，根据输入改变snake->direction的值
 */

void* key_process(void* arg) {
    struct snake_base* snake = (struct snake_base*)arg;
    char c;
    while ((c = getch()) != '.') {
        if (c == '\n')
            continue;
        switch (c) {
            case 'w':
                if (snake->direction != DIR_DOWN)
                    snake->direction = DIR_UP;
                break;
            case 's':
                if (snake->direction != DIR_UP)
                    snake->direction = DIR_DOWN;
                break;
            case 'a':
                if (snake->direction != DIR_RIGHT)
                    snake->direction = DIR_LEFT;
                break;
            case 'd':
                if (snake->direction != DIR_LEFT)
                    snake->direction = DIR_RIGHT;
                break;
            case '=':
                snake->sleeptime -= snake->sleeptime*0.2;
                printf("速度增加20%%\n");
                break;
            case '-':
                snake->sleeptime += snake->sleeptime*0.2;
                printf("速度减少20%%\n");
                break;
            default:
                printf("按键错误");
        }
    }
    return NULL;
}

/*!
 * 将地图中的内容刷新到屏幕上
 */
void print_snake(struct snake_base* snake) {
    system("clear");
    for (int i = 0; i < MAP_HEIGTH; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            switch (snake->MAP[i][j].i) {
                case MAP_BORDER:
                    if (i == 0 || i == MAP_HEIGTH - 1) {   //第一行 || 最后一行
                        putchar(CHAR_UP_BORDER);
                    } else {
                        putchar(CHAR_DOWN_BORDER);
                    }
                    break;
                case MAP_SPACE:
                    putchar(CHAR_SPACE);
                    break;
                case MAP_SNAKE:
                    putchar(CHAR_SNAKE);
                    break;
                case MAP_FOOD:
                    putchar(CHAR_FOOD);
                    break;
                default:
                    exit(-1);
            }
        }
        putchar('\n');
    }
    printf("您目前的得分是：%d\n", snake->len);
    printf("您目前的速度是：%2f倍\n", (double)DEFAULT_SLEEP_TIME / snake->sleeptime);
}

/*!
 * 描述贪吃蛇行进函数，本程序中最主要的函数
 */
void act(struct snake_base* snake) {

    //获取下一步头节点应该移动到的位置
    struct node* next = get_next_node(snake->header, snake->direction);

    //如果下一步移动到的位置是边界或者是贪吃蛇本身，则推出游戏
    if (next->i == MAP_BORDER || next->i == MAP_SNAKE) {
        system("stty cooked");
        printf("GAME OVER!\n");
        exit(0);
    }

    /*!
     * 如果下一步移动到的位置不是边界或者贪吃蛇本身则：
     * 1、将头节点的指针指向下一个节点
     * 2、将下一个节点作为当前的头节点
     */

    snake->header->next = next;
    snake->header = next;


    if (next->i == MAP_SPACE) {
        /*!
         * 如果下一步移动到的位置是空白，则
         * 1、将尾节点置为空白
         * 2、将尾节点的下一个节点作为新的尾节点
         */
        snake->tail->i = MAP_SPACE;
        snake->tail = snake->tail->next;
    } else {
        /*!
         * 如果下一步移动到的位置是食物，则新生成一个食物，蛇长加1
         */
        int x, y;
        get_rand(snake->MAP, &x, &y);
        snake->MAP[x][y].i = MAP_FOOD;
        snake->len++;
    }

    /*!
     * 无论新移动到的位置是空白还是食物，都需要将该位置标记为贪吃蛇本身
     */
    snake->header->i = MAP_SNAKE;

    /*!
     * 刷新屏幕
     */
    print_snake(snake);
}

/*!
 * 根据行进方向获取下一个节点的指针
 */
struct node* get_next_node(struct node* p, int direction) {
    switch (direction) {
        case DIR_UP:
            return p - MAP_WIDTH;
        case DIR_DOWN:
            return p + MAP_WIDTH;
        case DIR_LEFT:
            return p - 1;
        case DIR_RIGHT:
            return p + 1;
        default:
            printf("无此方向");
            exit(-1);
    }
}
