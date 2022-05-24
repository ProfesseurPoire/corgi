#pragma once

#include <corgi/containers/Vector.h>
#include <corgi/test/test.h>

#include <vector>

using namespace corgi;
using namespace corgi::test;

class Dummy
{
public:

	Dummy(int a, float b):a(a),b(b){}
	int a;
	float b;
};


class Foo
{
public:

	Foo(float f, int v):f(f),v(v){}
	float f;
	int v;
};

class DummyVector
{
public:

	DummyVector(float f, int v)
	{
		std::cout<<"Constructor"<< std::endl;
		datas.push_back(Foo(f, v));
	}

	DummyVector(const DummyVector& v)
	{
		datas = v.datas;
        //std::cout<<"Copy Constructor"<< std::endl;
	}

	DummyVector(DummyVector&& other)
	{
		datas = std::move(other.datas);
		//std::cout<< " Move Constructor"<< std::endl;
	}


	DummyVector& operator=(DummyVector&& other)
	{
		datas = std::move(other.datas);
        //std::cout<< " Move Operator"<< std::endl;
		return *this;
	}

	DummyVector& operator=(const DummyVector& other)
	{
		datas = other.datas;
        //std::cout<< " Copy Operator"<< std::endl;
		return *this;
	}
	
	~DummyVector()
	{
        //std::cout << "destructor" << std::endl;
	}

	std::vector<Foo> datas;
};


class VectorFixture : public Test
{
public:

	Vector<Dummy> v;
	Vector<Dummy> v2;
	Vector<DummyVector> v3;
};

//
//TEST_F(VectorFixture, test)
//{
//	assert_that(v.capacity(), equals(0));
//	assert_that(v.size(), equals(0));
//
//	v.emplace_back(5, 5.0f);
//
//	assert_that(v.size(), equals(1));
//	assert_that(v.capacity(), equals(1));
//	assert_that(v.at(0).a, equals(5));
//
//	v.emplace_back(10, 2.0f);
//
//	assert_that(v.size(), equals(2));
//	assert_that(v.capacity(), equals(2));
//	assert_that(v.at(0).a, equals(5));
//	assert_that(v.at(1).a, equals(10));
//
//	v.emplace_back(7, 2.0f);
//
//	assert_that(v.size(),		equals(3));
//	assert_that(v.capacity(),	equals(3));
//	assert_that(v.at(0).a,		equals(5));
//	assert_that(v.at(1).a,		equals(10));
//	assert_that(v.at(2).a,		equals(7));
//
//	v.remove_at(1);
//
//	assert_that(v.size(), equals(2));
//	assert_that(v.capacity(), equals(3));
//	assert_that(v.at(0).a, equals(5));
//	assert_that(v.at(1).a, equals(7));
//
//	v.emplace_back(7, 2.0f);
//
//	assert_that(v.size(), equals(3));
//	assert_that(v.capacity(), equals(3));
//	assert_that(v.at(0).a, equals(5));
//	assert_that(v.at(1).a, equals(7));
//	assert_that(v.at(2).a, equals(7));
//
//	v2 = v;
//
//	assert_that(v2.size(), equals(3));
//	assert_that(v2.capacity(), equals(3));
//	assert_that(v2.at(0).a, equals(5));
//	assert_that(v2.at(1).a, equals(7));
//	assert_that(v2.at(2).a, equals(7));
//
//
//	v2 = std::move(v);
//
//	assert_that(v2.size(), equals(3));
//	assert_that(v2.capacity(), equals(3));
//	assert_that(v2.at(0).a, equals(5));
//	assert_that(v2.at(1).a, equals(7));
//	assert_that(v2.at(2).a, equals(7));
//
//	assert_that(v.capacity(), equals(0));
//	assert_that(v.size(), equals(0));
//}


//TEST_F(VectorFixture, test_corruption)
//{
//	Vector<int> v;
//
//	for(int i=0; i< 10000; i++)
//	{
//		v.emplace_back(1);
//	}
//}

TEST_F(VectorFixture, test_stl2)
{
	std::vector<DummyVector> a;

	std::cout<<"Emplace back"<<std::endl;
	a.emplace_back(0.0f,1);
	
	std::cout<<"Emplace back"<<std::endl;
	a.emplace_back(0.0f,2);

	std::vector<DummyVector> b;
	
	std::cout<<"Moving a into b"<<std::endl;
	b=std::move(a);
}

TEST_F(VectorFixture, test_v2)
{
	Vector<DummyVector> a;

	std::cout<<"Emplace back"<<std::endl;
	a.emplace_back(0.0f,1);
	
	std::cout<<"Emplace back"<<std::endl;
	a.emplace_back(0.0f,2);

	Vector<DummyVector> b;

	b.emplace_back(0.0f, 3);
	
	std::cout<<"Moving a into b"<<std::endl;
	b = std::move(a);
}

TEST_F(VectorFixture, test_stl)
{
	std::vector<DummyVector> a;

	std::cout<<"Emplace back"<<std::endl;
	a.emplace_back(0.0f,1);
	
	std::cout<<"Emplace back"<<std::endl;
	a.emplace_back(0.0f,2);

	std::cout<<"Erase "<<std::endl;
	a.erase(a.begin());

	std::vector<DummyVector> b;
	
	b.emplace_back(0.0f, 3);
	std::cout<<"Copying a into b"<<std::endl;
	b=a;
}

TEST_F(VectorFixture, testnotstl)
{
	Vector<DummyVector> a;

	std::cout<<"Emplace back"<<std::endl;
	a.emplace_back(0.0f,1);
	
	std::cout<<"Emplace back"<<std::endl;
	a.emplace_back(0.0f,2);

	
	std::cout<<"Erase "<<std::endl;
	a.remove_at(0);
	
	Vector<DummyVector> b;
	
	std::cout<<"Copying a into b"<<std::endl;
	b=a;
}

TEST_F(VectorFixture, testDummyVector)
{
	std::vector<int> a{1};
	a.erase(a.begin(), a.end());
	a.erase(a.begin(), a.end());

	v3.emplace_back(0.0f,1);

	v3.at(0).datas.erase(v3.at(0).datas.begin(), v3.at(0).datas.end());
	v3.at(0).datas.emplace_back(0.0f, 5);

	v3.emplace_back(0.0f, 2);

	v3.at(0).datas.erase(v3.at(0).datas.begin(), v3.at(0).datas.end());

	v3.emplace_back(0.0f, 3);
	v3.emplace_back(2, 2);

	v3.emplace_back(v3.at(0));
	v3.back().datas.erase(v3.back().datas.begin(), v3.back().datas.end());
	v3.emplace_back(1, 1);

	auto cpy = v3;
	cpy.emplace_back(1, 1);
	cpy.remove_at(1);
	cpy.emplace_back(1, 1);
	cpy.emplace_back(1, 1);
	cpy.emplace_back(1, 1);
	cpy.emplace_back(1, 1);

	cpy.remove_at(1);

	Vector<DummyVector> mv;
	mv = std::move(cpy);

	mv.at(0).datas.emplace_back(2, 2);
	mv.at(0).datas.emplace_back(2, 2);
	mv.at(0).datas.emplace_back(2, 2);
	mv.at(0).datas.emplace_back(2, 2);

	mv.remove_at(2);

	mv.at(0).datas.emplace_back(2, 2);
	mv.at(0).datas.emplace_back(2, 2);
	mv.at(0).datas.emplace_back(2, 2);
	mv.at(0).datas.emplace_back(2, 2);
	mv.at(0).datas.emplace_back(2, 2);
	mv.at(0).datas.erase(mv.at(0).datas.begin(), mv.at(0).datas.end());
}