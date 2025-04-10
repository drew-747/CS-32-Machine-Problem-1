#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define EPS 1e-6f

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

typedef struct {
    Polynomial quotient;
    Polynomial remainder;
} DivisionResult;

Polynomial createPolynomial();
void destroyPolynomial(Polynomial *p);
int compareExponents(int x1, int y1, int z1, int x2, int y2, int z2);
void insertTerm(Polynomial *p, int ex, int ey, int ez, float c);
Polynomial readPolynomial();
void printPolynomial(Polynomial p);
Polynomial addPolynomial(Polynomial p1, Polynomial p2);
Polynomial subtractPolynomial(Polynomial p1, Polynomial p2);
Polynomial multiplyPolynomial(Polynomial p1, Polynomial p2);
Polynomial dividePolynomial(Polynomial p1, Polynomial p2);
Polynomial moduloPolynomial(Polynomial p1, Polynomial p2);
Polynomial copyPolynomial(Polynomial p);
Term *getLeadingTerm(Polynomial p);
int isZeroPolynomial(Polynomial p);
Polynomial multiplyTermByPolynomial(Term *t, Polynomial p);
DivisionResult polyLongDivision(Polynomial A, Polynomial B);

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
    if (x1 != x2)
        return x1 - x2;
    if (y1 != y2)
        return y1 - y2;
    return z1 - z2;
}

void insertTerm(Polynomial *p, int ex, int ey, int ez, float c) {
    if (fabs(c) < EPS) {
        return;
    }
    Term **current_ptr = &(p->head);
    while (*current_ptr != NULL) {
        Term *current_node = *current_ptr;
        int cmp = compareExponents(ex, ey, ez, current_node->exp_x, current_node->exp_y, current_node->exp_z);
        if (cmp > 0) {
            break;
        } else if (cmp == 0) {
            current_node->coeff += c;
            if (fabs(current_node->coeff) < EPS) {
                *current_ptr = current_node->next;
                free(current_node);
            }
            return;
        }
        current_ptr = &(current_node->next);
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
    newNode->next = *current_ptr;
    *current_ptr = newNode;
}

Polynomial readPolynomial() {
    int n;
    Polynomial p = createPolynomial();
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Error: Failed to read number of terms.\n");
        destroyPolynomial(&p);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; ++i) {
        int ex, ey, ez;
        float c;
        if (scanf("%d %d %d %f", &ex, &ey, &ez, &c) != 4) {
            fprintf(stderr, "Error: Failed to read term %d.\n", i + 1);
            destroyPolynomial(&p);
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
        if (fabs(current->coeff) >= EPS) {
            // Print coefficient with three decimal places
            printf("%d %d %d %.3f\n", current->exp_x, current->exp_y, current->exp_z, current->coeff);
            printed_term = 1;
        }
        current = current->next;
    }
    if (!printed_term) {
        printf("0 0 0 0.000\n");
    }
}

Polynomial addPolynomial(Polynomial p1, Polynomial p2) {
    Polynomial result = createPolynomial();
    Term *tail = NULL;
    Term *ptr1 = p1.head;
    Term *ptr2 = p2.head;
    Term dummyHead = {0, 0, 0, 0.0f, NULL};
    tail = &dummyHead;
    while (ptr1 != NULL || ptr2 != NULL) {
        int ex = 0, ey = 0, ez = 0;
        float newCoeff = 0.0f;
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
            ex = ptr1->exp_x;
            ey = ptr1->exp_y;
            ez = ptr1->exp_z;
            newCoeff = ptr1->coeff;
            ptr1 = ptr1->next;
        } else if (cmp < 0) {
            ex = ptr2->exp_x;
            ey = ptr2->exp_y;
            ez = ptr2->exp_z;
            newCoeff = ptr2->coeff;
            ptr2 = ptr2->next;
        } else {
            ex = ptr1->exp_x;
            ey = ptr1->exp_y;
            ez = ptr1->exp_z;
            newCoeff = ptr1->coeff + ptr2->coeff;
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        }
        if (fabs(newCoeff) >= EPS) {
            Term *newNode = (Term *)malloc(sizeof(Term));
            if (!newNode) {
                fprintf(stderr, "Error: malloc failed in addPolynomial.\n");
                result.head = dummyHead.next;
                destroyPolynomial(&result);
                exit(EXIT_FAILURE);
            }
            newNode->exp_x = ex;
            newNode->exp_y = ey;
            newNode->exp_z = ez;
            newNode->coeff = newCoeff;
            newNode->next = NULL;
            tail->next = newNode;
            tail = newNode;
        }
    }
    result.head = dummyHead.next;
    return result;
}

Polynomial subtractPolynomial(Polynomial p1, Polynomial p2) {
    Polynomial result = createPolynomial();
    Term *tail = NULL;
    Term *ptr1 = p1.head;
    Term *ptr2 = p2.head;
    Term dummyHead = {0, 0, 0, 0.0f, NULL};
    tail = &dummyHead;
    while (ptr1 != NULL || ptr2 != NULL) {
        int ex = 0, ey = 0, ez = 0;
        float newCoeff = 0.0f;
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
            ex = ptr1->exp_x;
            ey = ptr1->exp_y;
            ez = ptr1->exp_z;
            newCoeff = ptr1->coeff;
            ptr1 = ptr1->next;
        } else if (cmp < 0) {
            ex = ptr2->exp_x;
            ey = ptr2->exp_y;
            ez = ptr2->exp_z;
            newCoeff = -ptr2->coeff;
            ptr2 = ptr2->next;
        } else {
            ex = ptr1->exp_x;
            ey = ptr1->exp_y;
            ez = ptr1->exp_z;
            newCoeff = ptr1->coeff - ptr2->coeff;
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        }
        if (fabs(newCoeff) >= EPS) {
            Term *newNode = (Term *)malloc(sizeof(Term));
            if (!newNode) {
                fprintf(stderr, "Error: malloc failed in subtractPolynomial.\n");
                result.head = dummyHead.next;
                destroyPolynomial(&result);
                exit(EXIT_FAILURE);
            }
            newNode->exp_x = ex;
            newNode->exp_y = ey;
            newNode->exp_z = ez;
            newNode->coeff = newCoeff;
            newNode->next = NULL;
            tail->next = newNode;
            tail = newNode;
        }
    }
    result.head = dummyHead.next;
    return result;
}

Polynomial multiplyPolynomial(Polynomial p1, Polynomial p2) {
    Polynomial result = createPolynomial();
    for (Term *ptr1 = p1.head; ptr1 != NULL; ptr1 = ptr1->next) {
        for (Term *ptr2 = p2.head; ptr2 != NULL; ptr2 = ptr2->next) {
            float newCoeff = ptr1->coeff * ptr2->coeff;
            int new_ex = ptr1->exp_x + ptr2->exp_x;
            int new_ey = ptr1->exp_y + ptr2->exp_y;
            int new_ez = ptr1->exp_z + ptr2->exp_z;
            insertTerm(&result, new_ex, new_ey, new_ez, newCoeff);
        }
    }
    return result;
}

Polynomial copyPolynomial(Polynomial p) {
    Polynomial copy = createPolynomial();
    Term *tail = NULL;
    Term dummyHead = {0, 0, 0, 0.0f, NULL};
    tail = &dummyHead;
    for (Term *current = p.head; current != NULL; current = current->next) {
        Term *newNode = (Term *)malloc(sizeof(Term));
        if (!newNode) {
            fprintf(stderr, "Error: Memory allocation failed in copyPolynomial\n");
            copy.head = dummyHead.next;
            destroyPolynomial(&copy);
            exit(EXIT_FAILURE);
        }
        newNode->exp_x = current->exp_x;
        newNode->exp_y = current->exp_y;
        newNode->exp_z = current->exp_z;
        newNode->coeff = current->coeff;
        newNode->next = NULL;
        tail->next = newNode;
        tail = newNode;
    }
    copy.head = dummyHead.next;
    return copy;
}

Term *getLeadingTerm(Polynomial p) {
    return p.head;
}

int isZeroPolynomial(Polynomial p) {
    return p.head == NULL;
}

Polynomial multiplyTermByPolynomial(Term *t, Polynomial p) {
    Polynomial result = createPolynomial();
    Term *tail = NULL;
    Term dummyHead = {0, 0, 0, 0.0f, NULL};
    tail = &dummyHead;
    if (fabs(t->coeff) < EPS)
        return result;
    for (Term *ptr = p.head; ptr != NULL; ptr = ptr->next) {
        float newCoeff = t->coeff * ptr->coeff;
        if (fabs(newCoeff) >= EPS) {
            int new_ex = t->exp_x + ptr->exp_x;
            int new_ey = t->exp_y + ptr->exp_y;
            int new_ez = t->exp_z + ptr->exp_z;
            Term *newNode = (Term *)malloc(sizeof(Term));
            if (!newNode) {
                fprintf(stderr, "Error: Memory allocation failed in multiplyTermByPolynomial\n");
                result.head = dummyHead.next;
                destroyPolynomial(&result);
                exit(EXIT_FAILURE);
            }
            newNode->exp_x = new_ex;
            newNode->exp_y = new_ey;
            newNode->exp_z = new_ez;
            newNode->coeff = newCoeff;
            newNode->next = NULL;
            tail->next = newNode;
            tail = newNode;
        }
    }
    result.head = dummyHead.next;
    return result;
}

DivisionResult polyLongDivision(Polynomial A, Polynomial B) {
    DivisionResult res;
    res.quotient = createPolynomial();
    res.remainder = copyPolynomial(A);
    Term *lt_B = getLeadingTerm(B);
    if (isZeroPolynomial(B) || lt_B == NULL || fabs(lt_B->coeff) < EPS) {
        destroyPolynomial(&res.quotient);
        res.quotient = createPolynomial();
        return res;
    }
    while (!isZeroPolynomial(res.remainder)) {
        Term *lt_R = getLeadingTerm(res.remainder);
        if (lt_R == NULL)
            break;
        bool divisible = (lt_R->exp_x >= lt_B->exp_x) &&
                         (lt_R->exp_y >= lt_B->exp_y) &&
                         (lt_R->exp_z >= lt_B->exp_z);
        if (divisible) {
            float T_coeff = lt_R->coeff / lt_B->coeff;
            if (fabs(T_coeff) < EPS)
                break;
            int T_ex = lt_R->exp_x - lt_B->exp_x;
            int T_ey = lt_R->exp_y - lt_B->exp_y;
            int T_ez = lt_R->exp_z - lt_B->exp_z;
            insertTerm(&res.quotient, T_ex, T_ey, T_ez, T_coeff);
            Term T_term_struct = {T_ex, T_ey, T_ez, T_coeff, NULL};
            Polynomial T_times_B = multiplyTermByPolynomial(&T_term_struct, B);
            Polynomial old_R = res.remainder;
            res.remainder = subtractPolynomial(old_R, T_times_B);
            destroyPolynomial(&old_R);
            destroyPolynomial(&T_times_B);
        } else {
            break;
        }
    }
    return res;
}

Polynomial dividePolynomial(Polynomial p1, Polynomial p2) {
    DivisionResult dr = polyLongDivision(p1, p2);
    destroyPolynomial(&dr.remainder);
    return dr.quotient;
}

Polynomial moduloPolynomial(Polynomial p1, Polynomial p2) {
    DivisionResult dr = polyLongDivision(p1, p2);
    destroyPolynomial(&dr.quotient);
    return dr.remainder;
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
                result = multiplyPolynomial(p1, p2);
                processed = 1;
                break;
            case '/':
                result = dividePolynomial(p1, p2);
                processed = 1;
                break;
            case '%':
                result = moduloPolynomial(p1, p2);
                processed = 1;
                break;
            default:
                processed = 0;
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
