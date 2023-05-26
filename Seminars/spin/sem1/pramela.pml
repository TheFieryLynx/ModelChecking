bool flag[2] = 0; // оба получат 0, можно false, можно {0, 0}
byte turn = 0;

active [2] proctype P() { // 2 - количество процессов
   byte i = _pid;
   assert(0 <= i && i <= 1);
   NCS: printf("NCS %d\n", i); // можно пропустить
   SET: flag[i] = 1; turn = i;
   TST: !((flag[1 - i] == 1) && (turn == i));
   CRS: printf("CRS %d\n", i);
   RST: flag[i] = 0; goto NCS;
}

ltl safety {
    [] (!(P[0]@CRS && P[1]@CRS)) // глобально взаимное исключение процессов    
}

ltl liveness {
    [] (P[0]@SET -> <>(P[0]@CRS))
}
