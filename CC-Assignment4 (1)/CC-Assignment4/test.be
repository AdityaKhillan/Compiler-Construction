/*
#def A 104
#def B A
#def C B+A
#ifdef A
let bcd = 15+15;
let abc = 123 * 20 + 20 + ( 22 / 2 ) - 3;
dbg abc + C;

dbg bcd;

#undef A
let A=5;
dbg A;


#def DEBU 10
let x=5;
//#undef DEBUG
#ifdef DEBUG
dbg x;
#elif DEBU
dbg 2*x;
#endif


#def X 
#def C 
#def S 20
#ifdef S
dbg S;
#elif B
let B=10;
dbg B;
#elif C
dbg C;
#else
dbg 20;
#endif


let a = 0;
let b = 10;
let c = 5;
a = b + c;
dbg a;

*/



//let a = 5;
//let b = 10;
//let c = 15;
//dbg a;
//let e : short = 40000;
//let a : short = 0;
//let b : int = 0;
//let c : int = 3 * 4;
//let d : int = 10 - 2 - 3;
//dbg a;
//dbg b;
//dbg c;
//dbg d;
//dbg e;
//let y : int = 5;
//let x : int = y + (5 + 6/6);
//dbg x;


//let r : long = 5;
//let s : short = 10;
//let t : long = r + s;
//dbg t;


/*
if a{
    let k : int = 1;
    dbg 100;
} 
else{
    let p : int = 2;
    dbg 200;
}
*/



//fun d (a : int, b : long) : long {let s : int = 2; let u : int = 3;};



//a = a ? b : c;
//b = b ? b : b;
//a = b + c;
//dbg a;

/*

let cond : int = 15;
if 0{
    let cond : short = 100;
    dbg cond;
} else {
    let cond : long = 50000;
    dbg cond;
}

*/

fun add(a : int, b : int) : int {
    dbg 50;
}

fun main() : int{
    let z : int = 14;
    dbg z;
    let cond : int = 15;
    if 0{
        let cond : short = 100;
        dbg cond;
    } else {
        let cond : long = 50000;
        dbg cond;
    }
    let a : long  = 1;
    let b : int  = 1;
    let c : long  = a + b;
    dbg c;
    let e : int = 1 + 2 + 3;
}



