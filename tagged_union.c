#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Tagged union: a value is one of {int, double, owned string}.
  This demonstrates:
    - enum tag + union payload
    - switch-based dispatch
    - manual lifetime management (destroy + deep copy)
*/

typedef enum {
    V_INT,
    V_DOUBLE,
    V_OWNED_STR
} VTag;

typedef struct {
    VTag tag;
    union {
        long i;
        double d;
        char *s;   // owning pointer when tag == V_OWNED_STR
    } u;
} Value;

/* Constructors */
static inline Value value_int(long x) {
    Value v;
    v.tag = V_INT;
    v.u.i = x;
    return v;
}

static inline Value value_double(double x) {
    Value v;
    v.tag = V_DOUBLE;
    v.u.d = x;
    return v;
}

static Value value_owned_str(const char *src) {
    Value v;
    v.tag = V_OWNED_STR;
    size_t n = strlen(src) + 1;
    v.u.s = (char *)malloc(n);
    if (!v.u.s) abort();
    memcpy(v.u.s, src, n);
    return v;
}

/* Destructor: required because V_OWNED_STR owns heap memory */
static void value_destroy(Value *v) {
    if (!v) return;
    if (v->tag == V_OWNED_STR) {
        free(v->u.s);
        v->u.s = NULL;
    }
    v->tag = V_INT;
    v->u.i = 0;
}

/* Deep copy: required to avoid double free on owned string */
static Value value_copy(const Value *src) {
    if (!src) abort();
    switch (src->tag) {
        case V_INT:    return value_int(src->u.i);
        case V_DOUBLE: return value_double(src->u.d);
        case V_OWNED_STR:
            return value_owned_str(src->u.s ? src->u.s : "");
        default:
            abort();
    }
}

/* “Dispatch” by switching on tag */
static void value_print(const Value *v) {
    switch (v->tag) {
        case V_INT:
            printf("INT: %ld\n", v->u.i);
            break;
        case V_DOUBLE:
            printf("DOUBLE: %f\n", v->u.d);
            break;
        case V_OWNED_STR:
            printf("OWNED_STR: %s\n", v->u.s);
            break;
        default:
            abort();
    }
}

static long value_score(const Value *v) {
    /* Example “operation”: produce an integer score */
    switch (v->tag) {
        case V_INT:
            return v->u.i + 1;
        case V_DOUBLE:
            return (long)(v->u.d) + 2;
        case V_OWNED_STR:
            return (long)strlen(v->u.s) + 3;
        default:
            abort();
    }
}

int main(void) {
    Value a = value_int(10); 
    Value b = value_double(3.14);
    Value c = value_owned_str("hello");

    value_print(&a);
    value_print(&b);
    value_print(&c);

    printf("score(a)=%ld\n", value_score(&a));
    printf("score(b)=%ld\n", value_score(&b));
    printf("score(c)=%ld\n", value_score(&c));

    Value c2 = value_copy(&c);
    value_print(&c2);

    value_destroy(&a);
    value_destroy(&b);
    value_destroy(&c);
    value_destroy(&c2);
    return 0;
}
