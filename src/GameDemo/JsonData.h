#pragma once
#ifndef _JSONDATA_H
#define _JSONDATA_H
#include "json\json.h"
#include "TheCreator.h"

extern TheCreator theCreator;
extern vector<Model> Models;			//模型

//创建jsonff
void CreateJson(string& json)
{
	std::string jsonStr;

	//Value相当于一个结构体
	//Json::Value/* root, fruit, mail,*/ 
	Json::Value SceneGameObjects, GameObjectValue, TransformValue,
		RigidbodyValue,BoxColliderValue,ModelValue,PxVec3Value;

	Json::StreamWriterBuilder writerBuilder;
	std::ostringstream os;
	//自动换行
	writerBuilder.settings_["indentation"] = "\t";

	//root["Name"] = "Liangbaikai";
	//root["Age"] = 25010;
	//root["hasRigidbody"] = false;
	//root["gender"] = "man";

	//fruit[0] = "apple";
	//fruit[1] = "orange";
	//fruit[2] = "banana";
	//root["fruit"] = fruit;

	//mail["QQ"] = "XXX@qq.com";
	//mail["Google"] = "XXX@gmail.com";
	//root["mail"] = mail;
	//root["test"]["fruit"] = mail;

	for (int i = 0; i < theCreator.SceneGameObject.size(); i++)
	{
		GameObject cur = theCreator.SceneGameObject[i];

		//名字
		GameObjectValue["Name"] = cur.Name;

		//Transform
		TransformValue[0] = cur.transform.p.x;
		TransformValue[1] = cur.transform.p.y;
		TransformValue[2] = cur.transform.p.z;
		TransformValue[3] = cur.transform.q.x;
		TransformValue[4] = cur.transform.q.y;
		TransformValue[5] = cur.transform.q.z;
		TransformValue[6] = cur.transform.q.w;
		GameObjectValue["Transform"] = TransformValue;

		//Rigidbody
		if (cur.g_rigidBody == nullptr)
		{
			RigidbodyValue["has"]=false;
		}
		else
		{
			RigidbodyValue["has"] =true;
			if (cur.isStatic)
			{
				RigidbodyValue["isStatic"] = true;
			}
			else
			{
				RigidbodyValue["isStatic"] = false;
			}
			GameObjectValue["Rigidbody"] = RigidbodyValue;
		}

		//BoxCollider
		if (cur.hasComponent("BoxCollider"))
		{
			BoxCollider* box = (BoxCollider*)cur.GetComponent("BoxCollider");
			PxVec3Value[0] = box->Transform.p.x;
			PxVec3Value[1] = box->Transform.p.y;
			PxVec3Value[2] = box->Transform.p.z;

			BoxColliderValue["localTransform"] = PxVec3Value;

			PxVec3Value[0] = box->Size.x;
			PxVec3Value[1] = box->Size.y;
			PxVec3Value[2] = box->Size.z;

			BoxColliderValue["size"] = PxVec3Value;
			GameObjectValue["BoxCollider"] = BoxColliderValue;
		}

		//Model
		if (cur.hasComponent("ModelComponent"))
		{
			ModelComponent* modelP = (ModelComponent*)cur.
				GetComponent("ModelComponent");

			ModelValue["idx"] = modelP->item_current;
			GameObjectValue["ModelComponent"] = ModelValue;
		}



		SceneGameObjects[i] = GameObjectValue;
	}




	// jsonWriter是智能指针, 当jsonWriter被析构时, 它所指向的对象(内存)也被自动释放
	std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
	jsonWriter->write(SceneGameObjects, &os);
	jsonStr = os.str();

	//std::cout << jsonStr << std::endl;

	json = jsonStr;
}

//保存成文件
void StringToFile(string& path, string& str)
{
	ofstream ofs;
	ofs.open(path);
	if (!ofs.is_open())
	{
		cout << "open file failed" <<endl;
		return;
	}
	ofs << str;
	ofs.close();
	return;
}
//读取文件
void FileToString(string &path)
{
	ifstream ifs;
	ifs.open(path, ios::in);
	if (!ifs.is_open())
	{
		cout << "open file failed" << endl;
		return;
	}

	Json::CharReaderBuilder reader;
	Json::Value SceneGameObjects, GameObjectValue, TransformValue,
		RigidbodyValue, BoxColliderValue, ModelValue, PxVec3Value;

	//int iage = 0;
	JSONCPP_STRING errs;

	bool res = Json::parseFromStream(reader, ifs, &SceneGameObjects, &errs);
	if (!res || !errs.empty())
	{
		cout << "parseJson error!" << errs << endl;
		return;
	}

	for (int i = 0; i < SceneGameObjects.size(); i++)
	{
		GameObjectValue = SceneGameObjects[i];
		GameObject* gameObject = new GameObject();

		//name
		string name = GameObjectValue["Name"].asString();
		//cout << name << endl;
		gameObject->SetName(name.data());

		//Transform
		TransformValue = GameObjectValue["Transform"];
		PxVec3 pos(TransformValue[0].asFloat(), TransformValue[1].asFloat(), TransformValue[2].asFloat());
		PxQuat q(TransformValue[3].asFloat(), TransformValue[4].asFloat(), TransformValue[5].asFloat(), TransformValue[6].asFloat());
		PxTransform tm(pos,q);
		//cout << TransformValue[0] << TransformValue[1] << TransformValue[2] << endl;
		gameObject->SetTransform(tm);	//需要加上旋转

		//Rigidbody
		bool isnull= GameObjectValue["Rigidbody"].isNull();
		if (!isnull)
		{
			RigidbodyValue = GameObjectValue["Rigidbody"];
			bool hasRig = RigidbodyValue["has"].asBool();
			bool isStatic = RigidbodyValue["isStatic"].asBool();
			//cout << RigidbodyValue["has"] << " " << RigidbodyValue["isStatic"] << endl;
			if (hasRig)
			{
				if (isStatic)
				{
					RigidBody* r = new RigidBody(gameObject, true);
				}
				else
				{
					RigidBody* r = new RigidBody(gameObject, false);
				}
			}
		}
		
		//BoxCollider
		if (!GameObjectValue["BoxCollider"].isNull())
		{
			BoxColliderValue = GameObjectValue["BoxCollider"];
			PxVec3Value = BoxColliderValue["localTransform"];
			PxTransform boxTransform(PxVec3Value[0].asFloat(), PxVec3Value[1].asFloat(), PxVec3Value[2].asFloat());
			//cout << TransformValue[0] << " " << TransformValue[1] << " " << TransformValue[2] << endl;
			PxVec3Value = BoxColliderValue["size"];
			PxTransform boxSize(PxVec3Value[0].asFloat(), PxVec3Value[1].asFloat(), PxVec3Value[2].asFloat());
			//cout << TransformValue[0] << " " << TransformValue[1] << " " << TransformValue[2] << endl;
			BoxCollider* box = new BoxCollider(gameObject);
			box->Transform = boxTransform;
			box->Size = boxSize.p;
			box->SetShape();
		}
		
		//Model
		if (!GameObjectValue["ModelComponent"].isNull())
		{
			ModelValue = GameObjectValue["ModelComponent"];
			int idx = ModelValue["idx"].asInt();
			ModelComponent* mod = new ModelComponent(gameObject);
			mod->item_current = idx;
			mod->SetModel(Models[idx]);
		}

		theCreator.SceneGameObject.push_back(*gameObject);
	}
	//iage = root["Age"].asInt();
	//string name = root["Name"].asString();
	//fruit = root["fruit"];
	//mail = root["mail"];
	//ifs.close();

	//cout << "age:" << iage <<endl;
	//cout << name << endl;
	//cout << "fruit:" << fruit[0] << " " << fruit[1] << endl;
	//cout << "mail:" << mail["Google"] << " " << mail["QQ"] << endl;
	
}


static bool FileToValue(string& path, Json::Value& value)
{
	ifstream ifs;
	ifs.open(path);

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;

	JSONCPP_STRING errs;
	if (!Json::parseFromStream(builder, ifs, &value, &errs))
	{
		cout << errs << endl;
		ifs.close();
		return false;
	}
	ifs.close();
	return true;
}

static bool StringToValue(string jsonString, Json::Value& value)
{
	const auto length = static_cast<int>(jsonString.length());
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader>reader(builder.newCharReader());

	JSONCPP_STRING errs;
	if (!reader->parse(jsonString.c_str(), jsonString.c_str() + length, &value, &errs))
	{
		cout << errs << endl;
		return false;
	}
	return true;
}

static bool ValueToFile(const string& path, Json::Value& value)
{
	ofstream ofs(path.c_str());

	if (!ofs)
	{
		ofs.close();
		return false;
	}

	Json::StreamWriterBuilder builder;
	const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	writer->write(value, &ofs);
	ofs.close();
	return true;
}

static bool ValueToString(string& jsonString, Json::Value& value)
{
	Json::StreamWriterBuilder builder;
	jsonString = Json::writeString(builder, value);
	return true;
}
#endif // !_JSONDATA_H

