
add_executable(UnitTestMath 
	UnitTestMath.cpp
	UTMatrix.cpp
	UTVector2f.cpp
	UTVector3f.cpp
	UTVector4f.cpp
	UTCollisions.cpp
	MathBenchmarks.cpp
)

target_link_libraries(UnitTestMath PUBLIC
	corgi-math
	CorgiTest
)

set_property(TARGET UnitTestMath PROPERTY CXX_STANDARD 17)

#Tells cmake to run this application when calling the ctest command
add_test( NAME UnitTestMath COMMAND UnitTestMath)
