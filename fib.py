# merge sort
from typing import List
def merge_sort(a:List):
        if len(a) < 2:
                return
        mid = len(a) // 2
        left = a[:mid]
        right = a[mid:]
        merge_sort(left)
        merge_sort(right)
        merge(left, right, a)
def merge(a:List, b:List,n):
        i = 0
        j = 0
        while i < len(a) and j < len(b):
                if a[i] < b[j]:
                        i += 1
                else:
                        a.insert(i, b[j])
                        j += 1
                        i += 1
        if j < len(b):
                a.extend(b[j:])
# fill array with random numbers
def test():
        import random
        a = [random.randint(0, 10000) for _ in range(10000)]
        merge_sort(a)
        assert sorted(a)
# run the test 100 times
for i in range(100):
        test()
