// каналы из условия
chan a = [0] of {bit};
chan b = [0] of {bit};
chan c = [0] of {bit};
chan d = [0] of {bit};
chan e = [0] of {bit};
chan f = [0] of {bit};
chan g = [0] of {bit};
chan h = [0] of {bit};
chan i = [0] of {bit};
chan j = [0] of {bit};
chan k = [0] of {bit};
chan l = [0] of {bit};
chan buffer1 = [2] of {bit};
chan buffer2 = [3] of {bit};

// source(out) — время от времени порождает маркер на выходе;
// записываем 1 в канал
proctype source(chan out) {
	do
	:: 	atomic {
		out!1
	}
	od
}

// sink(in) — время от времени поглощает маркер на входе;
// читаем и удаляем из канала
proctype sink(chan in) {
	do
	::	atomic {
		in?_
	}
	od
}

// fork(in; out1, out2) — по одному входному маркеру генерирует два выходных;
proctype fork(chan in, out1, out2) {
	do
	::	in?_ -> atomic {
		out1!1
		out2!1
	}
	od
}

// join(in1, in2; out) — по двум входным маркерам генерирует один выходной;
proctype join(chan in1, in2, out) {
	do
	:: in1?_ -> in2?_ -> atomic {
		out!1
	}
	od
}

// switch(in; out1, out2) — перенаправляет входной маркер на один из выходов;
proctype switch(chan in, out1, out2) {
	do
	:: in?_ -> atomic {
		out1!1
	}
	:: in?_ -> atomic {
		out2!1
	}
	od
}

// merge(in1, in2; out) — перенаправляет один из входных маркеров на выход;
proctype merge(chan in1, in2, out) {
	do
	:: in1?_ -> atomic {
		out!1
	}
	:: in2?_ -> atomic {
		out!1
	}
	od
}

// queue[N](in; out) — добавляет в буфер емкости N маркер со входа, выдает из буфера
// маркер на выход.

// один перекладывает маркер со входа в буфер (канал заданной емкости)
proctype queue_in(chan in, buf) {
	do
	:: in?_ -> atomic {
		buf!1
	}
	od
}

// второй — из буфера на выход
proctype queue_out(chan out, buf) {
	do
	:: buf?_ -> atomic {
		out!1
	}
	od
}

active proctype watchdog() {
	do
	:: timeout -> assert(false)
	od
}

// errors 0
init {
	run sink(a);
	run sink(b);
	run switch(c, a, b);
	run join(d, e, c);
	run queue_in(f, buffer1);
	run queue_out(d, buffer1);
	run queue_in(g, buffer2);
	run queue_out(e, buffer2); 
	run merge(h, i, f);
	run merge(j, k, g); 
	run source(h); 
	run source(k); 
	run fork(l, i, j);
	run source(l);
}

/*
warning: no accept labels are defined, so option -a has no effect (ignored)

(Spin Version 6.5.2 -- 6 December 2019)
	+ Partial Order Reduction

Full statespace search for:
	never claim         	- (none specified)
	assertion violations	+
	acceptance   cycles 	- (not selected)
	invalid end states	+

State-vector 260 byte, depth reached 41623, errors: 0
    46646 states, stored
   164567 states, matched
   211213 transitions (= stored+matched)
        0 atomic steps
hash conflicts:       196 (resolved)

Stats on memory usage (in Megabytes):
   12.812	equivalent memory usage for states (stored*(State-vector + overhead))
    5.266	actual memory usage for states (compression: 41.11%)
         	state-vector as stored = 90 byte + 28 byte overhead
  128.000	memory used for hash table (-w24)
 5340.576	memory used for DFS stack (-m100000000)
 5473.752	total actual memory usage


unreached in proctype source
	n2.pml:25, state 6, "-end-"
	(1 of 6 states)
unreached in proctype sink
	n2.pml:35, state 6, "-end-"
	(1 of 6 states)
unreached in proctype fork
	n2.pml:45, state 8, "-end-"
	(1 of 8 states)
unreached in proctype join
	n2.pml:54, state 8, "-end-"
	(1 of 8 states)
unreached in proctype switch
	n2.pml:66, state 10, "-end-"
	(1 of 10 states)
unreached in proctype merge
	n2.pml:78, state 10, "-end-"
	(1 of 10 states)
unreached in proctype queue_in
	n2.pml:90, state 7, "-end-"
	(1 of 7 states)
unreached in proctype queue_out
	n2.pml:99, state 7, "-end-"
	(1 of 7 states)
unreached in proctype watchdog
	n2.pml:103, state 2, "assert(0)"
	n2.pml:105, state 6, "-end-"
	(2 of 6 states)
unreached in init
	(0 of 15 states)

pan: elapsed time 0.12 seconds
pan: rate 388716.67 states/second
*/

