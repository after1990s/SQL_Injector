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
	static const int GETMASK=0x001, POSTMASK=0x010, COOKIEMASK=0x100;
  //HTTPRequest(string &Dmoain,string &Page, INJECTCLASS method)
	HTTPRequest(string &Dmoain,string &Page, int method);
	void setPostData(string &PostData);
	string &GetContent();
private:
	string m_Content;
	string m_Domain;
	string m_Page;
	string m_PostData;
	int m_Method;
};