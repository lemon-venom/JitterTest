#include "..\headers\Entity.hpp"

Entity::Entity()
{
	size_.width = 192;
	size_.height = 192;

	position_.current.x = 0.0f;
	position_.previous.x = position_.current.x;
	position_.current.y = (SCREEN_HEIGHT / 2) - (size_.height / 2);
	position_.previous.y = position_.current.y;

	speed_ = 100.0f;

	movement_.x = speed_;
	movement_.y = speed_;
}

Entity::~Entity()
{
}

void Entity::updateBegin()
{
	position_.previous.x = position_.current.x;
	position_.previous.y = position_.current.y;
}

void Entity::update(double time)
{
	position_.current.x += (movement_.x * time);

	// To make it simpler, just go back and forth horizontally.
	//position.current.y += (movement.y * time);

	if (position_.current.x <= 0)
	{
		movement_.x = speed_;
	}
	else if (position_.current.x + size_.width >= SCREEN_WIDTH)
	{
		movement_.x = -speed_;
	}
	
	if (position_.current.y <= 0)
	{
		movement_.y = speed_;
	}
	else if (position_.current.y + size_.height >= SCREEN_HEIGHT)
	{
		movement_.y = -speed_;
	}
}

void Entity::render(Renderer* renderer, float lerp)
{
	float r = 0.59f;
	float g = 0.56f;
	float b = 0.0f;
	float a = 1.0f;

	Vec2 renderAt;

	renderAt.x = (position_.previous.x * (1 - lerp)) + (position_.current.x * lerp);
	renderAt.y = (position_.previous.y * (1 - lerp)) + (position_.current.y * lerp);

	std::ostringstream oss;

	oss << std::endl
		<< "FPS = " << timer.getFps() << std::endl
		<< "Current position = " << position_.current.x << ", " << position_.current.y << std::endl
		<< "Previous position = " << position_.previous.x << ", " << position_.previous.y << std::endl
		<< "lerp = " << lerp << std::endl
		<< "Rendering at " << renderAt.x << ", " << renderAt.y << " at time " << timer.getTotalTime() << " delta time = " << timer.getDeltaTime() << std::endl;


	logger.log(oss.str());

	std::vector<ColorVertex3D> polyVertices;

	ColorVertex3D v1;

	v1.position.x = renderAt.x;
	v1.position.y = renderAt.y;
	v1.position.z = 0.0f;

	v1.rgba.r = r;
	v1.rgba.g = g;
	v1.rgba.b = b;
	v1.rgba.a = a;

	polyVertices.push_back(v1);

	ColorVertex3D v2;

	v2.position.x = renderAt.x + size_.width;
	v2.position.y = renderAt.y;
	v2.position.z = 0.0f;

	v2.rgba.r = r;
	v2.rgba.g = g;
	v2.rgba.b = b;
	v2.rgba.a = a;

	polyVertices.push_back(v2);

	ColorVertex3D v3;

	v3.position.x = renderAt.x + size_.width;
	v3.position.y = renderAt.y + size_.height;
	v3.position.z = 0.0f;

	v3.rgba.r = r;
	v3.rgba.g = g;
	v3.rgba.b = b;
	v3.rgba.a = a;

	polyVertices.push_back(v3);

	ColorVertex3D v4;

	v4.position.x = renderAt.x;
	v4.position.y = renderAt.y + size_.height;
	v4.position.z = 0.0f;

	v4.rgba.r = r;
	v4.rgba.g = g;
	v4.rgba.b = b;
	v4.rgba.a = a;

	polyVertices.push_back(v4);

	renderer->drawPolygon(polyVertices);
}