// RUN: %clang_cc1 -triple x86_64-apple-darwin -emit-llvm -o - %s | FileCheck %s

typedef __SIZE_TYPE__ size_t;

int printf(const char * _Format, ...);
char *strcpy(char *s1, const char *s2);

struct S {
  S();
  int a[4];
};

// CHECK: define {{.*}} @_Z5test1PKc
// CHECK: %a = alloca [1000 x i8], align {{.*}}, !stack-protector ![[A:.*]]
void test1(const char *msg) {
  __attribute__((stack_protector_ignore))
  char a[1000];
  strcpy(a, msg);
  printf("%s\n", a);
}

// CHECK: define {{.*}} @_Z5test2
// CHECK-NOT: %b = alloca [1000 x i8], align {{.*}}, !stack-protector
void test2(const char *msg) {
  char b[1000];
  strcpy(b, msg);
  printf("%s\n", b);
}

// CHECK: define {{.*}} @_Z5test3v
// CHECK: %retval = alloca %struct.S, align {{.*}}, !stack-protector ![[A:.*]]
S test3() {
  __attribute__((stack_protector_ignore))
  S s;
  return s;
}

// CHECK: define {{.*}} @_Z5test4b
// CHECK: %retval = alloca %struct.S, align {{.*}}, !stack-protector ![[A:.*]]
// CHECK: call void @_ZN1SC1Ev
S test4(bool b) {
  __attribute__((stack_protector_ignore))
  S s;
  if ( b )
    return s;
  else
    return s;
}

// CHECK: define {{.*}} @_Z5test5b
// CHECK: %retval = alloca %struct.S, align {{.*}}
// CHECK-NOT: stack-protector
// CHECK: %s1 = alloca %struct.S, align {{.*}}, !stack-protector ![[A:.*]]
// CHECK: %s2 = alloca %struct.S, align {{.*}}
// CHECK-NOT: stack-protector
// CHECK: call void @_ZN1SC1Ev
// CHECK: call void @_ZN1SC1Ev
S test5(bool b) {
  __attribute__((stack_protector_ignore))
  S s1;
  S s2;
  if ( b )
    return s1;
  else
    return s2;
}

// CHECK: ![[A]] = !{i32 0}
