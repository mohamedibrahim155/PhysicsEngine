#pragma once
//#include <iostream>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Vertex.h"
#include "Triangle.h"







struct cAABB
{

	cAABB()
	{
	}
	cAABB(glm::vec3 minV, glm::vec3 maxV)
	{
		this->minV = minV;
		this->maxV = maxV;
	}
	~cAABB() {}
	glm::vec3 minV;
	glm::vec3 maxV;
	
};



struct cSphere
{
	cSphere(glm::vec3 position, float radius)
	{
		this->center = position;
		this->radius = radius;
	}
	cSphere()
	{

	}

	~cSphere() {}
	glm::vec3 center;
	float radius;

	
};

struct Triangle
{
	Triangle() {}
	Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 normal)
	{
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
		this->normal = normal;
	}

	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
	glm::vec3 normal;

	

};



static cAABB CalculateAABB(const std::vector<Vertex>& vertices)
{
	if (vertices.size() == 0)
	{
		return cAABB{ glm::vec3(0.0f), glm::vec3(0.0f) };
	}

	glm::vec3 min = vertices[0].Position;
	glm::vec3 max = vertices[0].Position;

	for (const Vertex& vertex : vertices)
	{
		
		min.x = glm::min(min.x, vertex.Position.x);
		min.y = glm::min(min.y, vertex.Position.y);
		min.z = glm::min(min.z, vertex.Position.z);

		max.x =glm::max(max.x, vertex.Position.x);
		max.y =glm::max(max.y, vertex.Position.y);
		max.z =glm::max(max.z, vertex.Position.z);
	}

	return cAABB{ min, max };
}


static bool CheckCOllisionAABBvsAABB(const cAABB& a, const cAABB& b)
{
	if (a.maxV[0] < b.minV[0] || a.minV[0] > b.maxV[0]) return false;
	if (a.maxV[1] < b.minV[1] || a.minV[1] > b.maxV[1]) return false;
	if (a.maxV[2] < b.minV[2] || a.minV[2] > b.maxV[2]) return false;

	return true;
}

static bool CheckCOllisionAABBvsAABB(const cAABB& a, const cAABB& b,
	std::vector<glm::vec3>& collisionPoint,
	std::vector<glm::vec3>& collisionNormal)
{
	// Exit with no intersection if separated along an axis
	if (a.maxV[0] < b.minV[0] || a.minV[0] > b.maxV[0]) return false;
	if (a.maxV[1] < b.minV[1] || a.minV[1] > b.maxV[1]) return false;
	if (a.maxV[2] < b.minV[2] || a.minV[2] > b.maxV[2]) return false;

	// Overlapping on all axes means AABBs are intersecting

	cAABB intersectionAABB;
	intersectionAABB.minV.x = glm::max(a.minV.x, b.minV.x);
	intersectionAABB.minV.y = glm::max(a.minV.y, b.minV.y);
	intersectionAABB.minV.z = glm::max(a.minV.z, b.minV.z);
							
	intersectionAABB.maxV.x = glm::min(a.maxV.x, b.maxV.x);
	intersectionAABB.maxV.y = glm::min(a.maxV.y, b.maxV.y);
	intersectionAABB.maxV.z = glm::min(a.maxV.z, b.maxV.z);

	glm::vec3 collisionpt = (intersectionAABB.minV + intersectionAABB.maxV) * 0.5f;
	collisionPoint.push_back(collisionpt);

	// Calculate the collision normal
	glm::vec3 centerA = (a.minV + a.maxV) * 0.5f;
	glm::vec3 centerB = (b.minV + b.maxV) * 0.5f;
	glm::vec3 collisionNr = centerA - centerB;
	collisionNr = glm::normalize(collisionNr);

	collisionNormal.push_back(collisionNr);
	return true;
}





static bool CheckSpherevsSphereCollision(const cSphere& sphere1, const cSphere& sphere2)
{
	float distance = glm::distance(sphere1.center, sphere2.center);
	float sumRadii = sphere1.radius + sphere2.radius;
	return distance <= sumRadii;
}
static bool CheckSphereVSSphereCollision(cSphere* sphere1, cSphere* sphere2,
	std::vector<glm::vec3>& collisionPoint,
	std::vector<glm::vec3>& collisionNormal)
{

	glm::vec3 collisionPt = glm::vec3(1.0f);
	glm::vec3 collisionNr = glm::vec3(1.0f);

	glm::vec3 d = sphere2->center - sphere1->center;
	float dist2 = glm::dot(d, d);

	float radiusSum = sphere1->radius + sphere2->radius;
	float radius2 = radiusSum * radiusSum;

	if (dist2 <= radius2)
	{
		if (dist2 != 0)
		{
			collisionNr = glm::normalize(d);
		}
		else
		{
			collisionNr = glm::normalize(glm::vec3(1.0f));
		}

		collisionPt = sphere1->center + sphere1->radius * collisionNr;
		//std::cout << "spherevs sphere detected : " << std::endl;
		collisionPoint.push_back(collisionPt);
		collisionNormal.push_back(collisionNr);

		//Debugger::Print("Collision");

		return true;
	}

	return false;
}





static float SqDistPointAABB(glm::vec3 p, cAABB b)
{
	float sqDist = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		// For each axis count any excess distance outside box extents
		float v = p[i];
		if (v < b.minV[i])
			sqDist += (b.minV[i] - v) * (b.minV[i] - v);
		if (v > b.maxV[i])
			sqDist += (v - b.maxV[i]) * (v - b.maxV[i]);
	}
	return sqDist;
}
static const glm::vec3& ClosestPtPointAABB(glm::vec3 p, cAABB b)
{
	glm::vec3 q;
	for (int i = 0; i < 3; i++) {
		float v = p[i];
		if (v < b.minV[i]) v = b.minV[i]; // v = max(v, b.min[i])
		if (v > b.maxV[i]) v = b.maxV[i]; // v = min(v, b.max[i])
		q[i] = v;
	}
	return q;
}


static bool CheckSphereVSAABBCollision(cSphere* sphere, const cAABB& aabb, bool isSphere,
	std::vector<glm::vec3>& collisionPoint,
	std::vector<glm::vec3>& collisionNormal)
{
	// Compute squared distance between sphere center and AABB
	float sqDist = SqDistPointAABB(sphere->center, aabb);
	float sqRadius = sphere->radius * sphere->radius;

	if (sqDist <= sqRadius)
	{
		glm::vec3 collisionPt = ClosestPtPointAABB(sphere->center, aabb);

		glm::vec3 collisonNr = glm::vec3(0.0f);

		if (isSphere)
		{
			collisonNr = collisionPt - sphere->center;
		}
		else
		{
			collisonNr = collisionPt - ((aabb.minV + aabb.maxV) * 0.5f);
			collisonNr = glm::normalize(collisonNr);
		}

		collisionPoint.push_back(collisionPt);
		collisionNormal.push_back(collisonNr);

		return true;
	}

	return false;
}

static bool CheckSphereAABBCollision(const cSphere& sphere, const cAABB& aabb) 
{
	glm::vec3 closestPoint;

	// Find the closest point on the AABB to the sphere
	closestPoint.x = glm::max(aabb.minV.x, glm::min(sphere.center.x, aabb.maxV.x));
	closestPoint.y = glm::max(aabb.minV.y, glm::min(sphere.center.y, aabb.maxV.y));
	closestPoint.z = glm::max(aabb.minV.z, glm::min(sphere.center.z, aabb.maxV.z));

	// Calculate squared distance between the sphere center and the closest point on the AABB
	float lengthSquared = glm::dot(closestPoint - sphere.center, closestPoint - sphere.center);

	// Check for collision
	return lengthSquared < (sphere.radius * sphere.radius);
}

static bool SphereTriangleCollision(const cSphere& sphere, const Triangle& triangle)
{
	// Calculate the vector from the sphere's center to a point on the triangle
	glm::vec3 diff = triangle.v1 - sphere.center;

	// Project this vector onto the triangle's normal
	float distance = glm::dot(diff, triangle.normal);

	// If the distance is greater than the sphere's radius, there is no collision
	if (distance > sphere.radius)
		return false;

	// Project triangle vertices onto the axis defined by the vector from sphere's center to triangle's normal
	std::vector<float> triangleProj = {
		glm::dot(triangle.v1 - sphere.center, triangle.normal),
		glm::dot(triangle.v2 - sphere.center, triangle.normal),
		glm::dot(triangle.v3 - sphere.center, triangle.normal)
	};

	// Calculate min and max values of the projections
	float minProj = glm::min(triangleProj[0], glm::min(triangleProj[1], triangleProj[2]));
	float maxProj = glm::max(triangleProj[0], glm::max(triangleProj[1], triangleProj[2]));

	// If there is no overlap along this axis, there is no collision
	if (maxProj < -sphere.radius || minProj > sphere.radius)
		return false;

	// Edge tests (cross products)
	glm::vec3 edge1 = triangle.v2 - triangle.v1;
	glm::vec3 edge2 = triangle.v3 - triangle.v2;
	glm::vec3 edge3 = triangle.v1 - triangle.v3;

	glm::vec3 normal1 = glm::cross(edge1, diff);
	glm::vec3 normal2 = glm::cross(edge2, diff);
	glm::vec3 normal3 = glm::cross(edge3, diff);

	if (glm::dot(normal1, normal1) > 0 && glm::dot(normal1, diff) < 0)
		return false;

	if (glm::dot(normal2, normal2) > 0 && glm::dot(normal2, diff) < 0)
		return false;

	if (glm::dot(normal3, normal3) > 0 && glm::dot(normal3, diff) < 0)
		return false;

	// If all tests pass, there is a collision
	return true;
}


#pragma region Extra

static glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;

	// Compute parametric position s for projection P' of P on AB,
	// P' = A + s*AB, s = snom/(snom+sdenom)
	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

	// Compute parametric position t for projection P' of P on AC,
	// P' = A + t*AC, s = tnom/(tnom+tdenom)
	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

	// Compute parametric position u for projection P' of P on BC,
	// P' = B + u*BC, u = unom/(unom+udenom)
	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
	glm::vec3 n = glm::cross(b - a, c - a);
	float vc = glm::dot(n, glm::cross(a - p, b - p));
	// If P outside AB and within feature region of AB,
	// return projection of P onto AB
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
	float va = glm::dot(n, glm::cross(b - p, c - p));
	// If P outside BC and within feature region of BC,
	// return projection of P onto BC
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
	float vb = glm::dot(n, glm::cross(c - p, a - p));
	// If P outside CA and within feature region of CA,
	// return projection of P onto CA
	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;

	// P must project inside face region. Compute Q using barycentric coordinates
	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v; // = vc / (va + vb + vc)
	return u * a + v * b + w * c;
}

static bool CollisionSphereVsTriangle(cSphere* sphere, const Triangle& triangle, glm::vec3& collisionPoint)
{
	glm::vec3 v1ToSphere = sphere->center - triangle.v1;

	float distanceToPlane = glm::dot(v1ToSphere, triangle.normal);

	if (distanceToPlane < -sphere->radius)
	{
		return false;
	}

	glm::vec3 closestPointOnTriangle = ClosestPtPointTriangle(sphere->center, triangle.v1, triangle.v2, triangle.v3);

	glm::vec3 closestPointToSphere = closestPointOnTriangle - sphere->center;
	float distanceToClosest = glm::length(closestPointToSphere);

	if (distanceToClosest <= sphere->radius)
	{
		collisionPoint = closestPointOnTriangle;
		return true;
	}
	return false;
}


static bool CollisionSphereVsMeshOfTriangles(cSphere sphere,
	const glm::mat4& transformMatrix,
	const std::vector <std::vector <Triangle>>& triangles,
	const std::vector <std::vector <cSphere*>>& triangleSpheres,
	std::vector<glm::vec3>& collisionPoints,
	std::vector<glm::vec3>& collisionNormals)
{

	float maxScale = glm::max(glm::max(transformMatrix[0][0], transformMatrix[1][1]), transformMatrix[2][2]);

	collisionPoints.clear();
	cSphere* sphereTriangle = new cSphere();

	for (size_t i = 0; i < triangles.size(); i++)
	{
		const std::vector<Triangle>& triangleList = triangles[i];
		const std::vector<cSphere*>& sphereList = triangleSpheres[i];
		for (size_t j = 0; j < triangleList.size(); j++)
		{
			Triangle triangle = triangleList[j];
			glm::vec4 transformedCenter = transformMatrix * glm::vec4(sphereList[j]->center, 1.0f);
			sphereTriangle->center = glm::vec3(transformedCenter);
			// Transform the sphere's position using the transformMatrix
			sphereTriangle->radius = sphereList[j]->radius * maxScale;
			// Transform the sphere's radius based on scaling

			// Now you can check for collision between the transformed sphere and sphereTriangle
			std::vector<glm::vec3> collisionPoint;
			std::vector<glm::vec3> collisionNormal;

			if (CheckSphereVSSphereCollision(&sphere, sphereTriangle, collisionPoint, collisionNormal))

			{
				glm::vec3 point = glm::vec3(0);
				cSphere* temp = new cSphere(sphere);
				triangle.v1 = transformMatrix * glm::vec4(triangle.v1, 1.0f);
				triangle.v2 = transformMatrix * glm::vec4(triangle.v2, 1.0f);
				triangle.v3 = transformMatrix * glm::vec4(triangle.v3, 1.0f);
				if (CollisionSphereVsTriangle(temp, triangle, point))
				{
					//Debugger::Print("Sphere vs Triangle");
					//glm::vec3 normal = point - sphere->position;
					std::cout << "spherevs trangle detected : " << std::endl;
					glm::vec3 normal = transformMatrix * glm::vec4(triangle.normal, 0.0f);

					collisionPoints.push_back(point);
					collisionNormals.push_back(normal);
				}
			}
		}
	}

	delete sphereTriangle;


	if (collisionPoints.size() > 0)
		return true;
	//std::cout << "Size : " << collisionPoints.size()<<std::endl;

	return false;
}

#pragma endregion
