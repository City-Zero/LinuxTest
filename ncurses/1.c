/*************************************************************************
	> File Name: 1.c
	> Author: 
	> Mail: 
	> Created Time: 2016年08月12日 星期五 15时34分20秒
 ************************************************************************/

#include<stdio.h>
#include <ncurses.h>
int main()
{
    int ch;
    initscr();
    /* 开始 curses 模式
     * */
    raw();
    /* 禁用行缓冲
     * */
    keypad(stdscr, TRUE);
    /* 开启功能键响应模式
     * */
    noecho();
    /* 当执行 getch()函数的时候关闭键盘回显 */
    printw("Type any character to see it in bold\n");
    ch = getch();
    /* 如果没有调用 raw()函数,
     * 我们必须按下 enter 键才可以将字符传递给程序*/
    if(ch == KEY_F(1))
    /* 如果没有调用 keypad()初始化,将不会执行这条语句 */
    printw("F1 Key pressed");
    /* 如果没有使用 noecho() 函数,一些控制字符将会被打印到屏幕上 */
    else
    {
        printw("The pressed key is ");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);

    }
    refresh();
    /* 将缓冲区的内容打印到显示器上 */
    getch();
    /* 等待用户输入 */
    endwin();
    /* 结束 curses 模式 */
    return 0;

}
