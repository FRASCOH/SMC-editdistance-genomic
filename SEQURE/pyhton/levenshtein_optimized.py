def edit_distance(s1, s2, s1len, s2len):
    def minimum(a, b, c):
        return min(a, b, c)

    row1 = [0] * (s2len + 1)
    row2 = [0] * (s2len + 1)

    for x in range(s2len + 1):
        row1[x] = x

    for y in range(1, s1len + 1):
        row2[0] = y
        for x in range(1, s2len + 1):
            row2[x] = minimum(row1[x] + 1, row2[x - 1] + 1, row1[x - 1] + (0 if s1[y - 1] == s2[x - 1] else 1))
        row1, row2 = row2, row1

    return row1[s2len]
