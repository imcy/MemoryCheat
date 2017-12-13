#pragma once
#include <windows.h>
#include <list>
#include <algorithm>
class CMemFinder
{
public:
	CMemFinder()
	{
		// ���һҳ�ڴ�Ĵ�С
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		m_dwPageSize = info.dwPageSize;
	}
	~CMemFinder()
	{
		// �رս��̾��
		SafeCloseHandle();
	}
	// �Ƿ��ǺϷ��� ���̾��
	bool IsValidHandle()
	{
		return (m_hProcess && INVALID_HANDLE_VALUE != m_hProcess);
	}
	// �򿪽���
	bool OpenProcess(DWORD dwProcessId)
	{
		// ��������Ǵ򿪵�,�ر�
		if (IsValidHandle()) {
			SafeCloseHandle();
		}

		// ��
		m_hProcess = ::OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE |
			PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD |
			PROCESS_QUERY_INFORMATION,
			FALSE, dwProcessId);
		if (IsValidHandle()) {
			return true;
		}
		else {
			SafeCloseHandle();
			return false;
		}
	}
	// �رս��̾��
	void SafeCloseHandle()
	{
		CloseHandle(m_hProcess);
		m_hProcess = INVALID_HANDLE_VALUE;
	}
	
	//��������
	template<typename T>
	bool FindFirst(DWORD dwProcessId, DWORD dwBegin, DWORD dwEnd, T value)
	{
		m_arList.clear();
		return _FindFirst(dwProcessId, dwBegin, dwEnd, value);
	}
	/*
	�� FindFirst����Ļ�����, �������бȽ�
	*/
	template<typename T>
	bool FindNext(T value)
	{
		return _FindNext(value);
	}
	// ���ó���ɨ��Ļص�����
	virtual void SetCallbackFirst(bool(__stdcall *pGoonFirst)(void *pArgs, size_t nAddrCount, size_t index), void *pArgs)
	{
		m_pGoonFirst = pGoonFirst;
		m_pArgsFirst = pArgs;
	}

	// ���ó�������ɨ��Ļص�����
	virtual void SetCallbackNext(bool(__stdcall *pGoonNext)(void *pArgs, size_t nAddrCount, size_t index), void *pArgs)
	{
		m_pGoonNext = pGoonNext;
		m_pArgsNext = pArgs;
	}

	// Ŀ����̾��
	HANDLE m_hProcess{ INVALID_HANDLE_VALUE };
	DWORD m_dwPageSize;
private:

	// �����Ĳ��Һ���
	template<typename T>
	bool _FindFirst(DWORD dwProcessId, DWORD dwBegin, DWORD dwEnd, T value)
	{

		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, dwProcessId);
		m_hProcess = hProcess;
		if (hProcess == NULL) {
			return false;
		}

		// Ŀ��ֵ�ĳ���
		const size_t len = sizeof(value);
		const void *pValue = &value;
		MEMORY_BASIC_INFORMATION mbi;
		DWORD dwBaseAddress = dwBegin;
		do {
			// �����ѯ�ڴ�����ʧ��: Խ����ҳ,����һ��ҳ�Ĳ�ѯ
			if (0 == VirtualQueryEx(hProcess, (LPVOID)dwBaseAddress, &mbi, sizeof(mbi))) {
				dwBaseAddress += m_dwPageSize;
				continue;
			}

			// ��һ����������ַ
			dwBaseAddress = (DWORD)mbi.BaseAddress + mbi.RegionSize;

			// �ص�����
			if (!m_pGoonFirst || !m_pGoonFirst(m_pArgsFirst, dwEnd - dwBegin, dwBaseAddress - dwBegin)) {
				return false;
			}

			// �����ڴ�����
			if (mbi.State != MEM_COMMIT || (mbi.Protect != PAGE_READWRITE &&
				mbi.Protect != PAGE_READONLY &&
				mbi.Protect != PAGE_EXECUTE_READ &&
				mbi.Protect != PAGE_EXECUTE_READWRITE)) {
				//����δ����򲻿ɶ�������
				continue;
			}


			// ��������
			DWORD dwReadSize;
			char *Buf = new char[mbi.RegionSize];
			if (ReadProcessMemory(hProcess, (LPVOID)mbi.BaseAddress, Buf, mbi.RegionSize, &dwReadSize) == 0) {
				delete[] Buf;
				CloseHandle(hProcess);
				return false;
			}

			//��������ڴ�����
			{
				DWORD dwBaseAddr = (DWORD)mbi.BaseAddress;
				for (size_t i = 0; i < mbi.RegionSize - len; ++i) {
					void *p = &Buf[i];
					// ����Ҫ���ҵ�ֵ��
					if (memcmp(p, pValue, len) == 0) {
						m_arList.push_back(dwBaseAddr + i);
					}
				}
			}
			// ɾ���ڴ�����
			delete[] Buf;
		} while (dwBaseAddress < dwEnd);
		return true;
	}
	// �����Ĳ��Һ���
	template<typename T>
	bool _FindNext(T value)
	{
		// Ŀ��ֵ�ĳ���
		const size_t len = sizeof(value);
		const void *pValue = &value;

		// �����ַ����
		size_t cnt = m_arList.size();
		size_t index = 0;
		std::list<DWORD> dwTemp;
		for (auto addr : m_arList) {
			// ����лص�����,���һص��������� FALSE,���˳�����
			if (this->m_pGoonNext && !this->m_pGoonNext(this->m_pArgsNext, cnt, index++)) {
				return false;
			}
			// ������Ϣ
			WaitForIdle();

			// ��������
			T tReadValue;
			if (!::ReadProcessMemory(m_hProcess, (LPCVOID)addr, &tReadValue, len, NULL)) {
				continue; // û�ж�ȡ�ɹ�
			}

			// ��Ŀ��ֵ���бȽ�
			if (0 == memcmp(pValue, &tReadValue, len)) {
				// ֵ���, ����
				dwTemp.push_back(addr);
			}
		}
		// ���汾�ν��
		m_arList = dwTemp;

		return !m_arList.empty();
	}
	// �ص�����
	typedef bool(__stdcall *PFUN_CALLBACK)(void *pArgs, size_t nPageCount, size_t index);
	// �״�ɨ�� �ص�����
	PFUN_CALLBACK m_pGoonFirst{ nullptr };
	// �´�ɨ�� �ص�����
	PFUN_CALLBACK m_pGoonNext{ nullptr };
	// �ص������Ĳ��� : Ϊ�û����ûص�����ʱ,��������ָ��,�������ڵ��ûص�����ʱ,ԭ�ⲻ������ȥ
	void *m_pArgsFirst{ nullptr };
	void *m_pArgsNext{ nullptr };
};

