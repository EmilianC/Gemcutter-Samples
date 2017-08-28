#pragma once
#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Math/Vector.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/RenderPass.h>
#include <Jewel3D/Rendering/RenderTarget.h>
#include <Jewel3D/Resource/ConfigTable.h>
#include <Jewel3D/Resource/Shader.h>

using namespace Jwl;

class Game
{
public:
	Game(ConfigTable& config);

	bool Init();

	void Update();
	void Draw();

	/* Events */
	Listener<KeyPressed> onKeyPressed;
	Listener<Resize> onResized;

	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr ground = Entity::MakeNew();
	Entity::Ptr shack = Entity::MakeNew();
	Entity::Ptr orb1 = Entity::MakeNew();
	Entity::Ptr orb2;
	Entity::Ptr directionalLight = Entity::MakeNew();
	Entity::Ptr cameraParent = Entity::MakeNew();
	Entity::Ptr orbParent = Entity::MakeNew();
	Entity::Ptr rootEntity = Entity::MakeNew();
	float angleY = -90.0f;
	float angleX = 0.0f;

	/* Assets */
	ConfigTable& config;

	/* FrameBuffers */
	RenderTarget::Ptr GBuffer = RenderTarget::MakeNew();
	RenderTarget::Ptr GBufferResolve;
	RenderTarget::Ptr godRaysBuffer1 = RenderTarget::MakeNew();
	RenderTarget::Ptr godRaysBuffer1Resolve;
	RenderTarget::Ptr godRaysBuffer2 = RenderTarget::MakeNew();
	RenderTarget::Ptr godRaysBuffer2Resolve;
	RenderTarget::Ptr workBuffer1 = RenderTarget::MakeNew();
	RenderTarget::Ptr workBuffer2 = RenderTarget::MakeNew();

	/* Shaders */
	Shader::Ptr godRaysRadialBlur;
	Shader::Ptr godRaysComposite;
	Shader::Ptr staticGeometryProgram;
	Shader::Ptr flatColorProgram;
	
	/* Rendering */
	RenderPass GBufferCall;
	RenderPass godRaysCall1;
	RenderPass godRaysCall2;
	RenderPass godRaysRadial1;
	RenderPass godRaysRadial2;
	RenderPass composite;
	UniformHandle<vec2> screenSpaceRadialPos;
	UniformHandle<vec3> orb1ColorHandle;
	UniformHandle<vec3> orb2ColorHandle;

	unsigned MSAA_Level = 1;
	vec3 orb1Color = vec3(0.16f, 0.16f, 0.21f);
	vec3 orb2Color = vec3(0.01f, 0.2f, 0.02f);
	vec3 lightDirection = vec3(-1.0f, -1.0f, -1.0f).GetNormalized();
};
