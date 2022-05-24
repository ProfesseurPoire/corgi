#include <fstream>

#include <corgi/test/test.h>

#include <corgi/utils/ResourcesCache.h>
#include <corgi/resources/Resource.h>
#include "config.h"

using namespace corgi;

class DummyStuff
{
public:

	int val;
	float v;
	float r;
	double z;
};

class DummyResource : public Resource
{
public:

	// We need a constructor with the first thing being the path and the second the id/name
	DummyResource(const SimpleString& path, const SimpleString& name)
	{
		std::ifstream f(path.c_str());
		std::string r;
		f >> r;
		content = r.c_str();
		this->name=name;
		this->path=path;
	}

	~DummyResource() override
	{
		//std::cout<<"DummyResource destructor called"<<std::endl;	
	}

	int memory_usage() override
	{
		return sizeof (DummyResource) + datas.capacity()*sizeof(int) + stuffs.capacity()* (sizeof(std::unique_ptr<DummyStuff>)+sizeof(DummyStuff));
	}

	std::vector<std::unique_ptr<DummyStuff>> stuffs;
	std::vector<int> datas;	//let's supposed this holds some data
	SimpleString name;
	SimpleString path;
	SimpleString content;
};

class ResourcesCacheTests : public test::Test
{
public:

	ResourcesCache resources_cache;

	void set_up() override
	{
		resources_cache.directories().emplace_back(LIBRARY_RESOURCE_DIRECTORY);
		resources_cache.directories().emplace_back(PROJECT_RESOURCE_DIRECTORY);
	}
};

TEST_F(ResourcesCacheTests, find_existing_resource_returns_valid_optional)
{
	const auto resource = resources_cache.find("dummy_resource.dumb");
	assert_that(resource, test::non_equals(""));

	const auto another_resource = resources_cache.find("test_folder/even_dumber.dumb");
	assert_that(resource, test::non_equals(""));
}

TEST_F(ResourcesCacheTests, find_non_existing_resource_returns_nullopt)
{
	const auto resource =resources_cache.find("ttt");
	assert_that(resource, test::equals(""));
}

TEST_F(ResourcesCacheTests, get_dummy_resource_from_folder)
{
	auto resource = resources_cache.get<DummyResource>("dummy_resource.dumb");

	assert_that(resource, test::non_equals(nullptr));
	assert_that(resource->content, test::equals("dumb"));
}

TEST_F(ResourcesCacheTests, already_in_cache_should_return_false)
{
	bool val = resources_cache.is_cached("dummy_resource.dumb");
	assert_that(val, test::equals(false));

	auto resource = resources_cache.get<DummyResource>("dummy_resource.dumb");
	assert_that(resource, test::non_equals(nullptr));
	assert_that(resource->content, test::equals("dumb"));
	
	assert_that(resources_cache.is_cached("dummy_resource.dumb"), test::equals(true));
}

//TEST_F(ResourcesCacheTests, resources_cache_size)
//{
//	auto r = resources_cache.get<DummyResource>("dummy_resource.dumb");
//
//	r->datas.resize(200);
//	r->stuffs.emplace_back();
//	// WARNING : This might fail if some std::vector are implemented differently or takes up more space
//	// I'll need to add a ifdef if this happens
//	assert_that(resources_cache.memory_usage(), test::equals(948));
//}

int main()
{
	test::run_all();
}