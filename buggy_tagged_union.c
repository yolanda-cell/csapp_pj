#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { V_INT, V_OWNED_STR } VTag;

typedef struct {
    VTag tag;
    union {
        long i;
        char *s;   // owning pointer when tag == V_OWNED_STR
    } u;
} Value;

static Value make_int(long x) {
    Value v;
    v.tag = V_INT;
    v.u.i = x;
    return v;
}

static Value make_owned_str(const char *src) {
    Value v;
    v.tag = V_OWNED_STR;
    size_t n = strlen(src) + 1;
    v.u.s = (char *)malloc(n);
    if (!v.u.s) abort();
    memcpy(v.u.s, src, n);
    return v;
}

static void destroy(Value *v) {
    if (!v) return;
    if (v->tag == V_OWNED_STR) {
        free(v->u.s);
        v->u.s = NULL;
    }
    v->tag = V_INT;
    v->u.i = 0;
}

/*
  BUG: "copy" is just a struct assignment. If tag==V_OWNED_STR,
  both Values will point to the same heap string. Destroying both
  causes double free.
*/
static Value shallow_copy(Value v) {
    return v; // buggy for owned strings
}

static void print_value(const Value *v) {
    if (v->tag == V_INT) {
        printf("INT: %ld\n", v->u.i);
    } else if (v->tag == V_OWNED_STR) {
        printf("OWNED_STR: %s\n", v->u.s);
    } else {
        printf("UNKNOWN\n");
    }
}

int main(void) {
    Value a = make_owned_str("hello");
    Value b = shallow_copy(a);    // b.u.s aliases a.u.s

    print_value(&a);
    print_value(&b);

    destroy(&a);
    destroy(&b);                  // double free here (or heap corruption earlier)

    return 0;
}