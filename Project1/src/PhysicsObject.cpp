#include "PhysicsObject.h"
#include "Triangle.h"


std::vector<glm::vec3> collisionPoint;
std::vector<glm::vec3> collisionNormal;

PhysicsObject::PhysicsObject(Model* model)
{
	this->model = model;
	acceleration = glm::vec3(0.0f);
	

}

PhysicsObject::PhysicsObject()
{

	acceleration = glm::vec3(0.0f);

}

PhysicsObject::~PhysicsObject()
{
	//delete model;
}

void PhysicsObject::update(float deltatime)
{
	glm::vec3 gravity(0.0f, -9.81f* gravityValue, 0.0f);
	acceleration = gravity;
	velocity += acceleration * deltatime;
	model->transform.position += velocity * deltatime;
	

}

//bool PhysicsObject::checkCollision( PhysicsObject& other)
//{
//
//	
//	switch (physicsType)
//	{
//	case PhysicsType::AABB:
//		if (other.physicsType == AABB)
//		{
//			return CheckCOllisionAABBvsAABB(UpdateAABB(), other.UpdateAABB());
//
//		}
//		else
//		{
//			return CheckSphereAABBCollision(other.UpdateSphere(), UpdateAABB());
//		}
//		break;
//
//	case PhysicsType::SPHERE:
//
//		if (other.physicsType == AABB)
//		{
//			return CheckSphereAABBCollision(UpdateSphere(), other.UpdateAABB());
//		}
//		else if(other.physicsType == SPHERE)
//		{
//			return CheckSpherevsSphereCollision(UpdateSphere(), other.UpdateSphere());
//		}
//		else if(other.physicsType == TRIANGLE)
//		{
//			return CollisionSphereVsMeshOfTriangles(UpdateSphere(), other.model->transform.getModelMatrix(),
//				other.listoftriangles, collisionPoint,collisionNormal);
//
//		}
//
//		break;
//
//	case PhysicsType::TRIANGLE:
//
//		if (other.physicsType == TRIANGLE)
//		{
//			
//		}
//		else
//		{
//			return CheckSpherevsSphereCollision(UpdateSphere(), other.UpdateSphere());
//		}
//
//		break;
//	}
//
//
//
//	return false;
//}


bool PhysicsObject::checkCollision(PhysicsObject* other, std::vector<glm::vec3>& collisionPoints, std::vector<glm::vec3>& collisionNormals)

{



	switch (physicsType)
	{
	case PhysicsType::AABB:
		if (other->physicsType == AABB)
		{
			return CheckCOllisionAABBvsAABB(UpdateAABB(), other->UpdateAABB(),collisionPoints,collisionNormals);

		}
		else if(other->physicsType == SPHERE)
		{
			//return CheckSphereAABBCollision(other->UpdateSphere(), UpdateAABB());
			cSphere* updatedSphere = new cSphere(other->UpdateSphere());
			return CheckSphereVSAABBCollision(updatedSphere, UpdateAABB(), false, collisionPoints, collisionNormals);
			
		}
		break;
#pragma region SphereVS
	case PhysicsType::SPHERE:

		if (other->physicsType == AABB)
		{
			cSphere* updatedSphere = new cSphere(UpdateSphere());
			return CheckSphereVSAABBCollision(updatedSphere, other->UpdateAABB(),true,collisionPoints,collisionNormals);
		}
		else if (other->physicsType == SPHERE)
		{
			cSphere* updatedSphere = new cSphere(UpdateSphere());
			cSphere* OtherupdatedSphere = new cSphere(other->UpdateSphere());
			return CheckSphereVSSphereCollision(updatedSphere, OtherupdatedSphere,collisionPoints,collisionNormals);
		}
		else if (other->physicsType == TRIANGLE)
		{

			return CollisionSphereVsMeshOfTriangles(UpdateSphere(), other->model->transform.GetModelMatrix(),
				other->listoftriangles, other->triangleSpheres,collisionPoints, collisionNormals);

		}
#pragma endregion
	case PhysicsType:: TRIANGLE :
		if (other->physicsType == SPHERE)
		{
			return CollisionSphereVsMeshOfTriangles(other->UpdateSphere(),model->transform.GetModelMatrix(),
				listoftriangles, triangleSpheres,collisionPoints, collisionNormals);
		}

		break;

	//case PhysicsType::TRIANGLE:

	//	if (other->physicsType == TRIANGLE)
	//	{

	//	}
	//	else if (other->physicsType == SPHERE)
	//	{
	//		return CheckSpherevsSphereCollision(UpdateSphere(), other->UpdateSphere());
	//	}

		break;
	}



	return false;
}

cAABB  PhysicsObject:: UpdateAABB()
{

	//cAABB temp;

	glm::vec3 originalMinV = aabb.minV;
	glm::vec3 originalMaxV = aabb.maxV;
	glm::mat4 transformMatrix = model->transform.GetModelMatrix();
	glm::vec4 transformedMinV = transformMatrix * glm::vec4(originalMinV, 1.0f);
	glm::vec4 transformedMaxV = transformMatrix * glm::vec4(originalMaxV, 1.0f);

	return  cAABB(glm::vec3(transformedMinV), glm::vec3(transformedMaxV));
	
}

cSphere PhysicsObject::UpdateSphere()
{
	glm::vec3 originalCenter = sphereShape.center;
	float orginalRadius = sphereShape.radius;
	glm::mat4 transformMatrix = model->transform.GetModelMatrix();
	glm::vec4 transformedCenter = transformMatrix * glm::vec4(originalCenter, 1.0f);

	glm::mat4 scaleMatrix = glm::mat4(1.0f); // Initialize the scale matrix
	scaleMatrix[0][0] = model->transform.scale.x;
	scaleMatrix[1][1] = model->transform.scale.y;
	scaleMatrix[2][2] = model->transform.scale.z;

	float maxScale = glm::max(model->transform.scale.x, glm::max(model->transform.scale.y, model->transform.scale.z));
	float updatedRadius = orginalRadius * maxScale;

	return cSphere(glm::vec3(transformedCenter), updatedRadius);
}

void PhysicsObject::CalculateTriangle()
{
	for (std::vector<cSphere*>&sphereList : triangleSpheres)
	{
		for (cSphere* s: sphereList)
		{
			delete s;
		}
		sphereList.clear();
	}
	
	listoftriangles.clear();
	triangleSpheres.clear();

	for  (std::shared_ptr<Mesh> mesh : model->meshes )
	{
		std::vector<Triangle> trianglelist;
		std::vector<cSphere*> meshSphers;

		trianglelist.reserve(mesh->triangle.size());
		meshSphers.reserve(mesh->triangle.size());

		for (const Triangles& triangle : mesh->triangle)
		{
			Triangle temp;
			temp.v1 = triangle.v1;
			temp.v2 = triangle.v2;
			temp.v3 = triangle.v3;
			temp.normal = triangle.normal;


			glm::vec3 sphereCenter = (temp.v1 + temp.v2 + temp.v3) / 3.0f;
			float radius = glm::max(glm::distance(sphereCenter, temp.v1),
				glm::max(glm::distance(sphereCenter, temp.v2), glm::distance(sphereCenter, temp.v3)));

			trianglelist.push_back(temp);
			meshSphers.push_back(new cSphere(sphereCenter, radius));
		}
		
		listoftriangles.push_back(trianglelist);
		triangleSpheres.push_back(std::move(meshSphers));


	}

	
}

 

void PhysicsObject::Initialize(bool isKinematic, bool collision, ObjectMode mode)
{
	this->isKinematic = isKinematic;
	this->collisionCallbool = collision;
	this->mode = mode;
	//if (this->isKinematic)
	//{
	//	gravityValue = 0;
	//}
	//else
	//{
	//	gravityValue = 0.1f;
	//}
	
	
		aabb = CalculateModelAABB();
		

	

	if (physicsType == SPHERE)
	{
		glm::vec3 position = (aabb.minV + aabb.maxV) * 0.5f;
		glm::vec3 sideLengths = aabb.maxV - aabb.minV;
		float radius = 0.5f * glm::max(sideLengths.x, glm::max(sideLengths.y, sideLengths.z));
		sphereShape =  cSphere(position, radius);
	}

	if (physicsType == TRIANGLE)
	{
		CalculateTriangle();
	}
	
}


cAABB PhysicsObject::CalculateModelAABB()
{
	if (model->meshes.empty())
	{
		return cAABB{ glm::vec3(0.0f), glm::vec3(0.0f) };
	}

	cAABB minMax;

	minMax.minV = model->meshes[0]->vertices[0].Position;
	minMax.maxV = model->meshes[0]->vertices[0].Position;

	for (std::shared_ptr<Mesh> mesh : model->meshes)
	{
		cAABB temp = CalculateAABB(mesh->vertices);

		minMax.minV.x =glm::min(temp.minV.x, minMax.minV.x);
		minMax.minV.y =glm::min(temp.minV.y, minMax.minV.y);
		minMax.minV.z =glm::min(temp.minV.z, minMax.minV.z);
					  
		minMax.maxV.x =glm::max(temp.maxV.x, minMax.maxV.x);
		minMax.maxV.y =glm::max(temp.maxV.y, minMax.maxV.y);
		minMax.maxV.z =glm::max(temp.maxV.z, minMax.maxV.z);
	}

	return cAABB{ minMax.minV, minMax.maxV };
}

glm::vec3 PhysicsObject::GetPosition()
{
	return model->transform.position;
}

void PhysicsObject::SetPosition(const glm::vec3& Position)
{
	model->transform.position = Position;
}

const std::function<void(PhysicsObject*)>& PhysicsObject::GetCollisionCall()
{
	return collisionCallback;
}

void PhysicsObject::DoCollisionCall(const std::function<void(PhysicsObject*)>& collisionCallback)
{
	this->collisionCallback = collisionCallback;
}

void PhysicsObject::SetMass(const float& massValue)
{
	this->mass = massValue;
	inverseMass = 1.0f / mass;
}
