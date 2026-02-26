#pragma once

#include "../Ogre/source/OgreMain/include/OgreVector2.h"
#include "../Ogre/source/OgreMain/include/OgreVector3.h"
#include "../Ogre/source/OgreMain/include/OgreTexture.h"
#include "../Ogre/source/OgreMain/include/OgreMaterial.h"
#include "../Ogre/source/OgreMain/include/OgreEntity.h"
#include "../Ogre/source/OgreMain/include/OgreSceneNode.h"

namespace cimg_library
{
	template<typename T> struct CImg;
};

using namespace Ogre;

class CLightMap
{
public:
	struct PixelInfo
	{
	public:
		PixelInfo() {}
		PixelInfo(float _r, float _g, float _b, float _a) { r = _r; g = _g, b = _b, a = _a; }

		float r = 0;
		float g = 0;
		float b = 0;
		float a = 0;

		PixelInfo operator+(PixelInfo& lhs)
		{
			return PixelInfo(r + lhs.r, g + lhs.g, b + lhs.b, a + lhs.a);
		}

		PixelInfo operator-(PixelInfo& lhs)
		{
			return PixelInfo(r - lhs.r, g - lhs.g, b - lhs.b, a - lhs.a);
		}

		PixelInfo & operator+=(const PixelInfo& lhs)
		{
			this->r += lhs.r;
			this->g += lhs.g;
			this->b += lhs.b;
			this->a += lhs.a;

			return *this;
		}

		PixelInfo& operator+=(const float& lhs)
		{
			this->r += lhs;
			this->g += lhs;
			this->b += lhs;
			this->a += lhs;

			return *this;
		}

		PixelInfo operator-=(const PixelInfo& lhs)
		{
			this->r -= lhs.r;
			this->g -= lhs.g;
			this->b -= lhs.b;
			this->a -= lhs.a;

			return *this;
		}

		PixelInfo operator*(PixelInfo& lhs)
		{
			return PixelInfo(r * lhs.r, g * lhs.g, b * lhs.b, a * lhs.a);
		}

		PixelInfo operator/(PixelInfo& lhs)
		{
			return PixelInfo(r / lhs.r, g / lhs.g, b / lhs.b, a / lhs.a);
		}

		PixelInfo operator*(float lhs)
		{
			return PixelInfo(r * lhs, g * lhs, b * lhs, a * lhs);
		}

		PixelInfo operator*=(const PixelInfo& lhs)
		{
			this->r *= lhs.r;
			this->g *= lhs.g;
			this->b *= lhs.b;
			this->a *= lhs.a;

			return *this;
		}
	};

	CLightMap(SubEntity* pSubEntity, int entNum, std::string objName, std::string saveDir, Real PixelsPerUnit = 0, int iTexSize = 512, bool bDebugLightmaps = false);
	~CLightMap(void);

	String GetName();
	static void ResetCounter();

	struct SortCoordsByDistance
	{
		bool operator()(std::pair<int, int>& left, std::pair<int, int>& right)
		{
			return (left.first * left.first + left.second * left.second) <
				(right.first * right.first + right.second * right.second);
		}
	};

	void LightTriangle(const Vector3& P1, const Vector3& P2, const Vector3& P3,
		const Vector3& N1, const Vector3& N2, const Vector3& N3,
		const Vector2& T1, const Vector2& T2, const Vector2& T3);

protected:
	PixelInfo GetLightIntensity(const Vector3 & Position, const Vector3 & Normal);
	void SetLightIntensity(int i, int j, PixelInfo color);

	bool CalculateLightMap();
	void SaveLightmap();
	void CreateTexture();
	void FillInvalidPixels();
	static void BuildSearchPattern();

	std::string saveTo = "";
	std::string objectName = "";
	int entityNum = 0;

	/// Convert between texture coordinates given as reals and pixel coordinates given as integers
	int GetPixelCoordinate(Real TextureCoord);
	Real GetTextureCoordinate(int iPixelCoord);

	/// Calculate coordinates of P in terms of P1, P2 and P3
	/// P = x*P1 + y*P2 + z*P3
	/// If any of P.x, P.y or P.z are negative then P is outside of the triangle
	Vector3 GetBarycentricCoordinates(const Vector2 & P1, const Vector2 & P2, const Vector2 & P3, const Vector2 & P);
	/// Get the surface area of a triangle
	Real GetTriangleArea(const Vector3 & P1, const Vector3 & P2, const Vector3 & P3);

	void BlurLightmap(float radius);

	//TexturePtr m_Texture;
	SubEntity* m_pSubEntity;
	//Image * m_LightMap;
	mutable PixelInfo* m_LightMap;
	String m_LightMapName;
	int m_iTexSize;
	static int m_iLightMapCounter;
	Real m_PixelsPerUnit;
	static std::vector<std::pair<int, int> > m_SearchPattern;
	bool m_bDebugLightmaps;
};

class CEntityLightMap
{
public:
	CEntityLightMap(Entity* pEntity, std::string saveDir, Real PixelsPerUnit = 0, int iTexSize = 512, bool bDebugLightmaps = false)
	{
		int i, iNumSubEntities = pEntity->getNumSubEntities();
		for (i = 0; i < iNumSubEntities; ++i)
		{
			CLightMap * LightMap = new CLightMap(pEntity->getSubEntity(i), i, pEntity->getParentSceneNode()->getName(), saveDir, PixelsPerUnit, iTexSize, bDebugLightmaps);
			m_LightMaps.push_back(LightMap);
		}
	}

	~CEntityLightMap()
	{
		for (auto it = m_LightMaps.begin(); it != m_LightMaps.end(); ++it)
		{
			CLightMap* map = *it;
			delete map;
		}

		m_LightMaps.clear();
	}

protected:
	std::vector<CLightMap*> m_LightMaps;
};