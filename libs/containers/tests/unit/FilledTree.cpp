#include <corgi/test/test.h>

using namespace corgi::test;
using namespace corgi;

int main()
{
    try
    {
        auto result = corgi::test::run_all();
        return result;
    }
    catch(std::exception e )
    {
        std::cout<<e.what()<<std::endl;
        return 1;
    }
}
//
//class TestTree : public Test
//{
//    //  11 
//    //  |____ 21
//    //  |     |____ 31
//    //  |     |____ 32
//    //  |           |____41
//    //  |_____22
//    void set_up()override
//    {
//        node11 =   &tree.emplace_back(11);
//        node21 =   &node11->emplace_back(21);
//        node22 =   &node11->emplace_back(22);
//        node31 =   &node21->emplace_back(31);
//        node32 =   &node21->emplace_back(32);
//        node41 =   &node32->emplace_back(41); 
//    }
//
//protected:
//
//    int created_count_{0};
//
//    Tree<Component> tree;
//    Node<Component>* node11;
//    Node<Component>* node21;
//    Node<Component>* node22;
//    Node<Component>* node31;
//    Node<Component>* node32;
//    Node<Component>* node41;
//};
//
//TEST_F(TestTree, size)
//{
//    assert_that(tree.size(), equals(1));
//}
//
//TEST_F(TestTree, empty)
//{
//    assert_that(tree.empty(), equals(false));
//}
//
//TEST_F(TestTree, children_emplace)
//{
//    // Action
//    
//    auto& component = tree.emplace_back();
//
//    // Assert
//
//    /*assert_that(component.get().number, equals(3));
//    assert_that(tree.size(), equals(2));*/
//}
//
//TEST_F(TestTree, children_emplace_parameter_pack)
//{
//   /* auto& node = tree.emplace_back(14);
//    assert_that(node.get().number, equals(14));*/
//}
//
//TEST_F(TestTree, clear)
//{
//    // Action
//    
//    tree.clear();
//
//    // Assert
//
//    assert_that(tree.empty(), equals(true));
//    assert_that(tree.size(), equals(0));
//}
//
//TEST_F(TestTree, node_emplace)
//{
//    auto& node  = tree.emplace_back();
//    auto& child = node.emplace_back(15);
//   // assert_that(child.get().number, equals(15));
//}
//
//TEST_F(TestTree, node_size)
//{
//    auto& node = tree.emplace_back();
//    assert_that(node.size(), equals(0));
//    assert_that(node.is_leaf(), equals(true));
//    auto& child = node.emplace_back(15);
//    assert_that(node.size(), equals(1));
//    assert_that(node.is_leaf(),equals(false));
//}
//
//TEST_F(TestTree, node_parent)
//{
//    auto& node  = tree.emplace_back(10);
//    auto& child = node.emplace_back(20);
//
//    assert_that(child.parent(), non_equals(nullptr));
//   // assert_that(child.parent()->get().number, equals(10));
//}
//
//TEST_F(TestTree, node_clear)
//{
//    auto& node  = tree.emplace_back(10);
//    auto& child = node.emplace_back(20);
//
//    assert_that(child.parent(), non_equals(nullptr));
//    //assert_that(child.parent()->get().number, equals(10));
//
//    node.clear();
//
//    assert_that(node.is_leaf(), equals(true));
//}
//
//TEST_F(TestTree, tree_iterator)
//{
//    //  11 
//    //  |____ 21
//    //  |     |____ 31
//    //  |     |____ 32
//    //  |           |____41
//    //  |_____22
//
//    tree.iterator_mode(IteratorMode::DepthFirst);
//
//    int i=0;
//
//    for(auto& node : tree)
//    {
//        switch(i)
//        {
//        case 0:
//            assert_that(node.get().number, equals(11));
//        break;
//        case 1:
//            assert_that(node.get().number, equals(21));
//        break;
//        case 2:
//            assert_that(node.get().number, equals(31));
//        break;
//        case 3:
//            assert_that(node.get().number, equals(32));
//        break;
//        case 4:
//            assert_that(node.get().number, equals(41));
//        break;
//        case 5:
//            assert_that(node.get().number, equals(22));
//        break;
//        }
//        i++;
//    }
//
//    // just making sure we iterated the right amount of time 
//    assert_that(i, equals(6));  
//
//    tree.iterator_mode(IteratorMode::BreadthFirst);
//
//    //  11 
//    //  |____ 21
//    //  |     |____ 31
//    //  |     |____ 32
//    //  |           |____41
//    //  |_____22
//
//    i=0;
//    for(auto& node : tree)
//    {
//        switch(i)
//        {
//        case 0:
//            assert_that(node.get().number, equals(11));
//        break;
//        case 1:
//            assert_that(node.get().number, equals(21));
//        break;
//        case 2:
//            assert_that(node.get().number, equals(22));
//        break;
//        case 3:
//            assert_that(node.get().number, equals(31));
//        break;
//        case 4:
//            assert_that(node.get().number, equals(32));
//        break;
//        case 5:
//            assert_that(node.get().number, equals(41));
//        break;
//        }
//        i++;
//    }
//    assert_that(i, equals(6)); // Checks if we iterated the right amount of time 
//}
//
//
//TEST_F(TestTree, node_iterator)
//{
//    tree.iterator_mode(IteratorMode::DepthFirst);
//
//    int i=0;
//
//    //  11 
//    //  |____ 21
//    //  |     |____ 31
//    //  |     |____ 32
//    //  |           |____41
//    //  |_____22
//
//    for(auto& node : *node11)
//    {
//        switch(i)
//        {
//        case 0:
//            assert_that(node.get().number, equals(21));
//        break;
//        case 1:
//            assert_that(node.get().number, equals(31));
//        break;
//        case 2:
//            assert_that(node.get().number, equals(32));
//        break;
//        case 3:
//            assert_that(node.get().number, equals(41));
//        break;
//        case 4:
//            assert_that(node.get().number, equals(22));
//        break;
//        }
//        i++;
//    }
//    assert_that(i, equals(5)); // Checks if we iterated the right amount of time
//
//    node11->iterator_mode(IteratorMode::BreadthFirst);
//
//    //  11 
//    //  |____ 21
//    //  |     |____ 31
//    //  |     |____ 32
//    //  |           |____41
//    //  |_____22
//
//    i=0;
//    for(auto& node : *node11)
//    {
//        switch(i)
//        {
//        case 0:
//            assert_that(node.get().number, equals(21));
//        break;
//        case 1:
//            assert_that(node.get().number, equals(22));
//        break;
//        case 2:
//            assert_that(node.get().number, equals(31));
//        break;
//        case 3:
//            assert_that(node.get().number, equals(32));
//        break;
//        case 4:
//            assert_that(node.get().number, equals(41));
//        break;
//        }
//        i++;
//    }
//    // just making sure we iterated the right amount of time 
//    assert_that(i, equals(5)); 
//}
////
////TEST_F(TestTree, Remove_RootNode_EmptyTheTree)
////{
////	
////    containers::remove(tree.data(), 0);
////
////    assert_that(tree.empty(), equals(true));
////    assert_that(tree.size(), equals(0));
////}
////
////TEST_F(TestTree, Remove_RemoveSecondeNode_KeepRootNode)
////{
////    containers::remove(node11->data(),0);
////
////    assert_that(tree.empty(),equals(false));
////    assert_that(node11->size(), equals(1));
////}
//
//TEST_F(TestTree, test_event)
//{
//    // auto creation_cb = [&](Node<Component>& comp)
//    // {
//    //     created_count_++;
//    // };
//
//    // auto remove_cb =  [&](Node<Component>& comp)
//    // {    
//    //     created_count_--;
//    // };
//
//    // tree.on_node_created_event().operator+=(creation_cb);
//    // tree.on_node_removed_event().operator+=(creation_cb);
//
//    // auto& node11 = tree.emplace_back(11);
//    // auto& node21 = node11.emplace_back(21);
//    // auto& node31 = node21.emplace_back(31);
//
//    // assert_that(created_count_, equals(3));
//}
//
//TEST_F(TestTree, tree_is_parent)
//{
//    assert_that(node11->is_parent(*node31), equals(true));
//    assert_that(node21->is_parent(*node31), equals(true));
//    assert_that(node11->is_parent(*node21), equals(true));
//    assert_that(node31->is_parent(*node11), equals(false));
//}
//
//TEST_F(TestTree, tree_push_back_copy)
//{
//    // bool result = node11->push_back(*node32);
//
//    // assert_that(result, equals(true));
//
//    // assert_that(node11->size(), equals(3));
//    // assert_that(node21->size(), equals(2));
//
//    // assert_that(node11->children()[2].get().number, equals(32));
//    // assert_that(node11->children()[2].children()[0].get().number,equals(41));
//}
//
//TEST_F(TestTree, tree_push_back_move)
//{
//    // // passing the thing by move
//    // bool result = node11->push_back(std::move(*node32));
//
//    // assert_that(result, equals(true));
//
//    // assert_that(node11->size(), equals(3));
//    // assert_that(node21->size(), equals(1));
//
//    // assert_that(node11->children()[2].get().number, equals(32));
//    // assert_that(node11->children()[2].children()[0].get().number,equals(41));
//}
//
//TEST_F(TestTree, tree_push_back_illegal_operation)
//{
//    // // Can't move a node that is a parent from the current node as a child
//    // bool result = node31->push_back(std::move(*node11));
//
//    // assert_that(result, equals(false));
//}
//
//TEST_F(TestTree, Detach_PutNodeAtRoot)
//{
//    // Action
//
//    const auto owning_pointer = node22->detach();
//
//    // Assert
//
//    assert_that(owning_pointer.get(), non_equals(nullptr));
//	
//    assert_that(node22->parent(), equals(nullptr));
//	
//    assert_that(node11->size(), equals(1));
//
//}
//
//TEST_F(TestTree, copy_node)
//{
//    // Start Tree
//    //  11 
//    //  |____ 21
//    //  |     |____ 31
//    //  |     |____ 32
//    //  |           |____41
//    //  |_____22
//
//    // End Tree
//    //  11 
//    //  |____ 21
//    //  |     |____ 31
//    //  |     |____ 32
//    //  |           |____41
//    //  |_____22
//    //        |_____21
//    //              |____ 31
//    //              |____ 32
//    //                   |____41
//    //        
//
//    node22->push_back(*node21);
//
//    assert_that(node22->size(), equals(1));
//   // assert_that((*node22)[0].get().number, equals(21));
//    assert_that((*node22)[0].size(), equals(2));
//    //assert_that((*node22)[0][0].get().number, equals(31));
//}
//
//// TEST_F(TestTree, tree_swap)
//// {
////     auto& node11 = tree.emplace_back(11);
////     auto& node21 = node11.emplace_back(21);
////     auto& node31 = node21.emplace_back(31);
//
////     node21.swap(node11);
//
////     assert_that(node31.parent()->get().get().number, equals(11))
////     assert_that(tree.children()[0].get().number, equals(21));
////     assert_that(tree.children()[0].parent().operator bool(), equals(false));
//// }