#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define EPSILON 1e-9

typedef struct Term
{
    int exp_x;         // Power of x
    int exp_y;         // Power of y
    int exp_z;         // Power of z
    float coeff;       // The number multiplying the variables
    struct Term *next; // Pointer to the next term in our list
} Term;

// And this holds the whole polynomial, basically just pointing
// to the first term (the 'head') of the linked list.
typedef struct
{
    Term *head; // Points to the highest-order term
} Polynomial;

// --- Function Prototypes (Just listing the functions we'll define) ---
Polynomial createPolynomial();
void destroyPolynomial(Polynomial *p);
int compareExponents(int x1, int y1, int z1, int x2, int y2, int z2);
void insertTerm(Polynomial *p, int ex, int ey, int ez, float c);
Polynomial readPolynomial();
void printPolynomial(Polynomial p);
Polynomial addPolynomial(Polynomial p1, Polynomial p2);

// --- These are placeholders for now ---
// Polynomial subtractPolynomial(Polynomial p1, Polynomial p2);
// Polynomial multiplyPolynomial(Polynomial p1, Polynomial p2);
// Polynomial dividePolynomial(Polynomial A, Polynomial B);
// Polynomial moduloPolynomial(Polynomial A, Polynomial B);

// --- Function Implementations ---

/**
 * @brief Makes a new, empty polynomial ready to use.
 * @return An empty Polynomial struct.
 */
Polynomial createPolynomial()
{
    Polynomial p;
    p.head = NULL; // NULL means the list is empty, no terms yet.
    return p;
}

/**
 * @brief Cleans up the memory used by a polynomial when we're done with it.
 *        Important to avoid memory leaks!
 * @param p Pointer to the Polynomial struct to clean up.
 */
void destroyPolynomial(Polynomial *p)
{
    Term *current = p->head;
    Term *next_term;
    while (current != NULL)
    {
        next_term = current->next; // Keep track of the next one...
        free(current);             // ...before freeing the current one!
        current = next_term;       // Move to the next one
    }
    p->head = NULL; // Make sure the head points to nothing now.
}

/**
 * @brief Compares two sets of exponents (x,y,z) to see which term is "bigger".
 *        We decided x is most important, then y, then z (lexicographical order).
 * @return Positive if term 1 > term 2, Negative if term 1 < term 2, Zero if they're the same.
 */
int compareExponents(int x1, int y1, int z1, int x2, int y2, int z2)
{
    if (x1 != x2)
        return x1 - x2; // Check x first
    if (y1 != y2)
        return y1 - y2; // Then check y
    return z1 - z2;     // Finally, check z
}

/**
 * @brief Adds a term to the polynomial list, keeping it sorted.
 *        If a term with the same powers already exists, it just adds the coefficients.
 *        If adding makes the coefficient zero (or super close), it removes the term.
 * @param p Pointer to the Polynomial to insert into.
 * @param ex Exponent of x.
 * @param ey Exponent of y.
 * @param ez Exponent of z.
 * @param c Coefficient.
 */
void insertTerm(Polynomial *p, int ex, int ey, int ez, float c)
{
    // If the coefficient is basically zero, don't bother adding it.
    if (fabs(c) < EPSILON)
    {
        return;
    }

    // Make space for the new term node
    Term *newNode = (Term *)malloc(sizeof(Term));
    if (!newNode)
    {
        fprintf(stderr, "Oh no! Memory allocation failed in insertTerm\n");
        exit(EXIT_FAILURE); // Can't continue if we can't get memory
    }
    newNode->exp_x = ex;
    newNode->exp_y = ey;
    newNode->exp_z = ez;
    newNode->coeff = c;
    newNode->next = NULL; // It's not linked to anything yet

    Term *current = p->head;
    Term *previous = NULL; // Need to keep track of the one before current

    // Let's walk through the list to find where this new term belongs
    while (current != NULL)
    {
        int cmp = compareExponents(ex, ey, ez, current->exp_x, current->exp_y, current->exp_z);
        if (cmp > 0)
        {
            // Found the spot! The new term comes *before* the current one.
            break;
        }
        else if (cmp == 0)
        {
            // Hey, these exponents match! Let's just add the coefficients.
            current->coeff += c;
            free(newNode); // Don't need the separate newNode anymore

            // Check if they cancelled each other out (coeff is now zero)
            if (fabs(current->coeff) < EPSILON)
            {
                // Yep, need to remove this term from the list.
                if (previous == NULL)
                { // If it was the first term
                    p->head = current->next;
                }
                else
                { // If it was somewhere in the middle/end
                    previous->next = current->next;
                }
                free(current); // Free the node we're removing
            }
            return; // Job done, whether we updated or removed.
        }
        // New term comes *after* the current one, keep looking.
        previous = current;
        current = current->next;
    }

    // If we got here, the newNode needs to be inserted.
    if (previous == NULL)
    { // Means it goes right at the beginning
        newNode->next = p->head;
        p->head = newNode;
    }
    else
    {                             // Means it goes after 'previous'
        newNode->next = current;  // Link it to whatever 'previous' was pointing to
        previous->next = newNode; // Make 'previous' point to the new node
    }
}

/**
 * @brief Reads a polynomial from the input (like the console).
 *        It expects the number of terms first, then each term's details.
 * @return The Polynomial struct built from the input.
 */
Polynomial readPolynomial()
{
    int n;                             // How many terms are coming?
    Polynomial p = createPolynomial(); // Start with an empty slate

    // Read the number of terms. Let's assume the input is nice and correct.
    if (scanf("%d", &n) != 1)
    {
        fprintf(stderr, "Trouble reading the number of terms.\n");
        exit(EXIT_FAILURE);
    }

    // Loop to read each term
    for (int i = 0; i < n; ++i)
    {
        int ex, ey, ez;
        float c;
        // Read the four values for this term. Again, hoping the input is valid.
        if (scanf("%d %d %d %f", &ex, &ey, &ez, &c) != 4)
        {
            fprintf(stderr, "Uh oh, couldn't read term %d properly.\n", i + 1);
            exit(EXIT_FAILURE);
        }
        // Use our handy insertTerm function. It handles sorting, merging,
        // and getting rid of zero terms all in one go.
        insertTerm(&p, ex, ey, ez, c);
    }
    return p; // Here's the polynomial we built!
}

/**
 * @brief Prints the polynomial out to the console in the specified format.
 * @param p The Polynomial to print.
 */
void printPolynomial(Polynomial p)
{
    printf("---\n"); // The required separator line
    Term *current = p.head;
    int printed_term = 0; // Keep track if we actually print anything

    // Walk through the list and print each term
    while (current != NULL)
    {
        // Only print if the coefficient isn't effectively zero
        if (fabs(current->coeff) >= EPSILON)
        {
            // Format: ex ey ez coeff (with 4 decimal places for coeff)
            printf("%d %d %d %.4f\n", current->exp_x, current->exp_y, current->exp_z, current->coeff);
            printed_term = 1; // Mark that we printed something
        }
        current = current->next; // On to the next term
    }

    // If the polynomial was empty or all terms cancelled out...
    if (!printed_term)
    {
        // ...print the special format for a zero polynomial.
        printf("0 0 0 0.0000\n");
    }
}

/**
 * @brief Adds two polynomials (p1 + p2) together.
 *        It goes through both lists sort of like merging sorted lists.
 * @param p1 The first polynomial.
 * @param p2 The second polynomial.
 * @return A *new* Polynomial struct holding the sum.
 */
Polynomial addPolynomial(Polynomial p1, Polynomial p2)
{
    Polynomial result = createPolynomial(); // Where we'll build the sum
    Term *tail = NULL;                      // Helps add new terms to the end efficiently
    Term *ptr1 = p1.head;                   // To walk through polynomial 1
    Term *ptr2 = p2.head;                   // To walk through polynomial 2

    // Keep going as long as there are terms left in *either* polynomial
    while (ptr1 != NULL || ptr2 != NULL)
    {
        int ex = 0, ey = 0, ez = 0; // For the exponents of the term we're adding to result
        float newCoeff = 0.0;       // For its coefficient
        int cmp = 0;                // To store the comparison result

        // Figure out which term comes next (highest order)
        if (ptr1 != NULL && ptr2 != NULL)
        {
            // Both polynomials still have terms, compare them
            cmp = compareExponents(ptr1->exp_x, ptr1->exp_y, ptr1->exp_z,
                                   ptr2->exp_x, ptr2->exp_y, ptr2->exp_z);
        }
        else if (ptr1 != NULL)
        {
            cmp = 1; // Only terms left in p1
        }
        else
        { // Only terms left in p2 (ptr2 must be non-NULL here)
            cmp = -1;
        }

        // Now, act based on the comparison
        if (cmp > 0)
        { // Term from p1 is "bigger" or p2 is finished
            ex = ptr1->exp_x;
            ey = ptr1->exp_y;
            ez = ptr1->exp_z;
            newCoeff = ptr1->coeff;
            ptr1 = ptr1->next; // Move to the next term in p1
        }
        else if (cmp < 0)
        { // Term from p2 is "bigger" or p1 is finished
            ex = ptr2->exp_x;
            ey = ptr2->exp_y;
            ez = ptr2->exp_z;
            newCoeff = ptr2->coeff;
            ptr2 = ptr2->next; // Move to the next term in p2
        }
        else
        { // Exponents match! Add the coefficients.
            ex = ptr1->exp_x;
            ey = ptr1->exp_y;
            ez = ptr1->exp_z; // Can use either one's exponents
            newCoeff = ptr1->coeff + ptr2->coeff;
            ptr1 = ptr1->next; // Move past this term in *both* lists
            ptr2 = ptr2->next;
        }

        // Only add this combined/copied term to the result if it's not zero
        if (fabs(newCoeff) >= EPSILON)
        {
            // Need memory for the new term node in the result list
            Term *newNode = (Term *)malloc(sizeof(Term));
            if (!newNode)
            {
                fprintf(stderr, "Oops, malloc failed in addPolynomial.\n");
                exit(EXIT_FAILURE);
            }
            newNode->exp_x = ex;
            newNode->exp_y = ey;
            newNode->exp_z = ez;
            newNode->coeff = newCoeff;
            newNode->next = NULL;

            // Add it to the end of the result list (using 'tail' makes this fast)
            if (tail == NULL)
            { // If this is the very first term for the result
                result.head = newNode;
                tail = newNode;
            }
            else
            { // Otherwise, tack it on after the current tail
                tail->next = newNode;
                tail = newNode; // And update the tail pointer
            }
        }
    }
    return result; // Return the calculated sum
}

// --- Main Program Logic ---
int main()
{
    char op; // To read the operation symbol (+, -, *, etc.)

    // Keep reading operations until we hit the '#' symbol
    while (scanf(" %c", &op) == 1 && op != '#')
    {

        // For each operation, read the two polynomials it works on.
        // printf("DEBUG: Reading polynomial 1 for op '%c'\n", op); // Temporary debug help
        Polynomial p1 = readPolynomial();
        // printf("DEBUG: Reading polynomial 2 for op '%c'\n", op); // Temporary debug help
        Polynomial p2 = readPolynomial();
        Polynomial result; // To store the outcome
        int processed = 0; // Flag to know if we actually did an operation

        // Figure out what to do based on the operation symbol
        switch (op)
        {
        case '+':
            // printf("DEBUG: Performing addition...\n"); // Temporary debug help
            result = addPolynomial(p1, p2);
            processed = 1; // Mark that we did something
            break;

        case '-':
            // TODO: Implement subtraction!
            printf("---\n"); // Print the separator as required
            printf("Subtraction not implemented yet.\n");
            // result = subtractPolynomial(p1, p2); // This would be the call later
            // processed = 1;
            break; // Need to break so we don't fall through

        case '*':
            // TODO: Implement multiplication!
            printf("---\n");
            printf("Multiplication not implemented yet.\n");
            // result = multiplyPolynomial(p1, p2); // This would be the call later
            // processed = 1;
            break;

        case '/':
            // TODO: Implement division!
            printf("---\n");
            printf("Division not implemented yet.\n");
            // result = dividePolynomial(p1, p2); // This would be the call later
            // processed = 1;
            break;

        case '%':
            // TODO: Implement modulo!
            printf("---\n");
            printf("Modulo not implemented yet.\n");
            // result = moduloPolynomial(p1, p2); // This would be the call later
            // processed = 1;
            break;

        default:
            // Should not happen based on problem description, but good to handle
            fprintf(stderr, "Hey, that's an unknown operation: %c\n", op);
            // No result to print, but we still need to clean up p1 and p2 below.
            break;
        }

        // If we actually calculated a result (only for '+' right now)...
        if (processed)
        {
            // printf("DEBUG: Printing result...\n"); // Temporary debug help
            printPolynomial(result);    // ...print it out.
            destroyPolynomial(&result); // And clean up the memory used by the result.
        }
        else
        {
            // If an operation wasn't implemented, we might need to print something
            // or maybe just do nothing depending on requirements.
            // The placeholder messages are printed inside the switch cases for now.
        }

        // IMPORTANT: Clean up the memory used by the two input polynomials
        //            for this operation cycle before reading the next one.
        // printf("DEBUG: Cleaning up p1 and p2...\n"); // Temporary debug help
        destroyPolynomial(&p1);
        destroyPolynomial(&p2);
        // printf("DEBUG: Ready for next operation or #\n"); // Temporary debug help

    } // Loop back to read the next operation symbol or '#'

    return 0;
}