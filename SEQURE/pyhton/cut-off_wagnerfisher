def wagner_fischer_O1(s, t):
    n = len(s)
    m = len(t)
    buf = list(range(m + 1))

    for i in range(1, n + 1):
        tmp = buf[0]
        buf[0] = i

        for j in range(1, m + 1):
            if s[i - 1] == t[j - 1]:
                tmp, buf[j] = buf[j], tmp
            else:
                val = min(buf[j], buf[j - 1], tmp) + 1
                tmp, buf[j] = buf[j], val

    return buf[m]
