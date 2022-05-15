def partition_size(ls: list, size):
    
# """

# Returns a new list with elements

# of which is a list of certain size.

# >>> partition_size([1, 2, 3, 4], 3)

# [[1, 2, 3], [4]]

# :param ls:

# :param size:

# :return: list cut by size

# """

    if size <= 0:

        return ls

    return [ls[i:i + size] for i in range(0, len(ls), size)]

def partition_cut(ls: list, ncut):

    # """

    # Returns a new list with elements

    # of which is a list of certain size.

    # >>> partition_cut([1, 2, 3, 4, 5], 3)

    # [[1, 2], [3, 4], [5]]

    # :param ls:

    # :param n:

    # :return: list has been cut

    # """

    n = len(ls)

    if n <= 1 or ncut <= 1:

        return ls

    res = []

    ncut_r = ncut

    n_r = n

    while ncut_r > 0:

    if n_r % ncut_r == 0:

    dist = int(n_r / ncut_r)

    for _ in range(ncut_r):

    next = n - n_r + dist

    res.insert(len(res), ls[n - n_r:next])

    n_r -= dist

    break

    else:

    dist = ceil(n_r / ncut_r)

    res.insert(len(res), ls[n - n_r:n - n_r + dist])

    ncut_r -= 1

    n_r -= dist

    continue

    return res

    def partition_cut2(ls: list, ncut):

    # """

    # Returns a new list with elements

    # of which is a list of certain size.

    # >>> partition_cut([1, 2, 3, 4, 5], 3)

    # [[1, 4], [2, 5], [3]]

    # :param ls:

    # :param n:

    # :return: list has been cut

    # """

    n = len(ls)

    if n <= 1 or ncut <= 1:

    return ls

    res = [[] for _ in range(ncut)]

    for i in range(n):

    idx = i % ncut

    res[idx].append(ls[i])

    return res

    if __name__ == '__main__':

    l1 = list(range(1, 6, 1))

    # for i in range(0, len(l1)):

    # print('cut {} : {}'.format(i, partition_size(l1, i)))

    for i in range(0, len(l1)):

    print('cut {} : {}'.format(i, partition_cut(l1, i)))

    print('finished')

    for i in range(0, len(l1)):

    print('cut {} : {}'.format(i, partition_cut2(l1, i)))

    print('finished')