# Project Euler Problem 745: Sum of Largest Square Divisors

This repository contains a C++ solution to [Project Euler Problem 745](https://projecteuler.net/problem=745).

## Problem Description

Let g(n) be the largest perfect square dividing n. For example, g(18) = 9, g(19) = 1, g(20) = 4.
Define S(N) = ∑_{n=1}^{N} g(n).
For example, S(10) = g(1) + g(2) + ... + g(10) = 1 + 1 + 1 + 4 + 1 + 1 + 1 + 4 + 9 + 1 = 24.

The problem asks for S(10<sup>14</sup>) modulo 10<sup>9</sup> + 7.

## Solution Approach

### Mathematical Formulation

The core idea is to rearrange the summation S(N) = ∑_{n=1}^{N} g(n) based on the possible values of g(n).
Let g(n) = d<sup>2</sup> for some integer d ≥ 1. This means n must be of the form n = d<sup>2</sup> * k, where k is a **square-free** integer (an integer not divisible by any perfect square other than 1).

Why must k be square-free? If k had a square factor s<sup>2</sup> > 1, so k = s<sup>2</sup> * m, then n = d<sup>2</sup> * s<sup>2</sup> * m = (d*s)<sup>2</sup> * m. The largest square divisor would then be at least (d*s)<sup>2</sup>, which is greater than d<sup>2</sup>, contradicting that g(n) = d<sup>2</sup>.

So, for a fixed square d<sup>2</sup>, we need to count how many integers n ≤ N have g(n) = d<sup>2</sup>. These are the integers n = d<sup>2</sup> * k ≤ N where k is square-free.
This condition is equivalent to k ≤ N / d<sup>2</sup>, where k is square-free.

Let `sf(x)` denote the count of square-free integers less than or equal to x.
Then, the number of n ≤ N such that g(n) = d<sup>2</sup> is precisely `sf(⌊N / d^2⌋)`.

We can rewrite the sum S(N) by grouping terms based on the value of d<sup>2</sup>:
S(N) = ∑_{d=1}^{⌊√N⌋} d<sup>2</sup> * (number of n ≤ N such that g(n) = d<sup>2</sup>)
**S(N) = ∑_{d=1}^{⌊√N⌋} d<sup>2</sup> * sf(⌊N / d<sup>2</sup>⌋)**

### Calculating sf(x) - The Count of Square-Free Numbers

To calculate S(N), we need an efficient way to compute `sf(x)`. A standard identity relates `sf(x)` to the Möbius function μ(n):
**sf(x) = ∑_{k=1}^{⌊√x⌋} μ(k) * ⌊x / k<sup>2</sup>⌋**

The Möbius function μ(n) is defined as:
*   μ(n) = 1 if n = 1.
*   μ(n) = (-1)<sup>r</sup> if n is the product of r distinct prime numbers.
*   μ(n) = 0 if n has a squared prime factor.

### Optimizations

1.  **Precomputation of Möbius Function:** We need μ(k) for k up to ⌊√N⌋ = 10<sup>7</sup>. We can precompute these values efficiently using a sieve method similar to the Sieve of Eratosthenes (`computeMobius` function).
2.  **Prefix Sums of Möbius Function:** To speed up the calculation of `sf(x)`, we precompute the prefix sums of μ(k), P(m) = ∑_{k=1}^{m} μ(k) (`computePrefixMu` function). Then, the sum ∑_{k=a}^{b} μ(k) can be calculated quickly as P(b) - P(a-1).
3.  **Block Summation / Grouping for sf(x):** When calculating `sf(x) = ∑_{k=1}^{⌊√x⌋} μ(k) * ⌊x / k^2⌋`, the value of `v = ⌊x / k^2⌋` remains constant over ranges of `k`. We group terms with the same value of `v`. For a fixed `v`, we find the largest `k_max` such that ⌊x / k_max<sup>2</sup>⌋ = v. The sum over this block [k, k_max] becomes `v * (P(k_max) - P(k-1))`. This optimization is implemented within the `sf(x)` function.
4.  **Memoization for sf(x):** The value `t = ⌊N / d^2⌋` can repeat for different values of `d`. We use memoization (caching) with an `unordered_map<ll, ll> memo` to store the results of `sf(t)` and avoid recomputing them.
5.  **Block Summation / Grouping for S(N):** Similarly, in the main sum S(N) = ∑_{d=1}^{⌊√N⌋} d<sup>2</sup> * sf(⌊N / d<sup>2</sup>⌋), the value `t = ⌊N / d^2⌋` remains constant over ranges of `d`. We group terms where `t` is constant. For a fixed `t`, find the range of `d` values [d, d_max] such that ⌊N / d<sup>2</sup>⌋ = t. The contribution of this block to the sum is `sf(t) * (∑_{i=d}^{d_max} i^2)`. This is the core logic within the `main` function's `while` loop.
6.  **Efficient Sum of Squares:** We need to compute ∑_{i=d}^{d_max} i<sup>2</sup> modulo M efficiently. The `sumSq(L, R)` function calculates this using the formula ∑_{i=1}^{R} i<sup>2</sup> = R(R+1)(2R+1)/6. The sum over a range [L, R] is then (∑_{i=1}^{R} i<sup>2</sup>) - (∑_{i=1}^{L-1} i<sup>2</sup>). Since we are working modulo M = 10<sup>9</sup> + 7 (a prime number), division by 6 is replaced by multiplication by the modular multiplicative inverse of 6 modulo M, which is precalculated as `inv6 = 166666668`.

## Code Explanation

1.  **Includes and Typedefs:** Includes standard C++ libraries (`bits/stdc++.h`) and defines `ll` as `long long`.
2.  **Constants:**
    *   `MOD = 1000000007`: The modulus for the final answer.
    *   `MAX = 10000000`: The upper limit for precomputation (⌊√N⌋ = ⌊√10<sup>14</sup>⌋ = 10<sup>7</sup>).
3.  **`computeMobius(mu)`:**
    *   Calculates the Möbius function μ(n) for n from 1 to `MAX` using a sieve. Populates the `mu` vector.
4.  **`computePrefixMu(mu, prefixMu)`:**
    *   Calculates the prefix sums of the `mu` array and stores them in `prefixMu`. `prefixMu[i] = mu[1] + ... + mu[i]`.
5.  **`sumSq(L, R)`:**
    *   Calculates (∑_{d=L}^{R} d<sup>2</sup>) mod `MOD` efficiently using the sum of squares formula and modular arithmetic, including modular inverse for division by 6.
6.  **`mu`, `prefixMu`, `memo`:** Global vectors/map to store precomputed Möbius values, their prefix sums, and memoized `sf(x)` results.
7.  **`sf(x)`:**
    *   Calculates the count of square-free integers ≤ x, `sf(x)`.
    *   Uses memoization (`memo`) to store and retrieve results for previously computed `x`.
    *   Implements the formula `sf(x) = ∑_{n=1}^{⌊√x⌋} μ(n) * ⌊x / n^2⌋` using the block summation optimization based on constant values of `⌊x / n^2⌋` and precomputed prefix sums (`prefixMu`).
8.  **`main()`:**
    *   Sets up fast I/O (`ios::sync_with_stdio(false); cin.tie(nullptr);`).
    *   Calls `computeMobius` and `computePrefixMu` for precomputation.
    *   Sets `N = 100000000000000LL` (10<sup>14</sup>).
    *   Calculates `dmax = ⌊√N⌋`.
    *   Initializes the final answer `ans = 0`.
    *   Implements the main loop S(N) = ∑_{d=1}^{dmax} d<sup>2</sup> * sf(⌊N / d<sup>2</sup>⌋) using the block summation optimization:
        *   The `while(d <= dmax)` loop iterates through blocks of `d`.
        *   Calculates the constant value `t = ⌊N / (d*d)⌋` for the current block.
        *   Finds the end of the block `ddmax` (the largest `d` with the same `t`).
        *   Calculates the sum of squares for the block `d` to `ddmax`: `blockSum = sumSq(d, ddmax)`.
        *   Gets the square-free count: `sfVal = sf(t)` (using memoization).
        *   Updates the total answer modulo MOD: `ans = (ans + (blockSum % MOD) * (sfVal % MOD)) % MOD`.
        *   Moves `d` to the start of the next block (`ddmax + 1`).
    *   Prints the final computed `ans` modulo `MOD`.

## How to Compile and Run

1.  **Save:** Save the code as `problem745.cpp`.
2.  **Compile:** Use a C++11 (or later) compliant compiler (like g++):
    ```bash
    g++ -std=c++11 -O2 -o problem745 problem745.cpp
    ```
    *(Using `-O2` or `-O3` optimization is recommended for performance)*
3.  **Run:** Execute the compiled program:
    ```bash
    ./problem745
    ```

## Result

The program calculates S(10<sup>14</sup>) mod (10<sup>9</sup> + 7). The precomputation and main loop take some time due to the large numbers involved.

The final output of the program is:
**`94586478`**
