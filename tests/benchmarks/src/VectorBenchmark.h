#pragma once

#include <corgi/utils/time/Timer.h>
#include <corgi/containers/Vector.h>

namespace corgi
{

	struct TestVector
	{
		int a;
		int b;

		[[nodiscard]] bool operator==(const TestVector& other) const noexcept
		{
			if (a != other.a)
				return false;

			if (b != other.b)
				return false;

			return true;
		}
	};

	static bool are_vectors_equals_simple(const std::vector<TestVector>& u1, const std::vector<TestVector>& u2)
	{
		if (u1.size() != u2.size())
			return false;

		for (unsigned i = 0; i < u1.size(); i++)
		{
			if (!(u1[i] == u2[i]))
				return false;
		}
		return true;
	}

	static bool are_vectors_equals(const std::vector<TestVector>& u1, const std::vector<TestVector>& u2)
	{
		const auto* ua1 = u1.data();
		const auto* ua2 = u2.data();
		const auto size1 = u1.size();

		if (size1 != u2.size())
			return false;

		for (unsigned i = 0; i < size1; i++)
		{
			if (!(ua1[i] == ua2[i]))
				return false;
		}
		return true;
	}

	static bool are_vectors_equals(const TestVector* u1, const TestVector* u2, int size1, int size2)
	{
		if (size1 != size2)
			return false;

		for (unsigned i = 0; i < size1; ++i)
		{
			if (u1[i] != u2[i])
				return false;
		}
		return true;
	}


	inline void test_vector_comparison()
	{
		int repeat = 20000000;
		
		std::vector<TestVector> a{{1,1},{1,2},{1,3},{1,4}};
		std::vector<TestVector> b{{1,1},{1,2},{1,3},{1,5}};

		TestVector arraya[4] = { {1,1},{1,2},{1,3},{1,4} };
		TestVector arrayb[4] = { {1,1},{1,2},{1,3},{1,5} };

		Vector<TestVector> v1;
		Vector<TestVector> v2;

		v1.push_back({ 1,1 });
		v1.push_back({ 1,1 });
		v1.push_back({ 1,1 });
		v1.push_back({ 1,1 });

		v2.push_back({ 1,1 });
		v2.push_back({ 1,1 });
		v2.push_back({ 1,1 });
		v2.push_back({ 1,2 });


		std::cout << "Starting default stl vector comparison" << std::endl;
		corgi::time::Timer timer;
		timer.start();
		for (int i = 0; i < repeat; i++)
			a==b;
		std::cout << "std vector compared in : " << timer.elapsed_time() * 1000.0f << " ms" << std::endl;
		
		std::cout << "Starting simple stl vector comparison" << std::endl;
		timer.start();
		for (int i = 0; i < repeat; i++)
			are_vectors_equals_simple(a, b);
		std::cout << "std vector compared in : " << timer.elapsed_time() * 1000.0f << " ms" << std::endl;
		
		std::cout << "Starting stl vector comparison" << std::endl;
		timer.start();
		for(int i=0; i< repeat; i++)
			are_vectors_equals(a, b);
		std::cout << "std vector compared in : " << timer.elapsed_time() * 1000.0f << " ms" << std::endl;


		std::cout << "Starting array comparison" << std::endl;
		timer.start();
		for (int i = 0; i < repeat; i++)
			are_vectors_equals(arraya, arrayb, 4,4);
		std::cout << "std vector compared in : " << timer.elapsed_time() * 1000.0f << " ms"<<std::endl;

		std::cout << "Starting Corgi Vector comparison" << std::endl;
		timer.start();
		for (int i = 0; i < repeat; i++)
			v1 == v2;
		std::cout << "std vector compared in : " << timer.elapsed_time() * 1000.0f << " ms" << std::endl;

		std::cout << "Starting Corgi Vector comparison by accessing array" << std::endl;
		timer.start();
		for (int i = 0; i < repeat; i++)
			are_vectors_equals(v1.data(), v2.data(), v1.size(), v2.size());
		std::cout << "std vector compared in : " << timer.elapsed_time() * 1000.0f << " ms" << std::endl;
		
	}
}