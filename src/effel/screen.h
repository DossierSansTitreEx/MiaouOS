#ifndef _SCREEN_H
#define _SCREEN_H 1

void kscroll();
void kputchar(char c);
void kprint(const char* s);
void kputs(const char* s);
void kprint_hex(uint64_t num);
void screen_init();

#endif
