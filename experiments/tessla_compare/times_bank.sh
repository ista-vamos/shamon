REP=10

for i in `seq 1 $REP`; do
    /bin/time -f '%e' -o tmp.file bank_tessla/target/debug/tessla_monitor < gen_tessla_input/bank.in
    MONPID=$!
    wait $MONPID
    printf "no_errors,%s,tessla\n" "$(<tmp.file)" >> bank_times.csv

    /bin/time -f '%e' -o tmp.file bank_tessla/target/debug/tessla_monitor < gen_tessla_input/bank_bad.in
    MONPID=$!
    wait $MONPID
    printf "errors,%s,tessla\n" "$(<tmp.file)" >> bank_times.csv

done

rm tmp.file