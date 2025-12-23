#include <stdio.h>
#include <stdlib.h>

/*
  Vtable dispatch in C:
    - base "class" Shape contains vtable pointer
    - derived "classes" embed Shape as first field
    - methods are function pointers stored in vtables
*/

typedef struct Shape Shape;

typedef struct {
    double (*area)(const Shape *self);
    void   (*destroy)(Shape *self);
} ShapeVTable;

struct Shape {
    const ShapeVTable *vt;
};

/* Base dispatchers (like virtual calls) */
static inline double shape_area(const Shape *s) {
    return s->vt->area(s);
}

static inline void shape_destroy(Shape *s) {
    s->vt->destroy(s);
}

/* ---- Circle ---- */
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

static Shape *circle_create(double r) {
    Circle *c = (Circle *)malloc(sizeof(Circle));
    if (!c) return NULL;
    c->base.vt = &circle_vt;
    c->r = r;
    return (Shape *)c;
}

/* ---- Rect ---- */
typedef struct {
    Shape base;
    double w, h;
} Rect;

static double rect_area(const Shape *s) {
    const Rect *r = (const Rect *)s;
    return r->w * r->h;
}

static void rect_destroy(Shape *s) {
    free(s);
}

static const ShapeVTable rect_vt = {
    .area = rect_area,
    .destroy = rect_destroy
};

static Shape *rect_create(double w, double h) {
    Rect *r = (Rect *)malloc(sizeof(Rect));
    if (!r) return NULL;
    r->base.vt = &rect_vt;
    r->w = w;
    r->h = h;
    return (Shape *)r;
}

int main(void) {
    Shape *s1 = circle_create(2.0);
    Shape *s2 = rect_create(3.0, 4.0);

    printf("area(s1)=%f\n", shape_area(s1));
    printf("area(s2)=%f\n", shape_area(s2));

    shape_destroy(s1);
    shape_destroy(s2);
    return 0;
}
