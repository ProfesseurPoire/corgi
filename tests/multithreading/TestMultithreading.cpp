#include <corgi/test/test.h>

#include <functional>
#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <cmath>

#include <corgi/math/Matrix.h>
#include <corgi/math/Vec3.h>
#include <corgi/math/Vec4.h>
#include <corgi/math/Collisions.h>

using namespace corgi;

int main()
{
	test::run_all();
}

class ThreadPool
{
public:

	// By default the thread pool will try to get the optimal number of threads
	// for the current computer, used the hardware_concurrency value
	ThreadPool()
	{
		for(auto i=0u; i< std::thread::hardware_concurrency(); ++i)
			threads.emplace_back(&ThreadPool::run, this);
	}

	ThreadPool(unsigned int thread_count)
	{
		for (auto i = 0u; i < thread_count; ++i)
			threads.emplace_back(&ThreadPool::run, this);
	}

	~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> l(tasks_mutex_);
			end = true;
		}
		cv.notify_all();

		for(auto& t : threads)
			t.join();
	}

	void start()
	{
		{
			std::unique_lock<std::mutex>l(tasks_mutex_);
			std::unique_lock<std::mutex> lll(work_left_mutex);
		}
		cv.notify_all();
	}

	// Lock the thread calling this function until the thread pool is empty 
	void wait_finished() 
	{
		std::unique_lock<std::mutex> l (tasks_mutex_);

		while(stuff_to_do_!=0)
			work_finished_.wait(l);
	}

	void add_task(std::function<void()> task)
	{
		{
			std::unique_lock<std::mutex> ll(tasks_mutex_);
			std::unique_lock<std::mutex> lll(work_left_mutex);
			tasks_.push(task);
			stuff_to_do_ ++;
		}
	}

	void add_tasks(std::vector<std::function<void()>> tasks)
	{
		std::unique_lock<std::mutex> ll(tasks_mutex_);
		std::unique_lock<std::mutex> lll(work_left_mutex);

		for(auto& task : tasks)
			tasks_.push(task);

		stuff_to_do_ = tasks_.size();
	}

	void run()
	{
		std::function<void()> task;

		for(;;)
		{
			{
				std::unique_lock<std::mutex> ll (tasks_mutex_);

				// the lambda here correspond to a while(lambda)
				//									cv.wait(ll);
				// So basically here, the thread will only escape the condition variable 
				// if there is at least 1 task or if the thread pool is deleted

				while(tasks_.empty() && (!end))
				{
					cv.wait(ll);
				}

				if (end && tasks_.empty())	// Directly kills a thread once it wakes up if the ThreadPool should be deleted and there's no more tasks
					return;

				// After a thread wakes up, it takes control of the lock
				task = tasks_.top();
				tasks_.pop();

			}

			task();

			{
				std::unique_lock<std::mutex> l(work_left_mutex);
				
				stuff_to_do_-- ;
				if (stuff_to_do_ == 0)
				{
					l.unlock();
					work_finished_.notify_all();
				}
			}
		}
	}

	std::vector<std::thread> threads;
	std::mutex work_left_mutex;

	int stuff_to_do_ = 0;

	std::mutex tasks_mutex_;
	std::condition_variable cv;
	std::condition_variable work_finished_;
	std::stack<std::function<void()>> tasks_;
	bool end = false;
	int thread_count=0;
	bool ended = false;
};


bool intersect_with_mesh_parallel(
	const unsigned int * indexes,
	const unsigned int indexes_size,
	const float* vertices,
	const int vertices_size,
	const int vertex_size,
	const int offset,
	Matrix model_matrix,
	Ray ray,
	Vec3& intersectionPoint,
	Vec3& intersectionNormal,
	bool& doesIntersect
)
{
	doesIntersect			= false;
	bool isClosestPointSet	= false;

	for (size_t i = 0; i < indexes_size ; i += 3)
	{
		Vec3 posA = model_matrix * Vec3(&vertices[indexes[i]   * vertex_size + offset]);
		Vec3 posB = model_matrix * Vec3(&vertices[indexes[i+1] * vertex_size + offset]);
		Vec3 posC = model_matrix * Vec3(&vertices[indexes[i+2] * vertex_size + offset]);

		// On a besoin de la normale pour trouver l'�quation du plan du triangle
		// Rappel, �quation du plan : ax+by+cz+d=0
		Vec3 normal = math::get_triangle_normal(posA, posB, posC);

		float A = normal.x;
		float B = normal.y;
		float C = normal.z;

		// � partir de la normale, on connait les composantes a,b et c de l'�quation ax+by+cz+d=0 du plan
		// On doit d�sormais trouver la composant D; Pour se faire, on s�lectionne un point de notre plan pour 
		// remplir x,y et c et trouver d

		float D = -(posA.x * A + posA.y * B + posA.z * C);

		// On calcule maintenant les composants de l'�quation param�trique de la droite 
		// � partir de son vecteur directeur

		float denom = ray.direction.x * A + ray.direction.y * B + ray.direction.z * C;

		// non  Colinarit�

		if (denom != 0)
		{
			float t = -ray.start.x * A - ray.start.y * B - ray.start.z * C - D;
			t = t / denom;

			Vec3 P
			(
				ray.start.x + t * ray.direction.x,
				ray.start.y + t * ray.direction.y,
				ray.start.z + t * ray.direction.z
			);

			// We skip if the point is not inside the ray
			if ( t <= 0 )
				continue;

			if (t >= ray.length)
				continue;

			if (Vec3::point_in_triangle(posA, posB, posC, P))
			{
				if (!isClosestPointSet)
				{
					isClosestPointSet = true;
					intersectionPoint	= P;
					intersectionNormal	= normal;
				}
				else
				{
					if ((P - ray.start).sqrt_length() < (intersectionPoint - ray.start).sqrt_length())
					{
						intersectionPoint = P;
						intersectionNormal = normal;
					}
				}
				doesIntersect = true;
			}
		}
	}

	return doesIntersect;
}

static bool intersect_with_mesh_p(
		const unsigned int * indexes,
		const unsigned int indexes_size,
		const float* vertices,
		const int vertices_size,
		const int vertex_size,
		const int offset,
		Matrix model_matrix,
		Ray ray,
		Vec3& intersection_point,
		Vec3& intersection_normal,
		ThreadPool& threadpool
	)
	{
		// we divide this in 10 tasks
		int tasks = 10;
		// but we could have an extra one in case we can't divide
		// properly

		int s		= indexes_size / 3;
		int left = s % tasks;
		s = static_cast<int>(std::floor(static_cast<float>(s) / static_cast<float>(tasks)));
		
			
		std::vector<Vec3> closest_intersection_point(tasks);
		std::vector<Vec3> normal_intersection_point(tasks);
		bool* does_intersect = new bool[tasks];

		for(int i=0; i< tasks; ++i)
		{
			int l = 0;

			if (left != 0 && i==(tasks-1))
				l = left;

			threadpool.add_task([=, &closest_intersection_point, &normal_intersection_point]()
				{
					intersect_with_mesh_parallel
					(
						&indexes[i*s*3],
						(s+l)*3,
						vertices,
						vertices_size,
						vertex_size,
						offset,
						model_matrix,
						ray,
						closest_intersection_point[i],
						normal_intersection_point[i],
						does_intersect[i]
					);
				});
		}

		threadpool.start();
		threadpool.wait_finished();

		// Then we just fetch the closest

		bool isClosestPointSet = false;

		for (auto i = 0u; i < closest_intersection_point.size(); ++i)
		{  
			if (does_intersect[i])
			{
				if (!isClosestPointSet)
				{
					isClosestPointSet = true;
					intersection_point	= closest_intersection_point[i];
					intersection_normal = normal_intersection_point[i];
				}
				else
				{
					if ((closest_intersection_point[i] - ray.start).sqrt_length() < (intersection_point - ray.start).sqrt_length())
					{
						intersection_point	= closest_intersection_point[i];
						intersection_normal = normal_intersection_point[i];
					}
				}
			}
		}
		delete does_intersect;
		return isClosestPointSet;
	}

class Multithreading : public test::Test
{
	public:

	Ray ray = Ray(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 100.0f));
	std::vector<Vec3> vertices;
	std::vector<unsigned int> indexes;

	Vec3 intersection_point;
	Vec3 normal_point;

	ThreadPool* thread_pool;
	std::vector<std::vector<unsigned int>> sub_indexes;

	void set_up()override
	{
		thread_pool = new ThreadPool();
		// Making a big mesh so it takes some time to actually compute the intersection point
		int discretisation=3000;
		float size=0.1f;
		float offset = - (discretisation*size)/2.0f;

		vertices.reserve(discretisation * discretisation * 4);
		indexes.reserve(discretisation * discretisation * 6);

		for(int i=0; i< discretisation; i++)
		{
			for(int j=0; j< discretisation; j++)
			{
				indexes.push_back(vertices.size());
				indexes.push_back(vertices.size()+1);
				indexes.push_back(vertices.size()+2);

				indexes.push_back(vertices.size());
				indexes.push_back(vertices.size()+2);
				indexes.push_back(vertices.size()+3);

				vertices.emplace_back(offset + float(j)*size,offset + float(i)*size, 20.0f);
				vertices.emplace_back(offset + float(j)*size + size, offset + float(i)*size, 20.0f);
				vertices.emplace_back(offset + float(j)*size + size, offset + float(i)*size + size, 20.0f);
				vertices.emplace_back(offset + float(j)*size,offset + float(i)*size + size, 20.0f);
			}
		}
	}

	void tear_down()override
	{
		vertices.clear();
		indexes.clear();
		sub_indexes.clear();
		delete thread_pool;
	}
};

TEST_F(Multithreading, OneThread)
{
   math::intersect_with_mesh(
	   indexes.data(),
	   indexes.size(),
	   (float*)vertices.data(),
	   0,
	   3,
	   Matrix(),
	   ray,
	   intersection_point,
	   normal_point
   );
}

TEST_F(Multithreading, Lambdas)
{
	bool v = intersect_with_mesh_p(indexes.data(), indexes.size(), (float*)vertices.data(), vertices.size(),
		3, 0, Matrix(), ray, intersection_point, normal_point, *thread_pool);

	assert_that(v, test::equals(true));
	assert_that(intersection_point.z, test::equals(20.0f));
}