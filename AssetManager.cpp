#include "pch.h"
#include "AssetManager.h"


//������Ʈ���� �̹����� �ϵ� �����͸� ��������� �ȵǱ� ������ Weak_ptr�� ���������Ѵ�.
AssetManager::AssetManager()
{
	doc = new tinyxml2::XMLDocument();
}

AssetManager::~AssetManager()
{
	delete doc;
	//imgDic.clear();
}

std::weak_ptr<Gdiplus::Image> AssetManager::GetImage(std::wstring str)
{
	//hash�� 
	std::hash<std::wstring> makeHash;
	//ret�� return value
	std::weak_ptr<Gdiplus::Image> ret;

	//hash�� ���� imgDic�� �ִ� img������ ã�´�.

	auto it = imgDic.find(makeHash(str));
	
	//���� ���� imgDic�� ���ٸ� MyLoadImage�� ���� img������ �������ش�.
	if (it == imgDic.end())
		ret = MyLoadImage(str);
	else
		ret = it->second;

	//expired�� ����ϸ� ���� ����ϰ��ִ� shared Point�� ���¸� Ȯ���� �� �ִ�.
	//if (ret.expired())
	//{
	//	auto p = ret.lock();
	//	p->
	//}
    //https://docs.microsoft.com/ko-kr/cpp/standard-library/weak-ptr-class?view=vs-2019
	
	return ret;
}

std::weak_ptr<Gdiplus::Image> AssetManager::MyLoadImage(std::wstring fileName)
{
	std::wstring temp(TEXT("Image\\"));
	temp.append(fileName);

	std::shared_ptr<Gdiplus::Image> Img = std::make_shared<Gdiplus::Image>(temp.c_str());

	/*if (Img = nullptr)
	{
		return;
	}*/

	std::hash<std::wstring> makeHash;
	imgDic.insert(std::make_pair(makeHash(fileName), Img));
	
	 return Img;
}

void AssetManager::SetXMLData(std::vector<Gdiplus::Rect> &Rects, char* fileName)
{

	doc->LoadFile(fileName);
	
	if (doc == nullptr) return;
	
	tinyxml2::XMLElement* Root = doc->RootElement();
	//tinyxml2::XMLElement* TextureAtlasInfo = Root->FirstChildElement("TextureAtlas");
	tinyxml2::XMLElement* atlasInfo = Root->FirstChildElement("sprite");

	for (tinyxml2::XMLElement* element = atlasInfo; element != nullptr; element = element->NextSiblingElement())
	{
		Gdiplus::Rect rect(Gdiplus::Rect(element->IntAttribute("x"), element->IntAttribute("y"),
			element->IntAttribute("w"), element->IntAttribute("h")) );
	Rects.emplace_back(rect);
	}

}

void AssetManager::SetXMLData(std::unordered_map<int, Gdiplus::Rect>& dic, char* fileName)
{
	doc->LoadFile(fileName);

	if (doc == nullptr) return;

	tinyxml2::XMLElement* Root = doc->RootElement();
	//tinyxml2::XMLElement* TextureAtlasInfo = Root->FirstChildElement("TextureAtlas");
	tinyxml2::XMLElement* atlasInfo = Root->FirstChildElement("sprite");

	for (tinyxml2::XMLElement* element = atlasInfo; element != nullptr; element = element->NextSiblingElement())
	{
		int key = element->IntAttribute("num");
		Gdiplus::Rect rect(element->IntAttribute("x"), element->IntAttribute("y"),
			element->IntAttribute("w"), element->IntAttribute("h"));
		dic.insert(std::make_pair(key,rect));
	}
}

void AssetManager::SetObjectXMLData(std::vector<ObjectXMLData>& objXMLDataVec, char* fileName)
{
	doc->LoadFile(fileName);

	if (doc == nullptr) return;

	tinyxml2::XMLElement* Root = doc->RootElement();
	tinyxml2::XMLElement* atlasInfo = Root->FirstChildElement("object");

	for (tinyxml2::XMLElement* element = atlasInfo; element != nullptr; element = element->NextSiblingElement())
	{
		ObjectXMLData oData(element->IntAttribute("num"), element->IntAttribute("x"), element->IntAttribute("y"), element->IntAttribute("w"), element->IntAttribute("h"));
		objXMLDataVec.emplace_back(oData);
	}
}

void AssetManager::SetCheckPointData(std::vector<int>& vec, char* fileName)
{
	doc->LoadFile(fileName);

	if (doc == nullptr) return;

	tinyxml2::XMLElement* Root = doc->RootElement();
	tinyxml2::XMLElement* atlasInfo = Root->FirstChildElement("point");

	for (tinyxml2::XMLElement* element = atlasInfo; element != nullptr; element = element->NextSiblingElement())
	{
		vec.emplace_back(element->IntAttribute("pos"));
	}
}
