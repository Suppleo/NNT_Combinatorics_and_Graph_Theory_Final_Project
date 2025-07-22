#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

// === CÁC HÀM TRỢ GIÚP (Không đổi) ===

void print_partition(const vector<int>& p) {
    cout << "     (";
    for (size_t i = 0; i < p.size(); ++i) {
        cout << p[i] << (i == p.size() - 1 ? "" : ", ");
    }
    cout << ")" << endl;
}

vector<int> reconstruct_from_hooks(const vector<int>& hooks) {
    if (hooks.empty()) {
        return {};
    }
    int h1 = hooks[0];
    int d1 = (h1 + 1) / 2;
    
    vector<int> remaining_hooks(hooks.begin() + 1, hooks.end());
    vector<int> sub_lambda = reconstruct_from_hooks(remaining_hooks);
    
    vector<int> result(d1);
    result[0] = d1;
    for (int i = 1; i < d1; ++i) {
        result[i] = 1 + (i - 1 < sub_lambda.size() ? sub_lambda[i - 1] : 0);
    }
    return result;
}

// === CÁC HÀM TÍNH TOÁN (Cập nhật) ===

// Tìm phân hoạch lẻ-phân biệt (không đổi)
void find_odd_distinct_partitions(int target, int k_rem, int max_val, vector<int>& current, vector<vector<int>>& result) {
    if (k_rem == 0) {
        if (target == 0) {
            result.push_back(current);
        }
        return;
    }
    if (target < k_rem * k_rem) {
        return;
    }
    for (int i = max_val; i >= 1; i -= 2) {
        if (target >= i) {
            current.push_back(i);
            find_odd_distinct_partitions(target - i, k_rem - 1, i - 2, current, result);
            current.pop_back();
        }
    }
}

// *** HÀM MỚI: Sinh tất cả các phân hoạch tự liên hợp của n ***
vector<vector<int>> generate_all_self_conjugate(int n) {
    vector<vector<int>> all_lambda_partitions;
    
    // Thử mọi số hook có thể (k_hooks)
    // Một phân hoạch với k_hooks cần tổng tối thiểu là k_hooks^2
    for (int k_hooks = 1; k_hooks * k_hooks <= n; ++k_hooks) {
        vector<vector<int>> hook_partitions;
        vector<int> current_hook_partition;
        int max_val_start = n - (k_hooks > 1 ? (k_hooks - 1) * (k_hooks - 1) : 0);
        if (max_val_start % 2 == 0) max_val_start--;

        find_odd_distinct_partitions(n, k_hooks, max_val_start, current_hook_partition, hook_partitions);

        for (const auto& hooks : hook_partitions) {
            all_lambda_partitions.push_back(reconstruct_from_hooks(hooks));
        }
    }
    return all_lambda_partitions;
}


// Hàm sinh phân hoạch có số phần tử lẻ (không đổi)
void generate_all_partitions_recursive(int target, int max_val, vector<int>& current, vector<vector<int>>& result) {
    if (target == 0) {
        result.push_back(current);
        return;
    }
    for (int i = min(target, max_val); i >= 1; --i) {
        current.push_back(i);
        generate_all_partitions_recursive(target - i, i, current, result);
        current.pop_back();
    }
}

// === HÀM MAIN (Cập nhật) ===

int main() {
    int n, k;
    cout << "Nhap so tu nhien n: ";
    cin >> n;
    cout << "Nhap so phan tu cua phan hoach cuoi cung (k): ";
    cin >> k;
    
    cout << "\n## PHAN HOACH TU LIEN HOP CUA " << n << " CO " << k << " PHAN TU ##\n";

    // 1. Sinh TẤT CẢ các phân hoạch tự liên hợp
    vector<vector<int>> all_sc_partitions = generate_all_self_conjugate(n);

    // 2. Lọc ra những phân hoạch có đúng k phần tử
    vector<vector<int>> filtered_partitions;
    for (const auto& p : all_sc_partitions) {
        if (p.size() == k) {
            filtered_partitions.push_back(p);
        }
    }

    // 3. In kết quả đã lọc
    cout << "   So luong tim thay: " << filtered_partitions.size() << endl;
    cout << "   Cac phan hoach tuong ung:" << endl;
    if (filtered_partitions.empty()) {
        cout << "     Khong co phan hoach nao." << endl;
    } else {
        for (const auto& p : filtered_partitions) {
            print_partition(p);
        }
    }

    cout << "\n--------------------------------------------------\n\n";

    cout << "## PHAN HOACH CUA " << n << " CO SO PHAN TU LE ##\n";
    vector<vector<int>> all_partitions;
    vector<int> current_partition;
    generate_all_partitions_recursive(n, n, current_partition, all_partitions);

    vector<vector<int>> odd_length_partitions;
    for(const auto& p : all_partitions) {
        if (p.size() % 2 != 0) {
            odd_length_partitions.push_back(p);
        }
    }
    
    cout << "   So luong: " << odd_length_partitions.size() << endl;
    cout << "   Cac phan hoach tuong ung:" << endl;
    if (odd_length_partitions.empty()) {
        cout << "     Khong co phan hoach nao." << endl;
    } else {
        for (const auto& p : odd_length_partitions) {
            print_partition(p);
        }
    }

    return 0;
}