/*
 Copyright (C) 2010 Steven Bai<baizhenxuan@gmail.com>

 This file is part of the WebKitadblockplus project

 */
#include "ADBFilter.h"
#include <wtf/Vector.h>
#include "CString.h"
#ifndef ADB_NO_QT_DEBUG
#include <QDebug>
#endif
namespace WebCore {

static void printString(const String & s)
{
#ifndef ADB_NO_QT_DEBUG
    ////qDebug()<<s;
#endif
	//printf("%s\n",s.utf8().data());
}
    /*
      首先manager已经判断过是过滤而不是隐藏规则了
      以@@开始，则是白名单，manager会优先考虑
      以||开始则是不匹配协议名的过滤，并去掉||
      以|开始，则去掉|，否则在开始处添加*
      含有$类型指定规则，去掉这些字符串，并处理类型
      以|结尾，去掉|，否则在结尾处添加*
      */
FilterRule::FilterRule( const String & r)
{
	String rule=r.stripWhiteSpace();
    m_rule=rule;
    this->m_isException=false;
    this->m_isMatchProtocol=true;
    m_filterThirdParty=false;
    m_type=0;
	if(rule.startsWith("@@")) {
        this->m_isException=true;
		//stripped+=2;//略过@@
		rule.remove(0,2);
	}
	if(rule.startsWith("||")) {
        this->m_isMatchProtocol=false;
		rule.remove(0,2);
	}
	if(rule.startsWith("|"))
		rule.remove(0,1);
	else
		rule.insert("*",0);
	int dollarPos=rule.reverseFind('$');
	if(dollarPos!=-1) {
		//const UChar* types = rule.substring(dollarPos).charactersWithNullTermination();
		String token;
		for (int i = dollarPos+1; i < rule.length(); i++) {
			if (rule[i] != ',' && i != rule.length()-1) {
				token.append(rule[i]);
			} else {
				if(i==rule.length()-1)
					token.append(rule[i]);
                //qDebug()<<"process token "<<token;
				token = token.lower();
                bool dor=false;
                if(token[0]=='~') {
                    token=token.substring(1);
                    dor=true;
                }
                if (token == "script")
                    m_type |= dor?~FILTER_TYPE_SCRIPT:FILTER_TYPE_SCRIPT;
				else if (token == "image")
                    m_type |= dor?~FILTER_TYPE_IMAGE:FILTER_TYPE_IMAGE;
				else if (token == "background")
                    m_type |= dor?~FILTER_TYPE_BACKGROUND:FILTER_TYPE_BACKGROUND ;
				else if (token == "object")
                    m_type |= dor?~FILTER_TYPE_OBJECT:FILTER_TYPE_OBJECT ;
				else if (token == "stylesheet")
                    m_type |= dor?~FILTER_TYPE_STYLESHEET:FILTER_TYPE_STYLESHEET ;
				else if (token == "xbl")
                    m_type |= dor?~FILTER_TYPE_XBL:FILTER_TYPE_XBL ;
				else if (token == "ping")
                    m_type |= dor?~FILTER_TYPE_PING:FILTER_TYPE_PING ;
				else if (token == "xmlhttprequest")
                    m_type |= dor?~FILTER_TYPE_XMLHTTPREQUEST:FILTER_TYPE_XMLHTTPREQUEST ;
				else if (token == "object_subrequest")
                    m_type |= dor?~FILTER_TYPE_OBJECT_SUBREQUEST:FILTER_TYPE_OBJECT_SUBREQUEST ;
				else if (token == "dtd")
                    m_type |= dor?~FILTER_TYPE_DTD:FILTER_TYPE_DTD ;
				else if (token == "subdocument")
                    m_type |= dor?~FILTER_TYPE_SUBDOCUMENT:FILTER_TYPE_SUBDOCUMENT ;
				else if (token == "document")
                    m_type |= dor?~FILTER_TYPE_DOCUMENT:FILTER_TYPE_DOCUMENT ;
				else if (token == "elemhide")
                    m_type |= dor?~FILTER_TYPE_ELEMHIDE:FILTER_TYPE_ELEMHIDE ;
                else if (token == "third-party") {
                    this->m_filterThirdParty=true;
                    this->m_matchFirstParty=dor;
                }
                else if (token.startsWith("domain")) {
                    //type |= FILTER_TYPE_DOMAIN;
                    token=token.substring(7);
                    this->processDomains(token);
                }
#if 0
				else if (token == "match-case")
					type |= FILTER_TYPE_MATCH_CASE;
				else if (token == "collapse")
					type |= FILTER_TYPE_COLLAPSE;
#endif
				token=String();
			}
		}
		rule=rule.left(dollarPos);
	}
	if (rule[rule.length() - 1] == '|')
		rule = rule.left(rule.length() - 1);
	else
		rule.append("*");
    this->m_reFilter=rule;

}
void FilterRule::print()
{
#ifndef ADB_NO_QT_DEBUG
    ////qDebug()<<"re:"<<reFilter;
    ////qDebug()<<"isExceptions:"<<this->isException;
    ////qDebug()<<"type:"<<type;
    ////qDebug()<<"isMatchProtocol:"<<this->isMatchProtocol;
    ////qDebug()<<"---------------------------";
#endif
}
HideRule::HideRule(const String & r)
{
	String rule=r.stripWhiteSpace();
	int pos=rule.find("##",0);
	String temp;
	for(int i=0;i<pos;i++) {
		if(rule[i]==',') {
			if(!temp.isEmpty()) {
				m_domains.append(temp);
				temp=String();
			}
			continue;
		}
		temp.append(rule[i]);
		if(i==pos-1)
			m_domains.append(temp);
	}
	m_sel=rule.substring(pos+2);
}
void HideRule::print()
{
#ifndef ADB_NO_QT_DEBUG
    ////qDebug()<<"sel:"<<m_sel;
	for(int i=0;i<this->m_domains.size();i++) {
        ////qDebug()<<"dom:"<<m_domains[i];
	}
    ////qDebug()<<"-------------------------";
#endif
}

bool FilterRule::processDomains(String &ds)
{
    const UChar * d=ds.characters();
    String token;
    for(int i=0;i<ds.length();i++) {
        if (d[i] != '|' && i != ds.length()-1) {
            token.append(d[i]);
        } else {
            if(i==ds.length()-1)
                token.append(d[i]);
            if(token[0]=='~')
                this->m_inverseDomains.append(token.substring(1));
            else
                this->m_domains.append(token);
            token=String();
        }
    }
#if 1
    //qDebug()<<"domains:";
    for(StringVector::iterator it=m_domains.begin();
    it!=m_domains.end();++it) {
        //qDebug()<<*it;
    }
    //qDebug()<<"inverse domains:";
    for(StringVector::iterator it=m_inverseDomains.begin();
    it!=m_inverseDomains.end();++it) {
        //qDebug()<<*it;
    }
    //qDebug()<<"--------------------------------------------";
#endif
}

bool FilterRule::isMatchThirdParty(const KURL &mainURL, const KURL &other)
{
    //qDebug()<<"m_filterThirdParty:"<<m_filterThirdParty;
    //qDebug()<<"m_matchFirstParty:"<<m_matchFirstParty;
    if(!this->m_filterThirdParty)
        return true;
    if(mainURL.isEmpty())
        return true;
    return other.host().endsWith(mainURL.host())==this->m_matchFirstParty;
}

bool FilterRule::isMatchDomains(const KURL &url)
{
    String h=url.host().lower();
    if(m_domains.isEmpty() && m_inverseDomains.isEmpty()) //not specify which domains;
        return true;
    for(StringVector::iterator it=this->m_domains.begin();it!=m_domains.end();++it) {
        if(h.endsWith(*it,false))
            return true;
    }
    if(m_inverseDomains.isEmpty()) //no inverse domains,so this filter rule should not apply
        return false;
    for(StringVector::iterator it=this->m_inverseDomains.begin();
    it!=m_inverseDomains.end();++it) {
        if(h.endsWith(*it,false))
            return false;
    }
    return true;
}

/*
 when t is 0, the caller wants that the FilterRule decide the url type itself

 */
bool FilterRule::isMatchType(const KURL &u, FilterType t)
{
    if(m_type==0) {
        return true;
    }
    if(t!=0) {
        return t&m_type;
    }
    String url=u.string().lower();
    t=m_type;
    t&=~FILTER_TYPE_SCRIPT;
    if(m_type&FILTER_TYPE_SCRIPT && url.endsWith(".js")) {
        return true;
    }
    t&=~FILTER_TYPE_IMAGE;
    if(m_type&FILTER_TYPE_IMAGE) {
        if(url.endsWith(".png")
            ||url.endsWith(".jpg")
            ||url.endsWith(".gif")
            ||url.endsWith(".bmp")
            ||url.endsWith(".jpeg")
            ||url.endsWith(".ico"))
            return true;
    }
    t&=~FILTER_TYPE_STYLESHEET;
    if(m_type&FILTER_TYPE_STYLESHEET && url.endsWith(".css") ) {
        return true;
    }
    return false; //unsupport format ,always doesn't filter
}

static bool adbMatch(const char *  s,  const char *  p,bool caseSensitivie=false);
bool FilterRule::shouldFilter(const KURL & mainURL,const KURL & u,FilterType t)
{
	String url=u.string();
	bool ret;
    if(!this->m_isMatchProtocol) {
		url=url.right(url.length()-u.protocol().length());
	}
    if(!this->isMatchThirdParty(mainURL,u)) {
        //qDebug()<<"third party not match";
        return false;
    }
    if(!this->isMatchDomains(u)) {
        //qDebug()<<"domains not match";
        return false;
    }
    if(!isMatchType(u,t)) {
        //qDebug()<<"type not match";
        return false;
    }
    ret=adbMatch(url.utf8().data(),m_reFilter.utf8().data());
#ifndef ADB_NO_QT_DEBUG
    if(ret) {
        //qDebug()<<this->m_reFilter<<" match "<<u.string();
    }
    else {
        ////qDebug()<<reFilter<<" not match "<<u.string();
    }
#endif
#if 0
	if(ret){
        ////qDebug()<<"match -----------------";
		this->print();
        ////qDebug()<<u.string();
	}
	else {
        ////qDebug()<<"unmatch ------------";
        ////qDebug()<<this->reFilter;
        ////qDebug()<<u.string();
	}
#endif
	return ret;
}

///////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
 * 	if getCaseChar is noncasesensitive ,the result will be noncasesentive
 *
 */
inline char getCaseChar( char c,bool caseSensitive)
{
    if(caseSensitive)
        return c;
    if(c>='A' && c<='Z')
        c='a'+c-'A';
    return c;
}
/*
  Separator character is anything but a letter,
a digit, or one of the following: – . %.
  */
static int  isSeperator( char ch)
{
    if(ch>='a' && ch <='z')
        return 0;
    if(ch>='A' && ch<='Z')
        return 0;
    if(ch=='-' || ch=='.' || ch=='%')
        return 0;
    return 1;
}
inline int max(int f1,int f2)
{
    return f1>f2?f1:f2;
}
inline int min(int f1,int f2)
{
    return f1<f2?f1:f2;
}

/*
find t in s and return a pointer.
if found ,return the first place t appears in s
else NULL;
*/
static const char* strfind(  const char* s,  const char* t)
{
    const char* bp;
    const char* sp;
    while (*s)
    {
        bp=s;
        sp=t;
        do
        {
            if(!*sp)
                return s;

        }
        while(*bp++==*sp++);
        s++;
    }
    return NULL;
}
/*
  get a much bigger step for *.
  */
static inline int  patternStep( const char * s, const  char * p)
{
    char temp[8];
    int step=0;
    const char * t=p;
    while(*t!='*' && *t!='^' && *t!='\0')
    {
        step++;
        t++;
    }
    if(!step) //just one character ,such as ^,*
        return 1;
    memset(temp,0,sizeof(temp));
    strncpy(temp,p,min(sizeof(temp)-1,step));
    //printf("temp=%s,step=%d\n",temp,step);
    const char * res=strfind(s,temp);
    if(!res) //没有找到
        return strlen(s); //移动真整个字符串
    else
        return max(1,res-s); //找到第一个匹配的字符串的位置
}
/*
   test if a given string  and a pattern  matches use adblock plus rule
   give a string s and a pattern p ,
   if they match,return 1, then return 0
   */
static bool adbMatch(const char *  s,  const char *  p,bool caseSensitivie) {
    for (;;) {
        switch(*p++) {
            case '*' :	// match 0-n of any characters
                if (!*p) return true; // do trailing * quickly
                while (!adbMatch(s, p,caseSensitivie))
                {
#if 0
                     if (!*s++) return 0;
#else
                    if(!*s) return false;
                    s+=patternStep(s,p);
#endif
                }
                return true;
            case '^':
                if(isSeperator(*s))
                {
                    s++;
                    break;
                }
                else
                    return false;//expect a sepetor,
            case '\0':	// end of pattern
                return !*s;
            default  :
                if (getCaseChar(*s++,caseSensitivie) !=
                    getCaseChar(*(p-1),caseSensitivie)) return false;
                break;
        }
    }
}
}
