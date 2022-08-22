def f(n):
    for i in [x for x in range(n) if x % 2 == 1]:
        for j in [x for x in range(n) if x % 2 == 0]:
            yield i

print([n for n in f(5)])