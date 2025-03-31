#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define EPSILON 1e-9

typedef struct Term {
    int exp_x;
    int exp_y;
    int exp_z;
    float coeff;
    struct Term *next;
} Term;

typedef struct {
    Term *head;
} Polynomial;

Polynomial createPolynomial();
void destroyPolynomial(Polynomial *p);
int compareExponents(int x1, int y1, int z1, int x2, int y2, int z2);
void insertTerm(Polynomial *p, int ex, int ey, int ez, float c);
Polynomial readPolynomial();
void printPolynomial(Polynomial p);
Polynomial addPolynomial(Polynomial p1, Polynomial p2);
Polynomial subtractPolynomial(Polynomial p1, Polynomial p2);
// Polynomial multiplyPolynomial(Polynomial p1, Polynomial p2);
// Polynomial dividePolynomial(Polynomial A, Polynomial B);
// Polynomial moduloPolynomial(Polynomial A, Polynomial B);


Polynomial createPolynomial() {
    Polynomial p;
    p.head = NULL;
    return p;
}

void destroyPolynomial(Polynomial *p) {
    Term *current = p->head;
    Term *next_term;
    while (current != NULL) {
        next_term = current->next;
        free(current);
        current = next_term;
    }
    p->head = NULL;
}

int compareExponents(int x1, int y1, int z1, int x2, int y2, int z2) {
    if (x1 != x2) return x1 - x2;
    if (y1 != y2) return y1 - y2;
    return z1 - z2;
}

void insertTerm(Polynomial *p, int ex, int ey, int ez, float c) {
    if (fabs(c) < EPSILON) {
        return;
    }

    Term *newNode = (Term *)malloc(sizeof(Term));
    if (!newNode) {
        fprintf(stderr, "Error: Memory allocation failed in insertTerm\n");
        exit(EXIT_FAILURE);
    }
    newNode->exp_x = ex;
    newNode->exp_y = ey;
    newNode->exp_z = ez;
    newNode->coeff = c;
    newNode->next = NULL;

    Term *current = p->head;
    Term *previous = NULL;

    while (current != NULL) {
        int cmp = compareExponents(ex, ey, ez, current->exp_x, current->exp_y, current->exp_z);
        if (cmp > 0) {
            break;
        } else if (cmp == 0) {
            current->coeff += c;
            free(newNode);

            if (fabs(current->coeff) < EPSILON) {
                if (previous == NULL) {
                    p->head = current->next;
                } else {
                    previous->next = current->next;
                }
                free(current);
            }
            return;
        }
        previous = current;
        current = current->next;
    }

    if (previous == NULL) {
        newNode->next = p->head;
        p->head = newNode;
    } else {
        newNode->next = current;
        previous->next = newNode;
    }
}

Polynomial readPolynomial() {
    int n;
    Polynomial p = createPolynomial();

    if (scanf("%d", &n) != 1) {
         fprintf(stderr, "Error: Failed to read number of terms.\n");
         exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; ++i) {
        int ex, ey, ez;
        float c;
        if (scanf("%d %d %d %f", &ex, &ey, &ez, &c) != 4) {
             fprintf(stderr, "Error: Failed to read term %d.\n", i + 1);
             exit(EXIT_FAILURE);
        }
        insertTerm(&p, ex, ey, ez, c);
    }
    return p;
}

void printPolynomial(Polynomial p) {
    printf("---\n");
    Term *current = p.head;
    int printed_term = 0;

    while (current != NULL) {
        if (fabs(current->coeff) >= EPSILON) {
             printf("%d %d %d %.4f\n", current->exp_x, current->exp_y, current->exp_z, current->coeff);
             printed_term = 1;
        }
        current = current->next;
    }

    if (!printed_term) {
        printf("0 0 0 0.0000\n");
    }
}

Polynomial addPolynomial(Polynomial p1, Polynomial p2) {
    Polynomial result = createPolynomial();
    Term *tail = NULL;
    Term *ptr1 = p1.head;
    Term *ptr2 = p2.head;

    while (ptr1 != NULL || ptr2 != NULL) {
        int ex = 0, ey = 0, ez = 0;
        float newCoeff = 0.0;
        int cmp = 0;

        if (ptr1 != NULL && ptr2 != NULL) {
            cmp = compareExponents(ptr1->exp_x, ptr1->exp_y, ptr1->exp_z,
                                   ptr2->exp_x, ptr2->exp_y, ptr2->exp_z);
        } else if (ptr1 != NULL) {
            cmp = 1;
        } else {
            cmp = -1;
        }

         if (cmp > 0) {
            ex = ptr1->exp_x; ey = ptr1->exp_y; ez = ptr1->exp_z;
            newCoeff = ptr1->coeff;
            ptr1 = ptr1->next;
        } else if (cmp < 0) {
            ex = ptr2->exp_x; ey = ptr2->exp_y; ez = ptr2->exp_z;
            newCoeff = ptr2->coeff;
            ptr2 = ptr2->next;
        } else {
            ex = ptr1->exp_x; ey = ptr1->exp_y; ez = ptr1->exp_z;
            newCoeff = ptr1->coeff + ptr2->coeff;
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        }

        if (fabs(newCoeff) >= EPSILON) {
             Term *newNode = (Term *)malloc(sizeof(Term));
             if (!newNode) { fprintf(stderr, "Oops, malloc failed in addPolynomial.\n"); exit(EXIT_FAILURE); }
             newNode->exp_x = ex; newNode->exp_y = ey; newNode->exp_z = ez;
             newNode->coeff = newCoeff;
             newNode->next = NULL;

             if (tail == NULL) {
                 result.head = newNode;
                 tail = newNode;
             } else {
                 tail->next = newNode;
                 tail = newNode;
             }
        }
    }
    return result;
}

Polynomial subtractPolynomial(Polynomial p1, Polynomial p2) {
    Polynomial result = createPolynomial();
    Term *tail = NULL;
    Term *ptr1 = p1.head;
    Term *ptr2 = p2.head;

    while (ptr1 != NULL || ptr2 != NULL) {
        int ex = 0, ey = 0, ez = 0;
        float newCoeff = 0.0;
        int cmp = 0;

        if (ptr1 != NULL && ptr2 != NULL) {
            cmp = compareExponents(ptr1->exp_x, ptr1->exp_y, ptr1->exp_z,
                                   ptr2->exp_x, ptr2->exp_y, ptr2->exp_z);
        } else if (ptr1 != NULL) {
            cmp = 1;
        } else {
            cmp = -1;
        }

         if (cmp > 0) {
            ex = ptr1->exp_x; ey = ptr1->exp_y; ez = ptr1->exp_z;
            newCoeff = ptr1->coeff;
            ptr1 = ptr1->next;
        } else if (cmp < 0) {
            ex = ptr2->exp_x; ey = ptr2->exp_y; ez = ptr2->exp_z;
            newCoeff = -ptr2->coeff;
            ptr2 = ptr2->next;
        } else {
            ex = ptr1->exp_x; ey = ptr1->exp_y; ez = ptr1->exp_z;
            newCoeff = ptr1->coeff - ptr2->coeff;
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        }

        if (fabs(newCoeff) >= EPSILON) {
             Term *newNode = (Term *)malloc(sizeof(Term));
             if (!newNode) { fprintf(stderr, "Oops, malloc failed in subtractPolynomial.\n"); exit(EXIT_FAILURE); }
             newNode->exp_x = ex; newNode->exp_y = ey; newNode->exp_z = ez;
             newNode->coeff = newCoeff;
             newNode->next = NULL;

             if (tail == NULL) {
                 result.head = newNode;
                 tail = newNode;
             } else {
                 tail->next = newNode;
                 tail = newNode;
             }
        }
    }
    return result;
}

int main() {
    char op;

    while (scanf(" %c", &op) == 1 && op != '#') {

        Polynomial p1 = readPolynomial();
        Polynomial p2 = readPolynomial();
        Polynomial result;
        int processed = 0;

        switch (op) {
            case '+':
                result = addPolynomial(p1, p2);
                processed = 1;
                break;

            case '-':
                result = subtractPolynomial(p1, p2);
                processed = 1;
                break;

            case '*':
                printf("---\n");
                printf("Multiplication not implemented yet.\n");
                break;

            case '/':
                 printf("---\n");
                 printf("Division not implemented yet.\n");
                break;

            case '%':
                 printf("---\n");
                 printf("Modulo not implemented yet.\n");
                 break;

            default:
                fprintf(stderr, "Error: Invalid operation symbol '%c'.\n", op);
                break;
        }

        if (processed) {
            printPolynomial(result);
            destroyPolynomial(&result);
        }

        destroyPolynomial(&p1);
        destroyPolynomial(&p2);

    }

    return 0;
}
