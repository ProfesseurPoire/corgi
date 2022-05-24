#include <corgi/components/MeshCollider.h>
#include <corgi/components/TilemapRenderer.h>
#include <corgi/components/Transform.h>
#include <corgi/components/Camera.h>
#include <corgi/components/Animator.h>
#include <corgi/components/BoxCollider.h>
#include <corgi/components/MeshRenderer.h>
#include <corgi/components/ParticleEmitter.h>
#include <corgi/components/StateMachine.h>

#include <corgi/ecs/Entity.h>

#include <iostream>

using namespace corgi;

int main()
{
	std::cout << "Size bool "			<< sizeof(bool) << "\n";
	std::cout << "long long "			<< sizeof(long long) << std::endl;
	std::cout << "Size string "			<< sizeof(std::string) << "\n";
	std::cout << "Size weak_ptr "		<< sizeof(std::weak_ptr<int>) << std::endl;
	std::cout << "Size shared_ptr "		<< sizeof(std::shared_ptr<int>) << std::endl;
	std::cout << "Size map "			<< sizeof(std::map<int, int>) << std::endl;
	std::cout << "Size vector "			<< sizeof(std::vector<int>) << std::endl<<std::endl;

	std::cout << "Size of Texture"			<< sizeof(Texture)			<< std::endl;
	std::cout << "Size of Animation "		<< sizeof(Animation)		<< std::endl;
	std::cout << "Size of Material "		<< sizeof(Material)			<< std::endl;
	std::cout << "Size of BoxCollider "		<< sizeof(BoxCollider)		<< std::endl;
	std::cout << "Size of MeshRenderer"		<< sizeof(MeshRenderer)		<< std::endl;
	std::cout << "Size of ParticleEmitter "	<< sizeof(ParticleEmitter)	<< std::endl;
	std::cout << "Size of StateMachine "	<< sizeof(StateMachine)		<< std::endl;
	std::cout << "Size of Animator "		<< sizeof(Animator)			<< std::endl;
	std::cout << "Size of MeshCollider "	<< sizeof(MeshCollider)		<< std::endl;
	std::cout << "Size Tilemap "			<< sizeof(TilemapRenderer)	<< std::endl;
	std::cout << "Size Transform "			<< sizeof(Transform)		<< std::endl;
	std::cout << "Size Mesh "				<< sizeof(Mesh)				<< std::endl;
	std::cout << "Size Entity "				<< sizeof(Entity)			<< std::endl;
	std::cout << "Size Camera "				<< sizeof(Camera)			<< std::endl;

	return 0;
}