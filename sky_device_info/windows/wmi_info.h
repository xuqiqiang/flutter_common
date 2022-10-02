#pragma once
#pragma warning (disable: 4101 4456 4701 4703 4715 4996 4189)

#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <wininet.h>
#include <Ws2tcpip.h>
#include <sstream>

#include <Wbemidl.h>
#include <tchar.h>
#include <comdef.h>
#include <exception>

#include<wlanapi.h>

#include "win32_utils.h"

#pragma comment(lib,"Iphlpapi.lib")
//#pragma comment(lib,"Wsock32.lib")
#pragma comment(lib,"ws2_32.lib")

#pragma comment(lib, "wbemuuid.lib")

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

using namespace std;

class WMIWBEMINFO
{
private:
	static WMIWBEMINFO* currentInstance;
	HRESULT hres;
	IWbemServices* pSvc;
	IWbemLocator* pLoc;
	WMIWBEMINFO() {

		// Step 1: --------------------------------------------------
		// Initialize COM. ------------------------------------------

		hres = CoInitializeEx(0, COINIT_MULTITHREADED);

		//TCHAR vRamCharBuff[50];
		//_stprintf(vRamCharBuff, _T("hres:%ld %d"), hres, hres < 0 ? 5:6);
		//wstring vrammegabytesStr = wstring(vRamCharBuff);
		//::MessageBox(NULL, vRamCharBuff, L"Dropped!", MB_ICONINFORMATION);
		// CoInitializeSecurity 返回0x80010119的原因 https://blog.csdn.net/xunying6967/article/details/82657534
		if (FAILED(hres) && RPC_E_TOO_LATE != hres) {
			throw std::exception("Failed to initialize COM library");
		}
		//if (1)return;

		// Step 2: --------------------------------------------------
		// Set general COM security levels --------------------------

		hres = CoInitializeSecurity(
			NULL,
			-1,                          // COM authentication
			NULL,                        // Authentication services
			NULL,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
			NULL,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities 
			NULL                         // Reserved
		);


		if (FAILED(hres) && RPC_E_TOO_LATE != hres) {
			CoUninitialize();
			throw std::exception("Failed to initialize COM security levels");
		}

		//obtain the initial locator to WMI

		this->pLoc = NULL;

		hres = CoCreateInstance(
			CLSID_WbemLocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator, (LPVOID*)& pLoc);

		if (FAILED(hres) && RPC_E_TOO_LATE != hres) {
			CoUninitialize();
			throw std::exception("Failed to create IWbemLocator object");
		}

		// Step 4: -----------------------------------------------------
		// Connect to WMI through the IWbemLocator::ConnectServer method

		this->pSvc = NULL;

		// Connect to the root\cimv2 namespace with
		// the current user and obtain pointer pSvc
		// to make IWbemServices calls.
		hres = pLoc->ConnectServer(
			_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
			NULL,                    // User name. NULL = current user
			NULL,                    // User password. NULL = current
			0,                       // Locale. NULL indicates current
			NULL,                    // Security flags.
			0,                       // Authority (for example, Kerberos)
			0,                       // Context object 
			&pSvc                    // pointer to IWbemServices proxy
		);

		if (FAILED(hres) && RPC_E_TOO_LATE != hres) {
			pLoc->Release();
			CoUninitialize();
			throw std::exception("Could not connect to root/cimv2 namespace");
		}


		// Step 5: --------------------------------------------------
		// Set security levels on the proxy -------------------------

		hres = CoSetProxyBlanket(
			pSvc,                        // Indicates the proxy to set
			RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
			RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
			NULL,                        // Server principal name 
			RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
			RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
			NULL,                        // client identity
			EOAC_NONE                    // proxy capabilities 
		);

		if (FAILED(hres) && RPC_E_TOO_LATE != hres) {
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
			throw std::exception("Could not set proxy blanket");
		}
	}
public:

	static WMIWBEMINFO* getWMIWBEMINFOInstance();
	static void deleteWMIWBEMINFOInstance();
	IWbemLocator *getWbemLocator();
	IWbemServices* getWbemServices();
	HRESULT getHres();
	~WMIWBEMINFO();
};


WMIWBEMINFO* WMIWBEMINFO::currentInstance;

WMIWBEMINFO* WMIWBEMINFO::getWMIWBEMINFOInstance() {
	if (currentInstance == nullptr) {
		currentInstance = new WMIWBEMINFO();
	}
	return currentInstance;
}

void WMIWBEMINFO::deleteWMIWBEMINFOInstance() {
	if (currentInstance != nullptr) {
		delete currentInstance;
		currentInstance = nullptr;
	}
}

IWbemLocator* WMIWBEMINFO::getWbemLocator() {
	return this->pLoc;
}

IWbemServices* WMIWBEMINFO::getWbemServices() {
	return this->pSvc;
}

HRESULT WMIWBEMINFO::getHres() {
	return this->hres;
}

WMIWBEMINFO::~WMIWBEMINFO() {
	this->pSvc->Release();
	this->pLoc->Release();
	CoUninitialize();
}










IEnumWbemClassObject* executeWQLQuery(HRESULT hres, IWbemLocator* pLoc,
	IWbemServices* pSvc, BSTR stringQuery) {
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		stringQuery,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	//test this behavior
	if (FAILED(hres)) {
		std::cout << "Fatal error: Query to operating system error: " << hres << std::endl;

		//pSvc->Release();
		//pLoc->Release();
		//CoUninitialize();
	}
	return pEnumerator;
}

bstr_t buildQueryString(const wchar_t* wmiClass, vector<LPCWSTR> attrs) {

	WCHAR queryString[256] = { 0 };
	wcscpy(queryString, L"SELECT ");
	auto it = attrs.begin();
	for (auto it = attrs.begin(); it != attrs.end(); it++) {
		wcscat(queryString, *it);
		if (!((attrs.end() - it == 1))) {
			wcscat(queryString, L", ");
		}
	}
	wcscat(queryString, L" FROM ");
	wcscat(queryString, wmiClass);
	return bstr_t(queryString);
}

int GetSSID(std::string& ssid) {
	// Declare and initialize variables.
	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;      //    
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	DWORD dwRetVal = 0;
	int iRet = 0;

	WCHAR GuidString[39] = { 0 };

	unsigned int i, j, k;

	/* variables used for WlanEnumInterfaces  */

	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;

	PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;
	PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;

	int iRSSI = 0;

	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanOpenHandle error: %u\n", dwResult);
		return 1;
		// You can use FormatMessage here to find out why the function failed
	}

	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanEnumInterfaces error: %u\n", dwResult);
		return 2;
		// You can use FormatMessage here to find out why the function failed
	}

	wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
	wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
	if (pIfList->dwNumberOfItems <= 0 || pIfList->dwIndex < 0
		|| pIfList->dwIndex >= pIfList->dwNumberOfItems) return 1;
	pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[pIfList->dwIndex];
	//wprintf(L"  Interface Index[%u]:\t %lu\n", i, i);
	iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
		sizeof(GuidString) / sizeof(*GuidString));

	if (pIfInfo->isState != wlan_interface_state_connected) return 3;

	// variables used for WlanQueryInterfaces for opcode = wlan_intf_opcode_current_connection
	PWLAN_CONNECTION_ATTRIBUTES pConnectInfo = NULL;
	DWORD connectInfoSize = sizeof(WLAN_CONNECTION_ATTRIBUTES);
	WLAN_OPCODE_VALUE_TYPE opCode = wlan_opcode_value_type_invalid;

	dwResult = WlanQueryInterface(hClient,
		&pIfInfo->InterfaceGuid,
		wlan_intf_opcode_current_connection,
		NULL,
		&connectInfoSize,
		(PVOID*)&pConnectInfo,
		&opCode);

	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanQueryInterface error: %u\n", dwResult);
		dwRetVal = 1;
		return 4;
	}

	if (pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength == 0) {
		return 5;
	}
	char cSSID[DOT11_SSID_MAX_LENGTH];
	k = 0;
	for (;
		k < pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength;
		k++) {
		cSSID[k] = pConnectInfo->wlanAssociationAttributes.dot11Ssid.
			ucSSID[k];
	}
	cSSID[k] = '\0';

	ssid = cSSID;
	return 0;
}

class PlatformInfo {

public:

	static std::wstring SafeString(const wchar_t* pString) {
		return std::wstring((pString == nullptr ? L"(null)" : pString));
	}

	struct {
		std::wstring Name{};
		std::wstring OSName{};
		std::wstring MonitorName{};
	} System;

	struct GPUObject {
		std::wstring Name{};
		std::wstring DriverVersion{};
	};
	std::vector <GPUObject> GPU{};

	//struct NetAddress {
	//	std::string IpAddress; // IP 地址
	//	std::string IpMaskAddress; // 子网地址
	//	std::string GatewayIpAddress; // 网关地址
	//};

	struct NetworkObject {
		std::string Name{};
		std::string Id{};
		ULONG Index{};
		ULONG Bandwidth;
		std::string ConnectionName{};
		SHORT ConnectionStatus;
		std::string SSID{};
		BOOL Enabled;
		UINT Type; // 网卡类型
		std::string MacAddress; // 网卡MAC地址
		//std::vector <NetAddress> NetAddresses{};
		std::string IpAddress; // IP 地址
		std::string IpMaskAddress; // 子网地址
		std::string GatewayIpAddress; // 网关地址
	};
	std::vector <NetworkObject> NetworkAdapters{};

	void trimNullTerminator(wstring& strToTrim) {
		strToTrim = strToTrim.erase(strToTrim.length());
	}

	void QuerySystem() {

		WMIWBEMINFO* wmiWbemInfo = WMIWBEMINFO::getWMIWBEMINFOInstance();

		vector<LPCWSTR> queryAttrs = { L"Name" };

		IEnumWbemClassObject* pEnumerator = NULL;
		IWbemClassObject* pclsObj = NULL;
		ULONG uReturn = 0;

		pEnumerator =
			executeWQLQuery(wmiWbemInfo->getHres(),
				wmiWbemInfo->getWbemLocator(), wmiWbemInfo->getWbemServices(),
				buildQueryString(L"Win32_ComputerSystem", queryAttrs));
		if (pEnumerator) {
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);
			if (0 != uReturn) {
				VARIANT vtProp;
				hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
				wstring name = vtProp.bstrVal;
				name.erase(name.length());
				trimNullTerminator(name);
				std::wcout << L"Win32_ComputerSystem Name:" << name << std::endl;
				System.Name = name;
				VariantClear(&vtProp);
			}
			if (NULL != pclsObj) pclsObj->Release();
			pEnumerator->Release();
		}
		
		pEnumerator =
			executeWQLQuery(wmiWbemInfo->getHres(),
				wmiWbemInfo->getWbemLocator(), wmiWbemInfo->getWbemServices(),
				buildQueryString(L"Win32_OperatingSystem", queryAttrs));
		if (pEnumerator) {
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);
			if (0 != uReturn) {
				VARIANT vtProp;
				hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
				wstring name = vtProp.bstrVal;
				name.erase(name.length());
				trimNullTerminator(name);

				//std::wstring wOSName{ SafeString(OSName.at(0)) };
				if (name.find('|') != std::wstring::npos) {
					name.resize(name.find('|'));
				}
				std::wcout << L"Win32_OperatingSystem Name:" << name << std::endl;
				System.OSName = name;
				VariantClear(&vtProp);
			}
			if (NULL != pclsObj) pclsObj->Release();
			pEnumerator->Release();
		}

		pEnumerator =
			executeWQLQuery(wmiWbemInfo->getHres(),
				wmiWbemInfo->getWbemLocator(), wmiWbemInfo->getWbemServices(),
				buildQueryString(L"Win32_DesktopMonitor", queryAttrs));
		if (pEnumerator) {
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);
			if (0 != uReturn) {
				VARIANT vtProp;
				hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
				wstring name = vtProp.bstrVal;
				name.erase(name.length());
				trimNullTerminator(name);
				std::wcout << L"Win32_DesktopMonitor Name:" << name << std::endl;
				System.MonitorName = name;
				VariantClear(&vtProp);
			}
			if (NULL != pclsObj) pclsObj->Release();
			pEnumerator->Release();
		}
	}


	void QueryGPU() {

		WMIWBEMINFO* wmiWbemInfo = WMIWBEMINFO::getWMIWBEMINFOInstance();

		vector<LPCWSTR> queryAttrs = { L"Name", L"DriverVersion" };

		IEnumWbemClassObject* pEnumerator =
			executeWQLQuery(wmiWbemInfo->getHres(),
				wmiWbemInfo->getWbemLocator(), wmiWbemInfo->getWbemServices(),
				buildQueryString(L"Win32_VideoController", queryAttrs));


		IWbemClassObject* pclsObj = NULL;
		ULONG uReturn = 0;

		GPU.clear();
		//std::vector <GPUObject> GPU{};

		while (pEnumerator) {
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn) {
				break;
			}

			VARIANT vtProp;
			hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
			wstring name = vtProp.bstrVal;
			name.erase(name.length());
			trimNullTerminator(name);

			std::wcout << L"name:" << name << std::endl;


			hr = pclsObj->Get(L"DriverVersion", 0, &vtProp, 0, 0);
			wstring version = vtProp.bstrVal;
			version.erase(version.length());
			trimNullTerminator(version);

			GPUObject _gpu{};
			_gpu.Name = name;
			_gpu.DriverVersion = version;

			GPU.push_back(_gpu);

			VariantClear(&vtProp);

			pclsObj->Release();
		}
		if (NULL != pEnumerator) pEnumerator->Release();
	}

	std::string virtual_adapter_macs[10];

	void QueryNetwork(bool exclude_virtual_adapter) {
		NetworkAdapters.clear();
		//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
		PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
		PIP_ADAPTER_INFO pAdapter = NULL;
		//得到结构体大小,用于GetAdaptersInfo参数
		unsigned long stSize = sizeof(IP_ADAPTER_INFO);
		//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
		int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
		if (ERROR_BUFFER_OVERFLOW == nRel) {
			//如果函数返回的是ERROR_BUFFER_OVERFLOW
			//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
			//这也是说明为什么stSize既是一个输入量也是一个输出量
			//释放原来的内存空间
			free(pIpAdapterInfo);
			//重新申请内存空间用来存储所有网卡信息
			pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
			//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
			nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
		}
		if (ERROR_SUCCESS == nRel) {
			//bool isNetInfo = FALSE;//判断是否联网
			pAdapter = pIpAdapterInfo;

			//输出网卡信息
			 //可能有多网卡,因此通过循环去判断
			while (pAdapter) {
				IP_ADDR_STRING* pIpAddrString = &(pAdapter->IpAddressList);
				if (atoi(pIpAddrString->IpAddress.String) != 0) {

					std::string macAddress = "";
					for (DWORD i = 0; i < pAdapter->AddressLength; i++) {
						char cs[10];
						sprintf_s(cs, "%02X", pAdapter->Address[i]);
						macAddress += cs;
						if (i < pAdapter->AddressLength - 1) {
							macAddress += ":";
						}
					}

					if (exclude_virtual_adapter) {
						// 综合stackoverflow和github得出（2016年10月8日）。
						// 包含以下MAC地址的前8个字节（前3段）是虚拟网卡
						// vmware1 "00:05:69";
						// vmware2 "00:0C:29";
						// vmware3 "00:50:56";
						// vmware4 "00:1c:14";
						// parallels1 "00:1C:42";
						// microsoft virtual pc "00:03:FF";
						// virtual iron 4 "00:0F:4B";
						// red hat xen , oracle vm , xen source, novell xen "00:16:3E";
						// virtualbox "08:00:27";
						// wsl "00:15:5D";
						bool is_virtual_adapter = false;
						for (int i = 0; i < 10; i++) {
							std::string head = virtual_adapter_macs[i];
							if (macAddress.compare(0, head.size(), head) == 0) {
								is_virtual_adapter = true;
								break;
							}
						}
						if (is_virtual_adapter) {
							//std::cout << "exclude_virtual_adapter: " << macAddress << std::endl;
							pAdapter = pAdapter->Next;
							continue;
						}
					}
					

					NetworkObject _adapter{};

					_adapter.MacAddress = macAddress;

					_adapter.Name = GBK_2_UTF8(pAdapter->Description);
					_adapter.Id = pAdapter->AdapterName;
					_adapter.Index = pAdapter->Index;
					_adapter.Type = pAdapter->Type;

					_adapter.IpAddress = pIpAddrString->IpAddress.String;
					_adapter.IpMaskAddress = pIpAddrString->IpMask.String;
					_adapter.GatewayIpAddress = pAdapter->GatewayList.IpAddress.String;

					
					

					/*do {
						NetAddress _address{};
						_adapter.NetAddresses.push_back(_address);
						cout << "IP 地址：" << pIpAddrString->IpAddress.String << endl;
						cout << "子网地址：" << pIpAddrString->IpMask.String << endl;
						cout << "网关地址：" << pAdapter->GatewayList.IpAddress.String << endl;
						pIpAddrString = pIpAddrString->Next;
					} while (pIpAddrString);*/





					IWbemClassObject* pclsObj = NULL;
					ULONG uReturn = 0;

					WMIWBEMINFO* wmiWbemInfo = WMIWBEMINFO::getWMIWBEMINFOInstance();
					vector<LPCWSTR> queryAttrs = { L"GUID", L"Speed", L"NetConnectionID",
						L"NetConnectionStatus", L"NetEnabled" };

					IEnumWbemClassObject* pEnumerator =
						executeWQLQuery(wmiWbemInfo->getHres(),
							wmiWbemInfo->getWbemLocator(), wmiWbemInfo->getWbemServices(),
							buildQueryString(L"Win32_NetworkAdapter", queryAttrs));

					while (NULL != pEnumerator) {
						pEnumerator->Next(WBEM_INFINITE, 1,
							&pclsObj, &uReturn);

						if (0 == uReturn) {
							break;
						}

						VARIANT vtProp;

						pclsObj->Get(L"GUID", 0, &vtProp, 0, 0);
						//std::wcout << L"Description vt:" << vtProp.vt << std::endl;
						if (vtProp.vt == VT_BSTR) {
							wstring guid = vtProp.bstrVal;
							guid.erase(guid.length());
							trimNullTerminator(guid);

							string id = W_To_A(guid);

							if (id.compare(pAdapter->AdapterName) == 0) {
								pclsObj->Get(L"Speed", 0, &vtProp, 0, 0);
								if (vtProp.vt == VT_BSTR) {
									wstring speed = vtProp.bstrVal;
									speed.erase(speed.length());
									trimNullTerminator(speed);

									std::wcout << L"Speed:" << speed << std::endl;

									wstringstream strIn;
									strIn << speed;
									long long llNum;
									strIn >> _adapter.Bandwidth;
									//_adapter.Bandwidth = 
								}

								pclsObj->Get(L"NetConnectionID", 0, &vtProp, 0, 0);
								//std::wcout << L"NetConnectionID vt:" << vtProp.vt << std::endl;
								if (vtProp.vt == VT_BSTR) {
									wstring connectionID = vtProp.bstrVal;
									connectionID.erase(connectionID.length());
									trimNullTerminator(connectionID);

									//std::cout << "NetConnectionID:" << W_To_A(connectionID) << std::endl;
									_adapter.ConnectionName = GBK_2_UTF8(W_To_A(connectionID));
								}

								/*
								* 已 中断 连线(0)
									连接(1)
									连接(2)
									中断(3) 的连线
									硬体不存在(4)
									硬体已停用(5)
									硬体故障(6)
									媒体已中断 连线(7)
									验证(8)
									验证成功(9)
									验证错误(10)
									不正确位址(11)
									需要认证(12)
									其他13–65535
								*/
								pclsObj->Get(L"NetConnectionStatus", 0, &vtProp, 0, 0);
								if (vtProp.vt == VT_I4) {
									_adapter.ConnectionStatus = vtProp.iVal;
								}

								pclsObj->Get(L"NetEnabled", 0, &vtProp, 0, 0);
								if (vtProp.vt == VT_BOOL) {
									_adapter.Enabled = vtProp.bVal;
								}

								break;
							}
						}
					}
					if (NULL != pEnumerator) pEnumerator->Release();

					// 无线网卡
					if (_adapter.Type == IF_TYPE_IEEE80211) {
						GetSSID(_adapter.SSID);
					}

					NetworkAdapters.push_back(_adapter);
					//if (onlyFirst) break;
				}
				pAdapter = pAdapter->Next;
			}
			/*if (isNetInfo == false) {
				cout << "请插入网线" << endl;
			}*/
		}
		//释放内存空间
		if (pIpAdapterInfo) {
			delete pIpAdapterInfo;
		}
	}

	std::string PrintSystem() {
		std::string info = "\"deviceName\":\"" + W_To_A(System.Name)
			+ "\",\"osName\":\"" + W_To_A(System.OSName)
			+ "\",\"monitorName\":\"" + W_To_A(System.MonitorName) + "\"";
		return info;
	}

	std::string PrintGPU() {
		std::string info = "\"gpu\":[";
		for (int i = 0; i < GPU.size(); i++) {
			if (i > 0) info += ",";
			info += "{\"name\":\"" + W_To_A(GPU.at(i).Name) + "\",\"version\":\""
				+ W_To_A(GPU.at(i).DriverVersion) + "\"}";
		}
		info += "]";
		return info;
	}

	std::string PrintNetwork() {
		std::string info = "\"networkAdapters\":[";

		for (int i = 0; i < NetworkAdapters.size(); i++) {
			if (i > 0) info += ",";
			NetworkObject _adapter = NetworkAdapters.at(i);
			info += "{\"name\":\"" + _adapter.Name
				+ "\",\"id\":\"" + _adapter.Id
				+ "\",\"index\":" + to_string(_adapter.Index)
				+ ",\"bandwidth\":" + to_string(_adapter.Bandwidth)
				+ ",\"connectionName\":\"" + _adapter.ConnectionName
				+ "\",\"connectionStatus\":" + to_string(_adapter.ConnectionStatus)
				+ ",\"ssid\":\"" + _adapter.SSID
				+ "\",\"enabled\":" + To_BOOL(_adapter.Enabled)
				+ ",\"type\":" + to_string(_adapter.Type)
				+ ",\"macAddress\":\"" + _adapter.MacAddress
				+ "\",\"ipAddress\":\"" + _adapter.IpAddress
				+ "\",\"ipMaskAddress\":\"" + _adapter.IpMaskAddress
				+ "\",\"gatewayIpAddress\":\"" + _adapter.GatewayIpAddress
				+ "\"}";
		}
		info += "]";
		return info;
	}

	/*std::string ToString(std::string id) {
		std::string info = "{\"id\":\"" + id + "\","
			+ PrintSystem() + ","
			+ PrintGPU() + ","
			+ PrintNetwork() + "}";
		return info;
	}*/

	PlatformInfo() {
		/*QuerySystem();
		QueryGPU();
		QueryNetwork();*/
		virtual_adapter_macs[0] = "00:05:69";
		virtual_adapter_macs[1] = "00:0C:29";
		virtual_adapter_macs[2] = "00:50:56";
		virtual_adapter_macs[3] = "00:1c:14";
		virtual_adapter_macs[4] = "00:1C:42";
		virtual_adapter_macs[5] = "00:03:FF";
		virtual_adapter_macs[6] = "00:0F:4B";
		virtual_adapter_macs[7] = "00:16:3E";
		virtual_adapter_macs[8] = "08:00:27";
		virtual_adapter_macs[9] = "00:15:5D";
	}
};
