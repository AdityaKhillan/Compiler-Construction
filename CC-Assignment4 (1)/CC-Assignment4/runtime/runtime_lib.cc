#include <cstdio>

extern "C"
void printi_i(int i) {
    printf("%d\n", i);
}
extern "C"
void printi_s(short i) {
    printf("%hd\n", i);
}
extern "C"
void printi_l(long i) {
    printf("%ld\n", i);
}