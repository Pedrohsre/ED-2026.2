#!/bin/sh
# Roda o programa do SAT com os .cnf de data/. Use: make sat && sh scripts/testar_sat.sh
# sat_*.cnf deve dar SAT, unsat_*.cnf deve dar UNSAT, invalidos/* deve dar erro (sem travar).
fail=0

for f in data/sat_*.cnf; do
    out=$(./build/sat "$f" | head -n 1)
    [ "$out" = "SAT" ] && echo "ok   $f" || { echo "FAIL $f (obteve: $out)"; fail=1; }
done

for f in data/unsat_*.cnf; do
    out=$(./build/sat "$f" | head -n 1)
    [ "$out" = "UNSAT" ] && echo "ok   $f" || { echo "FAIL $f (obteve: $out)"; fail=1; }
done

for f in data/invalidos/*.cnf; do
    msg=$(./build/sat "$f" 2>&1 >/dev/null); code=$?
    if [ $code -eq 1 ] && [ -n "$msg" ]; then echo "ok   $f -> $msg"
    else echo "FAIL $f (codigo $code)"; fail=1; fi
done

exit $fail
