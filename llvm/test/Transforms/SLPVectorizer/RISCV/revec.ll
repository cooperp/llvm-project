; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -mtriple=riscv64 -mcpu=sifive-x280 -passes=slp-vectorizer -S -slp-revec -slp-max-reg-size=1024 -slp-threshold=-100 %s | FileCheck %s

define i32 @test() {
; CHECK-LABEL: @test(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    br label [[IF_END_I87:%.*]]
; CHECK:       if.end.i87:
; CHECK-NEXT:    [[TMP0:%.*]] = call <4 x i32> @llvm.masked.gather.v4i32.v4p0(<4 x ptr> getelementptr (i32, <4 x ptr> <ptr inttoptr (i64 64036 to ptr), ptr inttoptr (i64 64036 to ptr), ptr inttoptr (i64 64064 to ptr), ptr inttoptr (i64 64064 to ptr)>, <4 x i64> <i64 0, i64 1, i64 0, i64 1>), i32 4, <4 x i1> <i1 true, i1 true, i1 true, i1 true>, <4 x i32> poison)
; CHECK-NEXT:    [[TMP2:%.*]] = call <4 x i32> @llvm.vector.insert.v4i32.v2i32(<4 x i32> poison, <2 x i32> zeroinitializer, i64 2)
; CHECK-NEXT:    [[TMP3:%.*]] = shufflevector <4 x i32> [[TMP0]], <4 x i32> [[TMP2]], <4 x i32> <i32 0, i32 1, i32 6, i32 7>
; CHECK-NEXT:    switch i32 0, label [[SW_BB509_I:%.*]] [
; CHECK-NEXT:      i32 1, label [[SW_BB509_I]]
; CHECK-NEXT:      i32 0, label [[IF_THEN458_I:%.*]]
; CHECK-NEXT:    ]
; CHECK:       if.then458.i:
; CHECK-NEXT:    br label [[SW_BB509_I]]
; CHECK:       sw.bb509.i:
; CHECK-NEXT:    [[TMP4:%.*]] = phi <4 x i32> [ [[TMP0]], [[IF_THEN458_I]] ], [ [[TMP3]], [[IF_END_I87]] ], [ [[TMP3]], [[IF_END_I87]] ]
; CHECK-NEXT:    ret i32 0
;
entry:
  %getelementptr0 = getelementptr i8, ptr null, i64 64036
  %getelementptr1 = getelementptr i8, ptr null, i64 64064
  br label %if.end.i87

if.end.i87:                                       ; preds = %entry
  %0 = load <2 x i32>, ptr %getelementptr0, align 4
  %1 = load <2 x i32>, ptr %getelementptr1, align 8
  switch i32 0, label %sw.bb509.i [
  i32 1, label %sw.bb509.i
  i32 0, label %if.then458.i
  ]

if.then458.i:                                     ; preds = %if.end.i87
  br label %sw.bb509.i

sw.bb509.i:                                       ; preds = %if.then458.i, %if.end.i87, %if.end.i87
  %4 = phi <2 x i32> [ %0, %if.then458.i ], [ %0, %if.end.i87 ], [ %0, %if.end.i87 ]
  %5 = phi <2 x i32> [ %1, %if.then458.i ], [ zeroinitializer, %if.end.i87 ], [ zeroinitializer, %if.end.i87 ]
  ret i32 0
}
