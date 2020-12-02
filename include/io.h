/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

Byte inb(unsigned short port);
void printc(char c);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);

void printc_color(Byte color, char c);
void printc_scroll(char c, Byte color);
void println(char *string);
void panic(char *string);
void printk_color(char *string, Byte color);

#endif /* __IO_H__ */
