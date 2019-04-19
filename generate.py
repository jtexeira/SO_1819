import sys
from string import ascii_lowercase
from random import randint, choice

def main():
    id = 1
    n = 25000
    if len(sys.argv) > 1:
        n = int(sys.argv[1])
    for i in range(n):
        c = randint(0,2)
        if c == 0:
            print("i {0} {1}".format(
                rString(),
                rPrice()))
            id += 1
        elif c == 1:
            print("n {0} {1}".format(
                randint(1, id),
                rString()))
        elif c == 2:
            print("p {0} {1}".format(
                randint(1, id),
                rPrice()))

def rString(strLen=10):
    return ''.join(choice(ascii_lowercase) for i in range(strLen))

def rPrice(priceMax=100000):
    return str(randint(1, priceMax) / 10)

main()
