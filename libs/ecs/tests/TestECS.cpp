
// So ATM I need to put the entity header before the test header
// because entities are static and they would be deleted before
// the scene thing ....
// I really need to make entities object again to avoid this kind
// of bs

#include <corgi/ecs/Entity.h>

#include <corgi/test/test.h>

#include <corgi/ecs/Scene.h>
#include <corgi/ecs/Component.h>

#include <set>


using namespace corgi;
using namespace test;

class OtherComponent : public Component
{
public:

	float y;
};

class TestComponent : public Component
{
public:

	TestComponent(int val = 0) :x(val){}
	
	TestComponent(TestComponent&& other) noexcept = default;
	TestComponent(const TestComponent& other) = default;

	TestComponent& operator=(TestComponent&& other)noexcept = default;
	TestComponent& operator=(const TestComponent& other) = default;
	
	int x;
};

int main()
{
	test::run_all();
}

class ComponentPoolsTest : public test::Test
{
public:

	Scene scene;

	ComponentPools& pools()
	{
		return scene.component_maps();
	}

	~ComponentPoolsTest() override
	{
		
	}

	void set_up()override{}
	void tear_down()override
	{
		
	}
};

TEST_F(ComponentPoolsTest, PoolInitialization)
{
	// We first make sure that the scene is correctly initialized with zero pools
	assert_that(pools().size(), equals(0));
}

TEST_F(ComponentPoolsTest, AddPool)
{
	pools().add<TestComponent>();
	assert_that(pools().contains<TestComponent>(), equals(true));
	assert_that(pools().size(), equals(1));
}

TEST_F(ComponentPoolsTest, CheckIfPoolDoesntExist)
{
	assert_that(pools().contains<TestComponent>(), equals(false));
}

TEST_F(ComponentPoolsTest, RemovingPool)
{
	scene.component_maps().add<TestComponent>();
	assert_that(pools().contains<TestComponent>(), equals(true));
	assert_that(pools().size(), equals(1));
	
	scene.component_maps().remove<TestComponent>();
	assert_that(pools().contains<TestComponent>(), equals(false));
	assert_that(pools().size(), equals(0));
}

//TEST_F(ComponentPoolsTest, AccessNonExistingPool)
//{
//	auto* pool = pools().get<TestComponent>();
//	assert_that(pool, equals(nullptr));
//}

TEST_F(ComponentPoolsTest, AccessPool)
{
	pools().add<TestComponent>();
	auto* pool = pools().get<TestComponent>();
	assert_that(pool, non_equals(nullptr));
}

TEST_F(ComponentPoolsTest, AddOverExistingComponentPool)
{
	pools().add<TestComponent>();
	
	pools().get<TestComponent>()->add_param(EntityId(0), 0);
	pools().get<TestComponent>()->add_param(EntityId(1), 1);

	assert_that(pools().get<TestComponent>()->size(), equals(2));

	auto* first_pool = pools().get<TestComponent>();

	// I want this to be ignored when we already have a component pool of the same
	// type
	pools().add<TestComponent>();

	auto* second_pool = pools().get<TestComponent>();

	assert_that(pools().get<TestComponent>()->size(), equals(2));
	assert_that(first_pool == second_pool, equals(true));
}

TEST_F(ComponentPoolsTest, IterateOver)
{
	pools().add<TestComponent>();
	pools().add<OtherComponent>();

	std::set<std::type_index> indexes { typeid(TestComponent), typeid(OtherComponent) };
	
	for(auto& pool : pools())
	{
		indexes.erase(pool.first);
	}

	assert_that(indexes.size(), equals(0));
	assert_that(pools().size(), equals(2));
}

class ComponentPoolTest : public test::Test
{
public:

	Scene scene;

	ComponentPool<TestComponent>* pool()
	{
		return scene.component_maps().get<TestComponent>();
	}

	void set_up()override
	{
		scene.component_maps().add<TestComponent>();
	}
	
	void tear_down()override
	{
		
	}
};

TEST_F(ComponentPoolTest, Initialization)
{
	assert_that(pool()->size(), equals(0));
}

TEST_F(ComponentPoolTest, Add)
{
	// Not a huge fan of doing that this way
	auto e = scene.root()->emplace_back("Test1");
	
	e->add_component<TestComponent>();
	auto* component_map = scene.component_maps().get<TestComponent>();
	
	// We check that our component map has grown
	assert_that(pool()->size(), corgi::test::equals(1));

	// We check that our component map contains a component for the EntityId we just used
	assert_that(component_map->contains(e->id()), equals(true));

	// We check that our entity contains the component 
	assert_that(e->has_component<TestComponent>(), equals(true));
}

TEST_F(ComponentPoolTest, AddWithParameters)
{
	auto e1 = scene.root()->emplace_back("Test1");
	e1->add_component<TestComponent>(1);

	// We check that our component map has grown
	assert_that(pool()->size(), equals(1));

	// We get back the component and check that the value inside it is correct
	assert_that(e1->get_component<TestComponent>()->x, equals(1));
}

TEST_F(ComponentPoolTest, AddMoreThanOne)
{
	auto ref_entity_1 = scene.root()->emplace_back("Test1");
	auto ref_entity_2 = scene.root()->emplace_back("Test2");
	auto ref_entity_3 = scene.root()->emplace_back("Test3");

	ref_entity_1->add_component<TestComponent>(10);
	ref_entity_2->add_component<TestComponent>(30);
	ref_entity_3->add_component<TestComponent>(20);

	// We check that our pool has grown
	assert_that(pool()->size(), equals(3));

	// We check the value inside our components
	assert_that(ref_entity_1->get_component<TestComponent>()->x, equals(10));
	assert_that(ref_entity_2->get_component<TestComponent>()->x, equals(30));
	assert_that(ref_entity_3->get_component<TestComponent>()->x, equals(20));

	assert_that(pool()->get(ref_entity_1->id()).x, equals(10));
	assert_that(pool()->get(ref_entity_2->id()).x, equals(30));
	assert_that(pool()->get(ref_entity_3->id()).x, equals(20));
}

TEST_F(ComponentPoolTest, Remove)
{
	auto ref_entity_1 = scene.root()->emplace_back("Test1");
	auto ref_entity_2 = scene.root()->emplace_back("Test2");
	auto ref_entity_3 = scene.root()->emplace_back("Test3");

	ref_entity_1->add_component<TestComponent>(10);
	ref_entity_2->add_component<TestComponent>(30);
	ref_entity_3->add_component<TestComponent>(20);

	ref_entity_2->remove_component<TestComponent>();

	// We check that the pool has shrunk 
	assert_that(pool()->size(), equals(2));
	
	// We check that the pool doesn't contain a component for the EntityId of the second entity
	assert_that(pool()->contains(ref_entity_2->id()), equals(false));
	assert_that(ref_entity_2->has_component<TestComponent>(), equals(false));

	// We check that the content of the 2 others component hasn't been altered

	assert_that(ref_entity_1->get_component<TestComponent>()->x, equals(10));
	assert_that(ref_entity_3->get_component<TestComponent>()->x, equals(20));
}

TEST_F(ComponentPoolTest, MoveOperator)
{
	auto e1 = scene.root()->emplace_back("Test1");
	auto e2 = scene.root()->emplace_back("Test2");
	auto e3 = scene.root()->emplace_back("Test3");

	e1->add_component<TestComponent>(1);
	e2->add_component<TestComponent>(2);
	e3->add_component<TestComponent>(3);

	//e1.move_component(std::move(e2.get_component<TestComponent>()));
	//
	////e1.get_component<TestComponent>() = std::move(e2.get_component<TestComponent>());

	//// After moving a component, we actually remove the moved component from the
	//// list
	//assert_that(pool()->size(), equals(2));

	//assert_that(e1.get_component<TestComponent>()->x, equals(2));
	//assert_that(e3.get_component<TestComponent>()->x, equals(3));

	//// I'm making sure the entity reference by the components are correct
	//
	//assert_that(e1.get_component<TestComponent>()->id(), equals(e1.id()));
	//assert_that(e2.has_component<TestComponent>(), equals(false));
	//assert_that(e3.get_component<TestComponent>()->id(), equals(e3.id()));
	//
	//// I'm also checking if I didn't fucked up something inside the map that links
	//// entity id to the component's position inside the vector
	//
	//assert_that(pool()->entity_id_to_components().size(), equals(2));
	//assert_that(pool()->entity_id_to_components().at(e1.id()), equals(0));
	//assert_that(pool()->entity_id_to_components().at(e3.id()), equals(1));
}

/*!
 * @brief	The move constructor won't impact the component map
 */
TEST_F(ComponentPoolTest, MoveConstructor)
{
	auto e1 = scene.root()->emplace_back("Test1");
	auto e2 = scene.root()->emplace_back("Test2");
	auto e3 = scene.root()->emplace_back("Test3");
	auto e4 = scene.root()->emplace_back("Test4");

	e1->add_component<TestComponent>(1);
	e2->add_component<TestComponent>(2);
	e3->add_component<TestComponent>(3);
	auto& c = *e2->get_component<TestComponent>();
	
	e4->add_component<TestComponent>( std::move(c) );

	assert_that(pool()->size(), equals(4));

	assert_that(e1->get_component<TestComponent>()->x, equals(1));
	assert_that(e2->get_component<TestComponent>()->x, equals(2));
	assert_that(e3->get_component<TestComponent>()->x, equals(3));
	assert_that(e4->get_component<TestComponent>()->x, equals(2));
}

TEST_F(ComponentPoolTest, CopyOperator)
{
	//auto& e1 = scene.root()->push_back("Test1");
	//auto& e2 = scene.root()->push_back("Test2");
	//auto& e3 = scene.root()->push_back("Test3");

	//e1.add_component<TestComponent>(1);
	//e2.add_component<TestComponent>(2);
	//e3.add_component<TestComponent>(3);

	//assert_that(e1.get_component<TestComponent>()->x, equals(1));
	//
	//e1.copy_component(e2.get_component<TestComponent>());

	//// After moving a component, we actually remove the moved component from the
	//// list
	//assert_that(pool()->size(), equals(3));

	//assert_that(e1.get_component<TestComponent>()->x, equals(2));
	//assert_that(e2.get_component<TestComponent>()->x, equals(2))
	//assert_that(e3.get_component<TestComponent>()->x, equals(3));

	//// I'm making sure the entity reference by the components are correct

	//assert_that(e1.get_component<TestComponent>()->id(), equals(e1.id()));
	//assert_that(e2.get_component<TestComponent>()->id(), equals(e2.id()));
	//assert_that(e3.get_component<TestComponent>()->id(), equals(e3.id()));

	//// I'm also checking if I didn't fucked up something inside the map that links
	//// entity id to the component's position inside the vector

	//assert_that(pool()->entity_id_to_components().size(), equals(3));
	//assert_that(pool()->entity_id_to_components().at(e1.id()), equals(0));
	//assert_that(pool()->entity_id_to_components().at(e2.id()), equals(1));
	//assert_that(pool()->entity_id_to_components().at(e3.id()), equals(2));
}

TEST_F(ComponentPoolTest, CopyConstructor)
{
	auto e1 = scene.root()->emplace_back("Test1");
	auto e2 = scene.root()->emplace_back("Test2");
	auto e3 = scene.root()->emplace_back("Test3");
	auto e4 = scene.root()->emplace_back("Test4");

	e1->add_component<TestComponent>(1);
	e2->add_component<TestComponent>(2);
	e3->add_component<TestComponent>(3);
	e4->add_component<TestComponent>(e2->get_component<TestComponent>().get());

	assert_that(pool()->size(), equals(4));

	assert_that(e1->get_component<TestComponent>()->x, equals(1));
	assert_that(e2->get_component<TestComponent>()->x, equals(2));
	assert_that(e3->get_component<TestComponent>()->x, equals(3));
	assert_that(e4->get_component<TestComponent>()->x, equals(2));
}

TEST_F(ComponentPoolTest, RangedBasedForLoop)
{
	auto e1 = scene.root()->emplace_back("Test1");
	auto e2 = scene.root()->emplace_back("Test2");
	auto e3 = scene.root()->emplace_back("Test3");
	auto e4 = scene.root()->emplace_back("Test4");

	e1->add_component<TestComponent>(1);
	e2->add_component<TestComponent>(2);
	e3->add_component<TestComponent>(3);
	e4->add_component<TestComponent>(4);

	int i = 0;
	for(auto& component : *pool())
	{
		switch (i)
		{
		case 0:
			assert_that(component.x, equals(1));
			break;
		case 1:
			assert_that(component.x, equals(2));
			break;
		case 2:
			assert_that(component.x, equals(3));
			break;
		case 3:
			assert_that(component.x, equals(4));
			break;
		}
		i++;
	}
}

class EntityTest : public test::Test
{
public:

	Scene scene;


	~EntityTest() override{}

	void set_up()override {}
	void tear_down()override{}
	
};

TEST_F(EntityTest, CloneEntity)
{
	auto entity = scene.new_entity("TestEntity");
	auto child  = entity->emplace_back("ChildTestEntity");
	
	child->add_component<TestComponent>(54);
	entity->add_component<TestComponent>(23);
	
	auto cloned = scene.clone(entity);

	// We check that the clone name's appends "clone" to the original entity's name
	//assert_that(cloned->name(), equals(entity->name() + " clone"));
	
	// We check that the id of the clone is different from the one of the original entity
	assert_that(cloned->id().id_, non_equals(entity->id().id_));

	// We check that by default the clone is appended to the scene's root
	assert_that(cloned->parent()->id(), equals(scene.root()->id()));

	// We check that our clone entity has a child
	
	auto cloned_child = cloned->children().at(0);
	
	assert_that(cloned->children().empty(), non_equals(true));
	
	assert_that(cloned_child->name(), equals("ChildTestEntity clone"));
	assert_that(cloned_child->parent()->id(), equals(cloned->id()));
	
	//We check that the clone's child has the same component as the original one
	assert_that(cloned_child->has_component<TestComponent>(), equals(true));
	assert_that(cloned_child->get_component<TestComponent>()->x, equals(child->get_component<TestComponent>()->x));
	

	// We check that the clone has sucessfully copied the TestComponent from the original entity
	assert_that(cloned->has_component<TestComponent>(), equals(true));
	assert_that(cloned->get_component<TestComponent>()->x, equals(entity->get_component<TestComponent>()->x));
}