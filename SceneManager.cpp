#include "pch.h"
#include "SceneManager.h"


SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
	for (auto& it : sceneVec)
	{
		delete it;
	}
}
void SceneManager::SetCurScene(Scene* scene)
{
	curScene = scene;
}

Scene* SceneManager::GetCurScene()
{
	return curScene;
}

void SceneManager::Init()
{
	introScene = new IntroScene();
	gameScene = new GameScene();
	endScene = new EndScene();
	curScene = introScene;
	

	sceneVec.emplace_back(introScene);
	sceneVec.emplace_back(gameScene);
	sceneVec.emplace_back(endScene);

	//SceneLevel 0���� ����
	sceneLevel = 0;
	//Scene�� Level �ְ� ���� ���� 3��
 	maxSceneLevel = 2;
}

void SceneManager::MoveNextScene()
{
	//������ ���� ������ �̵�
	//max���� ������ ������쿡��
	if (sceneLevel < maxSceneLevel)
	{
 		curScene = sceneVec[++sceneLevel];
	}
}

void SceneManager::Render(CRect rc)
{
	//Gdiplus::Rect rc2(rc.left, rc.top, rc.Width(), rc.Height());

	//Gdiplus::Bitmap BackBuffer(rc.Width(), rc.Height(), PixelFormat32bppARGB);
	//Gdiplus::Graphics MemG(&BackBuffer);

	//Gdiplus::SolidBrush WhiteBrush(Gdiplus::Color(255, 202, 202, 202));
	//MemG.FillRectangle(&WhiteBrush, rc2);

	//GetCurScene()->Render(&MemG);
	//MainG->DrawImage(&BackBuffer, 0, 0, rc.Width(), rc.Height());
}
