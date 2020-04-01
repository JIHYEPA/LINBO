#include "pch.h"
#include "Player.h"
#include "MainFrm.h"
#include "PlayerControlComponent.h"
#include <cmath>

#define MAX_velocity 300
#define INIT_velocity 0.5f
#define ACCELERATION 150
#define PI (3.1415926535897932f)
#define GRAVITY 10
#define JUMPFORCE 350
#define DEGTORAD(X) (X * (PI / 180))

float Lerp(float value1, float value2, float amount)
{
	return float(value1 + ((float)(value2 - value1) * amount));
}

Player::Player()
{
	tag = eTag_Player;
	state = eState_Idle;
	bFlagLeft = false;
	velocity = 0.1f;
	width = defines.playerWidth;
	height = defines.playerHeight;
	enable = true;
	bFlagNowCol = false;
	bFlagRightCol = false;
	bFlagLeftCol = false;
	bFlagJumpStartState = false;
	//	float speed = Lerp(0, 10, 2);

		//AnimationList�� �ִϸ��̼��� �߰����ش�.  enum������� �־���� �Ѵ٤�
	AddAnimation(new Animation_Idle());
	AddAnimation(new Animation_Run());
	AddAnimation(new Animation_Jump());
	AddAnimation(new Animation_Die());
	AddAnimation(new Animation_Interaction());
	AddAnimation(new Animation_InteractionMove());

	int screenSizeWidth = defines.screenSizeX;
	//x = screenSizeWidth * 0.5f;
	x = 850;
	y = 450;

	collider = new BoxCollider2D(x, y, width, height, false);

	playerScreenPosX = x - width * 0.5;
	playerScreenPosY = y - height * 0.5;

	EventManager::GetInstance()->AddEvent(std::bind(&Player::PlayerDie, this), EEvent::eEvent_PlayerDie);
	EventManager::GetInstance()->AddEvent(std::bind(&Player::MoveReady, this), EEvent::eEvent_MoveReady);

}

Player::~Player()
{
	for (auto& it : playerAnimationList)
	{
		delete it;
	}
	delete collider;
}

void Player::Update(float Delta)
{
	//printf("Delta : %f\n", Delta);
	//Component Update
	//physics ������Ʈ
	control.Update(*this);

	PhysicsUpdate(Delta);
	GameManager::GetInstance()->SetPlayerPosX(x);
	collider->SetX(x);
	collider->SetY(y);

	//���� Animation�� image�� XML������ ���� ��������.
	playerAnimationList[state]->Update(&atlasRect, Delta);
}

void Player::Render(Gdiplus::Graphics* _MemG)
{
	//Player�� ũ��
	Gdiplus::Rect rect(0, 0, width, height);

	Gdiplus::Bitmap bm(width, height, PixelFormat32bppARGB);
	Gdiplus::Graphics temp(&bm);
	temp.DrawImage(playerAnimationList[state]->GetAtlasImg().lock().get(), rect,
		atlasRect.X, atlasRect.Y, atlasRect.Width, atlasRect.Height, Gdiplus::Unit::UnitPixel, nullptr, 0, nullptr);

	int playerScreenWidth;
	int playerScreenHeight;

	int screenPosY = y - (height * 0.5f) + 10;

	switch (state)
	{
	case eState_Die:
		playerScreenWidth = defines.playerWidth_Die;
		playerScreenHeight = height;
		screenPosY += 10;
		break;
	case eState_InteractionMove:
		playerScreenWidth = width;
		playerScreenHeight = defines.playerHeight_InterMove;
		screenPosY += 20;
		break;
	default:
		playerScreenWidth = width;
		playerScreenHeight = height;
		break;
	}

	Gdiplus::Rect screenPosRect((defines.screenSizeX / 2) - (width * 0.5f), screenPosY, playerScreenWidth, playerScreenHeight);



	//���� ���������̶�� bit�� �¿� ���������ش�.
	if (bFlagLeft)
	{
		bm.RotateFlip(Gdiplus::Rotate180FlipY);
	}

	_MemG->DrawImage(&bm, screenPosRect);

#if defined VEL_DEBUG
	if (ptList.size() < 2) return;
	for (auto it = ptList.begin(); it != ptList.end() - 1; ++it)
	{
		Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 1);
		Gdiplus::PointF& pt1 = *it;
		Gdiplus::PointF& pt2 = *(it + 1);
		MemG->DrawLine(&pen, pt1, pt2);
	}
#endif
	//temp.DrawImage(img.lock,rect, rects[frame].X, rects[frame].Y, rects[frame].Width, rects[frame].Height,
	//	Gdiplus::Unit::UnitPixel,nullptr, 0, nullptr);

	//MemG->DrawImage(&bm, rect);
	//MemG->DrawImage()
}

static float AddDelta = 0;
void Player::Jump(bool bFlagLeft, int terrainY, float Delta)
{
	AddDelta += Delta;
	float AddVal = (0.5f * GRAVITY * AddDelta * AddDelta);

	if (AddDelta > 0.3f)
	{
		bFlagJumpStartState = false;
	}
	int velocity_Int = abs(velocity * Delta)+1;
	//printf("velocity : %d\n", velocity_Int);
	if (bFlagLeft && !bFlagLeftCol)
	{
		x -= velocity_Int;
	}
	else if (!bFlagLeft && !bFlagRightCol)
	{
		x += velocity_Int;
		//printf("vel = %f\n", velocity);
	}

	if (!bFlagBotmCol)
	{
		y = y + (-140 * Delta) + AddVal;
	}

	if (y > terrainY)
	{
		y = terrainY;
		ChangeState(eState_Idle);
		AddDelta = 0.0f;
		InitVelocity();
	}
}

void Player::AddAnimation(Animation* ani)
{
	playerAnimationList.emplace_back(ani);
}

void Player::ChangeState(EPlayerState _state)
{
	playerAnimationList[state]->End();
	state = _state;
	playerAnimationList[state]->Begin();
}

static float AddUpdateDelta = 0;
static float RunSoundDelta = 0;

void Player::PhysicsUpdate(float Delta)
{
	AddUpdateDelta += Delta;
	if (AddUpdateDelta > 1.0f)
	{
		AddUpdateDelta = 0.5f;
	}
	//GameManager�� ���� Player�� X��ǥ�� ���� Terrain�� Y ������ �޾ƿ´�.
	int terrainY = GameManager::GetInstance()->GetTerrainData(x) - height * 0.5f;
	
	switch (state)
	{
	case eState_Idle:
		if (!bFlagBotmCol)
		{
			y = y + GRAVITY * AddUpdateDelta + 10;
		}
		velocity = INIT_velocity; //�ӵ� �ʱ�ȭ
		AddUpdateDelta = 0.0f;
		//���� Player�� Y��ǥ�� Terrain���� ũ�ٸ�
		if (y >= terrainY)
		{
			y = terrainY;
		}
		break;
	case eState_Run:
	{
		RunSoundDelta += Delta;
		if (RunSoundDelta > 8.0f)
		{
			SoundManager::GetInstance()->Stop(ESound::sound_walk);
			SoundManager::GetInstance()->Play(ESound::sound_walk);
			RunSoundDelta = 0.0f;
		}
		if (!bFlagBotmCol)
		{
			y = y + GRAVITY * AddUpdateDelta + 10;
		}
		//y = y + GRAVITY * AddUpdateDelta;
		velocity = ACCELERATION * AddUpdateDelta * 1.5f;
		if (velocity > MAX_velocity)
		{
			velocity = MAX_velocity;
		}
		int velocity_Int = abs(velocity * Delta);
		if (bFlagLeft && !bFlagLeftCol)
		{
			x -= velocity_Int + 1;
		}
		else if (!bFlagLeft && !bFlagRightCol)
		{
			x += velocity_Int + 1;
		}

		if (y >= terrainY)
		{
			y = terrainY;
		}
	}

	break;
	case eState_Jump:
		Jump(bFlagLeft, terrainY, Delta);
		break;
	case eState_InteractionMove:
		//x += velocity * Delta;
		//y = y + GRAVITY * AddUpdateDelta;
		//���� Player�� Y��ǥ�� Terrain���� ũ�ٸ�
	case eState_Die:
		y = y + GRAVITY * AddUpdateDelta;
		if (y >= terrainY)
		{
			y = terrainY;
		}
		break;
	}
}


EPlayerState Player::GetState()
{
	return state;
}

void Player::Collision(Object* obj)
{
	//Trigger�� �ƴҰ�� ����
	if (!obj->GetCollider()->IsTrigger())
	{
		int objLeft = obj->GetCollider()->GetX() - obj->GetCollider()->GetWidth() * 0.5f;
		int objRight = obj->GetCollider()->GetX() + obj->GetCollider()->GetWidth() * 0.5f;
		int objTop = obj->GetCollider()->GetY() - obj->GetCollider()->GetHeight() * 0.5f;
		int objBottom = obj->GetCollider()->GetY() + obj->GetCollider()->GetHeight() * 0.5f;

		int pLeft = GetCollider()->GetX() - GetCollider()->GetWidth() * 0.5f;
		int pRight = GetCollider()->GetX() + GetCollider()->GetWidth() * 0.5f;
		int pTop = GetCollider()->GetY() - GetCollider()->GetHeight() * 0.5f;
		int pBottom = GetCollider()->GetY() + GetCollider()->GetHeight() * 0.5f;

		switch (state)
		{
		case eState_Die:
			break;
		case eState_Run:
		{
			if (pLeft < objRight && abs(objRight - pLeft) < width)
			{
				bFlagLeftCol = true;
			}
			else if (pRight > objLeft && abs(pRight - objLeft) < width)
			{
				bFlagRightCol = true;
			}

			float dist = abs(pBottom - objTop);
			if (pBottom > objTop && dist < 50)
			{
				//y = objTop - height * 0.5f;
				bFlagBotmCol = true;
			}
		}
		break;
		case eState_Idle:
		{
			float dist = abs(pBottom - objTop);
			if (pBottom > objTop && dist < 50)
			{
				bFlagBotmCol = true;
				ChangeState(eState_Idle);
			}


			if (GetAsyncKeyState(VK_UP) & 0x8001)
			{
				bFlagJumpStartState = true;
			}
		}
		break;
		case eState_Jump:
		{

			if (pLeft < objRight && abs(objRight - pLeft) < width)
			{
				bFlagLeftCol = true;
				break;
			}
			else if (pRight > objLeft && abs(pRight - objLeft) < width)
			{
				bFlagRightCol = true;
				break;
			}

			float dist = abs(pBottom - objTop);

			if (pBottom > objTop && dist < 10)
			{
				if (!bFlagJumpStartState)
				{
					bFlagBotmCol = true;
					AddDelta = 0.0f;
					ChangeState(eState_Idle);
				}

			}
		}
		break;
		case eState_Interaction:
			break;
		case eState_InteractionMove:
			break;
		default:
			break;
		}
	}
	else
	{
		return;
	}
}

bool Player::GetLeftFlag()
{
	return bFlagLeft;
}
void Player::SetLeftFlag(bool Flag)
{
	bFlagLeft = Flag;
}

void Player::InitVelocity()
{
	velocity = 50;
}

void Player::PlayerDie()
{
	ChangeState(eState_Die);
	SoundManager::GetInstance()->Play(ESound::sound_Dead);
}

bool Player::GetbFlagInteraction()
{
	return bFlagInteraction;
}

void Player::SetbFlagInteraction(bool _bFlag)
{
	bFlagInteraction = _bFlag;
}

void Player::MoveReady()
{
	ChangeState(eState_InteractionMove);
}

void Player::SetVelocity(float _velocity)
{
	velocity = _velocity;
}

bool Player::HasInteraction()
{
	return false;
}

void Player::InInteractionDistance(Object* obj)
{
	switch (state)
	{
	case eState_Die:
		break;
	case eState_Run:
		break;
	case eState_Idle:
		if (GetAsyncKeyState(VK_CONTROL) & 0x8001) //��ȣ�ۿ�
		{
			ChangeState(eState_Interaction);
		}
		break;
	case eState_Interaction:
		if (GetAsyncKeyState(VK_CONTROL) & 0x8001)
		{

		}
		else
		{
			ChangeState(eState_Idle);
		}
		break;
	case eState_InteractionMove:
		if (GetAsyncKeyState(VK_CONTROL) & 0x8001)
		{
			if (GetAsyncKeyState(VK_RIGHT) & 0x8001)
			{
				x += 1;
				obj->SetX(obj->GetPosX() + 1);
			}
			else if (GetAsyncKeyState(VK_LEFT) & 0x8001)
			{
				x -= 1;
				obj->SetX(obj->GetPosX() - 1);
			}
		}
		else
		{
			ChangeState(eState_Idle);
		}
		break;
	default:
		break;
	}
}

void Player::SetNowColState(bool bFlagState)
{
	bFlagNowCol = bFlagState;
}

bool Player::GetNowColState()
{
	return bFlagNowCol;
}

void Player::InitColState()
{
	bFlagNowCol = false;
	bFlagLeftCol = false;
	bFlagRightCol = false;
	bFlagBotmCol = false;
}

void Player::SetJumpStartState(bool bFlagstate)
{
	bFlagJumpStartState = bFlagstate;
}

