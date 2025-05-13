#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <functional>

#include "Nictophobia_iopath.h"
#include "Nictophobia_Assets.h"
#include "Nictophobia_AssetsLoader.h"
#include "Nictophobia_Camera.h"
#include "Nictophobia_FreeCamera.h"
#include "Nictophobia_Mesh.h"
#include "Nictophobia_Font.h"
#include "Nictophobia_NeAssets.h"
#include "Nictophobia_Necore.h"
#include "Nictophobia_NeContext.h"
#include "Nictophobia_Batch2D.h"
#include "Nictophobia_Batch3D.h"
#include "Nictophobia_Object.h"
#include "Nictophobia_Stage.h"
#include "Nictophobia_Window.h"

void queueAssets(AssetsLoader* loader) {
	loader->queue("fonts/ubuntu", neassets::font(iopath("res:UbuntuMono-R.ttf"), 16));
	loader->queue("textures/font", neassets::texture(iopath("res:font.png")));
	loader->queue("meshes/cube", neassets::mesh(iopath("res:cube.obj")));
}


int buildTheGame(NeContext* context) {
	Stage* stage2d = new Stage(new Camera({0, 0, 0}, context->window->getHeight(), false), "shaders/ui");
	stage2d->getCamera()->setFlipped(true);
	{
		Object* object = new Object({8, 5, 0});
		object->drawCallback = [](NeContext* context, Batch2D* batch, Batch3D*, Object* object) {
			glm::vec3 position = object->getPosition();
			batch->color(1.0f, 1.0f, 1.0f, 1.0f);
			long long mcs = context->frameTimeMicros;
			batch->drawText("fonts/ubuntu", L"mcs elapsed: "+std::to_wstring(mcs)+L", ms: "+std::to_wstring(mcs/1000)+L" draw-calls: "+std::to_wstring(Batch3D::drawCalls),
					position.x, position.y, true, false, context->timer * 0.001f);
			batch->flush();
		};
		stage2d->add(object);

	}
	context->stage = stage2d;

	context->camera = new Camera(glm::vec3(0, 2, 10), 3.141592/3, true);
	Stage* stage = new Stage(context->camera, "shaders/g3d");
	context->stage3d = stage;

	int k = 0;
	for (int z = 0; z < 240; z+=3) {
		for (int x = 0; x < 240; x+=3, k++) {
			{
				Object* object = new Object({x, 0, z});
				object->callback = [x, z, k](NeContext* context, Object* object) {
					object->setRotation(glm::rotate(glm::mat4(1.0f), context->timer * 0.001f+k, glm::vec3(0.0f,1.0f,0.0f)));
					object->setPosition(glm::vec3(x, sin(context->timer * 0.002f+k) * 0.5f + 0.5f, z));
				};
				object->drawCallback = [](NeContext* context, Batch2D*, Batch3D* batch3d, Object* object) {
					Mesh* mesh = (Mesh*) context->assets.get("meshes/cube");
					MeshData* meshData = mesh->getData();
					batch3d->putCTN(meshData, object->getMatrix());
				};
				stage->add(object);
			}

			{
				Object* object = new Object({x, -2.0f, z});
				object->drawCallback = [](NeContext* context, Batch2D*, Batch3D* batch3d, Object* object) {
					Mesh* mesh = (Mesh*) context->assets.get("meshes/cube");
					MeshData* meshData = mesh->getData();
					batch3d->putCTN(meshData, object->getPosition());
				};
				stage->add(object);
			}
		}
	}
	context->freeCamera.setCamera(context->camera);
	return 0;
}

void finishTheGame(NeContext* context) {
	delete context->stage;
}

int main(int, char*[]) {
	Necore core;
	NeGameProject project {"test project"};
	project.assets_ask = queueAssets;
	project.build_game = buildTheGame;
	project.destruct_game = finishTheGame;
	if (int status = core.run(&project)) {
		return status;
	}
	return 0;
}
