bit boat      = 0;    // на левом берегу - 0, на правом - 1
bit husband   = 0;
bit wife      = 0;
bit fisherman = 0;
bit child1    = 0;
bit child2    = 0;

// решение завязано на том, что дети могут грести и перемещают лодку влево / вправо
// 1) два ребенка отправляются на другой берег
// 2) один возвращается
// 3) один из взрослых переправляется
// 4) ребенок на другом берегу возвращается
// 5) забирает ребенка и оба снова переправляются
// 6) повторить пока все не окажутся на правом берегу

// в конце считаем что так как все на одном берегу, 
// то рыбак забирает свою лодку и делает с ней что хочет

active proctype main() {
  do
  //  случай когда оба ребенка на одном берегу и надо переправиться 
  ::  child1 == boat && child2 == boat -> atomic { 
        printf("1) два ребенка отправляются на другой берег\n")
        boat    = 1 - boat;
        child1  = 1 - child1; 
        child2  = 1 - child2;
      }

  //  один ребенок возвращается
  ::  child1 == boat -> atomic {
        printf("2) один возвращается\n")
        boat    = 1 - boat; 
        child1  = 1 - child1; 
      }

  //  второй ребенок возвращается за первым
  ::  child2 == boat -> atomic {
        printf("4) ребенок на другом берегу возвращается\n")
        boat    = 1 - boat; 
        child2  = 1 - child2; 
      }
  
  // три случая когда муж/жена/рыбак вместе с лодкой
  ::  husband == boat -> atomic {
        printf("3) один из взрослых переправляется (муж)\n")
        boat    = 1 - boat; 
        husband = 1 - husband;
      }
  ::  wife == boat -> atomic {
        printf("3) один из взрослых переправляется (жена)\n")
        boat    = 1 - boat; 
        wife    = 1 - wife;
      }
  ::  fisherman == boat -> atomic {
        printf("3) один из взрослых переправляется (рыбак)\n")
        boat    = 1 - boat; 
        fisherman = 1 - fisherman;
      }
  od
}

ltl GoalNeverReached {
  !(<>(boat       == 1 && 
       child1     == 1 && 
       child2     == 1 && 
       husband    == 1 && 
       wife       == 1 && 
       fisherman  == 1
      )
    )
}

/*
pan:1: assertion violated  !(((((((boat==1)&&(child1==1))&&(child2==1))&&(husband==1))&&(wife==1))&&(fisherman==1))) (at depth 52)
pan: wrote n1.pml.trail

(Spin Version 6.5.2 -- 6 December 2019)
Warning: Search not completed
  + Partial Order Reduction

Full statespace search for:
  never claim           + (GoalNeverReached)
  assertion violations  + (if within scope of claim)
  acceptance   cycles   + (fairness disabled)
  invalid end states  - (disabled by never claim)

State-vector 28 byte, depth reached 52, errors: 1
       33 states, stored
        6 states, matched
       39 transitions (= stored+matched)
        0 atomic steps
hash conflicts:         0 (resolved)

Stats on memory usage (in Megabytes):
    0.002 equivalent memory usage for states (stored*(State-vector + overhead))
    0.287 actual memory usage for states
  128.000 memory used for hash table (-w24)
 5340.576 memory used for DFS stack (-m100000000)
 5468.772 total actual memory usage



pan: elapsed time 0 seconds
*/




