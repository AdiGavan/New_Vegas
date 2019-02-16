#pragma once
// "Copyright [2019] Gavan Adrian-George, 334CA"
#include <Component/SimpleScene.h>
#include "LabCamera.h"
//#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

// Structura pentru informatiile unei strazi
struct info_street {
	// Coordonatele centrului
	float xc;
	float yc = -0.049f;
	float zc;
	// Inaltimea (pe axa OZ)
	float height;
	// Latimea (pe axa OX)
	float width;
	// Grosimea (pe axa OY)
	float thick = 0.1f;
};

// Structura pentru informatiile despre spoturi
struct info_spot {
	glm::vec3 light_position;
	glm::vec3 light_direction;
};

// Structura pentru informatiile elementelor din componenta unei cladiri
struct info_city_element {
	// Tipul de shader - necesar pentru a stii ce shader trebuie aplicat in cazul elementelor decorative
	std::string shader_type;
	// Numarul de spoturi asociat
	int spots_number = 0;
	// Vectorul cu spoturile cladirii curente
	std::vector<info_spot> spots;
	// Coordonatele centrului
	float position_xc;
	float position_yc;
	float position_zc;
	// Valorile de scalare
	float scale_x;
	float scale_y;
	float scale_z;
	// Variabila care ajuta la unghiul de rotire al elementelor
	// Valoarea se inmulteste cu o anumita valoare (pi / 2, pi / 4) pentru a simula 4 sau 8 pozitii de rotatie
	int rotation;
	// Numarul meshului din vectorul de meshuri
	int mesh_number;
};


class Tema3 : public SimpleScene
{
public:
	Tema3();
	~Tema3();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D* texture1 = NULL);
	void RenderSimpleMeshBuildings(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, std::vector<info_spot> &elements, Texture2D* texture1 = NULL);
	void Add_Meshes_Shaders_Textures();
	void generate_city_elements();

protected:
	// Variabile utilizate pentru camera
	Tema3Camera::Camera *camera;
	glm::mat4 projectionMatrix;
	float fov = 60;

	// Unordered map pentru texturi
	std::unordered_map<std::string, Texture2D*> mapTextures;
	
	// Vector pentru strazile pe orizontala
	std::vector<info_street> streets_o;
	// Vector pentru strazile pe verticala
	std::vector<info_street> streets_v;
	// Vector pentru elementele cladirilor
	std::vector<info_city_element> buildings;
	// Vector pentru elementele decorative
	std::vector<info_city_element> decorative;
	// Vector pentru a retine meshurile utilizate pentru cladiri
	std::vector<std::string> building_meshes;
	// Vector pentru a retine meshurile utilizate pentru elemente decorative
	std::vector<std::string> decorative_meshes;

	// Vector utilizat pentru alegerea axei de rotatie a cladirilor si elementelor decorative
	glm::vec3 axis = glm::vec3(0, 1, 0);

	// Variabile utilizate pentru iluminare
	glm::vec3 lightPosition;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;

};
