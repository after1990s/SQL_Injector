#include "Injectior.h"
#include<vector>
#include <string>
#include "stdafx.h"
using namespace std;
void Injector::SeparatePara()
{
	//http://
	int iHttpLength = 0;
	int iFirstSlash = 0;
	if((iHttpLength = strnlen_s("http://", 1024))!=-1)
		iFirstSlash = m_Url.find('/', iHttpLength);
	else if ((iHttpLength = strnlen_s("https://", 1024))!=-1)
		iFirstSlash = m_Url.find('/', iHttpLength);
	else
		iHttpLength = 0;
	
	m_Domain = m_Url.substr(iHttpLength, iFirstSlash - iHttpLength);
	if (m_Url.find('?',iFirstSlash)==-1)
		m_Page =m_Url.substr(iFirstSlash, m_Url.length()-iFirstSlash);
	else
		m_Page = m_Url.substr(iFirstSlash, m_Url.find('?',iFirstSlash)-iFirstSlash);//需要‘？’
	if (m_InjectClass & GETMASK)
	{//从url中分离GET参数
		string AllPara = m_Url.substr(m_Url.find('?',0)+1, m_Url.length()-m_Url.find('?',0)-1);
		string temp;
		for (string::iterator itr=AllPara.begin(); itr!=AllPara.end(); ++itr){
			if (*itr!='&'){
				temp += *itr;
			}
			else
			{
				m_GetPara.push_back(temp);
				temp.clear();
			}
		}
		m_GetPara.push_back(temp);
		temp.clear();	
	}
	if (m_InjectClass & POSTMASK)
	{
		string AllPara = m_csql_injectordlg->m_stringRawPost;
		string temp;
		for (string::iterator itr=AllPara.begin(); itr!=AllPara.end(); ++itr){
			if (*itr!='&'){
				temp += *itr;
			}
			else
			{
				m_PostPara.push_back(temp);
				temp.clear();
			}
		}
		m_PostPara.push_back(temp);
		temp.clear();
	}
	if (m_InjectClass & COOKIEMASK)
	{
		/*string AllPara = m_csql_injectordlg->m_stringRawCookie;
		string temp;
		for (string::iterator itr=AllPara.begin(); itr!=AllPara.end(); ++itr){
			if (*itr!='&'){
				temp += *itr;
			}
			else
			{
				m_CookiePara.push_back(temp);
				temp.clear();
			}
		}*/
		return;
	}
	return;
}
bool Injector::BooleanInjectTesting(int HTTPMethod)//
{//TODO:未包括POST
	string allPara;//保存全部参数
	string normalPara;//正常的参数
	string testingPara;//测试的参数
	string comparestring;
	comparestring= m_csql_injectordlg->m_CompareString;
	if (HTTPMethod &GETMASK){
		for (vector<string>::iterator i=m_GetPara.begin(); i!=m_GetPara.end(); ++i)
		{//需要测试的某个参数i
			//首先清除上次测试留下的数据
			allPara.clear();
			normalPara.clear();
			testingPara.clear();
			for (vector<string>::iterator j=m_GetPara.begin(); j!=m_GetPara.end(); ++j)
			{//将所有参数提取出来并连接，除了测试参数
				if (i!=j)
				{
					allPara += *j;
					allPara += '&';
				}
			}
			allPara = m_Page +"?"  + *i;
			normalPara = allPara+ "aNd 1=1";
			testingPara = allPara + " aNd 1=2"; 	
			HTTPRequest *normalRequest = new HTTPRequest(m_Domain, allPara, HTTPMethod);
			HTTPRequest *testingRequest = new HTTPRequest(m_Domain, testingPara, HTTPMethod);
			string normalContent = normalRequest->GetContent();
			if (normalContent.find(comparestring, 0) == -1)
			{//正常的访问出错
				delete normalRequest;
				delete testingRequest;
				continue;
			}
			string testingContent = testingRequest->GetContent();
			if (testingContent.find(comparestring, 0) == -1)
			{
				//有注入漏洞
				m_VulnerabilityClass = BOOLEANINJECT;
				return true;

			}
			//end loop
		}
	}
	else if(HTTPMethod &POSTMASK)//Post注入
	{

		for (vector<string>::iterator i=m_PostPara.begin(); i!=m_PostPara.end(); ++i)
		{//需要测试的某个参数i
			//首先清除上次测试留下的数据
			allPara.clear();
			normalPara.clear();
			testingPara.clear();
			for (vector<string>::iterator j=m_PostPara.begin(); j!=m_PostPara.end(); ++j)
			{//将所有参数提取出来并连接，除了测试参数
				if (i!=j)
				{
					allPara += *j;
					allPara += '&';
				}
			}
			allPara += *i;
			normalPara = allPara + "aNd 1=1";
			testingPara = allPara + "aNd 1=2";
			HTTPRequest *normalRequest = new HTTPRequest(m_Domain, m_Page, HTTPMethod);
			HTTPRequest *testingRequest = new HTTPRequest(m_Domain, m_Page, HTTPMethod);
			normalRequest->setPostDate(allPara);
			testingRequest->setPostDate(testingPara);
			string normalContent = normalRequest->GetContent();
			if (normalContent.find(comparestring, 0) == -1)
			{//正常的访问出错
				delete normalRequest;
				delete testingRequest;
				continue;
			}
			string testingContent = testingRequest->GetContent();
			if (testingContent.find(comparestring, 0) == -1)
			{
				//有注入漏洞
				m_VulnerabilityClass = BOOLEANINJECT;
				return true;

			}//end loop

		}
	}
	else{//未知的http请求方式
		return false;
	}
	return false;
}
bool Injector::StringInjectTesting(int HTTPMethod)//字符串注入
{
	string allPara;//保存全部参数
	string normalPara;//正常的参数
	string testingPara;//测试的参数
	string comparestring;
	comparestring= m_csql_injectordlg->m_CompareString;
	if (HTTPMethod &GETMASK){
		for (vector<string>::iterator i=m_GetPara.begin(); i!=m_GetPara.end(); ++i)
		{//需要测试的某个参数i
			//首先清除上次测试留下的数据
			allPara.clear();
			normalPara.clear();
			testingPara.clear();
			for (vector<string>::iterator j=m_GetPara.begin(); j!=m_GetPara.end(); ++j)
			{//将所有参数提取出来并连接，除了测试参数
				if (i!=j)
				{
					allPara += *j;
					allPara += '&';
				}
			}
			allPara = m_Page +"?"  + *i;
			normalPara = allPara+ "aNd '1'='1";
			testingPara = allPara + " aNd '1'='2"; 	
			HTTPRequest *normalRequest = new HTTPRequest(m_Domain, allPara, HTTPMethod);
			HTTPRequest *testingRequest = new HTTPRequest(m_Domain, testingPara, HTTPMethod);
			string normalContent = normalRequest->GetContent();
			if (normalContent.find(comparestring, 0) == -1)
			{//正常的访问出错
				delete normalRequest;
				delete testingRequest;
				continue;
			}
			string testingContent = testingRequest->GetContent();
			if (testingContent.find(comparestring, 0) == -1)
			{
				//有注入漏洞
				return true;
				m_VulnerabilityClass = STRINGINJECT;
			}
			//end loop
		}
	}
	else if(HTTPMethod &POSTMASK)//Post注入
	{

		for (vector<string>::iterator i=m_PostPara.begin(); i!=m_PostPara.end(); ++i)
		{//需要测试的某个参数i
			//首先清除上次测试留下的数据
			allPara.clear();
			normalPara.clear();
			testingPara.clear();
			for (vector<string>::iterator j=m_PostPara.begin(); j!=m_PostPara.end(); ++j)
			{//将所有参数提取出来并连接，除了测试参数
				if (i!=j)
				{
					allPara += *j;
					allPara += '&';
				}
			}
			allPara += *i;
			normalPara = allPara + "aNd '1'='1";
			testingPara = allPara + "aNd '1'='2";
			HTTPRequest *normalRequest = new HTTPRequest(m_Domain, m_Page, HTTPMethod);
			HTTPRequest *testingRequest = new HTTPRequest(m_Domain, m_Page, HTTPMethod);
			normalRequest->setPostDate(allPara);
			testingRequest->setPostDate(testingPara);
			string normalContent = normalRequest->GetContent();
			if (normalContent.find(comparestring, 0) == -1)
			{//正常的访问出错
				delete normalRequest;
				delete testingRequest;
				continue;
			}
			string testingContent = testingRequest->GetContent();
			if (testingContent.find(comparestring, 0) == -1)
			{
				//有注入漏洞
				m_VulnerabilityClass = STRINGINJECT;
				return true;

			}//end loop

		}
	}
	else{//未知的http请求方式
		return false;
	}
	return false;
}
void Injector::TryInject()
{//尝试各种可能的注入，如果成功则调用Inject。

	SeparatePara();
	if (m_InjectClass & GETMASK)
	{
		InjectClass injectclass=GET;
		if (BooleanInjectTesting(injectclass)==true)
			//Inject;
			::MessageBox(m_csql_injectordlg->m_hWnd, _T("Get Inject"), _T("f"), NULL);
			//Inject();
		if (StringInjectTesting(injectclass)==true)
			//Inject();
			;
	}
	else if (m_InjectClass & POSTMASK)
	{
		InjectClass injectclass=POST;
		if (BooleanInjectTesting(injectclass)==true)
			//Inject;
			//Inject();
			::MessageBox(m_csql_injectordlg->m_hWnd, _T("Get Inject"), _T("f"), NULL);
		if (StringInjectTesting(injectclass)==true)
			//Inject();
			;
	}
	else if(m_InjectClass & COOKIEMASK)
	{
		return;
	}




}