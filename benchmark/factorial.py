def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)


result = factorial(5)
print(f"5! = {result}")
