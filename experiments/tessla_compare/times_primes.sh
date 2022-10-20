REP=10

echo "type,time,monitor" > primes_times.csv
for i in `seq 1 $REP`; do
    experiments/primes/runprimes.sh & 
    /bin/time -f '%e' -o tmp.file experiments/tessla_compare/monitor_primes P_0:regex:/primes1 P_1:regex:/primes2
    printf "no_errors,%s,vamos\n" "$(<tmp.file)" >> primes_times.csv

    wait
    wait

    experiments/primes/runprimes-bad.sh &
    /bin/time -f '%e' -o tmp.file experiments/tessla_compare/monitor_primes P_0:regex:/primes1 P_1:regex:/primes2
    printf "errors,%s,vamos\n" "$(<tmp.file)" >> primes_times.csv
    wait
    wait

    /bin/time -f '%e' -o tmp.file experiments/tessla_compare/primes_tessla/target/debug/tessla_monitor < experiments/tessla_compare/gen_tessla_input/primes.in
    printf "no_errors,%s,tessla\n" "$(<tmp.file)" >> primes_times.csv

    /bin/time -f '%e' -o tmp.file experiments/tessla_compare/primes_tessla/target/debug/tessla_monitor < experiments/tessla_compare/gen_tessla_input/primes_bad.in
    printf "errors,%s,tessla\n" "$(<tmp.file)" >> primes_times.csv

done

mv primes_times.csv experiments/tessla_compare/primes_times.csv
rm tmp.file