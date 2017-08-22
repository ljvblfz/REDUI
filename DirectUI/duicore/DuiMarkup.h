#if !defined(__DUIMARKUP_H__)
#define __DUIMARKUP_H__

#pragma once

#define MaxConstTextTagBufLength 1024
#define MaxInlineTagBufLength 1024

class CMarkup;
class CMarkupNode;

//BOOL WINAPI DefParseMarkup(HANDLE hMarkuper, LPOLESTR& pstrText, ULONG iParent, ULONG& iPrevious);


class  CMarkup
{
	friend CMarkupNode;
	//friend DefParseMarkup;
public:
	CMarkup(LPCOLESTR pstrXML = NULL);
	~CMarkup();

	BOOL Load(LPCOLESTR pstrXML); // return 1:succeed; 0:error; -1:succeed but warning
	//BOOL LoadFromFile(LPCOLESTR pstrFilename);
	void Release();
	BOOL IsValid() const;

	void SetPreserveWhitespace(BOOL bPreserve = TRUE);

	LPCOLESTR GetLastErrorMessage() const;
	LPCOLESTR GetLastErrorLocation() const;
	LPCOLESTR GetLastWarningMessage() const;
	LPCOLESTR GetLastWarningLocation() const;

	void AddConstTextTag(LPCOLESTR lpszTag);
	void AddInlineTag(LPCOLESTR lpszTag);

	CMarkupNode GetRoot(BOOL bExcludeText=TRUE);

private:
	enum { type_element=1, type_text=3 };
	typedef struct tagXMLELEMENT
	{
		ULONG iStart; // 字符串索引
		ULONG iChild;		// 元素索引
		ULONG iNext;		// 元素索引
		ULONG iParent;	// 元素索引
		ULONG iData;	// 字符串索引
		int nType; // 如果 nType==type_text，iChild是无效的
	} XMLELEMENT;

	LPOLESTR m_pstrXML;
	XMLELEMENT* m_pElements;
	ULONG m_nElements;
	ULONG m_nReservedElements;

	OLECHAR m_szErrorMsg[100];
	OLECHAR m_szErrorXML[256];
	OLECHAR m_szWarningMsg[100];
	OLECHAR m_szWarningXML[256];

	BOOL m_bPreserveWhitespace;
	OLECHAR m_szConstTextTag[MaxConstTextTagBufLength]; // 保存仅包含文本的标签名，例如 script/pre 等等，标签里包含的所有内容只当做文本，不进一步解析，可包含 < > 等字符
									// 多个标签用 \0 分隔，例如 L"script\0pre\0"，最后一个标签必须跟两个0。
	OLECHAR m_szInlineTag[MaxInlineTagBufLength]; // 保存内联标签名，内联标签不能包含任何子元素，可以不用关闭标签，例如 <inline> 跟 <inline />相同

private:
	BOOL _Parse();
	BOOL _Parse(LPOLESTR& pstrText, ULONG iParent, ULONG& iPrevious, BOOL& bForceClose);
	XMLELEMENT* _ReserveElement();
	inline void _SkipWhitespace(LPOLESTR& pstr, BOOL bSpaceIsWhitespace=TRUE) const;
	inline void _SkipWhitespace(LPCOLESTR& pstr, BOOL bSpaceIsWhitespace=TRUE) const;
	inline void _SkipIdentifier(LPOLESTR& pstr) const;
	inline void _SkipIdentifier(LPCOLESTR& pstr) const;
	BOOL _ParseText(LPOLESTR& pstrText, LPOLESTR& pstrData, OLECHAR cEnd);
	void _ParseMetaChar(LPOLESTR& pstrText, LPOLESTR& pstrDest);
	BOOL _ParseAttributes(LPOLESTR& pstrText);
	BOOL _Failed(LPCOLESTR pstrError, LPCOLESTR pstrLocation = NULL);
	BOOL _Warning(LPCOLESTR pstrWarning, LPCOLESTR pstrLocation = NULL);
	LPOLESTR _FindConstTextTag(LPCOLESTR lpszTag, int cchCount=-1);
	LPOLESTR _FindInlineTag(LPCOLESTR lpszTag, int cchCount=-1);
};


class  CMarkupNode
{
	friend CMarkup;
private:
	CMarkupNode(CMarkup* pOwner, int iPos);

public:
	CMarkupNode();
	CMarkupNode(const CMarkupNode& other);
	//const CMarkupNode& operator = (const CMarkupNode& other);

	BOOL IsValid() const;
	BOOL IsTextBlock() const;

	CMarkupNode GetParent();
	CMarkupNode GetSibling(BOOL bExcludeText=TRUE);
	CMarkupNode GetChild(BOOL bExcludeText=TRUE);
	CMarkupNode GetChild(LPCOLESTR pstrName);

	BOOL HasSiblings(BOOL bExcludeText=TRUE) const;
	BOOL HasChildren(BOOL bExcludeText=TRUE) const;
	LPCOLESTR GetName() const;
	LPCOLESTR GetValue() const;

	BOOL HasAttributes();
	BOOL HasAttribute(LPCOLESTR pstrName, LPCOLESTR pstrName2=NULL, LPCOLESTR pstrName3=NULL);
	int GetAttributeCount();
	LPCOLESTR GetAttributeName(int iIndex);
	LPCOLESTR GetAttributeValue(int iIndex);
	LPCOLESTR GetAttributeValue(LPCOLESTR pstrName, LPCOLESTR pstrName2=NULL, LPCOLESTR pstrName3=NULL, LPCOLESTR pstrName4=NULL);
	BOOL GetAttributeValue(int iIndex, LPOLESTR pstrValue, SIZE_T cchMax);
	BOOL GetAttributeValue(LPCOLESTR pstrName, LPOLESTR pstrValue, SIZE_T cchMax);

private:
	void _MapAttributes();

	enum { MAX_XML_ATTRIBUTES = 200 };

	typedef struct
	{
		ULONG iName;
		ULONG iValue;
	} XMLATTRIBUTE;

	int m_iPos;
	int m_nAttributes;
	XMLATTRIBUTE m_aAttributes[MAX_XML_ATTRIBUTES];
	CMarkup* m_pOwner;
};


#endif // !defined(__DUIMARKUP_H__)

