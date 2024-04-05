# Nth Fibonacci number
def fib(n):
        if n < 0:
                raise Exception("n can't be negative")
        if n < 2:
                return n
        return fib(n - 1) + fib(n - 2);
print(fib(0))
