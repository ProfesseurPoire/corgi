#include <iostream>
#include <chrono>
#include <typeinfo>
#include <typeindex>
#include <map>
#include <unordered_map>
#include <vector>
#include <variant>

#include <corgi/test/test.h>

struct Transform
{
	static int Index()
	{
		return 0;
	}
};

struct Health 
{
	static int Index()
	{
		return 1;
	}
};

struct MeshRenderer 
{
	static int Index()
	{
		return 2;
	}
};

class AbstractComponentPool
{
	virtual void stuff(){}
};

template<class T, int Size = 100>
class ComponentPool : public AbstractComponentPool
{
public:
	T components[Size];
	std::vector<int> registeredComponents;
};

#define COUNT 3
 AbstractComponentPool* bars[COUNT] ;

template<class T>
 ComponentPool<T>* function()
{
	for (int i = 0; i< COUNT; ++i )
	{
		if (dynamic_cast< ComponentPool<T>*>(bars[i]))
		{
			return dynamic_cast< ComponentPool<T>*>(bars[i]);
		}
	}
	return nullptr;
}

std::unordered_map<std::type_index,  AbstractComponentPool*> componentPools;

template<class T>
ComponentPool<T>* GetPool()
{
	return static_cast<ComponentPool<T>*>
	(
		componentPools[std::type_index(typeid(T))]
	);
}

template<class T>
ComponentPool<T>* GetPoolIndex()
{
	return static_cast<ComponentPool<T>*>(bars[T::Index()]);
}

int main()
{
	/*componentPools.emplace(std::type_index(typeid(Transform)),	new ComponentPool<Transform>());
	componentPools.emplace(std::type_index(typeid(Health)),		new ComponentPool<Health>());
	componentPools.emplace(std::type_index(typeid(MeshRenderer)),		new ComponentPool<MeshRenderer>());

	bars[0] = new ComponentPool<Transform>();
	bars[1] = new ComponentPool<Health>();
	bars[2] = new ComponentPool<MeshRenderer>();

	int tries = 1000000;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	for (int i = 0; i < tries; ++i)
	{
		function<MeshRenderer>();
	}
	end = std::chrono::system_clock::now();

	int elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>
		(end - start).count();

	std::cout << "time elapsed " << elapsed_seconds<< std::endl;

	start = std::chrono::system_clock::now();
	for (int i = 0; i < tries; ++i)
	{
		 GetPool<MeshRenderer>();
	}
	end = std::chrono::system_clock::now();

	 elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>
		(end - start).count();

	std::cout << "time elapsed " << elapsed_seconds << std::endl;

	start = std::chrono::system_clock::now();
	for (int i = 0; i < tries; ++i)
	{
		GetPoolIndex<MeshRenderer>();
	}
	end = std::chrono::system_clock::now();

	elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>
		(end - start).count();

	std::cout << "time elapsed " << elapsed_seconds << std::endl;

	std::cin.peek();*/

	corgi::test::run_all();
}


struct Uniform
{
	virtual void apply() = 0;
};

struct UniformFloat : Uniform
{

	UniformFloat(int i)
	{
		v = static_cast<float>(i);
	}

	void apply()override
	{
		v += v;
	}

	float v;
};

struct UniformInt : Uniform
{
	UniformInt(int i)
	{
		v = i;
	}
	void apply()override
	{
		v += v;
	}
	int v;
};


class TestUniforms : public corgi::test::Test
{
public:
	std::vector<Uniform*>		uniforms;
	std::vector<float>	float_uniforms;
	std::vector<int>	int_uniforms;

	void set_up()override
	{
		int capacity = 9000000;

		uniforms.reserve(capacity);

		for (int i = 0; i < capacity; ++i)
		{
			if (i % 2 == 0)
			{
				uniforms.push_back(new UniformFloat(i));
				float_uniforms.push_back(float(i));
			}
			else
			{
				uniforms.push_back(new UniformInt(i));
				int_uniforms.push_back(i);
			}
		}
	}

	void tear_down()override
	{
		uniforms.clear();
		float_uniforms.clear();
		int_uniforms.clear();
	}
};


TEST_F(TestUniforms, inheritance)
{
	for (auto& uniform : uniforms)
		uniform->apply();
}

TEST_F(TestUniforms, no_inheritance)
{
	for (auto& uniform : float_uniforms)
		uniform += uniform;

	for (auto& uniform : int_uniforms)
		uniform += uniform;
}