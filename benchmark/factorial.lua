require("string")

local function factorial(n)
    if n <= 1 then
        return 1
    end
    return n * factorial(n - 1)
end

local result = factorial(5)
print(string.format("5! = %d", result))
