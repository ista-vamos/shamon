extern crate monitor;

use monitor::*;

fn main() {
    let mut new_input_ts = 0;
    let mut input_stream_name = String::new();
    let mut input_stream_value = String::new();

    let mut state = State::default();

    loop {
        if tessla_stdlib::parse_input(&mut input_stream_name, &mut input_stream_value, &mut new_input_ts) {
            break; // reached EOF
        }

        if new_input_ts != state.get_current_ts() {
            state.step(new_input_ts, false).expect("Step failed");
        }

if input_stream_name == "primes2" {
state.set_primes2(input_stream_value.as_str().into());
}
if input_stream_name == "primes1" {
state.set_primes1(input_stream_value.as_str().into());
}
if input_stream_name == "holes2" {
state.set_holes2(input_stream_value.as_str().into());
}
if input_stream_name == "holes1" {
state.set_holes1(input_stream_value.as_str().into());
}

    }

    state.flush().expect("Flush failed");
}