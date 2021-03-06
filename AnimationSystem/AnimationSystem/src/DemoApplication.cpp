// Author : Lewis Ward (http://www.lewis-ward.com)
// Program : Animation System Tech Demo
// Date    : 12/02/2015
// � 2015-2016 Lewis Ward. All rights reserved.
#include "DemoApplication.h"

#define ANIMPATH "./animations/"

Application::Application()
{
	m_eventCode = 0;
	m_state = 0; // 0 = main menu, 1 = controls, 2 = animation system
	m_currentState = Idle;

	 m_movement = new bool[]{
		false, // forward 
		false, // moveLeft
		false, // moveRight
		false, // collision detected? 
		false, // rotate camera left
		false, // rotate camera right
		false, // zoom camera in
		false  // zoom camera out
	};

	// setup GLEW, if falis quit application
	if (!initGLEW())
	{
		m_events.forceQuit();
	}
	else
	{
		// load animated meshes, non-animated meshes and textures
		m_mesh[0] = new Mesh(ANIMPATH"ExoIdle.amesh", "meshes/ExoCollision.meshes");
		m_mesh[1] = new Mesh(ANIMPATH"ExoWalk.amesh", "meshes/ExoCollision.meshes");

		m_object.resize(10);
		m_object[0] = new Object("meshes/Building_Shop.meshes", "meshes/Building_ShopCollision.meshes");
		m_object[1] = new Object("meshes/Floor.meshes", "meshes/Floor.meshes");
		m_object[2] = new Object("meshes/Building_Big.meshes", "meshes/Building_BigCollision.meshes");
		m_object[3] = new Object("meshes/Building_SmallHouse.meshes", "meshes/Building_SmallHouseCollision.meshes");
		m_object[4] = new Object("meshes/Building_Flats.meshes", "meshes/Building_FlatsCollision.meshes");
		m_object[5] = new Object("meshes/Wall.meshes", "meshes/Wall.meshes");
		m_object[6] = new Object("meshes/Wall.meshes", "meshes/Wall.meshes");
		m_object[7] = new Object("meshes/Wall.meshes", "meshes/Wall.meshes");
		m_object[8] = new Object("meshes/Wall.meshes", "meshes/Wall.meshes");
		m_object[9] = new Object("meshes/Building_Shop.meshes", "meshes/Building_ShopCollision.meshes");

		m_texture = new Texture("images/Floor.png");
		m_exoTexture = new Texture("images/Exo.png");
		m_bigTexture = new Texture("images/Big.png");
		m_shopTexture = new Texture("images/Shop.png");
		m_houseTexture = new Texture("images/Houses.png");
		m_wallTexture = new Texture("images/Wall.png");
		m_mainMenuTexture = new Texture("images/mainMenu.png");
		m_contMenuTexture = new Texture("images/controllsMenu.png");

		// scale and rotate the objects
		glm::mat4 scale;
		scale[0].x = 5;
		scale[1].y = 5;
		scale[2].z = 5;
		m_object[0]->scale(scale);
		m_object[9]->scale(scale);
		scale[0].x = 105;
		scale[1].y = 105;
		scale[2].z = 105;
		m_object[1]->scale(scale);
		scale[0].x = 3;
		scale[1].y = 3;
		scale[2].z = 3;
		m_object[2]->scale(scale);
		scale[0].x = 0.7;
		scale[1].y = 0.7;
		scale[2].z = 0.7;
		m_object[3]->scale(scale);
		scale[0].x = 5;
		scale[1].y = 5;
		scale[2].z = 5;
		m_object[4]->scale(scale);

		// walls
		scale[0].x = 500;
		scale[1].y = 50;
		scale[2].z = 5;
		m_object[5]->scale(scale);
		m_object[6]->scale(scale);
		scale[0].x = 5;
		scale[1].y = 50;
		scale[2].z = 500;
		m_object[7]->scale(scale);
		m_object[8]->scale(scale);

		// apply translation and rotation
		m_object[0]->translate(glm::vec3(-75.0f, 0.0f, -50.0f));
		m_object[9]->rotate(0.0f, 180.0f);
		m_object[9]->translate(glm::vec3(-110.0f, 0.0f, -50.0f));
		m_object[1]->translate(glm::vec3(0.0f, 0.0f, 20.0f));
		m_object[2]->translate(glm::vec3(58.0f, 0.0f, -30.0f));
		m_object[3]->rotate(0.0f, 180.0f);
		m_object[3]->translate(glm::vec3(-58.0f, 0.0f, 200.0f));
		m_object[4]->rotate(0.0f, 180.0f);
		m_object[4]->translate(glm::vec3(225.0f, 0.0f, 200.0f));
		m_object[5]->rotate(0.0f, 180.0f);
		m_object[5]->translate(glm::vec3(0.0f, 0.0f, 270.0f));
		m_object[6]->translate(glm::vec3(0.0f, 0.0f, -200.0f));
		m_object[7]->translate(glm::vec3(-250.0f, 0.0f, 40.0f));
		m_object[8]->translate(glm::vec3(250.0f, 0.0f, 40.0f));

		// create camera
		m_camera = std::make_shared<Camera>(m_window.width(), m_window.height());

		m_currentFrame = 0.0f;

		// shader programs
		m_program = new gls::Program();
		m_objects = new gls::Program();
		m_menuProgram = new gls::Program();

		// create an array of all shaders to load
		gls::Shader shaders[] = {
			gls::Shader("shaders/wall.vtx.glsl", gls::sVERTEX), ///< joint vertex shader
			gls::Shader("shaders/wall.pix.glsl", gls::sFRAGMENT), ///< joint fragment shader
			gls::Shader("shaders/object.vtx.glsl", gls::sVERTEX), ///< object vertex shader
			gls::Shader("shaders/object.pix.glsl", gls::sFRAGMENT), ///< object fragment shader
			gls::Shader("shaders/flatImage.vtx.glsl", gls::sVERTEX), ///< flatImage vertex shader
			gls::Shader("shaders/flatImage.pix.glsl", gls::sFRAGMENT), ///< flatImage fragment shader
		};
		
		// create shader programs
		m_program->create(&shaders[0], &shaders[1]);
		m_objects->create(&shaders[2], &shaders[3]);
		m_menuProgram->create(&shaders[4], &shaders[5]);

		// create menu system
		m_menu = new Menu();

		// create AABB's for the buttons that resize based on the window size
		AABB2 start(glm::vec2(30 * (m_window.width() / 512.0f), 381 * (m_window.height() / 512.0f)), glm::vec2(242 * (m_window.width() / 512.0f), 438 * (m_window.height() / 512.0f)));
		AABB2 controls(glm::vec2(272 * (m_window.width() / 512.0f), 381 * (m_window.height() / 512.0f)), glm::vec2(484 * (m_window.width() / 512.0f), 438 * (m_window.height() / 512.0f)));
		m_menu->addButton(start);
		m_menu->addButton(controls);
	}
}
Application::~Application()
{
	delete m_mainMenuTexture;
	delete m_contMenuTexture;
	delete m_texture;
	delete m_exoTexture;
	delete m_bigTexture;
	delete m_shopTexture;
	delete m_houseTexture;
	delete m_wallTexture;
	delete m_program;
	delete m_objects;
	delete m_menuProgram;
	delete m_menu;
	m_menu = nullptr;
	m_mainMenuTexture = nullptr;
	m_contMenuTexture = nullptr;
	m_program = nullptr;
	m_objects = nullptr;
	m_menuProgram = nullptr;
	m_texture = nullptr;
	m_exoTexture = nullptr;
	m_bigTexture = nullptr;
	m_shopTexture = nullptr;
	m_houseTexture = nullptr;
	m_wallTexture = nullptr;

	m_mesh[0]->~Mesh();
	m_mesh[1]->~Mesh();
	delete m_mesh[0];
	delete m_mesh[1];

	for (int i = 0; i < m_object.size(); ++i)
	{
		delete m_object[i];
		m_object[i] = nullptr;
	}

	m_camera->~Camera();
	m_camera.~shared_ptr();
	m_camera = nullptr;

	m_object.clear();
	m_window.~Window();
}
void Application::draw()
{
	// enable OpenGL 2D textures and depth testing
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	// clear the colour and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw animation system
	if (m_state == 2)
	{
		// get the MVP martix
		glm::mat4x4 MVP;
		glm::mat4x4 V = m_camera.get()->viewMatrix();
		glm::mat4x4 P = m_camera.get()->projectionMatrix();
		glm::mat4 model = m_mesh[m_currentState]->getModelMatrix();

		// compute the Model-View-Project Matrix
		glm::mat4x4 MV = V * model;
		MVP = P * MV;

		// bind program
		m_objects->bind();
		// bind textures
		m_texture->bind(0);
		m_exoTexture->bind(1);
		m_bigTexture->bind(2);
		m_shopTexture->bind(3);
		m_houseTexture->bind(4);

		m_objects->uniform_Matrix4("mvp", 1, false, MVP);
		m_objects->uniform_Matrix4("mv", 1, false, MV);
		m_objects->uniform_1i("texture", 1);
		m_objects->uniform_4f("ambient", 0.0f, 0.0f, 0.3f, 1.0f);
		m_objects->uniform_4f("diffuse", 1.0f, 1.0f, 1.0f, 1.0f);
		m_objects->uniform_4f("specular", 0.5f, 0.5f, 0.55f, 1.0f);

		m_mesh[m_currentState]->drawObject();

		// recompute MV/MVP matrix
		MV = V * m_object[0]->matrix();
		MVP = P * MV;

		// set shader uniforms
		m_objects->uniform_Matrix4("mvp", 1, false, MVP);
		m_objects->uniform_Matrix4("mv", 1, false, MV);
		m_objects->uniform_1i("texture", 3);
		m_objects->uniform_4f("specular", 0.0f, 0.0f, 0.15f, 1.0f);

		// draw
		m_object[0]->draw();

		MV = V * m_object[9]->matrix();
		MVP = P * MV;
		m_objects->uniform_Matrix4("mvp", 1, false, MVP);
		m_objects->uniform_Matrix4("mv", 1, false, MV);
		m_object[9]->draw();

		// recompute MV/MVP matrix, set uniforms and draw objects
		MV = V * m_object[3]->matrix();
		MVP = P * MV;
		m_objects->uniform_1i("texture", 4);
		m_objects->uniform_Matrix4("mvp", 1, false, MVP);
		m_objects->uniform_Matrix4("mv", 1, false, MV);
		m_object[3]->draw();

		MV = V * m_object[2]->matrix();
		MVP = P * MV;
		m_objects->uniform_1i("texture", 2);
		m_objects->uniform_Matrix4("mvp", 1, false, MVP);
		m_objects->uniform_Matrix4("mv", 1, false, MV);
		m_object[2]->draw();
		MV = V * m_object[4]->matrix();
		MVP = P * MV;
		m_objects->uniform_Matrix4("mvp", 1, false, MVP);
		m_objects->uniform_Matrix4("mv", 1, false, MV);
		m_objects->uniform_1i("texture", 3);
		m_object[4]->draw();

		// unbind textures
		m_texture->unbind();
		m_exoTexture->unbind();
		m_bigTexture->unbind();
		m_shopTexture->unbind();
		m_houseTexture->unbind();
		m_wallTexture->unbind();

		// unbind program
		m_objects->unbind();

		// bind program for walls
		m_program->bind();

		// bind textures
		m_wallTexture->bind(5);

		// floor
		MV = V * m_object[1]->matrix();
		MVP = P * MV;
		m_objects->uniform_1i("texture", 0);
		m_objects->uniform_Matrix4("mvp", 1, false, MVP);
		m_objects->uniform_Matrix4("mv", 1, false, MV);
		m_object[1]->draw();

		// walls 
		MV = V * m_object[5]->matrix();
		MVP = P * MV;
		m_program->uniform_1i("texture", 5);
		m_program->uniform_1f("textureRepeat", 12.0f);
		m_program->uniform_Matrix4("mvp", 1, false, MVP);
		m_program->uniform_Matrix4("mv", 1, false, MV);
		m_program->uniform_4f("ambient", 0.0f, 0.0f, 0.3f, 1.0f);
		m_program->uniform_4f("diffuse", 1.0f, 1.0f, 1.0f, 1.0f);
		m_program->uniform_4f("specular", 0.5f, 0.5f, 0.55f, 1.0f);
		m_object[5]->draw();
		MV = V * m_object[6]->matrix();
		MVP = P * MV;
		m_program->uniform_Matrix4("mvp", 1, false, MVP);
		m_program->uniform_Matrix4("mv", 1, false, MV);
		m_object[6]->draw();
		MV = V * m_object[7]->matrix();
		MVP = P * MV;
		m_program->uniform_Matrix4("mvp", 1, false, MVP);
		m_program->uniform_Matrix4("mv", 1, false, MV);
		m_object[7]->draw();
		MV = V * m_object[8]->matrix();
		MVP = P * MV;
		m_program->uniform_Matrix4("mvp", 1, false, MVP);
		m_program->uniform_Matrix4("mv", 1, false, MV);
		m_object[8]->draw();

		m_wallTexture->unbind();

		m_program->unbind();
	}
	
	// draw main menu
	if (m_state == 0)
	{
		m_menuProgram->bind();
			m_mainMenuTexture->bind(0);
			m_menuProgram->uniform_1i("texture", 0);
			m_menu->draw();
			m_mainMenuTexture->unbind();
		m_menuProgram->unbind();
	}
	else if (m_state == 1) // draw control menu
	{
		m_menuProgram->bind();
			m_contMenuTexture->bind(0);
			m_menuProgram->uniform_1i("texture", 0);
			m_menu->draw();
			m_mainMenuTexture->unbind();
		m_menuProgram->unbind();
	}

	// disable OpenGL textures and depth testing
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	// swap the SDL window
	SDL_GL_SwapWindow(m_window.getWindow());
}
void Application::update(float dt)
{
	// get lastest event 
	m_eventCode = m_events.update();

	// update keybaord and Xbox controller
	keybaordMovementUpdate();
	m_controller.update(dt);

	if (m_eventCode == kMDL)
		m_movement[6] = true;
	if (m_eventCode == kMDR)
		m_movement[7] = true;

	// if a menu, update menus others update the animation system
	if (m_state != 2)
	{
		int result = -1;
		
		// if left mose button pressed, has a button been clicked on?
		if (m_eventCode == kMDL)
			result = m_menu->buttonClicked(m_events.mouseUpdate());

		// if so which on
		if (result == 0)
			m_state = 2;
		else if (result == 1)
			m_state = 1;
	}
	else
	{
		animCycle lastState = m_currentState;

		if (fabs(m_controller.getRightStick().y) > 0.0f || m_movement[0] || m_movement[1] || m_movement[2])
		{
			// change state and update next animation cycle trajectory poisiton
			m_currentState = Walk;
			m_mesh[m_currentState]->setModelMatrix(m_trajectoryJoint);
		}
		else
		{
			// change state and update next animation cycle trajectory poisiton
			m_currentState = Idle;
			m_mesh[m_currentState]->setModelMatrix(m_trajectoryJoint);
		}

		// if the state has changed update the direction the mech is facing for other state and AABB translation
		if (lastState != m_currentState)
		{
			if (m_currentState == Walk)
			{
				m_mesh[Walk]->hAngle(m_mesh[Idle]->getHAngle());
				m_mesh[Walk]->setAABB(m_mesh[Idle]->getAABB());
			}
			else
			{
				m_mesh[Idle]->hAngle(m_mesh[Walk]->getHAngle());
				m_mesh[Idle]->setAABB(m_mesh[Walk]->getAABB());
			}
		}

		// Debug builds only
		#ifdef _DEBUG
			// turn on fill
			if (m_controller.getLastButtonPressed() == kX || m_eventCode == kCtrl) // X (Xbox controller) or Left Crtl for fill mode
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			// turn on wirefire
			if (m_controller.getLastButtonPressed() == kY || m_eventCode == kAlt) // Y (Xbox controller) or Left Alt for wireframe mode
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
		#endif

		for (int i = 0; i < m_object.size(); ++i)
		{
			if (m_object[i]->getAABB().intersect(m_mesh[m_currentState]->getAABB()))
			{
				std::cout << "Collision!!\n";
				m_movement[3] = true;
			}
		}

		// update the animation
		m_mesh[m_currentState]->update(dt, m_currentFrame, m_events, m_movement, m_controller);

		// get the Trajectory joint position
		m_trajectoryJoint = m_mesh[m_currentState]->getModelMatrix();

		// update the camera
		m_camera->update(dt, m_events, m_movement, m_controller, m_trajectoryJoint);

		// go to the next frame
		m_currentFrame += dt * NUM_OF_FRAMES;

		// reset the frame
		if (m_currentFrame > NUM_OF_FRAMES)
			m_currentFrame = 0.0f;

		m_movement[3] = false;
		m_movement[6] = false;
		m_movement[7] = false;
	}
}
void Application::keybaordMovementUpdate()
{
		if (m_eventCode == kWdown)
		{
			m_movement[0] = true;
		}
		else if (m_eventCode == kWUp)
		{
			m_movement[0] = false;
		}

		if (m_eventCode == kAdown)
		{
			m_movement[1] = true;
		}
		else if (m_eventCode == kAUp)
		{
			m_movement[1] = false;
		}
		else if (m_eventCode == kDdown)
		{
			m_movement[2] = true;
		}
		else if (m_eventCode == kDUp)
		{
			m_movement[2] = false;
		}

		if (m_eventCode == kQdown)
		{
			m_movement[4] = true;
		}
		else if (m_eventCode == kQUp)
		{
			m_movement[4] = false;
		}
		else if (m_eventCode == kEdown)
		{
			m_movement[5] = true;
		}
		else if (m_eventCode == kEUp)
		{
			m_movement[5] = false;
		}
}