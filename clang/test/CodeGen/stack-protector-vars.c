// RUN: %clang_cc1 -emit-llvm -o - %s | FileCheck %s

typedef __SIZE_TYPE__ size_t;

int printf(const char * _Format, ...);
char *strcpy(char *s1, const char *s2);

// CHECK: define {{.*}}void @test1
// CHECK: %a = alloca [1000 x i8], align 1, !stack-protector ![[A:.*]]
void test1(const char *msg) {
  __attribute__((no_stack_protector))
  char a[1000];
  strcpy(a, msg);
  printf("%s\n", a);
}

// CHECK: define {{.*}}void @test2
// CHECK-NOT: %b = alloca [1000 x i8], align 1, !stack-protector
void test2(const char *msg) {
  char b[1000];
  strcpy(b, msg);
  printf("%s\n", b);
}

// CHECK: ![[A]] = !{i32 0}