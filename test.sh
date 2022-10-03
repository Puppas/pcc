#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  echo "$input" | ./build/pcc -o ./build/tmp.s - || exit
  gcc -static -o ./build/tmp ./build/tmp.s
  ./build/tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 21 'int main() { return 5+20-4; }'
assert 41 'int main() { return  12 + 34 - 5 ; }'
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 4 'int main() { return (3+5)/2; }'
assert 10 'int main() { return -10+20; }'
assert 10 'int main() { return - -10; }'
assert 10 'int main() { return - - +10; }'

assert 0 'int main() { return 0==1; }'
assert 1 'int main() { return 42==42; }'
assert 1 'int main() { return 0!=1; }'
assert 0 'int main() { return 42!=42; }'

assert 1 'int main() { return 0<1; }'
assert 0 'int main() { return 1<1; }'
assert 0 'int main() { return 2<1; }'
assert 1 'int main() { return 0<=1; }'
assert 1 'int main() { return 1<=1; }'
assert 0 'int main() { return 2<=1; }'

assert 1 'int main() { return 1>0; }'
assert 0 'int main() { return 1>1; }'
assert 0 'int main() { return 1>2; }'
assert 1 'int main() { return 1>=0; }'
assert 1 'int main() { return 1>=1; }'
assert 0 'int main() { return 1>=2; }'

assert 3 'int main() { int a; a=3; return a; }'
assert 3 'int main() { int a=3; return a; }'
assert 8 'int main() { int a=3; int z=5; return a+z; }'

assert 1 'int main() { return 1; 2; 3; }'
assert 2 'int main() { 1; return 2; 3; }'
assert 3 'int main() { 1; 2; return 3; }'

assert 3 'int main() { int a=3; return a; }'
assert 8 'int main() { int a=3; int z=5; return a+z; }'
assert 6 'int main() { int a; int b; a=b=3; return a+b; }'
assert 3 'int main() { int foo=3; return foo; }'
assert 8 'int main() { int foo123=3; int bar=5; return foo123+bar; }'

assert 3 'int main() { if (0) return 2; return 3; }'
assert 3 'int main() { if (1-1) return 2; return 3; }'
assert 2 'int main() { if (1) return 2; return 3; }'
assert 2 'int main() { if (2-1) return 2; return 3; }'

assert 55 'int main() { int i=0; int j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }'
assert 3 'int main() { for (;;) return 3; return 5; }'

assert 10 'int main() { int i=0; while(i<10) i=i+1; return i; }'

assert 3 'int main() { {1; {2;} return 3;} }'
assert 5 'int main() { ;;; return 5; }'

assert 10 'int main() { int i=0; while(i<10) i=i+1; return i; }'
assert 55 'int main() { int i=0; int j=0; while(i<=10) {j=i+j; i=i+1;} return j; }'

assert 3 'int main() { int x=3; return *&x; }'
assert 3 'int main() { int x=3; int *y=&x; int **z=&y; return **z; }'
assert 5 'int main() { int x=3; int y=5; return *(&x+1); }'
assert 3 'int main() { int x=3; int y=5; return *(&y-1); }'
assert 5 'int main() { int x=3; int y=5; return *(&x-(-1)); }'
assert 5 'int main() { int x=3; int *y=&x; *y=5; return x; }'
assert 7 'int main() { int x=3; int y=5; *(&x+1)=7; return y; }'
assert 7 'int main() { int x=3; int y=5; *(&y-2+1)=7; return x; }'
assert 5 'int main() { int x=3; return (&x+2)-&x+3; }'
assert 8 'int main() { int x, y; x=3; y=5; return x+y; }'
assert 8 'int main() { int x=3, y=5; return x+y; }'


assert 32 'int main() { return ret32(); } int ret32() { return 32; }'
assert 7 'int main() { return add2(3,4); } int add2(int x, int y) { return x+y; }'
assert 1 'int main() { return sub2(4,3); } int sub2(int x, int y) { return x-y; }'
assert 55 'int main() { return fib(9); } int fib(int x) { if (x<=1) return 1; return fib(x-1) + fib(x-2); }'
assert 8 'int mul(int x, int y) { return x * y; } int main() { return mul(3 - 1, 4); }'

assert 3 'int main() { int x[2]; int *y=&x; *y=3; return *x; }'

assert 3 'int main() { int x[3]; *x=3; *(x+1)=4; *(x+2)=5; return *x; }'
assert 4 'int main() { int x[3]; *x=3; *(x+1)=4; *(x+2)=5; return *(x+1); }'
assert 5 'int main() { int x[3]; *x=3; *(x+1)=4; *(x+2)=5; return *(x+2); }'



assert 0 'int main() { int x[2][3]; int *y=x; *y=0; return **x; }'
assert 1 'int main() { int x[2][3]; int *y=x; *(y+1)=1; return *(*x+1); }'
assert 2 'int main() { int x[2][3]; int *y=x; *(y+2)=2; return *(*x+2); }'
assert 3 'int main() { int x[2][3]; int *y=x; *(y+3)=3; return **(x+1); }'
assert 4 'int main() { int x[2][3]; int *y=x; *(y+4)=4; return *(*(x+1)+1); }'
assert 5 'int main() { int x[2][3]; int *y=x; *(y+5)=5; return *(*(x+1)+2); }'


assert 3 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *x; }'
assert 4 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *(x+1); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *(x+2); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *(x+2); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; 2[x]=5; return *(x+2); }'

assert 0 'int main() { int x[2][3]; int *y=x; y[0]=0; return x[0][0]; }'
assert 1 'int main() { int x[2][3]; int *y=x; y[1]=1; return x[0][1]; }'
assert 2 'int main() { int x[2][3]; int *y=x; y[2]=2; return x[0][2]; }'
assert 3 'int main() { int x[2][3]; int *y=x; y[3]=3; return x[1][0]; }'
assert 4 'int main() { int x[2][3]; int *y=x; y[4]=4; return x[1][1]; }'
assert 5 'int main() { int x[2][3]; int *y=x; y[5]=5; return x[1][2]; }'

assert 4 'int main() { int x; return sizeof(x); }'
assert 4 'int main() { int x; return sizeof x; }'
assert 8 'int main() { int *x; return sizeof(x); }'
assert 16 'int main() { int x[4]; return sizeof(x); }'
assert 48 'int main() { int x[3][4]; return sizeof(x); }'
assert 16 'int main() { int x[3][4]; return sizeof(*x); }'
assert 4 'int main() { int x[3][4]; return sizeof(**x); }'
assert 5 'int main() { int x[3][4]; return sizeof(**x) + 1; }'
assert 5 'int main() { int x[3][4]; return sizeof **x + 1; }'
assert 4 'int main() { int x[3][4]; return sizeof(**x + 1); }'
assert 4 'int main() { int x=1; return sizeof(x=2); }'
assert 1 'int main() { int x=1; sizeof(x=2); return x; }'


assert 0 'int x; int main() { return x; }'
assert 3 'int x; int main() { x=3; return x; }'
assert 7 'int x; int y; int main() { x=3; y=4; return x+y; }'
assert 7 'int x, y; int main() { x=3; y=4; return x+y; }'
assert 0 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[0]; }'
assert 1 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[1]; }'
assert 2 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[2]; }'
assert 3 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[3]; }'

assert 4 'int x; int main() { return sizeof(x); }'
assert 16 'int x[4]; int main() { return sizeof(x); }'

assert 1 'int main() { char x=1; return x; }'
assert 1 'int main() { char x=1; char y=2; return x; }'
assert 2 'int main() { char x=1; char y=2; return y; }'

assert 1 'int main() { char x; return sizeof(x); }'
assert 10 'int main() { char x[10]; return sizeof(x); }'
assert 1 'int main() { return sub_char(7, 3, 3); } int sub_char(char a, char b, char c) { return a-b-c; }'


assert 0 'int main() { return ""[0]; }'
assert 1 'int main() { return sizeof(""); }'

assert 97 'int main() { return "abc"[0]; }'
assert 98 'int main() { return "abc"[1]; }'
assert 99 'int main() { return "abc"[2]; }'
assert 0 'int main() { return "abc"[3]; }'
assert 4 'int main() { return sizeof("abc"); }'
assert 2 'int main() { char a[3]; char *p = a; *p = 2; return a[0]; }'
assert 97 'int main() { char *p = "abc"; return p[0]; }'


assert 7 'int main() { return "\a"[0]; }'
assert 8 'int main() { return "\b"[0]; }'
assert 9 'int main() { return "\t"[0]; }'
assert 10 'int main() { return "\n"[0]; }'
assert 11 'int main() { return "\v"[0]; }'
assert 12 'int main() { return "\f"[0]; }'
assert 13 'int main() { return "\r"[0]; }'
assert 27 'int main() { return "\e"[0]; }'

assert 106 'int main() { return "\j"[0]; }'
assert 107 'int main() { return "\k"[0]; }'
assert 108 'int main() { return "\l"[0]; }'

assert 7 'int main() { return "\ax\ny"[0]; }'
assert 120 'int main() { return "\ax\ny"[1]; }'
assert 10 'int main() { return "\ax\ny"[2]; }'
assert 121 'int main() { return "\ax\ny"[3]; }'

assert 0 'int main() { return "\0"[0]; }'
assert 16 'int main() { return "\20"[0]; }'
assert 65 'int main() { return "\101"[0]; }'
assert 104 'int main() { return "\1500"[0]; }'
assert 0 'int main() { return "\x00"[0]; }'
assert 119 'int main() { return "\x77"[0]; }'
assert 165 'int main() { return "\xA5"[0]; }'
assert 255 'int main() { return "\x00ff"[0]; }'

assert 0 'int main() { return ({ 0; }); }'
assert 2 'int main() { return ({ 0; 1; 2; }); }'
assert 1 'int main() { ({ 0; return 1; 2; }); return 3; }'
assert 6 'int main() { return ({ 1; }) + ({ 2; }) + ({ 3; }); }'
assert 3 'int main() { return ({ int x=3; x; }); }'


assert 2 'int main() { /* return 1; */ return 2; }'
assert 2 'int main() { // return 1;
return 2; }'

assert 2 'int main() { int x=2; { int x=3; } return x; }'
assert 2 'int main() { int x=2; { int x=3; } { int y=4; return x; }}'
assert 3 'int main() { int x=2; { x=3; } return x; }'

assert 3 'int main() { return (1, 2, 3); }'
assert 5 'int main() { int i=2, j=3; (i=5,j)=6; return i; }'
assert 6 'int main() { int i=2, j=3; (i=5,j)=6; return j; }'

assert 1 'int main() { struct {int a; int b;} x; x.a=1; x.b=2; return x.a; }'
assert 2 'int main() { struct {int a; int b;} x; x.a=1; x.b=2; return x.b; }'

assert 1 'int main() { struct {char a; int b; char c;} x; x.a=1; x.b=2; x.c=3; return x.a; }'
assert 2 'int main() { struct {char a; int b; char c;} x; x.b=1; x.b=2; x.c=3; return x.b; }'
assert 3 'int main() { struct {char a; int b; char c;} x; x.a=1; x.b=2; x.c=3; return x.c; }'

assert 0 'int main() { struct {char a; char b;} x[3]; char *p=x; p[0]=0; return x[0].a; }'
assert 1 'int main() { struct {char a; char b;} x[3]; char *p=x; p[1]=1; return x[0].b; }'
assert 2 'int main() { struct {char a; char b;} x[3]; char *p=x; p[2]=2; return x[1].a; }'
assert 3 'int main() { struct {char a; char b;} x[3]; char *p=x; p[3]=3; return x[1].b; }'

assert 6 'int main() { struct {char a[3]; char b[5];} x; char *p=&x; x.a[0]=6; return p[0]; }'
assert 7 'int main() { struct {char a[3]; char b[5];} x; char *p=&x; x.b[0]=7; return p[3]; }'

assert 6 'int main() { struct { struct { char b; } a; } x; x.a.b=6; return x.a.b; }'

assert 4 'int main() { struct {int a;} x; return sizeof(x); }'
assert 8 'int main() { struct {int a; int b;} x; return sizeof(x); }'
assert 8 'int main() { struct {int a, b;} x; return sizeof(x); }'
assert 12 'int main() { struct {int a[3];} x; return sizeof(x); }'
assert 16 'int main() { struct {int a;} x[4]; return sizeof(x); }'
assert 24 'int main() { struct {int a[3];} x[2]; return sizeof(x); }'
assert 2 'int main() { struct {char a; char b;} x; return sizeof(x); }'
assert 8 'int main() { struct {char a; int b;} x; return sizeof(x); }'
assert 8 'int main() { struct {int a; char b;} x; return sizeof(x); }'
assert 0 'int main() { struct {} x; return sizeof(x); }'

assert 7 'int main() { int x; int y; char z; char *a=&y; char *b=&z; return b-a; }'
assert 1 'int main() { int x; char y; int z; char *a=&y; char *b=&z; return b-a; }'


assert 8 'int main() { struct t {int a; int b;} x; struct t y; return sizeof(y); }'
assert 8 'int main() { struct t {int a; int b;}; struct t y; return sizeof(y); }'
assert 2 'int main() { struct t {char a[2];}; { struct t {char a[4];}; } struct t y; return sizeof(y); }'
assert 3 'int main() { struct t {int x;}; int t=1; struct t y; y.x=2; return t+y.x; }'

assert 3 'int main() { struct t {char a;} x; struct t *y = &x; x.a=3; return y->a; }'
assert 3 'int main() { struct t {char a;} x; struct t *y = &x; y->a=3; return x.a; }'

assert 3 'int main() { struct {int a,b;} x,y; x.a=3; y=x; return y.a; }'
assert 7 'int main() { struct t {int a,b;}; struct t x; x.a=7; struct t y; struct t *z=&y; *z=x; return y.a; }'
assert 7 'int main() { struct t {int a,b;}; struct t x; x.a=7; struct t y, *p=&x, *q=&y; *q=*p; return y.a; }'
assert 5 'int main() { struct t {char a, b;} x, y; x.a=5; y=x; return y.a; }'

assert 3 'int main() { struct {int a,b;} x,y; x.a=3; y=x; return y.a; }'
assert 7 'int main() { struct t {int a,b;}; struct t x; x.a=7; struct t y; struct t *z=&y; *z=x; return y.a; }'
assert 7 'int main() { struct t {int a,b;}; struct t x; x.a=7; struct t y, *p=&x, *q=&y; *q=*p; return y.a; }'
assert 5 'int main() { struct t {char a, b;} x, y; x.a=5; y=x; return y.a; }'

assert 8 'int main() { struct t {int a; int b;} x; struct t y; return sizeof(y); }'
assert 8 'int main() { struct t {int a; int b;}; struct t y; return sizeof(y); }'

assert 8 'int main() { union { int a; char b[6]; } x; return sizeof(x); }'
assert 3 'int main() { union { int a; char b[4]; } x; x.a = 515; return x.b[0]; }'
assert 2 'int main() { union { int a; char b[4]; } x; x.a = 515; return x.b[1]; }'
assert 0 'int main() { union { int a; char b[4]; } x; x.a = 515; return x.b[2]; }'
assert 0 'int main() { union { int a; char b[4]; } x; x.a = 515; return x.b[3]; }'

assert 3 'int main() { union {int a,b;} x,y; x.a=3; y.a=5; y=x; return y.a; }'
assert 3 'int main() { union {struct {int a,b;} c;} x,y; x.c.b=3; y.c.b=5; y=x; return y.c.b; }'


echo OK
