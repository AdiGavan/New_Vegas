// "Copyright [2019] Gavan Adrian-George, 334CA"
#include "Tema3.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

Tema3::Tema3()
{
}

Tema3::~Tema3()
{
}

void Tema3::Init()
{
	// Se seteaza camera
	camera = new Tema3Camera::Camera();
	camera->Set(glm::vec3(0, 4, 7.0f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);

	// Pentru iluminare
	lightPosition = glm::vec3(0, 100000, 100000);
	materialShininess = 30;
	materialKd = 1;
	materialKs = 0.5;

	// Se apeleaza functia pentru adaugarea mesheurilor, texturilor si shaderurilor
	Add_Meshes_Shaders_Textures();

	// Se adauga mesh-urile pentru constructia cladirilor in vectorul cu meshuri pentru cladiri
	building_meshes.push_back("cube");
	building_meshes.push_back("cylinder");
	building_meshes.push_back("cylinderonecut");
	building_meshes.push_back("cylindertwocut");
	building_meshes.push_back("hexagon");
	building_meshes.push_back("pentagon");
	building_meshes.push_back("octagon");
	building_meshes.push_back("pentagontaiat");
	building_meshes.push_back("triangularprism");
	building_meshes.push_back("null");
	building_meshes.push_back("cone");
	building_meshes.push_back("pyramid");
	building_meshes.push_back("sphere");

	// Se adauga mesh-urile decoractive in vectorul ce contine meshurile pentru elementele decorative
	decorative_meshes.push_back("decorativ_cop");
	decorative_meshes.push_back("decorativ_bench");
	decorative_meshes.push_back("decorativ_helicopter");

	// Se apeleaza functia care genereaza orasul
	generate_city_elements();
}

void Tema3::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema3::Update(float deltaTimeSeconds)
{
	// Se randeaza strazile
	for (int i = 0; i < streets_o.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(streets_o[i].xc, streets_o[i].yc, streets_o[i].zc));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(streets_o[i].width, streets_o[i].thick, streets_o[i].height));
		RenderSimpleMesh(meshes["cube"], shaders["ShaderStreetsO"], modelMatrix, mapTextures["street_orizontal"]);
	}

	for (int i = 0; i < streets_v.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(streets_v[i].xc, streets_v[i].yc, streets_v[i].zc));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(streets_v[i].width, streets_v[i].thick, streets_v[i].height));
		RenderSimpleMesh(meshes["cube"], shaders["ShaderStreetsV"], modelMatrix, mapTextures["street_vertical"]);
	}
	
	// Se randeaza elementele cladirilor
	for (int i = 0; i < buildings.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(buildings[i].position_xc, buildings[i].position_yc, buildings[i].position_zc));
		// Se inmulteste pi/4 cu valori intre 0-7 pentru a obtine 8 pozitii de rotatie
		modelMatrix = glm::rotate(modelMatrix, 0.785f * buildings[i].rotation, axis);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(buildings[i].scale_x, buildings[i].scale_y, buildings[i].scale_z));
		RenderSimpleMeshBuildings(meshes[building_meshes[buildings[i].mesh_number]], shaders["ShaderCladiri"], modelMatrix, buildings[i].spots);
	}

	// Se randeaza suprafata
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.05f, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(50, 0.1f, 50));
		RenderSimpleMesh(meshes["cube"], shaders["ShaderSuprafata"], modelMatrix, mapTextures["surface"]);
	}
	
	// Se randeaza elementele decorative
	for (int i = 0; i < decorative.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(decorative[i].position_xc, decorative[i].position_yc, decorative[i].position_zc));
		// Se inmulteste pi/2 cu valori intre 0-3 pentru a obtine 4 pozitii de rotatie (Nord, Sud, Vest, Est)
		modelMatrix = glm::rotate(modelMatrix, 1.57f * decorative[i].rotation, axis);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(decorative[i].scale_x, decorative[i].scale_y, decorative[i].scale_z));
		RenderSimpleMesh(meshes[decorative_meshes[decorative[i].mesh_number]], shaders[decorative[i].shader_type], modelMatrix);
	}

}

void Tema3::FrameEnd()
{
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema3::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture1)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture1)
	{
		// Activate texture location 0
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture1 ID
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		// Send texture uniform value
		glUniform1i(glGetUniformLocation(shader->program, "texture1"), 0);
	}

	// Set light position
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	// Set material property uniforms (shininess, kd, ks) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	// Camera position
	glm::vec3 eyePosition = camera->GetPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema3::RenderSimpleMeshBuildings(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, std::vector<info_spot> &elements, Texture2D* texture1)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture1)
	{
		// Activate texture location 0
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture1 ID
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		// Send texture uniform value
		glUniform1i(glGetUniformLocation(shader->program, "texture1"), 0);
	}

	// Set light position
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	// Set material property uniforms (shininess, kd, ks) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	// Camera position
	glm::vec3 eyePosition = camera->GetPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Set spots informations

	int number_spots = elements.size();
	int number_spots_location = glGetUniformLocation(shader->program, "number_spots");
	glUniform1i(number_spots_location, number_spots);

	if (number_spots >= 1) {
		light_position = glGetUniformLocation(shader->program, "light_position1");
		glUniform3f(light_position, elements[0].light_position.x, elements[0].light_position.y, elements[0].light_position.z);

		int light_direction = glGetUniformLocation(shader->program, "light_direction1");
		glUniform3f(light_direction, elements[0].light_direction.x, elements[0].light_direction.y, elements[0].light_direction.z);

		if (number_spots >= 2) {
			light_position = glGetUniformLocation(shader->program, "light_position2");
			glUniform3f(light_position, elements[1].light_position.x, elements[1].light_position.y, elements[1].light_position.z);

			light_direction = glGetUniformLocation(shader->program, "light_direction2");
			glUniform3f(light_direction, elements[1].light_direction.x, elements[1].light_direction.y, elements[1].light_direction.z);
		
			if (number_spots >= 3) {
				light_position = glGetUniformLocation(shader->program, "light_position3");
				glUniform3f(light_position, elements[2].light_position.x, elements[2].light_position.y, elements[2].light_position.z);

				light_direction = glGetUniformLocation(shader->program, "light_direction3");
				glUniform3f(light_direction, elements[2].light_direction.x, elements[2].light_direction.y, elements[2].light_direction.z);
			
				if (number_spots >= 4) {
					light_position = glGetUniformLocation(shader->program, "light_position4");
					glUniform3f(light_position, elements[3].light_position.x, elements[3].light_position.y, elements[3].light_position.z);

					light_direction = glGetUniformLocation(shader->program, "light_direction4");
					glUniform3f(light_direction, elements[3].light_direction.x, elements[3].light_direction.y, elements[3].light_direction.z);
				}
			}
		}
	}

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema3::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;

		if (window->KeyHold(GLFW_KEY_W)) {
			camera->TranslateForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			camera->TranslateRight(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			camera->TranslateForward(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			camera->TranslateRight(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			camera->TranslateUpword(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			camera->TranslateUpword(cameraSpeed * deltaTime);
		}
	}

	// Taste pentru a misca pozitia luminii ce acopera orasul (ca si cum ar fi soare)
	if (window->KeyHold(GLFW_KEY_U)) {
		lightPosition.z += -10000 * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_J)) {
		lightPosition.z += 10000 * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_H)) {
		lightPosition.x += -10000 * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_K)) {
		lightPosition.x += 10000 * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_Y)) {
		lightPosition.y += -10000 * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_I)) {
		lightPosition.y += 10000 * deltaTime;
	}
}

void Tema3::OnKeyPress(int key, int mods)
{
	// Cand se apasa R se regenereaza orasul
	if (key == GLFW_KEY_R) {
		generate_city_elements();
	}
}

void Tema3::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			camera->RotateFirstPerson_OX(-deltaY * sensivityOX);
			camera->RotateFirstPerson_OY(-deltaX * sensivityOY);
		}
	}
}

void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema3::OnWindowResize(int width, int height)
{
}

/* Functie pentru generarea orasului */
void Tema3::generate_city_elements() {
	// Variabila utilizata pentru a vedea daca o un obiect va fi creat sau nu
	int chance = 0;
	// Variabila auxiliara pentru dimensiunea random
	float rand_dimension = 0;
	// Variabila auxiliara pentru strazi
	info_street aux;
	// Variabile auxiliare pentru elementele unei cladiri (poate avea 3 niveluri)
	info_city_element temp;
	info_city_element temp2;
	info_city_element temp3;
	// Variabila auxiliara pentru spoturi
	info_spot tmp;

	// Se golesc vectorii pentru strazi, cladiri si elemente decorative (in caz ca se apasa "R" sa se regenereze orasul)
	streets_o.clear();
	streets_v.clear();
	buildings.clear();
	decorative.clear();

	/* Generarea strazilor */

	// Marginea din stanga a orasului
	aux.xc = -24.5;
	aux.zc = 0;
	// Strazile din marginile orasului au latimea 1
	aux.width = 1;
	aux.height = 50;
	streets_v.push_back(aux);

	// Strazile verticale la stanga de axa OZ
	// Se vor genera maxim 11 strazi la stanga de axa OZ + marginea din stanga a orasului.
	// Strazile vor avea latimi diferite si nu toate cele 11 strazi vor fi create
	for (int i = 11; i >= 1; i--) {
		chance = rand() % 100;
		// Exista o sansa de 70 la suta ca strada sa fie creata
		if (chance <= 70) {
			// Se creste dupa aceasta formula pentru ca intre strazi sa avem cel putin 
			// un patrat cu latura de o unitate pentru cladiri
			aux.xc = -0.5 - (i * 2);
			aux.zc = 0;
			// Latimea random (intre 0.1 si 1) a strazii 
			rand_dimension = ((float)(rand() % 91) / 100.0f) + 0.1f;
			aux.width = rand_dimension;
			aux.height = 50;
			streets_v.push_back(aux);
		}
	}

	// Bulevardul principal pe verticala (centrul in 0,0)
	aux.xc = 0;
	aux.zc = 0;
	// Latimea random (intre 0.5 si 1) a strazii - fiind bulevard principal il fac sa nu coboare sub 0.5 
	rand_dimension = ((float)(rand() % 51) / 100.0f) + 0.5f;
	aux.width = rand_dimension;
	aux.height = 50;
	streets_v.push_back(aux);

	// Strazile verticale la dreapta de axa OZ
	// Se vor genera maxim 11 strazi la dreapta de axa OZ + marginea din dreapta a orasului.
	// Strazile vor avea latimi diferite si nu toate cele 11 strazi vor fi create
	for (int i = 1; i < 12; i++) {
		chance = rand() % 100;
		// Exista o sansa de 70 la suta ca strada sa fie creata
		if (chance <= 70) {
			// Se creste dupa aceasta formula pentru ca intre strazi sa avem cel putin 
			// un patrat cu latura de o unitate pentru cladiri
			aux.xc = 0.5 + (i * 2);
			aux.zc = 0;
			// Latimea random (intre 0.1 si 1) a strazii
			rand_dimension = ((float)(rand() % 91) / 100.0f) + 0.1f;
			aux.width = rand_dimension;
			aux.height = 50;
			streets_v.push_back(aux);
		}
	}

	// Marginea din dreapta a orasului
	aux.xc = 24.5;
	aux.zc = 0;
	// Strazile din marginile orasului au latimea 1
	aux.width = 1;
	aux.height = 50;
	streets_v.push_back(aux);

	// Marginea de sus a orasului
	aux.xc = 0;
	// La punctele de intersectie nu se vedea ok daca strazile erau exact la aceeasi inaltime
	// asa ca strazile pe orizonatala vor fi afisate putin mai sus
	aux.yc += 0.0001f;
	aux.zc = -24.5;
	// Sa nu intre pe suprafata marginilor din stanga si dreapta a orasului
	aux.width = 48;
	// Strazile din marginile orasului au latimea 1
	aux.height = 1;
	streets_o.push_back(aux);

	// Strazile orizontale in spate fata de axa OX
	// Se vor genera maxim 11 strazi in spate fata de axa OX + marginea din spate a orasului.
	// Strazile vor avea latimi diferite si nu toate cele 11 strazi vor fi create
	for (int i = 11; i >= 1; i--) {
		chance = rand() % 100;
		// Exista o sansa de 70 la suta ca strada sa fie creata
		if (chance <= 70) {
			aux.xc = 0;
			// La punctele de intersectie nu se vedea ok daca strazile erau exact la aceeasi inaltime
			// asa ca strazile pe orizonatala vor fi afisate putin mai sus
			aux.yc += 0.0001f;
			// Se creste dupa aceasta formula pentru ca intre strazi sa avem cel putin 
			// un patrat cu latura de o unitate pentru cladiri
			aux.zc = -0.5 - (i * 2);
			// Sa nu intre pe suprafata marginilor din stanga si dreapta a orasului
			aux.width = 48;
			// Latimea random (intre 0.1 si 1) a strazii
			rand_dimension = ((float)(rand() % 91) / 100.0f) + 0.1f;
			aux.height = rand_dimension;
			streets_o.push_back(aux);
		}
	}

	// Bulevardul principal pe orizontal (in 0,0)
	aux.xc = 0;
	// La punctele de intersectie nu se vedea ok daca strazile erau exact la aceeasi inaltime
	// asa ca strazile pe orizonatala vor fi afisate putin mai sus
	aux.yc += 0.0001f;
	aux.zc = 0;
	// Sa nu intre pe suprafata marginilor din stanga si dreapta a orasului
	aux.width = 48;
	// Latimea random (intre 0.5 si 1) a strazii - fiind bulevard principal acesta nu va avea ai putin de 0.5
	rand_dimension = ((float)(rand() % 51) / 100.0f) + 0.5f;
	aux.height = rand_dimension;
	streets_o.push_back(aux);

	// Strazile orizontale in fata fata de axa OX
	// Se vor genera maxim 11 strazi in fata fata de axa OX + marginea din fata a orasului.
	// Strazile vor avea latimi diferite si nu toate cele 11 strazi vor fi create
	for (int i = 1; i < 12; i++) {
		chance = rand() % 100;
		// Exista o sansa de 70 la suta ca strada sa fie creata
		if (chance <= 70) {
			aux.xc = 0;
			// La punctele de intersectie nu se vedea ok daca strazile erau exact la aceeasi inaltime
			// asa ca strazile pe orizonatala vor fi afisate putin mai sus
			aux.yc += 0.0001f;
			// Se creste dupa aceasta formula pentru ca intre strazi sa avem cel putin 
			// un patrat cu latura de o unitate pentru cladiri
			aux.zc = 0.5 + (i * 2);
			// Sa nu intre pe suprafata marginilor din stanga si dreapta a orasului
			aux.width = 48;
			// Latimea random (intre 0.1 si 1) a strazii
			rand_dimension = ((float)(rand() % 91) / 100.0f) + 0.1f;
			aux.height = rand_dimension;
			streets_o.push_back(aux);
		}
	}

	// Marginea de jos a orasului
	aux.xc = 0;
	// La punctele de intersectie nu se vedea ok daca strazile erau exact la aceeasi inaltime
	// asa ca strazile pe orizonatala vor fi afisate putin mai sus
	aux.yc += 0.0001f;
	aux.zc = 24.5;
	// Sa nu intre pe suprafata marginilor din stanga si dreapta a orasului
	aux.width = 48;
	// Strazile din marginile orasului au latimea 1
	aux.height = 1;
	streets_o.push_back(aux);

	// Vector utilizat pentru a nu crea 2 elemente de tip politist in acelasi loc
	std::vector<info_city_element> verificari_politisti;

	/* Generarea politistilor */

	// Se stabileste ca numarul maxim de politisti va fi numarul de intersectii pe 4
	// Va fi cate un politist in intersectie
	int max_cops = (streets_o.size() * streets_v.size()) / 4;
	// Indecsi pentru a stii la intersectia dintre ce linie orizontala 
	// si verticala este pozitionat un politist
	int index_o = 0;
	int index_v = 0;

	for (int i = 0; i < max_cops; i++) {
		// Se calculeaza random intersectia
		index_o = rand() % streets_o.size();
		index_v = rand() % streets_v.size();

		temp.shader_type = "ShaderCop";
		// Pozitia in vectorul cu meshuri pentru elemente decorative pentru meshul de tip politist.
		temp.mesh_number = 0;
		// Pozitia in care va fi rotit politistul. Numarul pozitiei va fi inmultit cu pi / 2 pentru 
		// a obtine 4 pozitii (nord, sud, vest, est)
		temp.rotation = rand() % 4;
		// Cu cat va fi scalat
		temp.scale_x = 0.0001;
		temp.scale_y = 0.0001;
		temp.scale_z = 0.0001;
		// Pozitia politistului
		temp.position_xc = streets_v[index_v].xc;
		temp.position_zc = streets_o[index_o].zc;
		temp.position_yc = 0.001f;

		int j = 0;
		// Toti politistii creati sunt adaugati in vectorul "verificari_politisti"
		// La fiecare nou politist se verifica pozitiile tuturor celorlalti pentru a fi sigur
		// ca nu se suprapun
		while (j < i) {
			// Daca se suprapun se genereaza alta pozitie pentru politistul nou creat si iar se verifica cu toti ceilalti
			if (verificari_politisti[j].position_xc == temp.position_xc && verificari_politisti[j].position_zc == temp.position_zc) {
				index_o = rand() % streets_o.size();
				index_v = rand() % streets_v.size();

				temp.position_xc = streets_v[index_v].xc;
				temp.position_zc = streets_o[index_o].zc;
			
				j = 0;
			}
			else {
				j++;
			}
		}

		verificari_politisti.push_back(temp);
		decorative.push_back(temp);

	}

	/* Generarea elicopterelor */

	// Vector utilizat pentru a nu crea 2 elemente de tip elicopter in acelasi loc
	std::vector<info_city_element> verificari_elicoptere;

	// Se vor crea 10 elicoptere
	for (int i = 0; i < 10; i++) {

		temp.shader_type = "ShaderHelicopter";
		// Pozitia in vectorul cu meshuri pentru elemente decorative pentru meshul de tip elicopter.
		temp.mesh_number = 2;
		// Pozitia in care va fi rotit elicopterul. Numarul pozitiei va fi inmultit cu pi / 2 pentru 
		// a obtine 4 pozitii (nord, sud, vest, est)
		temp.rotation = rand() % 4;
		// Cu cat va fi scalat
		temp.scale_x = 0.001;
		temp.scale_y = 0.001;
		temp.scale_z = 0.001;
		// Pozitia elicopterului (x, z random intre marginile orasului si y intre 4 - 5 unitati ca sa fie deasupra cladirilor)
		temp.position_xc = (rand() % 49) - 24;
		temp.position_yc = ((float)(rand() % 11) / 10.0f) + 4.0f;
		temp.position_zc = (rand() % 49) - 24;

		int j = 0;
		// Toate elicopterele create sunt adaugate in vectorul "verificari_elicoptere"
		// La fiecare nou elicopter se verifica pozitiile tuturor celorlalte pentru a fi sigur
		// ca nu se suprapun
		while (j < i) {
			// Daca se suprapun se genereaza alte coordonate X, Z si iar se verifica cu toate celelalte
			if (verificari_elicoptere[j].position_xc == temp.position_xc && verificari_elicoptere[j].position_zc == temp.position_zc) {
				temp.position_xc = (rand() % 50) - 24;
				temp.position_zc = (rand() % 50) - 24;
				j = 0;
			}
			else {
				j++;
			}
		}

		verificari_elicoptere.push_back(temp);
		decorative.push_back(temp);
	}

	int k = 0;
	int l = 0;

	/* Generare banci si cladiri */

	// Fiecare zona cu blocuri va avea o banca pozitionata pe mijlocul unei laturi a dreptunghiului suprafetei 
	// pe care se afla blocurile. Aceasta variabila e folosita pentru a stii in ce pozitie se afla banca
	int bench_location = 0;
	// Se parcurge fiecare spatiu in care vor fi amplasate blocurile si bancile
	for (int i = 0; i < streets_o.size() - 1; i++) {
		for (int j = 0; j < streets_v.size() - 1; j++) {

			/* Generare banci */

			// Se pune cate o banca in fiecare zona de blocuri, random pe una din laturi
			bench_location = rand() % 4;
			temp.shader_type = "ShaderBench";
			// Pozitia in vectorul cu meshuri pentru elemente decorative pentru meshul de tip banca.
			temp.mesh_number = 1;
			// Pozitia in care va fi rotita banca. Numarul pozitiei va fi inmultit cu pi / 2 pentru 
			// a obtine 4 pozitii (nord, sud, vest, est)
			temp.rotation = bench_location;
			// Cu cat va fi scalat obiectul de tip banca
			temp.scale_x = 0.0002;
			temp.scale_y = 0.0002;
			temp.scale_z = 0.0002;

			temp.position_yc = 0;

			// In functie de locatie, pozitia bancii va fi diferita
			// Latura din dreapta
			if (bench_location == 0) {
				temp.position_xc = streets_v[j + 1].xc - (streets_v[j + 1].width / 2.0f) - 0.01;
				// Mijlocul laturii dintre cele 2 strazi orizontale
				temp.position_zc = ((streets_o[i + 1].zc - (streets_o[i + 1].height / 2.0f) 
									- (streets_o[i].zc + (streets_o[i].height / 2.0f))) / 2.0f) 
									+ streets_o[i].zc + (streets_o[i].height / 2.0f);
			}
			// Latura din stanga
			else if (bench_location == 2) {
				temp.position_xc = streets_v[j].xc + (streets_v[j].width / 2.0f) + 0.01;
				// Mijlocul laturii dintre cele 2 strazi orizontale
				temp.position_zc = ((streets_o[i + 1].zc - (streets_o[i + 1].height / 2.0f) 
									- (streets_o[i].zc + (streets_o[i].height / 2.0f))) / 2.0f) 
									+ streets_o[i].zc + (streets_o[i].height / 2.0f);
			}
			// Latura de jos
			else if (bench_location == 3) {
				// Mijlocul laturii dintre cele 2 strazi verticale
				temp.position_xc = ((streets_v[j + 1].xc - (streets_v[j + 1].width / 2.0f) 
									- (streets_v[j].xc + (streets_v[j].width / 2.0f))) / 2.0f) 
									+ streets_v[j].xc + (streets_v[j].width / 2.0f);
				temp.position_zc = streets_o[i + 1].zc - (streets_o[i + 1].height / 2.0f) - 0.01;
			}
			// Latura de sus
			else if (bench_location == 1) {
				// Mijlocul laturii dintre cele 2 strazi verticale
				temp.position_xc = ((streets_v[j + 1].xc - (streets_v[j + 1].width / 2.0f) 
									- (streets_v[j].xc + (streets_v[j].width / 2.0f))) / 2.0f) 
									+ streets_v[j].xc + (streets_v[j].width / 2.0f);
				temp.position_zc = streets_o[i].zc + (streets_o[i].height / 2.0f) + 0.01;
			}

			decorative.push_back(temp);

			/* Generare cladiri */

			/* Sunt 2 cazuri. Cazul cand am doar o cladire mare pe tot spatiul sau mai multe cladiri mici */
			/* Cand am doar o cladire mare, aceasta poate avea doar 2 nivele mai mari */
			/* Cand am mai multe cladiri, acestea pot avea 3 nivele */
			
			// Se calculeaza lungimea si latimea spatiului
			float sq_height = ((streets_o[i + 1].zc - (streets_o[i + 1].height / 2.0f) - (streets_o[i].zc + (streets_o[i].height / 2.0f))));
			float sq_width = ((streets_v[j + 1].xc - (streets_v[j + 1].width / 2.0f) - (streets_v[j].xc + (streets_v[j].width / 2.0f))));
			
			// Cladirile mari se vor amplasa doar pe spatii care au latimea si lungimea mai mici decat 1.5
			// Se calculeaza random daca un spatiu de acest tip va contine doar o cladire mare sau mai multe cladiri mai mici
			int big = 0; // La fiecare pas big va fi 0 => daca spatiul nu este corespunzator oricum vor fi cladiri mici pe el
			// Daca spatiul este corespunzator, se calculeaza random daca va fi o cladire mare sau nu.
			if (sq_height <= 1.5 && sq_width <= 1.5) {
				big = rand() % 2;
			}
			// Daca trebuie pusa o cladire mare
			if (big == 1) {
				// La cladiri rotatia va fi intre 0-7 pentru a putea avea 8 pozitii de roatie
				// Rotatia pentru cladiri mari va fi doar 0, 2, 4 sau 6 ca sa nu poata fi rotit "oblic" 
				// pentru ca ar putea iesi din suprafata ( daca e scalat maxim si e rotit oblic poate iesi din suprafata)
				temp.rotation = (rand() % 4) * 2;
				// Componenta nivelului 2 va avea aceeasi rotatie
				temp2.rotation = temp.rotation;
				// Se calculeaza tipul elementului de pe nivelul 1
				temp.mesh_number = rand() % 9;

				// Se calculeaza random daca va exista si nivelul 2 sau nu
				int chance_aux = rand() % 2;
				// Daca exista => nivelul 2 va avea aceeasi forma ca nivelul 1
				if (chance_aux == 1) {
					temp2.mesh_number = temp.mesh_number;
				}
				// Daca nu => setam meshul ca fiind 9 (tipul "null")
				else {
					temp2.mesh_number = 9;
				}

				// Scalarea maxima a nivelelor va fi minimul dintre laturile suprafetei dintre strazi
				int max_scale = min(sq_height, sq_width);

				// Se calculeaza factorul de scalarea a lui x (intre 0.2 si max_scale)
				temp.scale_x = ((float)(rand() % (max_scale * 100 - 20 + 1)) / 100.0f) + 0.2f;

				// Se calculeaza factorul de scalarea a lui y
				// Daca nu mai am nivel 2 => poate lua valori de la 0.5 la 4 unitati
				if (temp2.mesh_number == 9) {
					temp.scale_y = ((float)(rand() % 36) / 10.0f) + 0.5f;
				}
				// Daca am nivel 2 poate lua valori de la 0.5 la 2 unitati
				else {
					temp.scale_y = ((float)(rand() % 151) / 100.0f) + 0.5f;
				}

				// Se calculeaza factorul de scalarea a lui z (intre 0.2 si max_scale)
				temp.scale_z = ((float)(rand() % (max_scale * 100 - 20 + 1)) / 100.0f) + 0.2f;

				// Daca ambii factori de scalare x si z sunt sub 0.5 unitati (se doreste o cladire mai mare), 
				// atunci se va mari random unul din ele intre 0.8 si max_scale
				if (temp.scale_x <= 0.5f && temp.scale_z <= 0.5f) {
					int chance_xz = rand() % 2;
					// Cazul cand se mareste random x
					if (chance_xz == 0) {
						temp.scale_x = ((float)(rand() % (max_scale * 100 - 80 + 1)) / 100.0f) + 0.80f;
					}
					// Cazul cand se mareste random z
					else {
						temp.scale_z = ((float)(rand() % (max_scale * 100 - 80 + 1)) / 100.0f) + 0.80f;
					}
				}

				// Se calculeaza random valorile de scalare pentru nivelul 2 (chiar daca este de tip "null")
				// X si Z vor avea valorea minima a obiectului = nivel 1 - 0.1, iar valoarea maxima 
				// va fi cea a nivelului 1
				temp2.scale_x = ((float)(rand() % 11) / 100.0f) + (temp.scale_x - 0.1f);
				temp2.scale_y = ((float)(rand() % 151) / 100.0f) + 0.5f;
				temp2.scale_z = ((float)(rand() % 11) / 100.0f) + (temp.scale_z - 0.1f);

				// Obiectele vor fi plasate in centrul suprafetei curente
				temp.position_xc = (sq_width / 2.0f) + streets_v[j].xc + (streets_v[j].width / 2.0f);
				temp2.position_xc = temp.position_xc;

				temp.position_zc = (sq_height / 2.0f) + streets_o[i].zc + (streets_o[i].height / 2.0f);
				temp2.position_zc = temp.position_zc;

				temp.position_yc = temp.scale_y / 2.0f;
				temp2.position_yc = temp.scale_y + (temp2.scale_y / 2.0f);

				// Se genereaza spoturile
				// Fiecare cladire va avea maxim 4 spoturi, cate unul in fiecare parte,
				// cu pozitionare si directie diferite, dar nu vor fi mereu generate toate cele 4 spoturi.
				for (int ii = 0; ii < 4; ii++) {
					// Spotul din dreapta
					if (ii == 0) {
						chance = rand() % 2;
						if (chance == 1) {
							tmp.light_direction.y = -1;
							tmp.light_direction.z = 0;
							// Random intre -0.4 si -0.8
							tmp.light_direction.x = -(((float)(rand() % 5) / 10.0f) + 0.4f);

							tmp.light_position.z = temp.position_zc;
							// Distanta fata de cladire spre partea dreapta va fi 0.2 - 0.7 fata de marginea cladirii
							tmp.light_position.x = temp.position_xc + (temp.scale_x / 2.0f) + (((float)(rand() % 6) / 10.0f) + 0.2f);

							// Inaltimea spotului va fi intre 0.2 si inaltimea cladirii + 0.5 unitati
							if (temp2.mesh_number != 9) {
								tmp.light_position.y = ((float)(rand() % (int)((temp2.position_yc + (temp2.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
							}
							else {
								tmp.light_position.y = ((float)(rand() % (int)((temp.position_yc + (temp.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
							}

							temp.spots.push_back(tmp);
							temp.spots_number++;

							temp2.spots.push_back(tmp);
							temp2.spots_number++;

						}
					}
					// Spotul din stanga
					else if (ii == 1) {
						chance = rand() % 2;
						if (chance == 1) {
							tmp.light_direction.y = -1;
							tmp.light_direction.z = 0;
							// Random intre 0.4 si 0.8
							tmp.light_direction.x = (((float)(rand() % 5) / 10.0f) + 0.4f);

							// Inaltimea spotului va fi intre 0.2 si inaltimea cladirii + 0.5 unitati
							if (temp2.mesh_number != 9) {
								tmp.light_position.y = ((float)(rand() % (int)((temp2.position_yc + (temp2.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
							}
							else {
								tmp.light_position.y = ((float)(rand() % (int)((temp.position_yc + (temp.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
							}

							tmp.light_position.z = temp.position_zc;
							// Distanta fata de cladire spre partea stanga va fi -0.2 - -0.7 fata de marginea cladirii
							tmp.light_position.x = temp.position_xc - (temp.scale_x / 2.0f) - (((float)(rand() % 6) / 10.0f) + 0.2f);

							temp.spots.push_back(tmp);
							temp.spots_number++;

							temp2.spots.push_back(tmp);
							temp2.spots_number++;
						}
					}
					// Spotul din fata
					else if (ii == 2) {
						chance = rand() % 2;
						if (chance == 1) {
							tmp.light_direction.y = -1;
							// Random intre -0.4 si -0.8
							tmp.light_direction.z = -(((float)(rand() % 5) / 10.0f) + 0.4f);
							
							tmp.light_direction.x = 0;

							// Inaltimea spotului va fi intre 0.2 si inaltimea cladirii + 0.5 unitati
							if (temp2.mesh_number != 9) {
								tmp.light_position.y = ((float)(rand() % (int)((temp2.position_yc + (temp2.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
							}
							else {
								tmp.light_position.y = ((float)(rand() % (int)((temp.position_yc + (temp.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
							}

							// Distanta fata de cladire spre partea din fata va fi 0.2 - 0.7 fata de marginea cladirii
							tmp.light_position.z = temp.position_zc + (temp.scale_z / 2.0f) + (((float)(rand() % 6) / 10.0f) + 0.2f);
							tmp.light_position.x = temp.position_xc;

							temp.spots.push_back(tmp);
							temp.spots_number++;

							temp2.spots.push_back(tmp);
							temp2.spots_number++;
						}
					}
					// Spotul din spate
					else if (ii == 3) {
						chance = rand() % 2;
						if (chance == 1) {
							tmp.light_direction.y = -1;
							// Random intre 0.4 si 0.8
							tmp.light_direction.z = (((float)(rand() % 5) / 10.0f) + 0.4f);

							tmp.light_direction.x = 0;

							// Inaltimea spotului va fi intre 0.2 si inaltimea cladirii + 0.5 unitati
							if (temp2.mesh_number != 9) {
								tmp.light_position.y = ((float)(rand() % (int)((temp2.position_yc + (temp2.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
							}
							else {
								tmp.light_position.y = ((float)(rand() % (int)((temp.position_yc + (temp.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
							}

							// Distanta fata de cladire spre partea din fata va fi 0.2 - 0.7 fata de marginea cladirii
							tmp.light_position.z = temp.position_zc - (temp.scale_z / 2.0f) - (((float)(rand() % 6) / 10.0f) + 0.2f);
							tmp.light_position.x = temp.position_xc;

							temp.spots.push_back(tmp);
							temp.spots_number++;

							temp2.spots.push_back(tmp);
							temp2.spots_number++;
						}
					}

				}

				// Se adauga primul nivel
				buildings.push_back(temp);
				// Daca exista si nivel 2 => se adauga si acesta
				if (temp2.mesh_number != 9) {
					buildings.push_back(temp2);
				}

				// Se golesc spoturile din temp-uri pentru a nu ramane in variabila
				temp.spots.clear();
				temp.spots_number = 0;

				temp2.spots.clear();
				temp2.spots_number = 0;

			}

			// Cazul cand sunt mai multe cladiri pe aceeasi suprafata si pot avea 3 nivele
			else {
				// In cadrul suprafetei ne ducem din 0.5 in 0.5 (din stanga in dreapta si de sus in jos)
				// Ne ducem din 0.5 in 0.5 pentru a avea loc de scalare (scalarea maxima va fi 0.4)
				// Se pleaca de la 0.25 fata de linia din stanga si de linia de sus tocmai pentru a avea loc de scalare
				// si se merge pana cand nu mai este loc de strada din dreapta sau de jos
				l = 0;
				while (streets_o[i].zc + (streets_o[i].height / 2.0f) + 0.25 + (l * 0.5) + 0.25 <= streets_o[i + 1].zc - (streets_o[i + 1].height / 2.0f)) {
					k = 0;
					while (streets_v[j].xc + (streets_v[j].width / 2.0f) + 0.25 + (k * 0.5) + 0.25 <= streets_v[j + 1].xc - (streets_v[j + 1].width / 2.0f)) {

						chance = 0;

						// Se calculeaza rotatia random
						temp.rotation = rand() % 8;
						temp2.rotation = temp.rotation;
						temp3.rotation = temp.rotation;
						
						// Se calculeaza random tipul primului nivel
						temp.mesh_number = rand() % 9;

						// Se claculeaza random tipul celui de-al doilea nivel
						// Daca se doresc mai putine cladiri cu 2-3 etaje => in loc de 26 se pune o valoare mult mai mare pentru a mari sansa
						// ca tipul nivelului 2 sa fie null.
						temp2.mesh_number = rand() % 26;
						// Daca niveulul 1 este prisma triunghiulara vom avea doar prisma triunghiulara deasupra sau nimic
						if (temp.mesh_number == 8) {
							chance = rand() % 2;
							// Avem tot prisma triunghiulara
							if (chance == 0) {
								temp2.mesh_number = 8;
							}
							// Nu avem nimic
							else {
								temp2.mesh_number = 9;
							}

						}
						// Daca valoarea lui "chance" este intre 9 si 21 => nivelul 2 va avea acelasi tip ca si nivelul 1
						else if (temp2.mesh_number > 9 && temp2.mesh_number < 21) {
							temp2.mesh_number = temp.mesh_number;
						}
						// Daca valoarea lui "chance" este >= 21 => nu mai avem alte nivele
						else if (temp2.mesh_number >= 21) {
							temp2.mesh_number = 9;
						}

						// Daca nivelul 2 este "null" => nu avem nivel 2 => nu avem nici nivel 3
						if (temp2.mesh_number == 9) {
							temp3.mesh_number = 9;
						}
						else {
							// Se calculeaza random tipul nivelului 3
							// Daca se doresc mai putine cladiri cu 3 etaje => in loc de 70 se pune o valoare mult mai mare pentru a mari sansa
							// ca tipul nivelului 3 sa fie null.
							temp3.mesh_number = rand() % 70;
							// Daca niveulul 1 este prisma triunghiulara vom avea doar prisma triunghiulara deasupra sau nimic
							if (temp.mesh_number == 8 || temp2.mesh_number == 8) {
								chance = rand() % 2;
								// Avem tot prisma triunghiulara
								if (chance == 0) {
									temp3.mesh_number = 8;
								}
								// Nu avem nimic
								else {
									temp3.mesh_number = 9;
								}

							}
							// Daca valoare lui "chance" este intre 12 - 25 => avem acelasi tip ca si nivelul 2
							// Trebuie sa fie mai mare decat 12 pentru ca la nivelul 3 putem avea si "sfera", "con" sau "piramida".
							else if (temp3.mesh_number > 12 && temp3.mesh_number < 25) {
								temp3.mesh_number = temp2.mesh_number;
							}
							// Daca "chance" este >=25 => nu vom avea nivel 3
							else if (temp3.mesh_number >= 25) {
								temp3.mesh_number = 9;
							}
						}

						// Se calculeaza random scalarea pentru nivelul 1
						// X si Z vor avea valori intre 0.2 si 0.4 unitati
						temp.scale_x = ((float)(rand() % 21) / 100.0f) + 0.2f;

						// Daca cladirea are doar un nivel => y poate avea de la 0.2 pana la 2 unitati
						if (temp2.mesh_number == 9) {
							temp.scale_y = ((float)(rand() % 18) / 10.0f) + 0.2f;
						}
						// Daca are mai multe nivele => y poate avea doar de la 0.2 pana la 1 unitati
						else {
							temp.scale_y = ((float)(rand() % 81) / 100.0f) + 0.2f;
						}

						temp.scale_z = ((float)(rand() % 21) / 100.0f) + 0.2f;

						// Se calculeaza scalarile pentru nivelul 2
						// Daca este acelasi tip ca la nivelul 1 => x si z pot varia intre valoare nivel 1 - 0.05 si valoare nivel 1
						if (temp2.mesh_number == temp.mesh_number) {
							temp2.scale_x = ((float)(rand() % 6) / 100.0f) + (temp.scale_x - 0.05f);
							// y poate avea valori intre 0.2 si 1 unitati
							temp2.scale_y = ((float)(rand() % 81) / 100.0f) + 0.2f;
							temp2.scale_z = ((float)(rand() % 6) / 100.0f) + (temp.scale_z - 0.05f);
						}
						// Daca nu este acelasi tip => va avea x si z = valorile de la nivelul 1 - 0.05
						else {
							temp2.scale_x = temp.scale_x - 0.05f;
							// y poate avea valori intre 0.2 si 1 unitati
							temp2.scale_y = ((float)(rand() % 81) / 100.0f) + 0.2f;
							temp2.scale_z = temp.scale_z - 0.05f;
						}

						// Se calculeaza scalarile pentru nivelul 3
						// Daca este acelasi tip ca la nivelul 2 => x si z pot varia intre valoare nivel 2 - 0.05 si valoare nivel 2
						if (temp3.mesh_number == temp2.mesh_number) {
							temp3.scale_x = ((float)(rand() % 6) / 100.0f) + (temp2.scale_x - 0.05f);
							// y poate avea valori intre 0.2 si 1 unitati
							temp3.scale_y = ((float)(rand() % 81) / 100.0f) + 0.2f;
							temp3.scale_z = ((float)(rand() % 6) / 100.0f) + (temp2.scale_z - 0.05f);
						}
						// Daca nu este acelasi tip => va avea x si z = valorile de la nivelul 2 - 0.05
						else {
							temp3.scale_x = temp2.scale_x - 0.05f;
							temp3.scale_z = temp2.scale_z - 0.05f;

							// Daca tipul nivelului 3 este "con", "sfera" sau "prisma triunghiulara" y va avea o valoare
							// cuprinsa doar intre 0.1 si 0.3
							if (temp3.mesh_number == 10 || temp3.mesh_number == 11 || temp3.mesh_number == 12) {
								temp3.scale_y = ((float)(rand() % 21) / 100.0f) + 0.1f;
							}
							// Altfel y poate avea valori intre 0.1 si 1 unitati
							else {
								temp3.scale_y = ((float)(rand() % 81) / 100.0f) + 0.1f;
							}
						}

						// Se calculeaza pozitiile celor 3 nivele
						temp.position_xc = streets_v[j].xc + (streets_v[j].width / 2.0f) + 0.25 + (k * 0.5);
						temp2.position_xc = temp.position_xc;
						temp3.position_xc = temp.position_xc;

						temp.position_zc = streets_o[i].zc + (streets_o[i].height / 2.0f) + 0.25 + (l * 0.5);
						temp2.position_zc = temp.position_zc;
						temp3.position_zc = temp.position_zc;


						temp.position_yc = temp.scale_y / 2.0f;
						temp2.position_yc = temp.scale_y + (temp2.scale_y / 2.0f);
						temp3.position_yc = temp.scale_y + temp2.scale_y + (temp3.scale_y / 2.0f);

						// Se genereaza spoturile
						// Fiecare cladire va avea maxim 4 spoturi, cate unul in fiecare parte,
						// cu pozitionare si directie diferite, dar nu vor fi mereu generate toate cele 4 spoturi.
						for (int ii = 0; ii < 4; ii++) {
							// Spotul din dreapta
							if (ii == 0) {
								chance = rand() % 2;
								if (chance == 1) {
									tmp.light_direction.y = -1;
									tmp.light_direction.z = 0;
									// Random intre -0.4 si -0.8
									tmp.light_direction.x = -(((float)(rand() % 5) / 10.0f) + 0.4f);

									tmp.light_position.z = temp.position_zc;
									// Distanta fata de cladire spre partea dreapta va fi 0.2 - 0.7 fata de marginea cladirii
									tmp.light_position.x = temp.position_xc + (temp.scale_x / 2.0f) + (((float)(rand() % 6) / 10.0f) + 0.2f);

									// Inaltimea spotului va fi intre 0.2 si inaltimea cladirii + 0.5 unitati
									if (temp3.mesh_number != 9) {
										tmp.light_position.y = ((float)(rand() % (int)((temp3.position_yc + (temp3.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}
									else if (temp2.mesh_number != 9) {
										tmp.light_position.y = ((float)(rand() % (int)((temp2.position_yc + (temp2.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}
									else {
										tmp.light_position.y = ((float)(rand() % (int)((temp.position_yc + (temp.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}

									temp.spots.push_back(tmp);
									temp.spots_number++;

									temp2.spots.push_back(tmp);
									temp2.spots_number++;

									temp3.spots.push_back(tmp);
									temp3.spots_number++;

								}
							}
							// Spotul din stanga
							else if (ii == 1) {
								chance = rand() % 2;
								if (chance == 1) {
									tmp.light_direction.y = -1;
									tmp.light_direction.z = 0;
									// Random intre 0.4 si 0.8
									tmp.light_direction.x = (((float)(rand() % 5) / 10.0f) + 0.4f);

									// Inaltimea spotului va fi intre 0.2 si inaltimea cladirii + 0.5 unitati
									if (temp3.mesh_number != 9) {
										tmp.light_position.y = ((float)(rand() % (int)((temp3.position_yc + (temp3.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}
									else if (temp2.mesh_number != 9) {
										tmp.light_position.y = ((float)(rand() % (int)((temp2.position_yc + (temp2.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}
									else {
										tmp.light_position.y = ((float)(rand() % (int)((temp.position_yc + (temp.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}

									tmp.light_position.z = temp.position_zc;
									// Distanta fata de cladire spre partea stanga va fi -0.2 - -0.7 fata de marginea cladirii
									tmp.light_position.x = temp.position_xc - (temp.scale_x / 2.0f) - (((float)(rand() % 6) / 10.0f) + 0.2f);

									temp.spots.push_back(tmp);
									temp.spots_number++;

									temp2.spots.push_back(tmp);
									temp2.spots_number++;

									temp3.spots.push_back(tmp);
									temp3.spots_number++;
								}
							}
							// Spotul din fata
							else if (ii == 2) {
								chance = rand() % 2;
								if (chance == 1) {
									tmp.light_direction.y = -1;
									// Random intre -0.4 si -0.8
									tmp.light_direction.z = -(((float)(rand() % 5) / 10.0f) + 0.4f);

									tmp.light_direction.x = 0;

									// Inaltimea spotului va fi intre 0.2 si inaltimea cladirii + 0.5 unitati
									if (temp3.mesh_number != 9) {
										tmp.light_position.y = ((float)(rand() % (int)((temp3.position_yc + (temp3.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}
									else if (temp2.mesh_number != 9) {
										tmp.light_position.y = ((float)(rand() % (int)((temp2.position_yc + (temp2.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}
									else {
										tmp.light_position.y = ((float)(rand() % (int)((temp.position_yc + (temp.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}

									// Distanta fata de cladire spre partea din fata va fi 0.2 - 0.7 fata de marginea cladirii
									tmp.light_position.z = temp.position_zc + (temp.scale_z / 2.0f) + (((float)(rand() % 6) / 10.0f) + 0.2f);
									tmp.light_position.x = temp.position_xc;

									temp.spots.push_back(tmp);
									temp.spots_number++;

									temp2.spots.push_back(tmp);
									temp2.spots_number++;

									temp3.spots.push_back(tmp);
									temp3.spots_number++;
								}
							}
							// Spotul din spate
							else if (ii == 3) {
								chance = rand() % 2;
								if (chance == 1) {
									tmp.light_direction.y = -1;
									// Random intre 0.4 si 0.8
									tmp.light_direction.z = (((float)(rand() % 5) / 10.0f) + 0.4f);

									tmp.light_direction.x = 0;

									// Inaltimea spotului va fi intre 0.2 si inaltimea cladirii + 0.5 unitati
									if (temp3.mesh_number != 9) {
										tmp.light_position.y = ((float)(rand() % (int)((temp3.position_yc + (temp3.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}
									else if (temp2.mesh_number != 9) {
										tmp.light_position.y = ((float)(rand() % (int)((temp2.position_yc + (temp2.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}
									else {
										tmp.light_position.y = ((float)(rand() % (int)((temp.position_yc + (temp.scale_y / 2.0f) + 0.4) * 10)) / 10.0f) + 0.2f;
									}

									// Distanta fata de cladire spre partea din fata va fi 0.2 - 0.7 fata de marginea cladirii
									tmp.light_position.z = temp.position_zc - (temp.scale_z / 2.0f) - (((float)(rand() % 6) / 10.0f) + 0.2f);
									tmp.light_position.x = temp.position_xc;

									temp.spots.push_back(tmp);
									temp.spots_number++;

									temp2.spots.push_back(tmp);
									temp2.spots_number++;

									temp3.spots.push_back(tmp);
									temp3.spots_number++;
								}
							}

						}

						// Se adauga primul nivel in vectorul cu elementele cladirilor
						buildings.push_back(temp);
						// Daca exista si nivel 2 se adauga si acesta
						if (temp2.mesh_number != 9) {
							buildings.push_back(temp2);
							// Daca exista nivel 3 se adauga si acesta
							if (temp3.mesh_number != 9) {
								buildings.push_back(temp3);
							}
						}

						// Se golesc spoturile din temp-uri pentru a nu ramane in variabila
						temp.spots.clear();
						temp.spots_number = 0;

						temp2.spots.clear();
						temp2.spots_number = 0;

						temp3.spots.clear();
						temp3.spots_number = 0;
						
						k++;
					}

					l++;
				}
			}
		}
	}

	// Se afiseaza numarul de elemente al cladirilor si numarul de elemente decorative
	cout << "Elemente cladiri: " << buildings.size() << endl;
	cout << "Elemente decorative: " << decorative.size() << endl;
	cout << endl;
}

/* Functie pentru adaugarea meshurilor, shaderelor si texturilor */
void Tema3::Add_Meshes_Shaders_Textures() {
	const string textureLoc = "Source/Laboratoare/Tema3/Textures/";
	// Meshuri
	{
		Mesh* mesh = new Mesh("cube");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("cone");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "cone.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("cylinder");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "cylinder.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("cylinderonecut");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "cylinderonecut.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("cylindertwocut");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "cylindertwocut.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("hexagon");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "hexagon.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("pentagon");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "pentagon.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("octagon");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "octagon.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("pentagontaiat");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "pentagontaiat.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("pyramid");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "pyramid.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("triangularprism");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "triangularprism.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("decorativ_cop");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "man_free.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("decorativ_bench");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "bench.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("decorativ_helicopter");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema3", "helicopter.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Shaders
	{
		Shader *shader = new Shader("ShaderStreetsO");
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/VertexShader_streets_o.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/FragmentShaderStrazi.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader *shader = new Shader("ShaderStreetsV");
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/VertexShader_streets_v.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/FragmentShaderStrazi.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader *shader = new Shader("ShaderCladiri");
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/FragmentShaderCladiri.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader *shader = new Shader("ShaderSuprafata");
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/VertexShader_surface.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/FragmentShaderSuprafata.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader *shader = new Shader("ShaderCop");
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/FragmentShaderPolitisti.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader *shader = new Shader("ShaderBench");
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/FragmentShaderBanci.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader *shader = new Shader("ShaderHelicopter");
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/FragmentShaderElicoptere.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Texturi
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "street_o.jpg").c_str(), GL_REPEAT);
		mapTextures["street_orizontal"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "street_v.jpg").c_str(), GL_REPEAT);
		mapTextures["street_vertical"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "suprafata.jpg").c_str(), GL_REPEAT);
		mapTextures["surface"] = texture;
	}

}