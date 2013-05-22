#pragma once
#include <vector>
#include <string>
#include "SQL_InjectorDlg.h"
#include "stdafx.h"
using namespace std;
class Injector{
public:
	static const int GETMASK=0x001, POSTMASK=0x010, COOKIEMASK=0x100;
	static const int BOOLEANINJECT=1, STRINGINJECT=2;
	Injector(string &Url, int &injectclass, CSQL_InjectorDlg *csql_injectordlg)
	{
		m_Url = Url;
		m_InjectClass = injectclass;
		m_csql_injectordlg = csql_injectordlg;
	};
private:
	const static int m_iKeyWordLength = 6;
	string m_Url;
	string m_Domain;
	string m_Page;
	string m_InjectionKeyWord;//返回数据前面的那个string,比如Result:value中的Result
	string m_InjectionKeyWordEnd;//尾部的keyword
	string m_UserName;
	string m_DatabaseName;
	int m_iColumn;//列长度
	int m_iHTTPMethod;
	string m_VulnerablePara;//完整的参数，如id=3
	vector<string> m_GetPara;//不包括有漏洞的参数
	vector<string> m_PostPara;//不包括有漏洞的参数
	vector<string> m_CookiePara;
	vector<string> m_vecDatabase;
	vector<string> m_vecTable;
	vector<string> m_vecColumnName;
	CSQL_InjectorDlg *m_csql_injectordlg;
	int m_InjectClass;//可能包括多种httpmethod
	int m_VulnerabilityClass;//1 for boolean,2 for string
public:
	void SeparatePara();/*分离出domain和请求的页面，并从url或Post数据中分割出para参数并保存在m_GetPara*/
	void TryInject(); /*尝试注入，如果成功则调用GetTableName*/
	bool BooleanInjectTesting(int HTTPMethod);
	bool StringInjectTesting(int HTTPMethod);
	int GetColumnNumber();//获得列长度,仅支持boolean注入
	bool GetInjectionKeyWord(string &ResultString);//获得返回数据前面的那个string,比如Result:value中的Result，将数据后面的string放入m_InjectionKeyWordEnd中。仅支持boolean注入
	bool FirstStepInject();//获得列长度，获得定位用string，获得数据库用户名和数据库名称
	bool SecondStepInject();
	bool GetUser();//获得database用户名，并保存在m_User中。
	bool GetDatabaseName();//获得databasename，并保存在m_vecDatabase中,并写入界面
	bool GetTableName();//获得表名,保存在m_vecTable中,并写入界面
	bool GetInjectionValue(string &ResultString,string &InjectString/*如User()*/);//获得注入后的结果

	bool ExportTable(string TableName,string FileName);//导出某表到某文件
	bool GetColumnName(string TableName);//将列名保存在m_vecColumnName
};