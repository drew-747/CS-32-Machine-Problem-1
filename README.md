# Polynomial Operations Solver

## Overview
This repository contains a C implementation for performing arithmetic operations on multivariate polynomials using a linked list representation. It is designed to solve the **Machine Exercise 1 - Polynomial Operations** as a course requirement for CS 32 (24.2 WFX_MLNO) - University of the Philippines Diliman, Department of Computer Science. 

## Problem Statement
The program implements five of the following polynomial operations at once:

- **Addition (+)**
- **Subtraction (-)**
- **Multiplication (*)**
- **Long Division (/)** (without remainder)
- **Modulo Operation (%)** (remainder of polynomial division)

It follows a **canonical ordering** of variables `(x, y, z)` using **lexicographical ordering** for polynomial terms.

## Input Format
- Each operation begins with one of the symbols: `+`, `-`, `*`, `/`, `%`.
- Each polynomial is represented as:
  1. First line: Number of non-zero terms.
  2. Next lines: Each term as four values:
     - Three non-negative integers (exponents of x, y, z).
     - A nonzero real number (coefficient).
  3. Multiple terms with the same exponents must be combined.
- Input ends when `#` is encountered.

### Example Input:
```txt
+
2
2 0 0 1
0 2 0 -1
2
1 0 0 1
0 1 0 1
#
```

## Output Format
- Each result starts with `---`.
- Each term follows the format `exponent_x exponent_y exponent_z coefficient`.
- Coefficients are printed with **four decimal places**.
- Terms are sorted in **descending order**, prioritizing x > y > z.

### Example Output:
```txt
---
2 0 0 1.0000
1 0 0 1.0000
0 2 0 -1.0000
0 1 0 1.0000
```

## Assumptions
- Input is always valid.
- Coefficients fit within the `float` data type.

## References
- [MathPortal Polynomial Calculator](https://www.mathportal.org/calculators/polynomials-solvers/polynomials-operations-calculator.php)
- [Wolfram Long Division](https://library.wolfram.com/webMathematica/Education/LongDivide.jsp)

## Authors
- **Leonard Ang** (2022-01328) 
- **Justin Chuah** (2023-00514)
 
