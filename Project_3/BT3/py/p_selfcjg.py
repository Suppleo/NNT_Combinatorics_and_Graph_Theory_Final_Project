def generate_self_conjugate(n, k, min_val=1, current=None, result=None):
    if current is None:
        current = []
    if result is None:
        result = []
    if n == 0 and len(current) == k:
        result.append(list(current))
        return result
    if n <= 0 or len(current) >= k:
        return result
    for i in range(min_val, n // (2 * len(current) + 1) + 1):
        current.append(i)
        generate_self_conjugate(n - i * (2 * len(current) - 1), k, i, current, result)
        current.pop()
    return result

def count_odd_partitions(n):
    count = 0
    current = []
    def dfs(remain, last):
        nonlocal count
        if remain == 0:
            if len(current) % 2 == 1:
                count += 1
            return
        for i in range(min(remain, last), 0, -1):
            current.append(i)
            dfs(remain - i, i)
            current.pop()
    dfs(n, n)
    return count

def dp_self_conjugate(n, k):
    dp = [[0] * (k + 1) for _ in range(n + 1)]
    dp[0][0] = 1
    for parts in range(1, k + 1):
        for total in range(n + 1):
            for i in range(1, total // (2 * parts - 1) + 1):
                dp[total][parts] += dp[total - i * (2 * parts - 1)][parts - 1]
    return dp[n][k]

def main():
    n = int(input('Nhap n: '))
    k = int(input('Nhap k: '))
    selfcjg_partitions = generate_self_conjugate(n, k)
    print(f"\nSo phan hoach tu lien hop cua n co k phan (p_k^{{selfcjg}}(n)): {len(selfcjg_partitions)}")
    print("Cac phan hoach tu lien hop:")
    for part in selfcjg_partitions:
        print(' '.join(map(str, part)))
    odd_parts = count_odd_partitions(n)
    print(f"\nSo phan hoach cua n co le phan: {odd_parts}")
    print(f"So sanh: p_k^{{selfcjg}}(n) = {len(selfcjg_partitions)}, so phan hoach co le phan = {odd_parts}")
    print(f"\nDP: So phan hoach tu lien hop cua n co k phan (dp): {dp_self_conjugate(n, k)}")

if __name__ == '__main__':
    main()
