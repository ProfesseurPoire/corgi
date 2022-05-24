//#include <corgi/test/test.h>
//#include <corgi/String.h>
//
//using namespace corgi::test;
//using namespace corgi;
//
//struct StringTesting : public Test
//{
//    String str = "cool";
//    String emptyString;
//};
//
//TEST_F(StringTesting, constructor)
//{
//    assert_that(str[0], equals('c'));
//    assert_that(str[1], equals('o'));
//    assert_that(str[2], equals('o'));
//    assert_that(str[3], equals('l'));
//    assert_that(str[4], equals('\0'));
//
//    assert_that(str.size(), equals(4));
//}
//
//TEST_F(StringTesting, empty)
//{
//    assert_that(str.empty(), equals(false));
//    assert_that(emptyString.empty(), equals(true));
//}
//
//TEST_F(StringTesting, size)
//{
//    assert_that(str.size(), equals(4));
//}
//
//TEST_F(StringTesting, first)
//{
//    assert_that(str.first(), equals('c'));
//}
//
//TEST_F(StringTesting, last)
//{
//    assert_that(str.last(), equals('l'));
//}
//
//TEST_F(StringTesting, copy)
//{
//    emptyString = str;
//
//    assert_that(emptyString[0], equals('c'));
//    assert_that(emptyString[1], equals('o'));
//    assert_that(emptyString[2], equals('o'));
//    assert_that(emptyString[3], equals('l'));
//    assert_that(emptyString[4], equals('\0'));
//
//    assert_that(emptyString.size(), equals(4));
//}
//
//TEST_F(StringTesting, AdditionOperator)
//{
//    String result = str + "hi";
//
//    assert_that(result[0], equals('c'));
//    assert_that(result[1], equals('o'));
//    assert_that(result[2], equals('o'));
//    assert_that(result[3], equals('l'));
//    assert_that(result[4], equals('h'));
//    assert_that(result[5], equals('i'));
//    assert_that(result[6], equals('\0'));
//
//    assert_that(result.size(), equals(6));
//}
//
//TEST_F(StringTesting, AssignmentAdditionOperator)
//{
//    str += "hi";
//
//    assert_that(str[0], equals('c'));
//    assert_that(str[1], equals('o'));
//    assert_that(str[2], equals('o'));
//    assert_that(str[3], equals('l'));
//    assert_that(str[4], equals('h'));
//    assert_that(str[5], equals('i'));
//    assert_that(str[6], equals('\0'));
//
//    assert_that(str.size(), equals(6));
//}
//
//
//TEST_F(StringTesting, substr)
//{
//    String result = str.substr(1, 2);
//
//    assert_that(result.size(), equals(2));
//
//    assert_that(result[0], equals('o'));
//    assert_that(result[1], equals('o'));
//
//
//    result = str.substr(2);
//
//    assert_that(result.size(), equals(2));
//    assert_that(result[0], equals('o'));
//    assert_that(result[1], equals('l'));
//    assert_that(result[2], equals('\0'));
//}