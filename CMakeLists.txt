cmake_minimum_required(VERSION 3.19.0)
project(stella-interpreter VERSION 0.1.0)

set (Stella_Syntax
  src/Stella/Absyn.H
  src/Stella/Absyn.C
  src/Stella/Parser.H
  src/Stella/Parser.C
  src/Stella/ParserError.H
  src/Stella/Printer.H
  src/Stella/Printer.C
  src/Stella/Buffer.H
  src/Stella/Buffer.C
  src/Stella/Bison.H
  src/Stella/Lexer.C
)

set (SRCS
  src/Interpret.cpp
  src/Eval.h
  src/Eval.cpp
  src/TypeCheck.h
  src/TypeCheck.cpp
  src/VisitTypeCheck.h
  src/VisitTypeCheck.cpp
)

add_executable(stella-interpreter ${Stella_Syntax} ${SRCS})
set_property(TARGET stella-interpreter PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(CTest)

add_test(NAME applying-non-function-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/applying-non-function-1.stella)
add_test(NAME applying-non-function-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/applying-non-function-2.stella)
add_test(NAME applying-non-function-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/applying-non-function-3.stella)
add_test(NAME argument-type-mismatch-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/argument-type-mismatch-1.stella)
add_test(NAME argument-type-mismatch-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/argument-type-mismatch-2.stella)
add_test(NAME argument-type-mismatch-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/argument-type-mismatch-3.stella)
add_test(NAME bad-if-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-if-1.stella)
add_test(NAME bad-if-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-if-2.stella)
add_test(NAME bad-succ-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-succ-1.stella)
add_test(NAME bad-succ-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-succ-2.stella)
add_test(NAME bad-succ-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-succ-3.stella)
add_test(NAME shadowed-variable-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/shadowed-variable-1.stella)
add_test(NAME undefined-variable-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/undefined-variable-1.stella)
add_test(NAME undefined-variable-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/undefined-variable-2.stella)
add_test(NAME bad-squares-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-squares-1.stella)
add_test(NAME bad-squares-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-squares-2.stella)
add_test(NAME bad-let-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-let-1.stella)
add_test(NAME bad-let-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-let-2.stella)
add_test(NAME bad-let-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-let-3.stella)
add_test(NAME bad-let-4 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-let-4.stella)
add_test(NAME bad-let-5 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-let-5.stella)
add_test(NAME bad-let-6 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-let-6.stella)
add_test(NAME bad-let-7 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-let-7.stella)
add_test(NAME 145 COMMAND stella-interpreter typecheck ../tests/well-typed/145.stella)
add_test(NAME 147 COMMAND stella-interpreter typecheck ../tests/well-typed/147.stella)
add_test(NAME bad-tuples-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-tuples-1.stella)
add_test(NAME bad-tuples-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-tuples-2.stella)
add_test(NAME bad-tuples-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-tuples-3.stella)
add_test(NAME tuples-1 COMMAND stella-interpreter typecheck ../tests/well-typed/tuples-1.stella)
add_test(NAME funcxxx COMMAND stella-interpreter typecheck ../tests/ill-typed/funcxxx.stella)
add_test(NAME bad-anonymous-function COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-anonymous-function.stella)
add_test(NAME bad-pair COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-pair.stella)

add_test(NAME bad-sum-types-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-1.stella)
add_test(NAME bad-sum-types-10 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-10.stella)
add_test(NAME bad-sum-types-11 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-11.stella)
add_test(NAME bad-sum-types-12 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-12.stella)
add_test(NAME bad-sum-types-13 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-13.stella)
add_test(NAME bad-sum-types-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-2.stella)
add_test(NAME bad-sum-types-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-3.stella)
add_test(NAME bad-sum-types-4 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-4.stella)
add_test(NAME bad-sum-types-5 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-5.stella)
add_test(NAME bad-sum-types-6 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-6.stella)
add_test(NAME bad-sum-types-7 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-7.stella)
add_test(NAME bad-sum-types-8 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-8.stella)
add_test(NAME bad-sum-types-9 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-sum-types-9.stella)
add_test(NAME good-anonymous-function COMMAND stella-interpreter typecheck ../tests/well-typed/good-anonymous-function.stella)
add_test(NAME good-pair COMMAND stella-interpreter typecheck ../tests/well-typed/good-pair.stella)
add_test(NAME sum-types-1 COMMAND stella-interpreter typecheck ../tests/well-typed/sum-types-1.stella)
add_test(NAME sum-types-2 COMMAND stella-interpreter typecheck ../tests/well-typed/sum-types-2.stella)
add_test(NAME sum-types-3 COMMAND stella-interpreter typecheck ../tests/well-typed/sum-types-3.stella)

add_test(NAME ambiguous-list-type-isempty COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-list-type-isempty.stella)
add_test(NAME bad-lists-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-lists-1.stella)
add_test(NAME bad-lists-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-lists-2.stella)
add_test(NAME bad-variants-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-variants-1.stella)
add_test(NAME not-a-list-head COMMAND stella-interpreter typecheck ../tests/ill-typed/not-a-list-head.stella)
add_test(NAME not-a-list-isempty COMMAND stella-interpreter typecheck ../tests/ill-typed/not-a-list-isempty.stella)
add_test(NAME not-a-list-tail COMMAND stella-interpreter typecheck ../tests/ill-typed/not-a-list-tail.stella)
add_test(NAME unexepected-list-unit COMMAND stella-interpreter typecheck ../tests/ill-typed/unexepected-list-unit.stella)
add_test(NAME unexpected-empty-list COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-empty-list.stella)
add_test(NAME unexpected-list-if-condition COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-list-if-condition.stella)
add_test(NAME unexpected-list-let-bindings COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-list-let-bindings.stella)
add_test(NAME unexpected-list-natrec COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-list-natrec.stella)
add_test(NAME unexpected-list-pair COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-list-pair.stella)

add_test(NAME empty-list COMMAND stella-interpreter typecheck ../tests/well-typed/empty-list.stella)
add_test(NAME lists-1 COMMAND stella-interpreter typecheck ../tests/well-typed/lists-1.stella)
add_test(NAME lists-2 COMMAND stella-interpreter typecheck ../tests/well-typed/lists-2.stella)
add_test(NAME lists-3 COMMAND stella-interpreter typecheck ../tests/well-typed/lists-3.stella)

add_test(NAME bad-records-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-1.stella)
add_test(NAME bad-records-10 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-10.stella)
add_test(NAME bad-records-11 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-11.stella)
add_test(NAME bad-records-12 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-12.stella)
add_test(NAME bad-records-13 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-13.stella)
add_test(NAME bad-records-14 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-14.stella)
add_test(NAME bad-records-15 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-15.stella)
add_test(NAME bad-records-16 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-16.stella)
add_test(NAME bad-records-17 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-17.stella)
add_test(NAME bad-records-18 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-18.stella)
add_test(NAME bad-records-19 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-19.stella)
add_test(NAME bad-records-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-2.stella)
add_test(NAME bad-records-20 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-20.stella)
add_test(NAME bad-records-21 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-21.stella)
add_test(NAME bad-records-23 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-23.stella)
add_test(NAME bad-records-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-3.stella)
add_test(NAME bad-records-4 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-4.stella)
add_test(NAME bad-records-5 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-5.stella)
add_test(NAME bad-records-6 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-6.stella)
add_test(NAME bad-records-7 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-7.stella)
add_test(NAME bad-records-8 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-8.stella)
add_test(NAME bad-records-9 COMMAND stella-interpreter typecheck ../tests/ill-typed/bad-records-9.stella)
add_test(NAME records-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/records-3.stella)

add_test(NAME records-1 COMMAND stella-interpreter typecheck ../tests/well-typed/records-1.stella)
add_test(NAME records-2 COMMAND stella-interpreter typecheck ../tests/well-typed/records-2.stella)
add_test(NAME records-4 COMMAND stella-interpreter typecheck ../tests/well-typed/records-4.stella)
add_test(NAME records-5 COMMAND stella-interpreter typecheck ../tests/well-typed/records-5.stella)
add_test(NAME records-6 COMMAND stella-interpreter typecheck ../tests/well-typed/records-6.stella)
add_test(NAME records-7 COMMAND stella-interpreter typecheck ../tests/well-typed/records-7.stella)
add_test(NAME records-8 COMMAND stella-interpreter typecheck ../tests/well-typed/records-8.stella)

add_test(NAME ambiguous-reference-type-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-reference-type-1.stella)
add_test(NAME not-a-reference-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/not-a-reference-1.stella)
add_test(NAME not-a-reference-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/not-a-reference-2.stella)
add_test(NAME not-a-reference-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/not-a-reference-3.stella)
add_test(NAME not-a-reference-4 COMMAND stella-interpreter typecheck ../tests/ill-typed/not-a-reference-4.stella)
add_test(NAME not-a-reference-5 COMMAND stella-interpreter typecheck ../tests/ill-typed/not-a-reference-5.stella)
add_test(NAME unexpected-memory-address-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-memory-address-1.stella)
add_test(NAME unexpected-memory-address-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-memory-address-2.stella)
add_test(NAME unexpected-memory-address-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-memory-address-3.stella)
add_test(NAME unexpected-memory-address-4 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-memory-address-4.stella)
add_test(NAME unexpected-type-for-expression-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-1.stella)
add_test(NAME unexpected-type-for-expression-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-2.stella)
add_test(NAME unexpected-type-for-expression-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-3.stella)
add_test(NAME unexpected-type-for-expression-4 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-4.stella)

add_test(NAME refs-1 COMMAND stella-interpreter typecheck ../tests/well-typed/refs-1.stella)
add_test(NAME refs-2 COMMAND stella-interpreter typecheck ../tests/well-typed/refs-2.stella)
add_test(NAME refs-3 COMMAND stella-interpreter typecheck ../tests/well-typed/refs-3.stella)
add_test(NAME refs-4 COMMAND stella-interpreter typecheck ../tests/well-typed/refs-4.stella)

add_test(NAME panic COMMAND stella-interpreter typecheck ../tests/well-typed/panic.stella)

add_test(NAME panicill COMMAND stella-interpreter typecheck ../tests/ill-typed/panicill.stella)

add_test(NAME ambiguous-panic-type-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-panic-type-1.stella)
add_test(NAME ambiguous-panic-type-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-panic-type-2.stella)
add_test(NAME ambiguous-panic-type-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-panic-type-3.stella)
add_test(NAME ambiguous-panic-type-4 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-panic-type-4.stella)
add_test(NAME ambiguous-throw-type-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-throw-type-1.stella)
add_test(NAME ambiguous-throw-type-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-throw-type-2.stella)
add_test(NAME ambiguous-throw-type-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-throw-type-3.stella)
add_test(NAME exception-type-not-declared-throw COMMAND stella-interpreter typecheck ../tests/ill-typed/exception-type-not-declared-throw.stella)
add_test(NAME exception-type-not-declared-try-catch COMMAND stella-interpreter typecheck ../tests/ill-typed/exception-type-not-declared-try-catch.stella)
add_test(NAME exception-type-not-declared-try-with COMMAND stella-interpreter typecheck ../tests/ill-typed/exception-type-not-declared-try-with.stella)
add_test(NAME unexpected-type-for-expression-throw-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-throw-1.stella)
add_test(NAME unexpected-type-for-expression-throw-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-throw-2.stella)
add_test(NAME unexpected-type-for-expression-try-catch-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-try-catch-1.stella)
add_test(NAME unexpected-type-for-expression-try-catch-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-try-catch-2.stella)
add_test(NAME unexpected-type-for-expression-try-catch-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-try-catch-3.stella)
add_test(NAME unexpected-type-for-expression-try-with-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-try-with-1.stella)
add_test(NAME unexpected-type-for-expression-try-with-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-try-with-2.stella)
add_test(NAME unexpected-type-for-expression-try-with-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-for-expression-try-with-3.stella)
add_test(NAME panic-1 COMMAND stella-interpreter typecheck ../tests/well-typed/panic-1.stella)
add_test(NAME panic-2 COMMAND stella-interpreter typecheck ../tests/well-typed/panic-2.stella)
add_test(NAME panic-3 COMMAND stella-interpreter typecheck ../tests/well-typed/panic-3.stella)
add_test(NAME throw-1 COMMAND stella-interpreter typecheck ../tests/well-typed/throw-1.stella)
add_test(NAME throw-2 COMMAND stella-interpreter typecheck ../tests/well-typed/throw-2.stella)
add_test(NAME try-catch-1 COMMAND stella-interpreter typecheck ../tests/well-typed/try-catch-1.stella)
add_test(NAME try-catch-2 COMMAND stella-interpreter typecheck ../tests/well-typed/try-catch-2.stella)
add_test(NAME try-with-1 COMMAND stella-interpreter typecheck ../tests/well-typed/try-with-1.stella)
add_test(NAME try-with-2 COMMAND stella-interpreter typecheck ../tests/well-typed/try-with-2.stella)
add_test(NAME ambiguous-variant-type-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-variant-type-1.stella)
add_test(NAME ambiguous-variant-type-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-variant-type-2.stella)
add_test(NAME ambiguous-variant-type-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/ambiguous-variant-type-3.stella)
add_test(NAME non-exaustive-match-patterns-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/non-exaustive-match-patterns-1.stella)
add_test(NAME unexpected-pattern-match-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-pattern-match-1.stella)
add_test(NAME unexpected-pattern-match-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-pattern-match-2.stella)
add_test(NAME unexpected-type-of-expression-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-of-expression-1.stella)
add_test(NAME unexpected-type-of-expression-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-of-expression-2.stella)
add_test(NAME unexpected-type-of-expression-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-of-expression-3.stella)
add_test(NAME unexpected-type-of-expression-4 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-of-expression-4.stella)
add_test(NAME unexpected-type-of-expression-with-alternative-errors COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-type-of-expression-with-alternative-errors.stella)
add_test(NAME unexpected-variable-label-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-variable-label-1.stella)
add_test(NAME unexpected-variable-label-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-variable-label-2.stella)
add_test(NAME unexpected-variant-1 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-variant-1.stella)
add_test(NAME unexpected-variant-2 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-variant-2.stella)
add_test(NAME unexpected-variant-3 COMMAND stella-interpreter typecheck ../tests/ill-typed/unexpected-variant-3.stella)
add_test(NAME variants-1 COMMAND stella-interpreter typecheck ../tests/well-typed/variants-1.stella)
add_test(NAME variants-2 COMMAND stella-interpreter typecheck ../tests/well-typed/variants-2.stella)
add_test(NAME variants-3 COMMAND stella-interpreter typecheck ../tests/well-typed/variants-3.stella)

set_tests_properties(
  ambiguous-variant-type-1
  ambiguous-variant-type-2
  ambiguous-variant-type-3
  non-exaustive-match-patterns-1
  unexpected-pattern-match-1
  unexpected-pattern-match-2
  unexpected-type-of-expression-1
  unexpected-type-of-expression-2
  unexpected-type-of-expression-3
  unexpected-type-of-expression-4
  unexpected-type-of-expression-with-alternative-errors
  unexpected-variable-label-1
  unexpected-variable-label-2
  unexpected-variant-1
  unexpected-variant-2
  unexpected-variant-3
  ambiguous-panic-type-1
  ambiguous-panic-type-2
  ambiguous-panic-type-3
  ambiguous-panic-type-4
  ambiguous-throw-type-1
  ambiguous-throw-type-2
  ambiguous-throw-type-3
  exception-type-not-declared-throw
  exception-type-not-declared-try-catch
  exception-type-not-declared-try-with
  unexpected-type-for-expression-throw-1
  unexpected-type-for-expression-throw-2
  unexpected-type-for-expression-try-catch-1
  unexpected-type-for-expression-try-catch-2
  unexpected-type-for-expression-try-catch-3
  unexpected-type-for-expression-try-with-1
  unexpected-type-for-expression-try-with-2
  unexpected-type-for-expression-try-with-3
  panicill
  ambiguous-reference-type-1
  not-a-reference-1
  not-a-reference-2
  not-a-reference-3
  not-a-reference-4
  not-a-reference-5
  unexpected-memory-address-1
  unexpected-memory-address-2
  unexpected-memory-address-3
  unexpected-memory-address-4
  unexpected-type-for-expression-1
  unexpected-type-for-expression-2
  unexpected-type-for-expression-3
  unexpected-type-for-expression-4
  records-3
  bad-records-1
  bad-records-2
  bad-records-3
  bad-records-4
  bad-records-5
  bad-records-6
  bad-records-7
  bad-records-8
  bad-records-9
  bad-records-10
  bad-records-11
  bad-records-12
  bad-records-13
  bad-records-14
  bad-records-15
  bad-records-16
  bad-records-17
  bad-records-18
  bad-records-19
  bad-records-20
  bad-records-21
  bad-records-23
  ambiguous-list-type-isempty
  bad-lists-1
  bad-lists-2
  bad-variants-1
  not-a-list-head
  not-a-list-isempty
  not-a-list-tail
  unexepected-list-unit
  unexpected-empty-list
  unexpected-list-if-condition
  unexpected-list-let-bindings
  unexpected-list-natrec
  unexpected-list-pair

  bad-sum-types-1
  bad-sum-types-2
  bad-sum-types-3
  bad-sum-types-4
  bad-sum-types-5
  bad-sum-types-6
  bad-sum-types-7
  bad-sum-types-8
  bad-sum-types-9
  bad-sum-types-10
  bad-sum-types-11
  bad-sum-types-12
  bad-sum-types-13
  bad-pair
  bad-anonymous-function
  bad-squares-1
  bad-squares-2
  applying-non-function-1
  applying-non-function-2
  applying-non-function-3
  argument-type-mismatch-1
  argument-type-mismatch-2
  argument-type-mismatch-3
  bad-if-1
  bad-if-2
  bad-succ-1
  bad-succ-2
  bad-succ-3
  shadowed-variable-1
  undefined-variable-1
  undefined-variable-2
  bad-let-1
  bad-let-2
  bad-let-3
  bad-let-4
  bad-let-5
  bad-let-6
  bad-let-7
  bad-let-7
  bad-tuples-1
  bad-tuples-2
  bad-tuples-3
  funcxxx
  PROPERTIES WILL_FAIL TRUE)

add_test(NAME factorial COMMAND stella-interpreter typecheck ../tests/well-typed/factorial.stella)
add_test(NAME increment_twice COMMAND stella-interpreter typecheck ../tests/well-typed/increment_twice.stella)
add_test(NAME higher-order-1 COMMAND stella-interpreter typecheck ../tests/well-typed/higher-order-1.stella)
add_test(NAME logical-operators COMMAND stella-interpreter typecheck ../tests/well-typed/logical-operators.stella)
add_test(NAME squares COMMAND stella-interpreter typecheck ../tests/well-typed/squares.stella)
