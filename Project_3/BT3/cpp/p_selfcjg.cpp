#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Sinh phân hoạch tự liên hợp của n có k phần (self-conjugate partition)
void generate_self_conjugate(int n, int k, int min_val, vector<int>& current, vector<vector<int>>& result) {
    if (n == 0 && current.size() == k) {
        result.push_back(current);
        return;
    }
    if (n <= 0 || current.size() >= k) return;
    for (int i = min_val; i * (2 * (current.size()) + 1) <= n; ++i) {
        current.push_back(i);
        generate_self_conjugate(n - i * (2 * (current.size()) - 1), k, i, current, result);
        current.pop_back();
    }
}

int count_odd_partitions_helper(int n, int last, int len) {
    if (n == 0) {
        return (len % 2 == 1) ? 1 : 0;
    }
    int count = 0;
    for (int i = std::min(n, last); i >= 1; --i) {
        count += count_odd_partitions_helper(n - i, i, len + 1);
    }
    return count;
}

int count_odd_partitions(int n) {
    return count_odd_partitions_helper(n, n, 0);
}

// DP: dp[n][k] = số phân hoạch tự liên hợp của n có k phần
int dp_self_conjugate(int n, int k) {
    vector<vector<int>> dp(n + 1, vector<int>(k + 1, 0));
    dp[0][0] = 1;
    for (int parts = 1; parts <= k; ++parts) {
        for (int sum = 0; sum <= n; ++sum) {
            for (int i = 1; i * (2 * parts - 1) <= sum; ++i) {
                dp[sum][parts] += dp[sum - i * (2 * parts - 1)][parts - 1];
            }
        }
    }
    return dp[n][k];
}

int main() {
    int n, k;
    cout << "Nhap n, k: ";
    cin >> n >> k;
    vector<vector<int>> selfcjg_partitions;
    vector<int> current;
    generate_self_conjugate(n, k, 1, current, selfcjg_partitions);
    cout << "\nSo phan hoach tu lien hop cua n co k phan (p_k^{selfcjg}(n)): " << selfcjg_partitions.size() << endl;
    cout << "Cac phan hoach tu lien hop:" << endl;
    for (auto& part : selfcjg_partitions) {
        for (int x : part) cout << x << ' ';
        cout << endl;
    }
    int odd_parts = count_odd_partitions(n);
    cout << "\nSo phan hoach cua n co le phan: " << odd_parts << endl;
    cout << "So sanh: p_k^{selfcjg}(n) = " << selfcjg_partitions.size() << ", so phan hoach co le phan = " << odd_parts << endl;
    cout << "\nDP: So phan hoach tu lien hop cua n co k phan (dp): " << dp_self_conjugate(n, k) << endl;
    return 0;
}
