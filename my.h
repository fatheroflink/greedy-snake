//
// Created by wangqingfeng on 2020/9/21.
//

#ifndef TEST3_MY_H
#define TEST3_MY_H

#endif //TEST3_MY_H

#include "stdbool.h"

#define MAP_WIDTH 50
#define MAP_HEIGTH 25

#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3

#define CHAR_UP_BORDER '-'
#define CHAR_DOWN_BORDER '|'
#define CHAR_SNAKE '>'
#define CHAR_SPACE ' '
#define CHAR_FOOD 'O'

#define MAP_BORDER 0
#define MAP_SPACE 1
#define MAP_SNAKE 2
#define MAP_FOOD 3

#define DEFAULT_SLEEP_TIME 1000000

/**
 * 二维数组节点结构
 */
struct node {
    int i;
    struct node* next;
};

/**
 * 整个游戏代表的内存结构
 * 地图为一个HEIGTH*WIDTH大小的二维数组
 *
 */
struct snake_base {
    struct node MAP[MAP_HEIGTH][MAP_WIDTH];     //地图数组
    struct node* header;    /*! 贪吃蛇头节点 */
    struct node* tail;  /*! 贪吃蛇尾节点 */
    volatile int direction;  /*! 当前贪吃蛇行进方向
                    DIR_UP 上
                    DIR_DOWN 下
                    DIR_LEFT 左
                    DIR_RIGHT 右 */

    int sleeptime;   /*! 游戏速度，单位：微秒 */
    int len;    /*! 贪吃蛇长度 */
};

/**
 * 处理键盘输入
 */
void* key_process(void* arg);

/**
 * 创建用户输入线程
 */
void create_thread(struct snake_base* );

void get_rand(struct node  p[][MAP_WIDTH], int* , int* );

void print_snake(struct snake_base*);
void init_snake(struct snake_base*);
void act(struct snake_base*);
struct node* get_next_node(struct node*, int);