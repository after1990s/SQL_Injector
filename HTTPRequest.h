/*用于发送和接收HTTP信息。
 */
#pragma once
#include <string>
#include "stdafx.h"
#include <windows.h>
#include <WinInet.h>
#pragma comment(lib,"Wininet.lib") 
using namespace std;

class HTTPRequest{
public:
	enum HTTPREQUESTCLASS{GET, POST, COOKIE};
	HTTPRequest(string &Url, bool bProxy=false);
	void SetProxy(bool proxy);/**<使用IE代理设置,更改代理设置需要销毁之前的internet handle*/
	bool send(string &Content, HTTPREQUESTCLASS RequestClass);/**<发送请求*/
	bool recv(string &Content);/**<接收数据*/
	void setHeader(string Header);
private:
	string m_Header;
	string m_Url;
	string m_Domain;
	string m_ReqPath;
	bool m_bProxy;
	HINTERNET m_hNet;// = InternetOpen( customheader[1].c_str(), INTERNET_OPEN_TYPE_PRECONFIG,/*使用ie代理设置*/ NULL, NULL, 0);
	HINTERNET m_hSession;// = InternetConnect(hSendMailNet, postDomain.c_str(), iPort, NULL, NULL, INTERNET_SERVICE_HTTP,NULL, NULL);
	HINTERNET m_hRequest;// = 
	bool m_bHandleUse;//使用中不能设置代理
	bool m_bInitial;

};