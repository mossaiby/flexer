/* 
   Parser stress test file
   Intentionally uses many C constructs.
   No preprocessor directives anywhere.
*/

typedef int i32;          // 32-bit integer alias
typedef unsigned long usize; /* platform-sized unsigned */

// String escaping
const char *s1 = "\\";
const char *s2 = "\"";
const char *s3 = "\n";
const char *s4 = "\r";
const char *s5 = "\t";
const char *s6 = "\b";
const char *s7 = "\f";
const char *s8 = "\a";
const char *s9 = "\v";
const char *s10 = "This is a\ntest!";

enum Color {
    RED = 1,      // explicit value
    GREEN,        // implicit increment
    BLUE = 10     /* jump in enum values */
};

/* Testing multiple comments *//* Simple 2D vector */
struct Vec2 {
    double x; /* x-coordinate */
    double y; /* y-coordinate */
};

/* Linked list node */
struct Node {
    int value;
    struct Node* next; // pointer to next node
};

/* Global state (avoid in real code!) */
static int global_counter = 0;

/* Adds two integers */
int add(int a, int b)
{
    return a + b; // trivial
}

/* Recursive factorial
   n <= 1 is the base case
*/
int factorial(int n)
{
    if (n <= 1)
        return 1; /* stop recursion */
    return n * factorial(n - 1);
}

/* Dot product of two vectors */
double dot(struct Vec2 a, struct Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

/* Increments a global variable */
void increment_global(void)
{
    global_counter++; // side effect
}

/* Sum elements of an integer array */
int sum_array(const int* arr, int count)
{
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += arr[i]; // accumulate
    }
    return sum;
}

/* Builds a simple linked list in-place
   Assumes enough contiguous memory
*/
void build_list(struct Node* head, int n)
{
    struct Node* current = head;
    for (int i = 0; i < n; i++) {
        current->value = i;
        current->next = (struct Node*)0; /* null pointer */
        if (i + 1 < n) {
            current->next = current + 1; // pointer arithmetic
            current = current->next;
        }
    }
}

/* Expression and operator test */
int test_expressions(void)
{
    int a = 3;
    int b = 4;
    int c = 0;

    c = a + b * 2;               // precedence
    c += (a < b) ? 1 : -1;       /* ternary */
    c = (int)((double)c / 2.0);  // cast chain

    return c;
}

/* Switch statement test */
int switch_test(enum Color c)
{
    switch (c) {
        case RED:
            return 10;
        case GREEN:
            return 20;
        case BLUE:
            return 30;
        default: /* fallback */
            return -1;
    }
}

/* While-loop test */
int while_test(int n)
{
    int acc = 0;
    while (n > 0) {
        acc += n; // accumulate
        n--;
    }
    return acc;
}

/* Do-while-loop test */
int do_while_test(int n)
{
    int acc = 0;
    do {
        acc++; /* runs at least once */
        n--;
    } while (n > 0);
    return acc;
}

/* Pointer dereference test */
int pointer_test(void)
{
    int x = 5;
    int* p = &x; // address-of
    *p += 3;     /* indirect write */
    return x;
}

/* Entry point */
int main(void)
{
    int values[5];
    for (int i = 0; i < 5; i++)
        values[i] = i + 1; // initialize array

    struct Vec2 v1;
    struct Vec2 v2;

    v1.x = 1.0;
    v1.y = 2.0;
    v2.x = 3.0;
    v2.y = 4.0;

    struct Node nodes[3];
    build_list(nodes, 3); /* list construction */

    int result = 0;
    result += add(1, 2);
    result += factorial(5);
    result += (int)dot(v1, v2); // narrowing cast
    result += sum_array(values, 5);
    result += test_expressions();
    result += switch_test(GREEN);
    result += while_test(4);
    result += do_while_test(3);
    result += pointer_test();

    increment_global();
    result += global_counter; /* observe global state */

    return result; // final value
}
