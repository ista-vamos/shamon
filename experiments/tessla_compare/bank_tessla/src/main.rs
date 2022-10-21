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

if input_stream_name == "monwithdraw" {
state.set_monwithdraw(input_stream_value.as_str().into());
}
if input_stream_name == "montransfer" {
state.set_montransfer(input_stream_value.as_str().into());
}
if input_stream_name == "reset" {
state.set_reset(input_stream_value.as_str().into());
}
if input_stream_name == "monbalance" {
state.set_monbalance(input_stream_value.as_str().into());
}
if input_stream_name == "deposit" {
state.set_deposit(input_stream_value.as_str().into());
}

    }

    state.flush().expect("Flush failed");
}