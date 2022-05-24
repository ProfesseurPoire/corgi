//#include <corgi/test/test.h>
//#include <corgi/containers/Tree.h>
//
//#include <Component.h>
//
//using namespace corgi::test;
//using namespace corgi;
//
//class TestEmptyTree : public Test
//{
//public:
//
////Lifecycle 
//
//    TestEmptyTree()=default;
//    TestEmptyTree(const TestEmptyTree& other)=default;
//    TestEmptyTree(TestEmptyTree&& other)=default;
//    virtual ~TestEmptyTree()=default;
//
//    TestEmptyTree& operator=(const TestEmptyTree& other)=default;
//    TestEmptyTree& operator=(TestEmptyTree&& other)=default;
//
//protected:
//
//    Tree<Component> tree;
//};
//
//TEST_F(TestEmptyTree, emplace_back_size_equals_one)
//{
//    tree.emplace_back();
//    assert_that(tree.size(), equals(1));
//}
//
//TEST_F(TestEmptyTree, empty_equals_true)
//{
//    assert_that(tree.empty(), equals(true));
//}
//
//TEST_F(TestEmptyTree, emplace_back_node_value_equals_3)
//{
//    // Action
//    auto& a = tree.emplace_back();
//
//    // Assert
//    //assert_that(a.get().number, equals(3));
//    assert_that(tree.size(), equals(1));
//    assert_that(tree.empty(), equals(false));
//}
//
//TEST_F(TestEmptyTree, children_emplace_parameter_pack)
//{
//    // Action
//    auto& node = tree.emplace_back(14);
//
//    // Assert
//    //assert_that(node.get().number, equals(14));
//    assert_that(tree.size(), equals(1));
//    assert_that(tree.empty(), equals(false));
//}