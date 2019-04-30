import sys
from string import ascii_lowercase
from random import randint, choice

def main():
    n = 25000
    if len(sys.argv) > 1:
        n = int(sys.argv[1])
    MAX_ID = 2500
    if len(sys.argv) > 2:
        MAX_ID = int(sys.argv[2])

    for i in range(n):
        c = randint(0,1)
        v = [
             lambda: "{}".format(
                 randint(1, MAX_ID)),
             lambda: "{} {}".format(
                 randint(1, MAX_ID),
                 rQuant())
            ]
        print(v[c]())

def rQuant(priceMax=10000):
    return randint(1, priceMax)

main()
