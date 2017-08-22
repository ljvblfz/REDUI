
#include "StdAfx.h"
#include "DuiMarkup.h"

#ifndef TRACE
#define TRACE
#endif


///////////////////////////////////////////////////////////////////////////////////////
//
//
//

CMarkupNode::CMarkupNode() : m_pOwner(NULL), m_iPos(0), m_nAttributes(-1)
{
}

CMarkupNode::CMarkupNode(CMarkup* pOwner, int iPos) : m_pOwner(pOwner), m_iPos(iPos), m_nAttributes(-1)
{
}

CMarkupNode::CMarkupNode(const CMarkupNode& other)
{
	m_pOwner = other.m_pOwner;
	m_iPos = other.m_iPos;
	m_nAttributes = other.m_nAttributes;
	memcpy_s(&m_aAttributes, sizeof(m_aAttributes), &other.m_aAttributes, sizeof(m_aAttributes));
}

//CMarkupNode& CMarkupNode::operator=( const CMarkupNode& other )
//{
//	m_pOwner = other.m_pOwner;
//	m_iPos = other.m_iPos;
//	m_nAttributes = other.m_nAttributes;
//	memcpy_s(&m_aAttributes, sizeof(m_aAttributes), &other.m_aAttributes, sizeof(m_aAttributes));
//	return *this;
//}

CMarkupNode CMarkupNode::GetSibling(BOOL bExcludeText/*=TRUE*/)
{
	if( m_pOwner == NULL ) return CMarkupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
	while (bExcludeText && iPos>0 && m_pOwner->m_pElements[iPos].nType==CMarkup::type_text)
		iPos = m_pOwner->m_pElements[iPos].iNext;
	if( iPos == 0 ) return CMarkupNode();
	return CMarkupNode(m_pOwner, iPos);
}

BOOL CMarkupNode::HasSiblings(BOOL bExcludeText/*=TRUE*/) const
{
	if( m_pOwner == NULL ) return FALSE;
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
	while (bExcludeText && iPos>0 && m_pOwner->m_pElements[iPos].nType==CMarkup::type_text)
		iPos = m_pOwner->m_pElements[iPos].iNext;
	return iPos > 0;
}

CMarkupNode CMarkupNode::GetChild(BOOL bExcludeText/*=TRUE*/)
{
	if( m_pOwner == NULL ) return CMarkupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
	while (bExcludeText && iPos>0 && m_pOwner->m_pElements[iPos].nType==CMarkup::type_text)
		iPos = m_pOwner->m_pElements[iPos].iNext;
	if( iPos == 0 ) return CMarkupNode();
	return CMarkupNode(m_pOwner, iPos);
}

CMarkupNode CMarkupNode::GetChild(LPCOLESTR pstrName)
{
	if( m_pOwner == NULL ) return CMarkupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
	while( iPos != 0 ) {
		if( m_pOwner->m_pElements[iPos].nType!=CMarkup::type_text &&
			lstrcmpiW(m_pOwner->m_pstrXML + m_pOwner->m_pElements[iPos].iStart, pstrName) == 0 ) {
				return CMarkupNode(m_pOwner, iPos);
		}
		iPos = m_pOwner->m_pElements[iPos].iNext;
	}
	return CMarkupNode();
}

BOOL CMarkupNode::HasChildren(BOOL bExcludeText/*=TRUE*/) const
{
	if( m_pOwner == NULL ) return FALSE;
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
	while (bExcludeText && iPos>0 && m_pOwner->m_pElements[iPos].nType==CMarkup::type_text)
		iPos = m_pOwner->m_pElements[iPos].iNext;
	return iPos != 0;
}

CMarkupNode CMarkupNode::GetParent()
{
	if( m_pOwner == NULL ) return CMarkupNode();
	ULONG iPos = m_pOwner->m_pElements[m_iPos].iParent;
	if( iPos == 0 ) return CMarkupNode();
	return CMarkupNode(m_pOwner, iPos);
}

BOOL CMarkupNode::IsValid() const
{
	return m_pOwner != NULL;
}

BOOL CMarkupNode::IsTextBlock() const
{
	return m_pOwner != NULL && m_pOwner->m_pElements[m_iPos].nType == CMarkup::type_text;
}

LPCOLESTR CMarkupNode::GetName() const
{
	if( m_pOwner == NULL || IsTextBlock() ) return NULL;
	return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
}

LPCOLESTR CMarkupNode::GetValue() const
{
	if( m_pOwner == NULL ) return NULL;
	return m_pOwner->m_pstrXML + (IsTextBlock() ? m_pOwner->m_pElements[m_iPos].iStart : m_pOwner->m_pElements[m_iPos].iData);
}

LPCOLESTR CMarkupNode::GetAttributeName(int iIndex)
{
	if( m_pOwner == NULL || IsTextBlock() ) return NULL;
	if( m_nAttributes < 0 ) _MapAttributes();
	if( iIndex < 0 || iIndex >= m_nAttributes ) return NULL; //OLESTR("");
	return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iName;
}

LPCOLESTR CMarkupNode::GetAttributeValue(int iIndex)
{
	if( m_pOwner == NULL || IsTextBlock() ) return NULL;
	if( m_nAttributes < 0 ) _MapAttributes();
	if( iIndex < 0 || iIndex >= m_nAttributes ) return NULL; //OLESTR("");
	return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue;
}

LPCOLESTR CMarkupNode::GetAttributeValue(LPCOLESTR pstrName, LPCOLESTR pstrName2/*=NULL*/, LPCOLESTR pstrName3/*=NULL*/, LPCOLESTR pstrName4/*=NULL*/)
{
	if( m_pOwner == NULL || IsTextBlock() ) return NULL;
	if( m_nAttributes < 0 ) _MapAttributes();
	for( int i = 0; i < m_nAttributes; i++ ) {
		if( lstrcmpiW(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return m_pOwner->m_pstrXML + m_aAttributes[i].iValue;
	}
	if (pstrName2)
	{
		for( int i = 0; i < m_nAttributes; i++ ) {
			if( lstrcmpiW(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName2) == 0 ) return m_pOwner->m_pstrXML + m_aAttributes[i].iValue;
		}
	}
	if (pstrName3)
	{
		for( int i = 0; i < m_nAttributes; i++ ) {
			if( lstrcmpiW(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName3) == 0 ) return m_pOwner->m_pstrXML + m_aAttributes[i].iValue;
		}
	}
	if (pstrName4)
	{
		for( int i = 0; i < m_nAttributes; i++ ) {
			if( lstrcmpiW(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName4) == 0 ) return m_pOwner->m_pstrXML + m_aAttributes[i].iValue;
		}
	}
	return NULL; //OLESTR("");
}

BOOL CMarkupNode::GetAttributeValue(int iIndex, LPOLESTR pstrValue, SIZE_T cchMax)
{
	if( m_pOwner == NULL || IsTextBlock() ) return FALSE;
	if( m_nAttributes < 0 ) _MapAttributes();
	if( iIndex < 0 || iIndex >= m_nAttributes ) return FALSE;
	lstrcpynW(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue, cchMax);
	return TRUE;
}

BOOL CMarkupNode::GetAttributeValue(LPCOLESTR pstrName, LPOLESTR pstrValue, SIZE_T cchMax)
{
	if( m_pOwner == NULL || IsTextBlock() ) return FALSE;
	if( m_nAttributes < 0 ) _MapAttributes();
	for( int i = 0; i < m_nAttributes; i++ ) {
		if( lstrcmpiW(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) {
			lstrcpynW(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[i].iValue, cchMax);
			return TRUE;
		}
	}
	return FALSE;
}

int CMarkupNode::GetAttributeCount()
{
	if( m_pOwner == NULL || IsTextBlock() ) return 0;
	if( m_nAttributes < 0 ) _MapAttributes();
	return m_nAttributes;
}

BOOL CMarkupNode::HasAttributes()
{
	if( m_pOwner == NULL || IsTextBlock() ) return FALSE;
	if( m_nAttributes < 0 ) _MapAttributes();
	return m_nAttributes > 0;
}

BOOL CMarkupNode::HasAttribute(LPCOLESTR pstrName, LPCOLESTR pstrName2/*=NULL*/, LPCOLESTR pstrName3/*=NULL*/)
{
	if( m_pOwner == NULL || IsTextBlock() ) return FALSE;
	if( m_nAttributes < 0 ) _MapAttributes();
	for( int i = 0; i < m_nAttributes; i++ ) {
		if( lstrcmpiW(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return TRUE;
	}
	if (pstrName2)
	{
		for( int i = 0; i < m_nAttributes; i++ ) {
			if( lstrcmpiW(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName2) == 0 ) return TRUE;
		}
	}
	if (pstrName3)
	{
		for( int i = 0; i < m_nAttributes; i++ ) {
			if( lstrcmpiW(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName3) == 0 ) return TRUE;
		}
	}
	return FALSE;
}

void CMarkupNode::_MapAttributes()
{
	if( m_pOwner == NULL || IsTextBlock() ) return;
	m_nAttributes = 0;
	LPOLESTR pstr = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
	LPCOLESTR pstrEnd = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
	// 	if (m_pOwner->m_pElements[m_iPos].iChild > 0)
	// 		pstrEnd = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_pOwner->m_pElements[m_iPos].iChild].iStart;
	// 	else if (m_pOwner->m_pElements[m_iPos].iNext > 0)
	// 		pstrEnd = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_pOwner->m_pElements[m_iPos].iNext].iStart;
	pstr += lstrlenW(pstr) + 1;
	while( pstr < pstrEnd )
	{
		m_pOwner->_SkipWhitespace(pstr);
		m_aAttributes[m_nAttributes].iName = pstr - m_pOwner->m_pstrXML;
		while (*pstr!=0 && *pstr!=2) pstr++;
		//pstr += lstrlenW(pstr) + 1;
		// 消除后面的空白
		int i = -1;
		while (/*::iswspace*/(pstr[i])==1) pstr[i--] = 0;
		if (*pstr == 2) // 无值的属性
		{
			*pstr = 0;
			m_aAttributes[m_nAttributes++].iValue = pstr - m_pOwner->m_pstrXML;
			pstr ++;
			if( m_nAttributes >= MAX_XML_ATTRIBUTES ) return;
			continue;
		}
		pstr ++;

		m_pOwner->_SkipWhitespace(pstr);
		if (*pstr == L'\"' || *pstr==L'\'') pstr++;
		m_aAttributes[m_nAttributes++].iValue = pstr - m_pOwner->m_pstrXML;
		pstr += lstrlenW(pstr) + 1;
		i = -2;
		while (/*::iswspace*/(pstr[i])==1) pstr[i--] = 0;
		if( m_nAttributes >= MAX_XML_ATTRIBUTES ) return;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
//
//
//

CMarkup::CMarkup(LPCOLESTR pstrXML)
{
	memset(this, 0, sizeof(CMarkup));

	m_pstrXML = NULL;
	m_pElements = NULL;
	m_nElements = 0;
	m_bPreserveWhitespace = FALSE;
	if( pstrXML != NULL ) Load(pstrXML);
}

CMarkup::~CMarkup()
{
	Release();
}

BOOL CMarkup::IsValid() const
{
	return m_pElements != NULL;
}

void CMarkup::SetPreserveWhitespace(BOOL bPreserve)
{
	m_bPreserveWhitespace = bPreserve;
}

BOOL CMarkup::Load(LPCOLESTR pstrXML)
{
	Release();
	SIZE_T cchLen = lstrlenW(pstrXML) + 1;
	m_pstrXML = static_cast<LPOLESTR>(malloc(cchLen * sizeof(OLECHAR)));
	::CopyMemory(m_pstrXML, pstrXML, cchLen * sizeof(OLECHAR));
	BOOL bRes = _Parse();
	if( !bRes ) Release();
	return bRes;
}
//
//BOOL CMarkup::LoadFromFile(LPCOLESTR pstrFilename)
//{
//   Release();
//   HANDLE hFile = ::CreateFileW(pstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//   if( hFile == INVALID_HANDLE_VALUE ) return _Failed(OLESTR("Error opening file"));
//   DWORD dwSize = ::GetFileSize(hFile, NULL);
//   if( dwSize == 0 ) return _Failed(OLESTR("File is empty"));
//   DWORD dwRead = 0;
//#ifdef _UNICODE
//   ::CloseHandle(hFile);
//   return FALSE;
//#else
//   m_pstrXML = static_cast<LPTSTR>(malloc(dwSize + 1));
//   ::ReadFile(hFile, m_pstrXML, dwSize, &dwRead, NULL);
//   ::CloseHandle(hFile);
//   m_pstrXML[dwSize] = '\0';
//#endif // _UNICODE
//   if( dwRead != dwSize ) {
//      Release();
//      return _Failed(OLESTR("Could not read file"));
//   }
//   BOOL bRes = _Parse();
//   if( !bRes ) Release();
//   return bRes;
//}

void CMarkup::Release()
{
	if( m_pstrXML != NULL ) free(m_pstrXML);
	if( m_pElements != NULL ) free(m_pElements);
	m_pstrXML = NULL;
	m_pElements = NULL;
	m_nElements;
}

LPCOLESTR CMarkup::GetLastErrorMessage() const
{
	return m_szErrorMsg;
}

LPCOLESTR CMarkup::GetLastErrorLocation() const
{
	return m_szErrorXML;
}

LPCOLESTR CMarkup::GetLastWarningMessage() const
{
	return m_szWarningMsg;
}

LPCOLESTR CMarkup::GetLastWarningLocation() const
{
	return m_szWarningXML;
}

CMarkupNode CMarkup::GetRoot(BOOL bExcludeText/*=TRUE*/)
{
	if( m_nElements == 0 ) return CMarkupNode();
	CMarkupNode node(this, 1);
	if (!bExcludeText || !node.IsTextBlock()) return node;
	return node.GetSibling(bExcludeText);
}

//BOOL WINAPI DefParseMarkup(HANDLE hMarkuper, LPOLESTR& pstrText, ULONG iParent, ULONG& iPrevious)
//{
//	if (hMarkuper==NULL) return FALSE;
//
//	CMarkup* markup = (CMarkup*)hMarkuper;
//	return markup->_Parse(pstrText, iParent, iPrevious);
//}

BOOL CMarkup::_Parse()
{
	_ReserveElement(); // Reserve index 0 for errors
	::ZeroMemory(m_szErrorMsg, sizeof(m_szErrorMsg));
	::ZeroMemory(m_szErrorXML, sizeof(m_szErrorXML));
	LPOLESTR pstrXML = m_pstrXML;
	ULONG iPrev = 0;
	BOOL bForceClose = FALSE;
	return _Parse(pstrXML, 0, iPrev, bForceClose);
}

BOOL CMarkup::_Parse(LPOLESTR& pstrText, ULONG iParent, ULONG& iPrevious, BOOL& bForceClose)
{
	//ULONG iPrevious = 0;
	for( ; ; ) 
	{
		_SkipWhitespace(pstrText);
		if( *pstrText == 0 && iParent <= 1 ) return TRUE;
		if( *pstrText != L'<' ) //return _Failed(OLESTR("Expected start tag"), pstrText);
		{
			if (iParent==0)
				_Warning(OLESTR("Expected start tag"), pstrText);
				//return _Failed(OLESTR("Expected start tag"), pstrText);
			// 创建一个文本节点
			XMLELEMENT* pe = _ReserveElement();
			ULONG iTextPos = pe - m_pElements;
			pe->iStart = pstrText - m_pstrXML;
			pe->iParent = iParent;
			pe->iNext = pe->iChild = 0;
			pe->nType = type_text;
			if( iPrevious != 0 ) m_pElements[iPrevious].iNext = iTextPos;
			else if( iParent > 0 ) m_pElements[iParent].iChild = iTextPos;
			iPrevious = iTextPos;

			LPOLESTR pstrDest = pstrText;
			if( !_ParseText(pstrText, pstrDest, L'<') ) return bForceClose=TRUE, _Warning(L"Missing closing tag", pstrText);
			pstrDest = pstrText - 1;
			while (/*::iswspace*/(*pstrDest)==1) *pstrDest-- = 0;
			continue;
		}
		if( pstrText[1] == L'/' ) return TRUE;
		*pstrText++ = 0; // 现在指向标签名称
		// Skip comment or processing directive
		if( *pstrText == L'!' || *pstrText == L'?' )
		{
			OLECHAR chEnd = (*pstrText == L'!') ? L'-' : L'?';
			while( *pstrText != 0 && !(*pstrText == chEnd && *(pstrText + 1) == L'>') ) pstrText++;
			if( *pstrText != 0 ) pstrText += 2;
			continue;
		}
		// Fill out element structure
		XMLELEMENT* pEl = _ReserveElement();
		ULONG iPos = pEl - m_pElements;
		pEl->iStart = pstrText - m_pstrXML;
		pEl->iParent = iParent;
		pEl->iNext = pEl->iChild = 0;
		pEl->nType = type_element;
		if( iPrevious != 0 ) m_pElements[iPrevious].iNext = iPos;
		else if( iParent > 0 ) m_pElements[iParent].iChild = iPos;
		iPrevious = iPos;
		// Parse name
		LPCOLESTR pstrName = pstrText;
		_SkipIdentifier(pstrText);
		LPOLESTR pstrNameEnd = pstrText;
		if( *pstrText == 0 ) return _Failed(OLESTR("Error parsing element name"), pstrText);
		//*pstrNameEnd = 0;
		// Parse attributes
		if( !_ParseAttributes(pstrText) ) return FALSE;
		_SkipWhitespace(pstrText);
		pEl->iData = pstrText - m_pstrXML;
		if( pstrText[0] == L'/' && pstrText[1] == L'>' )
		{
			*pstrText = 0;
			pstrText += 2;
		}
		else if (pstrText[0] == L'>' && *_FindInlineTag(pstrName, pstrNameEnd-pstrName) != 0) // 内联标签无需使用 />
		{
			*pstrText = 0;
			pstrText ++;
		}
		else
		{
			if( *pstrText++ != L'>' ) return _Failed(OLESTR("Expected start-tag closing"), pstrText);

			_SkipWhitespace(pstrText);
			ULONG iChildPrevious = 0;

			// 如果是常量文本标签，所有内容被当做文本，例如 script/pre/style
			// ！！！注意：常量文本标签不可以使用无名称的关闭标签</>，必须带名称，例如</script>
			LPCOLESTR pc = (LPCOLESTR)_FindConstTextTag(pstrName, pstrNameEnd-pstrName);
			if (*pc != 0)
			{
				int nSkip = 0;
				// 忽略 <![CDATA[ 和 ]]> 标签
				if (_wcsnicmp(pstrText, L"<![CDATA[", 9) == 0) nSkip = 9;
				pstrText += nSkip;

				// 创建一个文本节点
				XMLELEMENT* pe = _ReserveElement();
				ULONG iTextPos = pe - m_pElements;
				pe->iStart = pstrText - m_pstrXML;
				pe->iParent = iPos;
				pe->iNext = pe->iChild = 0;
				pe->nType = type_text;
				if (iChildPrevious != 0) m_pElements[iChildPrevious].iNext = iTextPos;
				else pEl->iChild = iTextPos;
				iChildPrevious = iTextPos;

				if (nSkip==9)
				{
					LPOLESTR p = StrStrIW(pstrText, L"]]>");
					if (p==NULL) return *pstrNameEnd=0, bForceClose=TRUE, _Warning(L"Unmatched closing tag", pstrText);
					*p = 0;
					pstrText = p + 3;
				}

				OLECHAR closeTag[64] = L"</";
				lstrcatW(closeTag, pc);
				lstrcatW(closeTag, L">");
				LPOLESTR p = StrStrIW(pstrText, closeTag);
				if (p==NULL) return *pstrNameEnd=0, bForceClose=TRUE, _Warning(L"Unmatched closing tag", pstrText);
				pstrText = p--;
				//while (::iswspace(*p)) *p-- = 0;
				//continue;
			}

			// Parse text and child
			for (;;)
			{
				if( *pstrText == 0 && iParent <= 1 ) return TRUE;
				if (*pstrText == 0) return *pstrNameEnd=0, bForceClose=TRUE, _Warning(L"Unmatched closing tag", pstrText);
				// Parse text node data
				// 特殊处理 <![CDATA[  ]]>
				if (_wcsnicmp(pstrText, L"<![CDATA[", 9) == 0)
				{
					pstrText += 9;
					// 创建一个文本节点
					XMLELEMENT* pe = _ReserveElement();
					ULONG iTextPos = pe - m_pElements;
					pe->iStart = pstrText - m_pstrXML;
					pe->iParent = iPos;
					pe->iNext = pe->iChild = 0;
					pe->nType = type_text;
					if (iChildPrevious != 0) m_pElements[iChildPrevious].iNext = iTextPos;
					else pEl->iChild = iTextPos;
					iChildPrevious = iTextPos;

					LPOLESTR p = StrStrIW(pstrText, L"]]>");
					if (p==NULL) return *pstrNameEnd=0, bForceClose=TRUE, _Warning(L"Unmatched closing tag", pstrText);
					*p = 0;
					pstrText = p + 3;
					continue;
				}
				if (*pstrText != L'<')
				{
					// 创建一个文本节点
					XMLELEMENT* pe = _ReserveElement();
					ULONG iTextPos = pe - m_pElements;
					pe->iStart = pstrText - m_pstrXML;
					pe->iParent = iPos;
					pe->iNext = pe->iChild = 0;
					pe->nType = type_text;
					if (iChildPrevious != 0) m_pElements[iChildPrevious].iNext = iTextPos;
					else pEl->iChild = iTextPos;
					iChildPrevious = iTextPos;
					//// 特殊处理 <script> 和 </script> 之间的所有内容都不做转换处理
					//if (pstrNameEnd-pstrName==6 && _wcsnicmp(pstrName, L"script", pstrNameEnd-pstrName)==0)
					//{
					//	LPOLESTR p = wcsstr(pstrText, L"</script>");
					//	if (p==NULL) return _Failed(L"Unmatched closing tag", pstrText);
					//	pstrText = p--;
					//	while (::iswspace(*p)) *p-- = 0;
					//	//*p = 0;
					//	//pstrText = p + 9;
					//	//_SkipWhitespace(pstrText);
					//	continue;
					//}
					//else
					{
						LPOLESTR pstrDest = pstrText;
						if( !_ParseText(pstrText, pstrDest, L'<') ) return FALSE;
						pstrDest = pstrText - 1;
						while (/*::iswspace*/(*pstrDest)==1) *pstrDest-- = 0;
					}
				}
				else // *pstrText == L'<'
				{
					// 解析普通子节点
					if( pstrText[1] != L'/' ) 
					{
						if( !_Parse(pstrText, iPos, iChildPrevious, bForceClose) ) return FALSE;
						if (bForceClose)
						{
							*pstrNameEnd = 0;
							return -1;
						}
					}
					else // if( pstrText[1] == L'/' ) 
					{
						//*pstrDest = 0;
						*pstrText = 0;
						// 在没有歧义的情况下，关闭标签可以省略名称，即使用 </> 表示自动关闭，</sometag>也可用
						if (pstrText[2] == L'>')
						{
							pstrText += 3;
							break;
						}
						pstrText += 2;
						SIZE_T cchName = pstrNameEnd - pstrName;
						if( lstrlenW(pstrName)==cchName && _wcsnicmp(pstrText, pstrName, cchName) != 0 ) return *pstrNameEnd=0, bForceClose=TRUE, _Warning(OLESTR("Unmatched closing tag"), pstrText);
						if( pstrText[cchName] != L'>' ) return *pstrNameEnd=0, bForceClose=TRUE, _Warning(OLESTR("Unmatched closing tag"), pstrText);
						pstrText += cchName + 1;
						break;
					}
				}
			}
		}
		*pstrNameEnd = 0;
	}
}

CMarkup::XMLELEMENT* CMarkup::_ReserveElement()
{
	if( m_nElements == 0 ) m_nReservedElements = 0;
	if( m_nElements >= m_nReservedElements ) {
		m_nReservedElements += (m_nReservedElements / 2) + 500;
		m_pElements = static_cast<XMLELEMENT*>(realloc(m_pElements, m_nReservedElements * sizeof(XMLELEMENT)));
	}
	return &m_pElements[m_nElements++];
}

void CMarkup::_SkipWhitespace(LPCOLESTR& pstr, BOOL bSpaceIsWhitespace/*=TRUE*/) const
{
	while( *pstr != 0 && ::iswspace(*pstr) && (bSpaceIsWhitespace || *pstr!=L' ') ) pstr++;
}

void CMarkup::_SkipWhitespace(LPOLESTR& pstr, BOOL bSpaceIsWhitespace/*=TRUE*/) const
{
	while( *pstr != 0 && ::iswspace(*pstr) && (bSpaceIsWhitespace || *pstr!=L' ') ) pstr++;
}

void CMarkup::_SkipIdentifier(LPCOLESTR& pstr) const
{
	while( *pstr != 0 && (::iswalnum(*pstr) || *pstr==L'_' || *pstr==L':' || *pstr==L'-' || *pstr==L'.') ) pstr++;
}

void CMarkup::_SkipIdentifier(LPOLESTR& pstr) const
{
	while( *pstr != 0 && (::iswalnum(*pstr) || *pstr==L'_' || *pstr==L':' || *pstr==L'-' || *pstr==L'.') ) pstr++;
}

BOOL CMarkup::_ParseAttributes(LPOLESTR& pstrText)
{   
	if( *pstrText == L'>' || (*pstrText==L'/' && pstrText[1]==L'>') ) return TRUE;
	*pstrText++ = 0;
	_SkipWhitespace(pstrText);
	while( *pstrText != 0 && *pstrText != L'>' && *pstrText != L'/' ) {
		_SkipIdentifier(pstrText);
		if (*pstrText != L'=')
		{
			//*pstrText++ = 0;
			_SkipWhitespace(pstrText);
		}
		if( *pstrText != L'=' )
		{
			//return _Failed(OLESTR("Error while parsing attributes"), pstrText);
			// 无值属性
			pstrText[-1] = 2;
			continue;
		}
		*pstrText++ = 0;
		_SkipWhitespace(pstrText);
		OLECHAR cQuoto = *pstrText;
		if (cQuoto!=L'\"' && cQuoto!=L'\'') cQuoto = L' ';
		else pstrText++;
		//if( cQuoto != L'\"' ) return _Failed(OLESTR("Expected attribute value"), pstrText);
		LPOLESTR pstrDest = pstrText;
		if( !_ParseText(pstrText, pstrDest, cQuoto) ) return FALSE;
		if( *pstrText == 0 ) return _Failed(OLESTR("Error while parsing attribute string"), pstrText);
		//*pstrDest = 0;
		*pstrText++ = 0;
		_SkipWhitespace(pstrText);
	}
	return TRUE;
}

BOOL CMarkup::_ParseText(LPOLESTR& pstrText, LPOLESTR& pstrDest, OLECHAR cEnd)
{
	while( *pstrText != 0 && (*pstrText != cEnd /*|| pstrText[-1]==L'\\'*/) )
	{
		//if (*pstrText == L'\\')
		//{
		//	*pstrDest++ = *(++pstrText)++;
		//	continue;
		//}
		if( *pstrText == L'&' ) {
			_ParseMetaChar(++pstrText, pstrDest);
			continue;
		}
		if( ::iswspace(*pstrText) ) {
			if (*pstrText == L' ')
			{
				*pstrDest++ = *pstrText++;
				//_SkipWhitespace(pstrText);
			}
			if( !m_bPreserveWhitespace ) _SkipWhitespace(pstrText/*, FALSE*/);
		}
		else {
			*pstrDest++ = *pstrText++;
		}
	}
	// Make sure that MapAttributes() works correctly when it parses
	// over a value that has been transformed.
	LPOLESTR pstrFill = pstrDest; // + 1;
	while( pstrFill < pstrText ) *pstrFill++ = 1; /*L' '*/; // 1 是一个特殊字符，用来在后面置成0
	return TRUE;
}

void CMarkup::_ParseMetaChar(LPOLESTR& pstrText, LPOLESTR& pstrDest)
{
	if( pstrText[0] == L'a' && pstrText[1] == L'm' && pstrText[2] == L'p' && pstrText[3] == L';' ) {
		*pstrDest++ = L'&';
		pstrText += 4;
	}
	else if( pstrText[0] == L'l' && pstrText[1] == L't' && pstrText[2] == L';' ) {
		*pstrDest++ = L'<';
		pstrText += 3;
	}
	else if( pstrText[0] == L'g' && pstrText[1] == L't' && pstrText[2] == L';' ) {
		*pstrDest++ = L'>';
		pstrText += 3;
	}
	else if( pstrText[0] == L'q' && pstrText[1] == L'u' && pstrText[2] == L'o' && pstrText[3] == L't' && pstrText[4] == L';' ) {
		*pstrDest++ = L'\"';
		pstrText += 5;
	}
	else if( pstrText[0] == L'a' && pstrText[1] == L'p' && pstrText[2] == L'o' && pstrText[3] == L's' && pstrText[4] == L';' ) {
		*pstrDest++ = L'\'';
		pstrText += 5;
	}
	else if( pstrText[0] == L'n' && pstrText[1] == L'b' && pstrText[2] == L's' && pstrText[3] == L'p' && pstrText[4] == L';' ) {
		*pstrDest++ = L' ';
		pstrText += 5;
	}
	else {
		*pstrDest++ = L'&';
	}
}

BOOL CMarkup::_Failed(LPCOLESTR pstrError, LPCOLESTR pstrLocation)
{
	// Register last error
	TRACE(OLESTR("XML Error: %s"), pstrError);
	TRACE(pstrLocation);
	lstrcpynW(m_szErrorMsg, pstrError, (sizeof(m_szErrorMsg) / sizeof(m_szErrorMsg[0])) - 1);
	lstrcpynW(m_szErrorXML, pstrLocation != NULL ? pstrLocation : OLESTR(""), _countof(m_szErrorXML) - 1);
	return FALSE; // Always return 'FALSE'
}

BOOL CMarkup::_Warning(LPCOLESTR pstrWarning, LPCOLESTR pstrLocation /* = NULL */)
{
	TRACE(OLESTR("XML Warning: %s"), pstrWarning);
	TRACE(pstrLocation);
	lstrcpynW(m_szWarningMsg, pstrWarning, (sizeof(m_szWarningMsg) / sizeof(m_szWarningMsg[0])) - 1);
	lstrcpynW(m_szWarningXML, pstrLocation != NULL ? pstrLocation : OLESTR(""), _countof(m_szWarningXML) - 1);
	return -1; // Always return -1 (TRUE with warning)
}

LPOLESTR CMarkup::_FindConstTextTag( LPCOLESTR lpszTag, int cchCount/*==-1*/ )
{
	// 如果 lpszTag 为空，表示寻找最后可以添加的空闲位置（两个0）
	// 如果 cchCount <= 0，表示 lpszTag 字符串以0结束
	if (cchCount<=0 && lpszTag) cchCount = lstrlenW(lpszTag);

	LPOLESTR p = m_szConstTextTag;
	int len = 0;
	while (*p!=0 && (cchCount<=0 || (len=lstrlenW(p))!=cchCount || _wcsnicmp(p,lpszTag,len)!=0)) p += len + 1;
	return p;
}

void CMarkup::AddConstTextTag( LPCOLESTR lpszTag )
{
	if (lpszTag==NULL || *lpszTag==0) return;

	LPOLESTR p = _FindConstTextTag(lpszTag);
	if (*p==0 && (MaxConstTextTagBufLength-(p-m_szConstTextTag))>lstrlenW(lpszTag)+1)
	{
		lstrcatW(p, lpszTag);
		lstrcatW(p, L"\0");
	}
}

LPOLESTR CMarkup::_FindInlineTag( LPCOLESTR lpszTag, int cchCount/*==-1*/ )
{
	// 如果 lpszTag 为空，表示寻找最后可以添加的空闲位置（两个0）
	// 如果 cchCount <= 0，表示 lpszTag 字符串以0结束
	if (cchCount<=0 && lpszTag) cchCount = lstrlenW(lpszTag);

	LPOLESTR p = m_szInlineTag;
	int len = 0;
	while (*p!=0 && (cchCount<=0 || (len=lstrlenW(p))!=cchCount || _wcsnicmp(p,lpszTag,len)!=0)) p += len + 1;
	return p;
}

void CMarkup::AddInlineTag( LPCOLESTR lpszTag )
{
	if (lpszTag==NULL || *lpszTag==0) return;

	LPOLESTR p = _FindInlineTag(lpszTag);
	if (*p==0 && (MaxInlineTagBufLength-(p-m_szInlineTag))>lstrlenW(lpszTag)+1)
	{
		lstrcatW(p, lpszTag);
		lstrcatW(p, L"\0");
	}
}
