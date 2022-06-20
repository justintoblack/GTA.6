#pragma once
#ifndef _JSONDATA_H
#define _JSONDATA_H
#include "json\json.h"

extern TheCreator theCreator;


//创建jsonff
void CreateJson(string& json)
{
	std::string jsonStr;

	//Value相当于一个结构体
	//Json::Value/* root, fruit, mail,*/ 
	Json::Value SceneGameObjects, GameObjectValue, TransformValue,
		RigidbodyValue,BoxColliderValue,ModelValue;

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
		}
		GameObjectValue["Rigidbody"] = RigidbodyValue;

		//BoxCollider
		if (cur.hasComponent("BoxCollider"))
		{
			BoxCollider* box = (BoxCollider*)cur.GetComponent("BoxCollider");
			TransformValue[0] = box->Transform.p.x;
			TransformValue[1] = box->Transform.p.y;
			TransformValue[2] = box->Transform.p.z;

			BoxColliderValue["localTransform"] = TransformValue;

			TransformValue[0] = box->Size.x;
			TransformValue[1] = box->Size.y;
			TransformValue[2] = box->Size.z;

			BoxColliderValue["size"] = TransformValue;
		}
		GameObjectValue["BoxCollider"] = BoxColliderValue;

		//Model
		if (cur.hasComponent("ModelComponent"))
		{
			ModelComponent* modelP = (ModelComponent*)cur.
				GetComponent("ModelComponent");

			ModelValue["idx"] = modelP->item_current;
		}
		GameObjectValue["ModelComponent"] = ModelValue;



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
	Json::Value root, fruit, mail;

	int iage = 0;
	JSONCPP_STRING errs;

	bool res = Json::parseFromStream(reader, ifs, &root, &errs);
	if (!res || !errs.empty())
	{
		cout << "parseJson error!" << errs << endl;
		return;
	}

	iage = root["Age"].asInt();
	string name = root["Name"].asString();
	fruit = root["fruit"];
	mail = root["mail"];
	ifs.close();

	cout << "age:" << iage <<endl;
	cout << name << endl;
	cout << "fruit:" << fruit[0] << " " << fruit[1] << endl;
	cout << "mail:" << mail["Google"] << " " << mail["QQ"] << endl;
	
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
