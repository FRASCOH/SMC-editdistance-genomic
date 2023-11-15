def minimum(a, b, c):
    return a if a < b and a < c else b if b < c else c

def minimum_two(a, b):
    return a if a < b else b

def maximum_two(a, b):
    return a if a > b else b

def edit_distance_generalized_threshold(s1, s2, s1len, s2len):
    row = [0] * (s2len + 1)

    gmax = (s2len - 1) // 2
    gmin = 1 - gmax - (s1len - s2len)

    for j in range(gmax + 1):
        row[j] = j

    for i in range(1, s1len + 1):
        row[0] = i - 1

        gmin += 1
        gmax += 1
        min_val = maximum_two(gmin, 1)
        max_val = minimum_two(gmax, s2len)
        dia = row[min_val - 1]
        top = row[min_val]

        if s1[i - 1] != s2[min_val - 1]:
            dia = minimum_two(dia, top) + 1

        row[min_val] = dia
        left = dia
        dia = top

        for j in range(min_val + 1, max_val + 1):
            top = row[j]

            if s1[i - 1] != s2[j - 1]:
                dia = minimum(dia, top, left) + 1

            row[j] = dia
            left = dia
            dia = top

        if s2len == max_val:
            continue

        if s1[i - 1] != s2[max_val]:
            dia = minimum_two(dia, left) + 1

        row[max_val + 1] = dia

    dia = row[s2len]
    return dia
