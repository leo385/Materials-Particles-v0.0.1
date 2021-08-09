/*
 * state.h
 *
 *  Created on: 9 sie 2021
 *  	Author: Leonard Wieloch
 *
 *  Materials Simulation Project, I took into account some materials like: Fire, Water, Sand, Mud, Ice and Stones.
 *  This Simulator has one specific purpose, show physics of these materials with diffrent proporties, like gravity, mass, overlap objects.
 *  I didn't check any physics among these materials, because I needed break to the rest of my mind :D.
 *
 *  In this simulator we've got edges that we can set them the way we like it, enough to drag the black circles.
 *  We can choose materials on toolbar by pressing A or D :)
 *  If we want to use any material, we must use left mouse button and hold it.
 *
 *  I hope this text didn't tire you out :)
 *
 *  PS: I've used David Barr's Graphics Library from Github, called olcPixelGameEngine. WebSite: http://www.onelonecoder.com/
 *
 */

#ifndef SRC_STATE_H_
#define SRC_STATE_H_

#include "olcPixelGameEngine.h"

#include <iostream>
#include <vector>
#include <array>
#include <math.h>
#include <cstdlib>


class state : public olc::PixelGameEngine
{
private:

	struct sMaterial
	{
		olc::vf2d p;
		olc::vf2d v;
		olc::vf2d op;

		float ax, ay;
		float fmass;
		float fradius;
		float fAngle;
		float fnSimTimeRemaining;

		olc::Pixel color;

		int nID;

	};

	struct sLineSegment
	{
		olc::vf2d start;
		olc::vf2d end;

		float fRadius = 6.0f;
	};

	std::vector<sMaterial> vecFireParticles;
	std::vector<sMaterial> vecWaterParticles;
	std::vector<sMaterial> vecSandParticles;
	std::vector<sMaterial> vecMudParticles;
	std::vector<sMaterial> vecIceParticles;
	std::vector<sMaterial> vecStoneParticles;

	std::vector<std::pair<sMaterial*, sMaterial*>> vecCollidingPair;
	std::vector<sLineSegment> vecLines;
	std::vector<sMaterial*> vecFakeBalls;

	std::unique_ptr<olc::Sprite> spr;
	std::unique_ptr<olc::Sprite> sprActiveSlot;
	std::unique_ptr<olc::Sprite> itemPointer[6];

	olc::vi2d vDefaultBeginLocation = {0,0};
	int32_t nIDItem = -1;

	enum materials { FIRE = -1, WATER, SAND, MUD, ICE, STONE, NAAH};

	sMaterial* p_selectedParticle = nullptr;
	sLineSegment* p_selectedSegment = nullptr;

	bool bSelectedStart = false;

	sMaterial m;

	int id = 0;

public:
	state(){sAppName = "Materials_Simulator";}
	virtual ~state(){ vecFireParticles.clear(); vecWaterParticles.clear(); vecSandParticles.clear();
					  vecIceParticles.clear(); }

	void loadGFX()
	{
		spr = std::make_unique<olc::Sprite>("gfx/toolbar1.png");
		sprActiveSlot = std::make_unique<olc::Sprite>("gfx/active_slot1.png");
			if(spr == nullptr || sprActiveSlot == nullptr)
				std::cout << "Could not loaded toolbars sprite!" << "\n";

		itemPointer[0] = std::make_unique<olc::Sprite>("gfx/fire.png");
		itemPointer[1] = std::make_unique<olc::Sprite>("gfx/water.png");
		itemPointer[2] = std::make_unique<olc::Sprite>("gfx/sand.png");
		itemPointer[3] = std::make_unique<olc::Sprite>("gfx/mud.png");
		itemPointer[4] = std::make_unique<olc::Sprite>("gfx/ice.png");
		itemPointer[5] = std::make_unique<olc::Sprite>("gfx/stone.png");

		for(int i = 0; i < 6; i++)
			if(itemPointer[i] == nullptr)
				std::cout << "Could not loaded items sprite!" << "\n";
	}

	float distanceSquared(olc::vf2d a, olc::vf2d b)
	{
		float distX = a.x - b.x;
		float distY = a.y - b.y;

		float distance = (distX * distX + distY * distY);
		return sqrtf(distance);
	}

	bool ParticleVsParticle(const sMaterial* p1, const sMaterial* p2)
	{
		return distanceSquared(p1->p, p2->p) < (p1->fradius + p2->fradius);
	}


	bool mPointInSegmentStart(const olc::vf2d& mouse, const sLineSegment* edge)
	{
		return std::fabs((mouse.x - edge->start.x)*(mouse.x - edge->start.x)+(mouse.y - edge->start.y)*(mouse.y - edge->start.y))
				< (edge->fRadius + edge->fRadius);
	}

	bool mPointInSegmentEnd(const olc::vf2d& mouse, const sLineSegment* edge)
	{
		return std::fabs((mouse.x - edge->end.x)*(mouse.x - edge->end.x)+(mouse.y - edge->end.y)*(mouse.y - edge->end.y))
						< (edge->fRadius + edge->fRadius);
	}

	bool OnUserCreate() override
	{
		loadGFX();
		vDefaultBeginLocation = {50, 30};

		std::srand((unsigned int)time(0));

		SetPositionMaterial(vecWaterParticles);
		SetPositionMaterial(vecSandParticles);
		SetPositionMaterial(vecMudParticles);
		SetPositionMaterial(vecIceParticles);
		SetPositionMaterial(vecStoneParticles);

		//edges push
		vecLines.push_back({{150.0f, 200.0f},{300.0f, 200.0f}});
		vecLines.push_back({{200.0f, 250.0f},{100.0f, 250.0f}});
		vecLines.push_back({{200.0f, 100.0f},{250.0f, 100.0f}});

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		olc::vi2d vScreenLocation = {50,30};
		olc::vi2d vSize = {1,1};
		olc::vi2d vPadding = {2, 2};

		int32_t nCollumns = 6;
		int32_t nMaxSizeToolbar = 100;

		int32_t nSegmentOfToolbar = nMaxSizeToolbar / nCollumns;

		olc::vf2d mousePos = {(float)GetMouseX(), (float)GetMouseY()};

		if(GetKey(olc::Key::D).bPressed)
		{
			vDefaultBeginLocation.x += nSegmentOfToolbar;
			nIDItem+=1;

		}

		if(GetKey(olc::Key::A).bPressed)
		{
			vDefaultBeginLocation.x -= nSegmentOfToolbar;
			nIDItem-=1;

		}


		//Handle Mouse Event
		if(GetMouse(0).bPressed)
		{
			p_selectedSegment = nullptr;

			 for(auto& v : vecLines)
			 {
				 if(mPointInSegmentStart(mousePos, &v))
				 {
					 bSelectedStart = true;
					 p_selectedSegment = &v;
					 break;
				 }

				 if(mPointInSegmentEnd(mousePos, &v))
				 {
					 bSelectedStart = false;
					 p_selectedSegment = &v;
					 break;
				 }

			 }

		}


		if(GetMouse(0).bHeld)
		{
			if(p_selectedParticle != nullptr)
				p_selectedParticle->p = {mousePos.x, mousePos.y};

			if(p_selectedSegment != nullptr)
			{
				if(bSelectedStart)
					p_selectedSegment->start = {mousePos.x, mousePos.y};
				else
					p_selectedSegment->end = {mousePos.x, mousePos.y};
			}

		}

		if(GetMouse(0).bReleased)
		{
			p_selectedParticle = nullptr;
			p_selectedSegment = nullptr;
		}


		std::vector<int> vecItemPosition = {50, 68, 83, 99, 116, 133};


		float fDefaultRadius = 0;
				float fTime = 0;


			switch(nIDItem)
					{

					case FIRE:

					vDefaultBeginLocation.x = vecItemPosition.at(0);

					if(GetMouse(0).bHeld && p_selectedSegment == nullptr)
					{
						for(int i = 0; i < 10; i++)
						{

							fDefaultRadius = 0.90f;
							fTime = 0.5f;

							float angle = ((float)rand() / (float)RAND_MAX * 2.0f * 3.14159f);
							float velocity = ((float)rand() / (float)RAND_MAX * 10.0f);

							m.p = {static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY())};
							m.v = { velocity * (float)cos(angle), velocity * (float)sin(angle) };

							m.fradius = fDefaultRadius;
							m.fnSimTimeRemaining = fTime;
							m.fAngle = angle;
							m.color = olc::RED;

							vecFireParticles.push_back(m);
						}
					}


			break;

				case WATER:

				vDefaultBeginLocation.x = vecItemPosition.at(1);
				SetParamMaterial(vecWaterParticles, 2.0f);

				break;

				case SAND:

				vDefaultBeginLocation.x = vecItemPosition.at(2);
				SetParamMaterial(vecSandParticles, 0.5f);

				break;

				case MUD:

				vDefaultBeginLocation.x = vecItemPosition.at(3);
				SetParamMaterial(vecMudParticles, 2.0f);

				break;

				case ICE:

				vDefaultBeginLocation.x = vecItemPosition.at(4);
				SetParamMaterial(vecIceParticles, 2.0f);

				break;

				case STONE:

				vDefaultBeginLocation.x = vecItemPosition.at(5);
				SetParamMaterial(vecStoneParticles, 3.0f);

				break;

				default:
							std::cout << "NO IDENTIFY!" << "\n";
				break;
			}

		//erases fire
		vecFireParticles.erase(std::remove_if(vecFireParticles.begin(), vecFireParticles.end(),
					[](const sMaterial& m){return m.fnSimTimeRemaining < 0.0f;}), vecFireParticles.end());

		//erase Materials
		EraseMaterial(vecWaterParticles);
		EraseMaterial(vecSandParticles);
		EraseMaterial(vecMudParticles);
		EraseMaterial(vecIceParticles);
		EraseMaterial(vecStoneParticles);

		//Clamp Boundaries
		if(vDefaultBeginLocation.x < 50 && nIDItem < -1) {vDefaultBeginLocation.x = 50; nIDItem = -1;}
		if(vDefaultBeginLocation.x > 133 && nIDItem > 4) {vDefaultBeginLocation.x = 133; nIDItem = 4;}

		std::vector<std::pair<int, int>> vecPairIDLocation;

		int fOffsetItemX = 0;

		for(int nID = -1; nID < 5; nID++)
		{
			if(nID == -1) fOffsetItemX = vecItemPosition.at(0);
			if(nID == 0) fOffsetItemX = vecItemPosition.at(1);
			if(nID == 1) fOffsetItemX = vecItemPosition.at(2);
			if(nID == 2) fOffsetItemX = vecItemPosition.at(3);
			if(nID == 3) fOffsetItemX = vecItemPosition.at(4);
			if(nID == 4) fOffsetItemX = vecItemPosition.at(5);

			vecPairIDLocation.push_back({nID, fOffsetItemX});
		}

		//update fire
		for(auto& m : vecFireParticles)
		{
			m.v += olc::vf2d(0.0f, 20.0f) * fElapsedTime;
			m.p += m.v * fElapsedTime;
			m.fAngle += 5.0f * fElapsedTime;
			m.fnSimTimeRemaining -= fElapsedTime;
			m.color.g = (m.fnSimTimeRemaining / 0.5f) * 200;
		}

	int nSimulationSteps = 4;
	int nMaxSimulationSteps = 15;

	float fElapsedSimulationTime = fElapsedTime / (float)nSimulationSteps;

	float fEfficienty = 1.00f;

	//Update Materials
	for(int i = 0; i < nMaxSimulationSteps; i++)
	{
		//Water
		SetPropMaterial(vecWaterParticles, 200.0f, 0.1f, 0.1f, fElapsedSimulationTime);

		//Sand
		SetPropMaterial(vecSandParticles, 100.0f, 0.1f, 0.5f, fElapsedSimulationTime);

		//Mud
		SetPropMaterial(vecMudParticles, 50.0f, 0.0f, 0.1f, fElapsedSimulationTime);

		//Ice
		SetPropMaterial(vecIceParticles, 100.0f, 0.5f, 0.1f, fElapsedSimulationTime);

		//Stone
		SetPropMaterial(vecStoneParticles, 100.0f, 0.2f, 0.1f, fElapsedSimulationTime);
	}


	//NON-MODYFYING
		//DYNAMIC COLLISION
			for(auto v : vecCollidingPair)
			{
				sMaterial* b1 = v.first;
				sMaterial* b2 = v.second;

				float fDistance = distanceSquared(b1->p, b2->p);

				float nx = (b2->p.x - b1->p.x) / fDistance;
				float ny = (b2->p.y - b1->p.y) / fDistance;

				float tx = -ny;
				float ty = nx;

				float fDotNorm1 = (b1->v.x * nx + b1->v.y * ny);
				float fDotNorm2 = (b2->v.x * nx + b2->v.y * ny);

				float fDotTangent1 = (b1->v.x * tx + b1->v.y * ty);
				float fDotTangent2 = (b2->v.x * tx + b2->v.y * ty);

				//momentum in 1Dimension
				float m1 = fEfficienty * (fDotNorm1 * (b1->fmass - b2->fmass) + 2.0f * b2->fmass * fDotNorm2) / (b1->fmass + b2->fmass);
				float m2 = fEfficienty * (fDotNorm2 * (b2->fmass - b1->fmass) + 2.0f * b1->fmass * fDotNorm1) / (b1->fmass + b2->fmass);

				b1->v = {tx * fDotTangent1 + nx * m1, ty * fDotTangent1 + ny * m1};
				b2->v = {tx * fDotTangent2 + nx * m2, ty * fDotTangent2 + ny * m2};

			}


		std::string textbox = "Choose item by A or D";

		Clear(olc::GREY);

		//Text above toolbar
		DrawString(vScreenLocation.x, vScreenLocation.y - 10, textbox);

		olc::Pixel::Mode currentPixelMode = this->GetPixelMode();
		SetPixelMode(olc::Pixel::ALPHA);

		//toolbar z-index = -1
		DrawSprite(vScreenLocation, spr.get(), vSize.x, vSize.y);

		//itemPointers draw z-index = 0
		for(int i = 0; i < 6; i++)
			DrawSprite(vecPairIDLocation[i].second + vPadding.x, vScreenLocation.y + vPadding.y ,itemPointer[i].get(), vSize.x, vSize.y);

		//Active Slot z-index = 1
		DrawSprite(vDefaultBeginLocation, sprActiveSlot.get(), vSize.x, vSize.y);
		SetPixelMode(currentPixelMode);


		 for(auto& m : vecFireParticles)
			 	 FillCircle(m.p, m.fradius, m.color);

		for(auto& m : vecWaterParticles)
				 FillRect(m.p.x, m.p.y, 5, 5, olc::PixelF(101, 47, 60));

		 for(auto& m : vecSandParticles)
				 FillCircle(m.p, m.fradius, olc::YELLOW);

		 for(auto& m : vecMudParticles)
			 	 FillRect(m.p.x, m.p.y, 5, 5, olc::PixelF(200, 0, 0));

		 for(auto& m : vecIceParticles)
			 	 FillRect(m.p.x, m.p.y, 5, 5, olc::PixelF(0, 1, 5));

		 for(auto& m : vecStoneParticles)
			 	 FillCircle(m.p, m.fradius, olc::DARK_GREY);

		for(auto& l : vecLines)
		{
			float nx = -(l.end.y - l.start.y);
			float ny = (l.end.x - l.start.x);

			float d = sqrt(nx * nx + ny * ny);

			nx /= d;
			ny /= d;

			DrawLine((l.start.x - nx * l.fRadius), (l.start.y - ny * l.fRadius), (l.end.x - nx * l.fRadius), (l.end.y - ny * l.fRadius), olc::WHITE);
			FillCircle(l.start, l.fRadius, olc::BLACK);
			FillCircle(l.end, l.fRadius, olc::BLACK);
			DrawLine((l.start.x + nx * l.fRadius), (l.start.y + ny * l.fRadius), (l.end.x + nx * l.fRadius), (l.end.y + ny * l.fRadius), olc::WHITE);
		}


		//Clear vectors
		vecPairIDLocation.clear();
		vecItemPosition.clear();

		for(auto& f : vecFakeBalls) delete f;
		vecFakeBalls.clear();
		vecCollidingPair.clear();


		return true;
	}

public:

	void SetPositionMaterial(std::vector<sMaterial>& vec)
	{
		for(auto& m : vec)
			m.p = {ScreenWidth() + 10.0f, 100.0f};
	}

	void SetParamMaterial(std::vector<sMaterial>& vec, float radius)
	{
		if(GetMouse(0).bHeld && p_selectedSegment == nullptr)
		{
			for(int i = 0; i < 1; i++)
			{
				id++;
				m.p = {(float)GetMouseX(), (float)GetMouseY()};
				m.fradius = radius;
				m.nID = id;

				vec.push_back(m);

			}

		}
	}

	void EraseMaterial(std::vector<sMaterial>& vec)
	{
		vec.erase(std::remove_if(vec.begin(), vec.end(),
				[&](const sMaterial& m){return m.p.y > this->ScreenHeight();}), vec.end());
	}

	void SetPropMaterial(std::vector<sMaterial>& vec, float gravity, float mass, float fStaticOverlap, float dt)
	{
		for(auto& b : vec)
					b.fnSimTimeRemaining = dt;

				//Update particles water
				for(auto& p : vec)
				{
					p.op = p.p;

					p.ax = -p.v.x * 0.8f;
					p.ay = -p.v.y * 0.8f + gravity;

					p.v.x += p.ax * p.fnSimTimeRemaining;
					p.v.y += p.ay * p.fnSimTimeRemaining;

					p.p += {p.v.x * p.fnSimTimeRemaining, p.v.y * p.fnSimTimeRemaining};

					p.fmass = p.fradius * 10.0f;


					for(auto& edge : vecLines)
					{
						float fLineX1 = (edge.end.x - edge.start.x);
						float fLineY1 = (edge.end.y - edge.start.y);

						float fLineX2 = (p.p.x - edge.start.x);
						float fLineY2 = (p.p.y - edge.start.y);

						//make dot product
						float fDotProduct = (fLineX1 * fLineX2 + fLineY1 * fLineY2);

						//edge length
						float fEdgeLength = (fLineX1 * fLineX1 + fLineY1 * fLineY1);

						float t = std::max(0.0f, std::min(fEdgeLength, fDotProduct)) / fEdgeLength;

						//Make Closest Points
						olc::vf2d fClosestPoint = {edge.start.x + t * fLineX1, edge.start.y + t * fLineY1};
						float fDistance = distanceSquared(p.p, fClosestPoint);

						if(fDistance < (p.fradius + edge.fRadius))
						{
							sMaterial* fakeball = new sMaterial();

							fakeball->p.x = fClosestPoint.x;
							fakeball->p.y = fClosestPoint.y;
							fakeball->fradius = edge.fRadius;
							fakeball->fmass = p.fmass * mass;
							fakeball->v.x = -p.v.x;
							fakeball->v.y = -p.v.y;

							vecFakeBalls.push_back(fakeball);
							vecCollidingPair.push_back({&p, fakeball});

							float fOverlap = 1.0f * (fDistance - p.fradius - fakeball->fradius);

							p.p.x -= fOverlap * (p.p.x - fakeball->p.x) / fDistance;
							p.p.y -= fOverlap * (p.p.y - fakeball->p.y) / fDistance;

						}

					}

					//STATIC COLLISION
					for(auto& t : vec)
					{
						if(p.nID != t.nID)
						{
							if(ParticleVsParticle(&p, &t))
							{
								vecCollidingPair.push_back({&p, &t});

								float fDistance = distanceSquared(p.p, t.p);
								float fOverlap = fStaticOverlap * (fDistance - p.fradius - t.fradius);

								p.p.x -= (fOverlap * (p.p.x - t.p.x)) / fDistance;
								p.p.y -= (fOverlap * (p.p.y - t.p.y)) / fDistance;
								t.p.x += (fOverlap * (p.p.x - t.p.x)) / fDistance;
								t.p.y += (fOverlap * (p.p.y - t.p.y)) / fDistance;

							}
						}
					}

					float fIntendedSpeed = sqrtf(p.v.x * p.v.x + p.v.y * p.v.y);
					float fIntendedDistance = fIntendedSpeed * p.fnSimTimeRemaining;
					float fActualDistance = sqrtf((p.p.x - p.op.x)*(p.p.x - p.op.x)+(p.p.y - p.op.y)*(p.p.y - p.op.y));
					float fActualTime = fActualDistance / fIntendedSpeed;

					p.fnSimTimeRemaining = p.fnSimTimeRemaining - fActualTime;

			}
	}

};


#endif /* SRC_STATE_H_ */
