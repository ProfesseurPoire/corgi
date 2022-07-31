#include <corgi/ui/UiUtils.h>

using namespace corgi;

static std::shared_ptr<Mesh> nine_slice_quad_mesh {nullptr};

void UiUtils::release_nineslice_mesh()
{
	nine_slice_quad_mesh.reset();
}

Mesh& UiUtils::nine_slice_quad()
{
	float bot1 = 0.0f;
	float bot2 = 0.25f;

	float top1 = 0.75f;
	float top2 = 1.0f;

	float left 		= 0.0f;
	float left2 	= 0.25f;

	float right 	= 0.75f;
	float right2 	= 1.0f;

	float uvBot1 = 1.0f;
	float uvBot2 = 0.75f;

	float uvTop1 = 0.25f;
	float uvTop2 = 0.0f;

	// left --- left2 -------------right -----right2
	//  

	if(!nine_slice_quad_mesh)
	{
		nine_slice_quad_mesh =  Mesh::new_standard_2D_mesh
		(
			{
				left, 	bot1,	0.0f, 0.00f , uvBot1,
				left2, 	bot1,	0.0f, 0.25f , uvBot1,
				left2,  bot2,	0.0f, 0.25f , uvBot2,
				left,   bot2,	0.0f, 0.00f , uvBot2,

				right,  bot1,	0.0f, 0.75f , uvBot1,
				right2, bot1,	0.0f, 1.00f , uvBot1,
				right2, bot2,	0.0f, 1.00f , uvBot2,
				right,  bot2,	0.0f, 0.75f , uvBot2,

				left,  top1,	0.0f, 0.00f , uvTop1,
				left2, top1,	0.0f, 0.25f , uvTop1,
				left2, top2,	0.0f, 0.25f , uvTop2,
				left,  top2,	0.0f, 0.00f , uvTop2,

				right,  top1,	0.0f, 0.75f , uvTop1,
				right2, top1,	0.0f, 1.00f , uvTop1,
				right2, top2,	0.0f, 1.00f , uvTop2,
				right,  top2,	0.0f, 0.75f , uvTop2
			},
			{
				0, 1, 2,		0, 2, 3,
				1, 4, 7,		1, 7, 2,
				4, 5, 6,		4, 6, 7,
				
				3, 2, 9,		3, 9, 8,
				2, 7, 12,		2, 12, 9,
				7, 6, 13,		7, 13, 12,

				8, 9, 10,		8, 10, 11,
				9, 12, 15,		9, 15, 10,
				12, 13, 14,		12, 14, 15
			}
		);
	}
	return *nine_slice_quad_mesh;
}

