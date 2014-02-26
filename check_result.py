#!/usr/bin/env python3
import sys

valuation = set()
for line in sys.stdin.readlines():
    if line.startswith('s SATISFIABLE'):
        continue
    elif line.startswith('s UNSATISFIABLE'):
    			print('ok (unsatisfiable)')
    			exit()
    (mode, variable) = line.split(' ')
    valuation.add(int(variable.strip()))

with open(sys.argv[1]) as fd:
    for line in fd:
        literals = line.split()
        if literals and literals[0] not in ('c', 'p'):
            literals = [int(x) for x in literals if x != '0']
            assert any([x in valuation for x in literals]), literals
print('ok')
