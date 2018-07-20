#include <iostream>
#include "Application.h"
#include "Shader.h"
#include "TestCube.h"
#include "TestTriangle.h"
#include "utilities.hpp"
#include "scope.h"
#include "FreeCamera.h"

using namespace std;

int main(int argc, char **argv)
{
    cout << "Hello world!" << endl;

    Application *app = new Application("Sexer", 1920, 1080);
    if(!app->init()) {
        cout << "Error setting up SDL or context" << endl;
    }

    FreeCamera *camera = new FreeCamera(app->getInputManager());
    app->getRenderer()->setCamera(camera);

    Shader shader(string("shaders/advanced/materials.vert"), string("shaders/advanced/materials.frag"));
    //Shader shader(string("shaders/basic/light.vert"), string("shaders/basic/light.frag"));
    app->getRenderer()->setShader(shader); // loads the shader

    vector<StaticMesh*> meshlist = loadOBJ_static("objects/space_scene.obj", false, true);
    vector<AbstractMaterial*> materials_list = loadMTL("objects/space_scene.mtl", true);
    // TODO : Access the OBJ and the MTL by name, and load both at the same time, automatically linked

    StaticMesh *car = meshlist[0];
    car->setMaterial(materials_list[0]);

    StaticMesh *spaceship = meshlist[1];
    spaceship->setMaterial(materials_list[1]);

    //spaceship->getMaterial()->setSpecularStrength(0.5);

    car->load();
    spaceship->load();

    app->getRenderer()->addMesh(car);
    app->getRenderer()->addMesh(spaceship);

    /*vector<StaticMesh*> meshlist = loadOBJ_static("objects/plane.obj", false, false);
    vector<AbstractMaterial*> materials_list = loadMTL("objects/plane.mtl", true);

    StaticMesh *plane = meshlist[0];
    plane->setMaterial(materials_list[0]);

    plane->getMaterial()->setSpecularStrength(0.5);
    cout << "Specular Exponent : " << plane->getMaterial()->getSpecularExponent() << endl;

    plane->load();
    app->getRenderer()->addMesh(plane);*/

    /* Test scene */
    /*
    cout << "Loading scene.obj..." << endl;
    vector<StaticMesh*> meshlist = loadOBJ_static("objects/scene.obj", false, true);
    cout << "Loaded the file (" << meshlist.size() << " meshes)" << endl;

    StaticMesh *gun = meshlist[0];
    StaticMesh *cylinder = meshlist[1];
    StaticMesh *cone = meshlist[2];
    StaticMesh *leaves01 = meshlist[3];
    StaticMesh *ground = meshlist[4];
    StaticMesh *base_tree = meshlist[5];

    cout << "Associated meshes" << endl;

    AbstractTexture *tex_grass = new AbstractTexture("textures/veg010.jpg");
    AbstractTexture *tex_wood = new AbstractTexture("textures/wooden.png");
    AbstractTexture *tex_leaves = new AbstractTexture("textures/HazelnutLeaves.png");
    AbstractTexture *tex_base_tree = new AbstractTexture("textures/HazelnutBark.png");
    AbstractTexture *tex_gun = new AbstractTexture("textures/handgun_C.jpg");
    AbstractTexture *tex_cone = new AbstractTexture("textures/wall068b.jpg");

    cout << "Created textures" << endl;

    cylinder->setTexture(tex_wood);
    ground->setTexture(tex_grass);
    gun->setTexture(tex_gun);
    cone->setTexture(tex_cone);
    leaves01->setTexture(tex_leaves);
    base_tree->setTexture(tex_base_tree);

    cout << "Associated textures" << endl;

    cylinder->load();
    ground->load();
    gun->load();
    cone->load();
    leaves01->load();
    base_tree->load();

    cout << "Loaded meshes" << endl;

    app->getRenderer()->addMesh(cylinder);
    app->getRenderer()->addMesh(ground);
    app->getRenderer()->addMesh(gun);
    app->getRenderer()->addMesh(cone);
    app->getRenderer()->addMesh(leaves01);
    app->getRenderer()->addMesh(base_tree);


    cout << "Ready!" << endl;*/
    app->loop(120); // 120 fps

    return 0;
}
