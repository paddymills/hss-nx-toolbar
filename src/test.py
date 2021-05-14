
import re

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
