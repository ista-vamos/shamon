// This Rust code was automatically created by tessla-compiler from a TeSSLa Specification
// #![allow(unused_parens, unused_variables, non_snake_case, non_camel_case_types, uncommon_codepoints, non_upper_case_globals)]

extern crate tessla_stdlib;

use std::borrow::BorrowMut;
use std::marker::PhantomData;

use tessla_stdlib::*;



pub struct State<
    E,
Fok, BFok
> where
Fok: (FnMut(TesslaBool, i64) -> Result<(), E>) + ?Sized,
BFok: BorrowMut<Fok>
{
    current_ts: i64,
    last_processed_ts: i64,
    _marker: PhantomData<E>,
out_ok: BFok /* ok */,
var_1001_: EventContainer<TesslaInt>,
var_curr_979_: EventContainer<TesslaInt>,
var_mergedStream_975_: EventContainer<TesslaInt>,
var_972_: EventContainer<TesslaInt>,
var_on_971_: EventContainer<TesslaInt>,
var_963_: EventContainer<TesslaInt>,
var_962_: EventContainer<TesslaInt>,
var_curr_977_: EventContainer<TesslaInt>,
var_cnt_970_: EventContainer<TesslaInt>,
var_resetCount_969_: EventContainer<TesslaInt>,
var_982_: EventContainer<TesslaInt>,
var_968_: EventContainer<TesslaBool>,
var_sum_985_: EventContainer<TesslaInt>,
var_967_: EventContainer<TesslaInt>,
var_dropped1_966_: EventContainer<TesslaInt>,
var_pos1_961_: EventContainer<TesslaInt>,
var_990_: EventContainer<TesslaInt>,
var_989_: EventContainer<TesslaInt>,
var_mergedStream_1002_: EventContainer<TesslaInt>,
var_999_: EventContainer<TesslaInt>,
var_on_998_: EventContainer<TesslaInt>,
var_curr_1004_: EventContainer<TesslaInt>,
var_cnt_997_: EventContainer<TesslaInt>,
var_1005_: EventContainer<TesslaInt>,
var_1009_: EventContainer<TesslaInt>,
var_995_: EventContainer<TesslaBool>,
var_sum_1012_: EventContainer<TesslaInt>,
var_994_: EventContainer<TesslaInt>,
var_dropped2_993_: EventContainer<TesslaInt>,
var_pos2_988_: EventContainer<TesslaInt>,
var_953_: EventContainer<TesslaBool>,
var_positions__same_952_: EventContainer<TesslaUnit>,
var_mergedStream_959_: EventContainer<TesslaValue<(TesslaInt, TesslaInt,)>>,
var_958_: EventContainer<TesslaValue<(TesslaInt, TesslaInt,)>>,
var_mergedStream_955_: EventContainer<TesslaValue<(TesslaInt, TesslaInt,)>>,
var_954_: EventContainer<TesslaValue<(TesslaInt, TesslaInt,)>>,
var_on_951_: EventContainer<TesslaValue<(TesslaInt, TesslaInt,)>>,
var_on_957_: EventContainer<TesslaValue<(TesslaInt, TesslaInt,)>>,
var_950_: EventContainer<TesslaBool>,
_marker_ok: PhantomData<Fok>,
var_primes2: EventContainer<TesslaValue<(TesslaInt, TesslaInt,)>>,
var_primes1: EventContainer<TesslaValue<(TesslaInt, TesslaInt,)>>,
var_holes2: EventContainer<TesslaInt>,
var_holes1: EventContainer<TesslaInt>
}

fn var_isSome_1034_<T_374_: 'static + Clone>(var_opt_1050_: TesslaOption<TesslaValue<T_374_>>) -> TesslaBool {
let var_1051_ = !(var_opt_1050_.clone().is_none());
return var_1051_.clone();
}
fn var_1008_(var_c_1043_: TesslaOption<TesslaInt>, var_e_1044_: TesslaOption<TesslaValue<(TesslaInt, TesslaInt,)>>) -> TesslaOption<TesslaInt> {
let var_1045_ = match /* if */ (var_isSome_1034_(var_e_1044_.clone())) {
    Error(error) => Error(error),
    Value(true) => { /* then */ Value(Some((var_c_1043_.clone().get_some() + Value(1_i64)))) },
    Value(false) => { /* else */ var_c_1043_.clone() }
};
return var_1045_.clone();
}
fn var_960_(var_trig_1020_: TesslaOption<TesslaUnit>, var_str_1021_: TesslaOption<TesslaValue<(TesslaInt, TesslaInt,)>>) -> TesslaOption<TesslaValue<(TesslaInt, TesslaInt,)>> {
let var_1022_ = match /* if */ (var_trig_1020_.clone().is_none()) {
    Error(error) => Error(error),
    Value(true) => { /* then */ Value(None) },
    Value(false) => { /* else */ var_str_1021_.clone() }
};
return var_1022_.clone();
}
fn var_976_(var_trig_1025_: TesslaOption<TesslaInt>, var_str_1026_: TesslaOption<TesslaInt>) -> TesslaOption<TesslaInt> {
let var_1027_ = match /* if */ (var_trig_1025_.clone().is_none()) {
    Error(error) => Error(error),
    Value(true) => { /* then */ Value(None) },
    Value(false) => { /* else */ var_str_1026_.clone() }
};
return var_1027_.clone();
}

impl<
    E,
Fok, BFok
> State<
    E,
Fok, BFok
> where
Fok: (FnMut(TesslaBool, i64) -> Result<(), E>) + ?Sized,
BFok: BorrowMut<Fok>
{
    pub fn new(
out_ok: BFok /* ok */,
    ) -> Self {
        Self {
            current_ts: 0,
            last_processed_ts: 0,
            _marker: PhantomData,
var_1001_: init(),
var_curr_979_: init_with_value(Value(0_i64)),
var_mergedStream_975_: init(),
var_972_: init(),
var_on_971_: init(),
var_963_: init_with_value(Value(0_i64)),
var_962_: init(),
var_curr_977_: init(),
var_cnt_970_: init(),
var_resetCount_969_: init(),
var_982_: init(),
var_968_: init(),
var_sum_985_: init_with_value(Value(0_i64)),
var_967_: init(),
var_dropped1_966_: init(),
var_pos1_961_: init(),
var_990_: init_with_value(Value(0_i64)),
var_989_: init(),
var_mergedStream_1002_: init(),
var_999_: init(),
var_on_998_: init(),
var_curr_1004_: init(),
var_cnt_997_: init(),
var_1005_: init(),
var_1009_: init(),
var_995_: init(),
var_sum_1012_: init_with_value(Value(0_i64)),
var_994_: init(),
var_dropped2_993_: init(),
var_pos2_988_: init(),
var_953_: init(),
var_positions__same_952_: init(),
var_mergedStream_959_: init(),
var_958_: init(),
var_mergedStream_955_: init(),
var_954_: init(),
var_on_951_: init(),
var_on_957_: init(),
var_950_: init(),
out_ok: out_ok,
_marker_ok: PhantomData,
var_primes2: init(),
var_primes1: init(),
var_holes2: init(),
var_holes1: init()
        }
    }

    pub fn get_current_ts(&self) -> i64 {
        self.current_ts
    }

pub fn set_primes2(&mut self, value: TesslaValue<(TesslaInt, TesslaInt,)>) {
self.var_primes2.set_event(value);
}

pub fn set_primes1(&mut self, value: TesslaValue<(TesslaInt, TesslaInt,)>) {
self.var_primes1.set_event(value);
}

pub fn set_holes2(&mut self, value: TesslaInt) {
self.var_holes2.set_event(value);
}

pub fn set_holes1(&mut self, value: TesslaInt) {
self.var_holes1.set_event(value);
}

    pub fn flush(&mut self) -> Result<(), E> {
        self.step(self.current_ts, true)
    }

    pub fn step(&mut self, new_input_ts: i64, flush: bool) -> Result<(), E> {
        let mut flush_required = flush;

        if new_input_ts > self.current_ts || flush_required {
            let mut do_processing = true;
            while do_processing {



                if self.current_ts == new_input_ts && !flush_required {
                    do_processing = false;
                } else {

default(&mut self.var_curr_979_, &self.var_1001_);
last(&mut self.var_mergedStream_975_, &self.var_curr_979_, &self.var_holes1);
merge(&mut self.var_972_, vec![&self.var_curr_979_, &self.var_mergedStream_975_]);
lift2(&mut self.var_on_971_, &self.var_holes1, &self.var_972_, TesslaValue::wrap(var_976_ as fn(_, _) -> _));
count(&mut self.var_963_, &self.var_primes1);
merge(&mut self.var_962_, vec![&self.var_963_, &self.var_curr_979_]);
last(&mut self.var_curr_977_, &self.var_resetCount_969_, &self.var_primes1);
merge(&mut self.var_cnt_970_, vec![&self.var_on_971_, &self.var_curr_977_, &self.var_curr_979_]);
lift2(&mut self.var_resetCount_969_, &self.var_cnt_970_, &self.var_primes1, TesslaValue::wrap(var_1008_ as fn(_, _) -> _));
last(&mut self.var_982_, &self.var_holes1, &self.var_primes1);
slift2(&mut self.var_968_, &self.var_resetCount_969_, &self.var_982_, TesslaValue::wrap((|tLPar_0: TesslaInt, tLPar_1: TesslaInt|{ return tLPar_0.lt(&tLPar_1) }) as fn(_, _) -> _));
fold(&mut self.var_sum_985_, &self.var_holes1, TesslaValue::wrap((|tLPar_0: TesslaInt, tLPar_1: TesslaInt|{ return (tLPar_0 + tLPar_1) }) as fn(_, _) -> _));
slift3(&mut self.var_967_, &self.var_968_, &self.var_curr_979_, &self.var_sum_985_, TesslaValue::wrap((|tLPar_0: TesslaBool, tLPar_1: TesslaInt, tLPar_2: TesslaInt|{ return match /* if */ (tLPar_0) {
    Error(error) => Error(error),
    Value(true) => { /* then */ tLPar_1 },
    Value(false) => { /* else */ tLPar_2 }
} }) as fn(_, _, _) -> _));
merge(&mut self.var_dropped1_966_, vec![&self.var_967_, &self.var_curr_979_]);
slift2(&mut self.var_pos1_961_, &self.var_962_, &self.var_dropped1_966_, TesslaValue::wrap((|tLPar_0: TesslaInt, tLPar_1: TesslaInt|{ return (tLPar_0 + tLPar_1) }) as fn(_, _) -> _));
count(&mut self.var_990_, &self.var_primes2);
merge(&mut self.var_989_, vec![&self.var_990_, &self.var_curr_979_]);
last(&mut self.var_mergedStream_1002_, &self.var_curr_979_, &self.var_holes2);
merge(&mut self.var_999_, vec![&self.var_curr_979_, &self.var_mergedStream_1002_]);
lift2(&mut self.var_on_998_, &self.var_holes2, &self.var_999_, TesslaValue::wrap(var_976_ as fn(_, _) -> _));
last(&mut self.var_curr_1004_, &self.var_1005_, &self.var_primes2);
merge(&mut self.var_cnt_997_, vec![&self.var_on_998_, &self.var_curr_1004_, &self.var_curr_979_]);
lift2(&mut self.var_1005_, &self.var_cnt_997_, &self.var_primes2, TesslaValue::wrap(var_1008_ as fn(_, _) -> _));
last(&mut self.var_1009_, &self.var_holes2, &self.var_primes2);
slift2(&mut self.var_995_, &self.var_1005_, &self.var_1009_, TesslaValue::wrap((|tLPar_0: TesslaInt, tLPar_1: TesslaInt|{ return tLPar_0.lt(&tLPar_1) }) as fn(_, _) -> _));
fold(&mut self.var_sum_1012_, &self.var_holes2, TesslaValue::wrap((|tLPar_0: TesslaInt, tLPar_1: TesslaInt|{ return (tLPar_0 + tLPar_1) }) as fn(_, _) -> _));
slift3(&mut self.var_994_, &self.var_995_, &self.var_curr_979_, &self.var_sum_1012_, TesslaValue::wrap((|tLPar_0: TesslaBool, tLPar_1: TesslaInt, tLPar_2: TesslaInt|{ return match /* if */ (tLPar_0) {
    Error(error) => Error(error),
    Value(true) => { /* then */ tLPar_1 },
    Value(false) => { /* else */ tLPar_2 }
} }) as fn(_, _, _) -> _));
merge(&mut self.var_dropped2_993_, vec![&self.var_994_, &self.var_curr_979_]);
slift2(&mut self.var_pos2_988_, &self.var_989_, &self.var_dropped2_993_, TesslaValue::wrap((|tLPar_0: TesslaInt, tLPar_1: TesslaInt|{ return (tLPar_0 + tLPar_1) }) as fn(_, _) -> _));
slift2(&mut self.var_953_, &self.var_pos1_961_, &self.var_pos2_988_, TesslaValue::wrap((|tLPar_0: TesslaInt, tLPar_1: TesslaInt|{ return tLPar_0.eq(&tLPar_1) }) as fn(_, _) -> _));
unit_if(&mut self.var_positions__same_952_, &self.var_953_);
last(&mut self.var_mergedStream_959_, &self.var_primes2, &self.var_positions__same_952_);
merge(&mut self.var_958_, vec![&self.var_primes2, &self.var_mergedStream_959_]);
last(&mut self.var_mergedStream_955_, &self.var_primes1, &self.var_positions__same_952_);
merge(&mut self.var_954_, vec![&self.var_primes1, &self.var_mergedStream_955_]);
lift2(&mut self.var_on_951_, &self.var_positions__same_952_, &self.var_954_, TesslaValue::wrap(var_960_ as fn(_, _) -> _));
lift2(&mut self.var_on_957_, &self.var_positions__same_952_, &self.var_958_, TesslaValue::wrap(var_960_ as fn(_, _) -> _));
slift2(&mut self.var_950_, &self.var_on_951_, &self.var_on_957_, TesslaValue::wrap((|tLPar_0: TesslaValue<(TesslaInt, TesslaInt,)>, tLPar_1: TesslaValue<(TesslaInt, TesslaInt,)>|{ return tLPar_0.eq(&tLPar_1) }) as fn(_, _) -> _));
self.var_950_.call_output(self.out_ok.borrow_mut(), self.current_ts)?;



self.var_1001_.update_last();
self.var_curr_979_.update_last();
self.var_mergedStream_975_.update_last();
self.var_972_.update_last();
self.var_on_971_.update_last();
self.var_963_.update_last();
self.var_962_.update_last();
self.var_curr_977_.update_last();
self.var_cnt_970_.update_last();
self.var_resetCount_969_.update_last();
self.var_982_.update_last();
self.var_968_.update_last();
self.var_sum_985_.update_last();
self.var_967_.update_last();
self.var_dropped1_966_.update_last();
self.var_pos1_961_.update_last();
self.var_990_.update_last();
self.var_989_.update_last();
self.var_mergedStream_1002_.update_last();
self.var_999_.update_last();
self.var_on_998_.update_last();
self.var_curr_1004_.update_last();
self.var_cnt_997_.update_last();
self.var_1005_.update_last();
self.var_1009_.update_last();
self.var_995_.update_last();
self.var_sum_1012_.update_last();
self.var_994_.update_last();
self.var_dropped2_993_.update_last();
self.var_pos2_988_.update_last();
self.var_953_.update_last();
self.var_positions__same_952_.update_last();
self.var_mergedStream_959_.update_last();
self.var_958_.update_last();
self.var_mergedStream_955_.update_last();
self.var_954_.update_last();
self.var_on_951_.update_last();
self.var_on_957_.update_last();
self.var_950_.update_last();
self.var_primes2.update_last();
self.var_primes1.update_last();
self.var_holes2.update_last();
self.var_holes1.update_last();

                    flush_required = flush && (self.current_ts != new_input_ts);
                    self.last_processed_ts = self.current_ts;
                    self.current_ts = new_input_ts;
                }
            }
        } else if new_input_ts < self.current_ts {
            panic!("{}: FATAL: decreasing timestamp received", self.current_ts);
        }
        Ok(())
    }
}

impl Default for State<
    (),
fn(TesslaBool, i64) -> Result<(), ()>,
fn(TesslaBool, i64) -> Result<(), ()>
> {
    fn default() -> Self {
        Self::new(
|value, ts| Ok(output_var(value, "ok", ts, false))
        )
    }
}