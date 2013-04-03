#include "HTTPRequest.h"
#include <windows.h>
#include <string>
#include <WinInet.h>
using namespace std;
HTTPRequest::HTTPRequest(string &Url, bool bProxy/* =false */)
{//http://after1990s.info/post.php
	m_bInitial = false;
	m_Header = "Python 3.3";
	this->m_Url = Url;
	if (Url.find("http://") == NULL))
	{
		return;
	}
	int i_domainEndIndex=Url.find('/', 7)-7;
	m_Domain = Url.substr(7, i_domainEndIndex);
	m_ReqPath = Url.substr(i_domainEndIndex, Url.length()-i_domainEndIndex);

	if (bProxy)
	{
		m_hNet = InternetOpen( m_Url.c_str(), INTERNET_OPEN_TYPE_PRECONFIG,/*使用ie代理设置*/ NULL, NULL, 0);
	}
	else
	{
		m_hNet = InternetOpen( m_Url.c_str(), INTERNET_OPEN_TYPE_DIRECT,/*不使用ie代理设置*/ NULL, NULL, 0);
	}
	m_bHandleUse = false;
	m_bInitial = true;
}
void HTTPRequest::setHeader(string Header)
{
	m_Header = Header;
}
void HTTPRequest::SetProxy(bool proxy)
{
	if (this->m_bProxy != proxy)
	{
		CloseHandle(m_hSession);
		CloseHandle(m_hRequest);
		CloseHandle(m_hNet);
	}
	this->m_bProxy = proxy;
	if (m_bProxy)
	{
		m_hNet = InternetOpen( m_Url.c_str(), INTERNET_OPEN_TYPE_PRECONFIG,/*使用ie代理设置*/ NULL, NULL, 0);
	}
	else
	{
		m_hNet = InternetOpen( m_Url.c_str(), INTERNET_OPEN_TYPE_DIRECT,/*不使用ie代理设置*/ NULL, NULL, 0);
	}
	m_bHandleUse = false;
}

bool HTTPRequest::send(string &Content, HTTPREQUESTCLASS RequestClass)
{
	string customheader[10];
	customheader[0]= "Accept: */*"; 
	customheader[1]= "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 6.1)"; 
	customheader[2]= "Host: " + m_Domain; 
	customheader[3]= "Connection: no";
	customheader[4]= "Proxy-Connection: no";
	m_hSession = InternetConnect(m_hNet, m_Domain.c_str(), 80, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
	for (int i=0; i<4; i++){
		HttpAddRequestHeaders(m_hSession, customheader[i].c_str(), m_Header.length(), HTTP_ADDREQ_FLAG_ADD);
	}
	switch(RequestClass){
	case GET:
		break;
	case POST:
		break;
	case COOKIE:
		break;

	}
}
