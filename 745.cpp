#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
 
// Modulo constant
const ll MOD = 1000000007;
 
// Maximum needed for mu; note that sqrt(1e14)=1e7.
const int MAX = 10000000;
 
// Precompute the Möbius function μ(n) for n up to MAX.
void computeMobius(vector<int>& mu) {
    vector<bool> isPrime(MAX+1, true);
    vector<int> primes;
    mu.assign(MAX+1, 1);
    for (int i = 2; i <= MAX; i++) {
        if (isPrime[i]) {
            primes.push_back(i);
            mu[i] = -1;
        }
        for (int p : primes) {
            if ((ll)i * p > MAX) break;
            isPrime[i * p] = false;
            if (i % p == 0) {
                mu[i * p] = 0;
                break;
            } else {
                mu[i * p] = -mu[i];
            }
        }
    }
}
 
// Precompute prefix sums of mu: P(n)=sum_{i=1}^n mu(i).
void computePrefixMu(const vector<int>& mu, vector<ll>& prefixMu) {
    prefixMu.resize(MAX+1);
    prefixMu[0] = 0;
    for (int i = 1; i <= MAX; i++) {
        prefixMu[i] = prefixMu[i-1] + mu[i];
    }
}
 
// Fast modular sum of squares from L to R modulo MOD.
// Uses the formula: sum_{d=L}^{R} d^2 = (R(R+1)(2R+1) - (L-1)L(2L-1))/6.
ll sumSq(int L, int R) {
    auto modMul = [&](ll a, ll b) {
        return (a % MOD) * (b % MOD) % MOD;
    };
    ll b = R, a = L;
    ll term1 = modMul(b, b+1);
    term1 = modMul(term1, 2*b+1);
    ll a1 = a - 1;
    ll term2 = modMul(a1, a);
    term2 = modMul(term2, 2*a1+1);
    ll diff = (term1 - term2) % MOD;
    if(diff < 0) diff += MOD;
    // Multiply by modular inverse of 6 mod MOD.
    ll inv6 = 166666668; // because MOD is prime
    return modMul(diff, inv6);
}
 
// Global arrays for mu and its prefix sum.
vector<int> mu;
vector<ll> prefixMu;
 
// We now define a function to compute sf(x), the number of square–free numbers <= x,
// using the identity: sf(x)=sum_{n=1}^{floor(sqrt(x))} mu(n) * floor(x/n^2).
// We use grouping to speed up the summation and cache results in memo.
unordered_map<ll,ll> memo;
 
ll sf(ll x) {
    if(x < 0) return 0;
    int r = (int)floor(sqrt((long double)x));
    // Use memoization to avoid recomputation.
    if(memo.count(x))
        return memo[x];
    ll res = 0;
    int n = 1;
    while(n <= r) {
        // For fixed n, let v = floor(x/(n^2)).
        ll v = x / ((ll)n * n);
        // Find the maximum n (nmax) such that floor(x/(n^2)) stays constant.
        int nmax = (int)min((ll)r, (ll)floor(sqrt((long double)x / v)));
        // Sum mu(n) for n in [n, nmax] quickly using our prefix sums.
        ll sumMu = prefixMu[nmax] - prefixMu[n-1];
        res += v * sumMu;
        n = nmax + 1;
    }
    memo[x] = res;
    return res;
}
 
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    // Precompute mu and prefix sums.
    computeMobius(mu);
    computePrefixMu(mu, prefixMu);
    
    // Our target N.
    ll N = 100000000000000LL; // 10^14
    int dmax = (int)floor(sqrt((long double)N));
    
    ll ans = 0;
    int d = 1;
    // Loop over d in blocks such that t = floor(N/d^2) remains constant.
    while(d <= dmax) {
        ll t = N / ((ll)d * d);
        // Determine the largest d (ddmax) for which floor(N/d^2) equals t.
        int ddmax = (int)floor(sqrt((long double)N / t));
        if(ddmax > dmax) ddmax = dmax;
        // Sum d^2 over the block [d, ddmax].
        ll blockSum = sumSq(d, ddmax);
        // Multiply by the number of square–free numbers up to t.
        ll sfVal = sf(t);
        ans = (ans + (blockSum % MOD) * (sfVal % MOD)) % MOD;
        d = ddmax + 1;
    }
    
    cout << ans % MOD << "\n";
    return 0;
}
