#include "HTTPRequest.h"
#include <windows.h>
#include <string>
#include <WinInet.h>
#include "stdafx.h"
using namespace std;
HTTPRequest::HTTPRequest(string &Dmoain,string &Page,int method)
{
	m_Domain = Dmoain;
	m_Page = Page;//包括 /php/inject.php?cols=2
	m_Method = method;
	m_PostData = "";
}
void HTTPRequest::setPostDate(string &PostData)
{
	m_PostData = PostData;
}
string & HTTPRequest::GetContent()
{// 仅仅用于GET方式
	CInternetSession session;
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 20);
	session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);
	CHttpConnection* pConnection = session.GetHttpConnection(m_Domain.c_str(),(INTERNET_PORT)80);
	if (m_Method&GETMASK){
		CHttpFile *pFile =  pConnection->OpenRequest( CHttpConnection::HTTP_VERB_GET,
			m_Page.c_str());
		string szHeaders = _T("Accept:text/plain, text/html, text/htm\r\n");
		pFile->AddRequestHeaders(szHeaders.c_str());
		pFile->SendRequest();
		DWORD dwRet = 0;
		pFile->QueryInfoStatusCode(dwRet);
		if(dwRet != HTTP_STATUS_OK)
		{
			return m_Content;
		}
		char temp[1024]={0};
		int ReadNumber = 1023;
		while (pFile->Read(temp, ReadNumber)==1023)
		{
			m_Content += temp;
			ZeroMemory(temp, sizeof(temp));
		}
		m_Content += temp;
		ZeroMemory(temp, sizeof(temp));
		pFile->Close();
		session.Close();
		delete pFile;
		return m_Content;
	}
	else if(m_Method & POSTMASK)
	{
		CHttpFile *pFile =  pConnection->OpenRequest( CHttpConnection::HTTP_VERB_POST,
			m_Page.c_str());
		string szHeaders   = "Content-Type: application/x-www-form-urlencoded;";
		pFile->SendRequest(szHeaders.c_str(), szHeaders.length(), (LPVOID)m_PostData.c_str(), m_PostData.length());
		DWORD dwRet = 0;
		pFile->QueryInfoStatusCode(dwRet);
		if(dwRet != HTTP_STATUS_OK)
		{
			return m_Content;
		}
		char temp[1024]={0};
		int ReadNumber = 1023;
		while (pFile->Read(temp, ReadNumber)==1023)
		{
			m_Content += temp;
			ZeroMemory(temp, sizeof(temp));
		}
		m_Content += temp;
		ZeroMemory(temp, sizeof(temp));
		pFile->Close();
		session.Close();
		delete pFile;
		return m_Content;
	}
	else
	{
		return m_Content;
	}
}