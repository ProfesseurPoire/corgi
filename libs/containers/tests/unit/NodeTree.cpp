#include <corgi/test/test.h>
#include <corgi/containers/Node.h>

using namespace corgi::test;
using namespace corgi;

class TestEmptyNode : public Test
{
public:

protected:

    Node<int> node= Node<int>(10);
};

TEST_F(TestEmptyNode, empty_should_return_false_after_construction)
{
    assert_that(node.empty(), equals(true));
}

TEST_F(TestEmptyNode, size_should_return_0_after_construction)
{
    assert_that(node.size(), equals(0));
}

TEST_F(TestEmptyNode, parent_bool_operator_should_return_false_after_construction)
{
    assert_that(node.parent(), equals(nullptr));
}

TEST_F(TestEmptyNode, empty_should_return_true_after_adding_one_child)
{
    node.children().emplace_back();

    assert_that(node.empty(), equals(false));
}

TEST_F(TestEmptyNode, size_should_return_1_after_adding_one_child)
{
    node.children().emplace_back();

    assert_that(node.size(), equals(1));
}

TEST_F(TestEmptyNode, added_child_parent_operator_function_should_return_true)
{
    auto& child = node.emplace_back();

    assert_that(child.parent(), non_equals(nullptr));
}

TEST_F(TestEmptyNode, add_child_then_clear_empty_should_return_true)
{
    node.emplace_back();
    node.clear();

    assert_that(node.empty(), equals(true));
}

TEST_F(TestEmptyNode, add_child_then_clear_then_size_should_return_0)
{
    node.emplace_back();
    node.clear();

    assert_that(node.size(), equals(0));
}

TEST_F(TestEmptyNode, add_2_child_then_pop_back_then_size_shoud_return_1)
{
    node.emplace_back();
    node.emplace_back();

    node.pop_back();

    assert_that(node.size(), equals(1));
}

TEST_F(TestEmptyNode, construct_node_with_parent)
{
    // If I allow this, when Node n2 is destroyed before
    // n1, then everything breaks
    Node<int> n2;

    // Copy or move, but no pointer here 
    auto copiedNode = node.attach(n2);

    assert_that(node.size(), equals(1));
    assert_that(node.empty(), equals(false));
    assert_that(copiedNode.parent(), equals(&node));
}


TEST_F(TestEmptyNode, testMoveAttach)
{
    auto& n1 = node.emplace_back(1);
    auto& n2 = n1.emplace_back(2);
    auto& n3 = n1.emplace_back(3);
    auto& n4 = node.emplace_back(4);

    n4.attach(std::move(node.children()[0]));

    //assert_that(n1.parent_, equals(nullptr));
    //assert_that(n2.parent_, equals(nullptr));
    //assert_that(n3.parent_, equals(nullptr));

    //assert_that(n1.children_.empty(), equals(true));
    //assert_that(n2.children_.empty(), equals(true));
    //assert_that(n3.children_.empty(), equals(true));
    

    assert_that(node.size(), equals(1));
    assert_that(node.at(0).size(), equals(1));
}

class FooNode 
{
    public:

        float val;

    private:
};

class TestInheritedNode : public Test
{
public:

protected:

    Node<FooNode> node;

};

TEST_F(TestInheritedNode, iteration_test)
{
    node.emplace_back(4);
    node.emplace_back(3);
    node.emplace_back(2);

    int index=0;

    for(auto& n : node)
    {
        if(index==0)
        {
            assert_that(n->val, equals(4));
        }
        if(index==1)
        {
            assert_that(n->val, equals(3));
        }
        if(index==2)
        {
            assert_that(n->val, equals(2));
        }
        index++;
    }
}

TEST_F(TestInheritedNode, iteration_depth_test)
{
    auto& a = node.emplace_back(5);
    a.emplace_back(1);

    node.emplace_back(3);
    node.emplace_back(2);

    node.mode(Node<FooNode>::Iterator::Mode::DepthFirst);

    int index=0;

    for(auto& n : node)
    {

        if(index==0)
        {
            assert_that(n->val, equals(5));
        }

        if(index==1)
        {
            assert_that(n->val, equals(1));
        }

        if(index==2)
        {
            assert_that(n->val, equals(3));
        }

        if(index==3)
        {
            assert_that(n->val, equals(2));
        }
        index++;
    }
}
//
//class TestIterationBreath : public Test
//{
//public:
//
//    void set_up() override
//    {
//        auto& a = node.emplace_back<FooNode>(5);
//        a.emplace_back<FooNode>(1);
//
//        node.emplace_back<FooNode>(3);
//        node.emplace_back<FooNode>(2);
//
//        node.mode(Node::Iterator::Mode::BreadthFirst);
//    }
//
//protected:
//
//    FooNode node;
//};
//
//TEST_F(TestIterationBreath, erase_stuff)
//{
//    node.remove_child(0);
//
//    assert_that( ((FooNode*)(node.children()[0].get()))->val, equals(3));
//}
//
//TEST_F(TestIterationBreath, iteration_breadth_test)
//{
//    int index=0;
//
//    for(auto n : node)
//    {
//        auto nn = dynamic_cast<FooNode*>(n);
//
//        if(index==0)
//        {
//            assert_that(nn->val, equals(5));
//        }
//
//        if(index==1)
//        {
//            assert_that(nn->val, equals(3));
//        }
//
//        if(index==2)
//        {
//            assert_that(nn->val, equals(2));
//        }
//
//        if(index==3)
//        {
//            assert_that(nn->val, equals(1));
//        }
//        index++;
//    }
//}