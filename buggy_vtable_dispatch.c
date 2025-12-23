#include <stdio.h>
#include <stdlib.h>

typedef struct Shape Shape;

typedef struct {
    double (*area)(const Shape *self);
    void   (*destroy)(Shape *self);
} ShapeVTable;

struct Shape {
    const ShapeVTable *vt;
};

static inline double shape_area(const Shape *s) {
    return s->vt->area(s);   // crash if vt is NULL/garbage
}

static inline void shape_destroy(Shape *s) {
    s->vt->destroy(s);
}

/* One concrete type: Circle */
typedef struct {
    Shape base;
    double r;
} Circle;

static double circle_area(const Shape *s) {
    const Circle *c = (const Circle *)s;
    return 3.14159 * c->r * c->r;
}

static void circle_destroy(Shape *s) {
    free(s);
}

static const ShapeVTable circle_vt = {
    .area = circle_area,
    .destroy = circle_destroy
};

/*
  BUG: forget to set c->base.vt = &circle_vt;
  So vt is uninitialized, likely NULL/garbage, and dispatch crashes.
*/
static Shape *circle_create_buggy(double r) {
    Circle *c = (Circle *)malloc(sizeof(Circle));
    if (!c) return NULL;
    /* missing: c->base.vt = &circle_vt; */
    c->r = r;
    return (Shape *)c;
}

int main(void) {
    Shape *s = circle_create_buggy(2.0);

    printf("area=%f\n", shape_area(s));   // typically crashes here

    shape_destroy(s);
    return 0;
}