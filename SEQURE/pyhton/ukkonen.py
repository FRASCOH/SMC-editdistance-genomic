def minimum(a, b, c):
    return min(a, b, c)

def minimum_two(a, b):
    return min(a, b)

def maximum_two(a, b):
    return max(a, b)

def edit_distance_threshold(s1, s2, s1len, s2len, threshold):
    if threshold == -1:
        threshold = float('inf')

    p = [0] * (s2len + 1)
    d = [float('inf')] * (s2len + 1)

    i, row, col = 0, 0, 0
    min_val, max_val = 0, 0
    boundary = minimum_two(s2len, threshold) + 1

    for i in range(boundary):
        p[i] = i

    for i in range(i, s2len + 1):
        p[i] = float('inf')

    for row in range(1, s1len + 1):
        d[0] = row

        min_val = maximum_two(1, row - threshold)
        max_val = minimum_two(s2len, row + threshold)

        if min_val > max_val:
            return -1

        if min_val > 1:
            d[min_val - 1] = float('inf')

        for col in range(min_val, max_val + 1):
            if s1[row - 1] == s2[col - 1]:
                d[col] = p[col - 1]
            else:
                d[col] = minimum(p[col - 1], d[col - 1], p[col]) + 1

        p, d = d, p

    if p[s2len] <= threshold:
        return p[s2len]
    else:
        return -1
