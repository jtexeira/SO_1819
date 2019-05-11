import sys
from string import ascii_lowercase
from random import randint, choice

def main():
    n = 25000
    if len(sys.argv) > 1:
        n = int(sys.argv[1])
    for i in range(n):
        print("i {0} {1}".format(i, 0))
    for i in range(n):
        print("n {0} {1}".format(i, i*10))
    for i in range(n):
        print("p {0} {1}". format(i, i))

main()
