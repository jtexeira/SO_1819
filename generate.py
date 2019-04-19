import sys
from string import ascii_lowercase
from random import randint, choice

def main():
    MAX_ID = 1
    n = 25000
    if len(sys.argv) > 1:
        n = int(sys.argv[1])
    for i in range(n):
        c = randint(0,2)
        v = [
             lambda: "i {} {}".format(
                 rString(),
                 rPrice()),
             lambda: "n {} {}".format(
                 randint(1, MAX_ID),
                 rString()),
             lambda: "p {} {}".format(
                 randint(1, MAX_ID),
                 rPrice())
            ]
        print(v[c]())
        if c is 0: MAX_ID += 1

def rString(strLen=10):
    return ''.join(choice(ascii_lowercase) for i in range(strLen))

def rPrice(priceMax=100000):
    return randint(1, priceMax) / 10

main()
