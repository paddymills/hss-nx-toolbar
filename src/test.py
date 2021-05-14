
import re


def test1():
    patt = re.compile(r"^[ xX]+$")

    tests = [
        "Xx x xX",
        "xxx",
        " xX ",
        "thisx",
        "Xx x txX",
    ]

    for test in tests:
        print(patt.match(test))

def test2():
    patt = re.compile(r"[-_]web")

    tests = [
        "1190181A-x1a",
        "1190181A-x1a_web",
        "1190181A-x1a-web",
        "1190181A-x1aweb",
    ]

    for test in tests:
        print(patt.search(test))

# test1()
test2()
