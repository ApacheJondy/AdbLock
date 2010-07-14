/*
 Copyright (C) 2010 Steven Bai<baizhenxuan@gmail.com>

 This file is part of the WebKitadblockplus project

 */

#include "FilterManager.h"
#include <fstream>
#include "ADBFilter.h"
#include "CString.h"
#ifndef ADB_NO_QT_DEBUG
#include <QDebug>
#endif

using namespace std;

namespace WebCore{

struct FilterRuleList {
	FilterRule * r;
	FilterRuleList * next;
    inline bool doFilter(const KURL & mainURL,const KURL & url, FilterType t) {
		FilterRuleList * l = this;
		while (l) {
            if (l->r->shouldFilter(mainURL,url,t))
				return true;
			l = l->next;
		}
		return false;
	}
};
struct HideRuleList {
	HideRule * r;
	HideRuleList * next;
};

bool FilterManager::FilterRuleMap::doFilter(const KURL & mainURL,const String &key, const KURL & url,FilterType t) {
    FilterRuleMap::iterator mapit=find(key);
	if(mapit!=end()) {
        //qDebug()<<"finds key:"<<key;
        unsigned int address=(unsigned int) mapit->second;
        if(unMatchRules.find(address)!=unMatchRules.end()) // already match this rules;
                return false;
        this->unMatchRules.add(address);
        if(mapit->second->doFilter(mainURL,url,t))
			return true;
    }
    //qDebug()<<"not find key:"<<key;
	return false;
}
FilterManager::FilterRuleMap::~FilterRuleMap() {
	for (FilterRuleMap::iterator it = this->begin(); it != this->end(); ++it) {
		FilterRuleList * l = it->second;
		FilterRuleList * n = l->next;
		while (n) {
			FilterRuleList * temp = n->next;
			delete n;
			n = temp;
		}
		delete l;
	}
}
FilterManager::FilterManager(const String & filename) {
	ifstream infile(filename.utf8().data());
	if (!infile)
        fprintf(stderr,"open Adblock plus configfile :%s failed \n", filename.utf8().data());
	else {
		char rule[1000];
		infile.getline(rule, 1000); //忽略第一行版本号
		while (!infile.eof()) {
			memset(rule, 0, 1000);
			infile.getline(rule, 1000);
			if (rule[0] == '!') {
                ////qDebug()<<"comment:"<<rule;
				//printf("comment: %s \n", rule);
			} else {
				int n = strlen(rule);
				if (rule[n - 1] == '\n' || rule[n - 1] == '\r') {
					rule[n - 1] = '\0';
				}
				if (rule[n - 2] == '\n' || rule[n - 2] == '\r') {
					rule[n - 2] = '\0';
				} //clear \n \r
				if (strlen(rule) <= 3) //too short rule ,maybe some error
					continue;
				String s(rule);
                //qDebug()<<"add rule:"<<s;
				if (s.contains("##")) {
					HideRule hr(s);
					this->addRule(&hr);
				} else {
					this->addRule(new FilterRule(s));
				}
			}
		}
	}
}
FilterManager::FilterManager(const StringVector & rules)
{
	for(int i=0;i<rules.size();i++) {
		String r=rules[i];
		if(r[0]=='!') {
			continue;
		}
		if(r.contains("##")) {
            //HideRule rule(r);
            //rule.print();
		}
		else {
            this->addRule(new FilterRule(r));
		}
	}

}

FilterManager::~FilterManager()
{
	for (FilterRuleVector::iterator it=m_AllFilterRules.begin();
			it!=m_AllFilterRules.end();++it) {
		delete *it;
	}
//hiderule process
}
#define RULE_KEY_HASH_LENGTH 8
inline int min(int f1,int f2)
{
    return f1<f2?f1:f2;
}

static bool collectShortcuts(const String & str, StringVector & vs) {
	unsigned int i = 0;
	bool isFindShoutcut = false;
	while (i < min(str.length() - RULE_KEY_HASH_LENGTH,80)) {
		unsigned int j = i;
		for (; j < str.length(); j++) {
			if ((str[j] == '*' || str[j] == '^')) {
				break;
			}
		}
		for (unsigned int k = i; j - k >= RULE_KEY_HASH_LENGTH; k++) {
			isFindShoutcut = true;
			String key = str.substring(k, RULE_KEY_HASH_LENGTH);
			vs.append(key);
		}
		i = j + 1;
	}
	return isFindShoutcut;
}
void FilterManager::addRule(FilterRule * r)
{
	this->m_AllFilterRules.append(r); //for delete FilterRule;
	FilterRuleMap * rules = &m_ShortcutFilterRules;
	FilterRuleVector * unshortcutRules = &m_UnshortcutFilterRules;
	if (r->isWhiteFilter()) {
		rules = &m_ShortcutWhiteRules;
		unshortcutRules = &m_UnshortcutWhiteRules;
	}
	const String & reFilter = r->getRegularFilter();
	StringVector shortcuts;
	if (collectShortcuts(reFilter, shortcuts)) {
		unsigned int i = 0;
		for (; i < shortcuts.size(); i++) {
			String key = shortcuts[i];
			FilterRuleMap::iterator it = rules->find(key);
            //qDebug()<<"add key:"<<key;
			if (it != rules->end()) { // already exists
                //qDebug()<<"key already exits:";
				FilterRuleList * l = it->second;
				while (l->next)
					l = l->next;
				FilterRuleList * n = new FilterRuleList();
				n->r = r;
				n->next = NULL;
				l->next = n;
			} else {
				FilterRuleList * n = new FilterRuleList();
				n->r = r;
				n->next = NULL;
                rules->add(key,n);
				//pair<FilterRuleMap::iterator, bool> res = rules->add(key, n);
			}
		}
	} else
		unshortcutRules->append(r);
    //qDebug()<<"filter:"<<rules->size();
    //qDebug()<<"unshort:"<<unshortcutRules->size();
}
void FilterManager::addRule(HideRule * r) {
	const StringVector & domains=r->domains();
	const String & sel=r->selector();
	for(StringVector::const_iterator it=domains.begin();it!=domains.end();it++) {
		const String & key=*it;
		HideRuleMap::iterator hit=this->m_hiderules.find(key);
		if(hit!=m_hiderules.end()) {
			hit->second=hit->second+String(",")+sel;
		}
		else {
			m_hiderules.add(*it,sel);
		}
	}
}
bool FilterManager::addRule(String rule)
{
	//not implemented
	return true;
}
bool FilterManager::hideRule(int id)
{
	//not implemented
	return true;
}
bool FilterManager::shouldFilter(const KURL & mainURL,const KURL & url,FilterType t)
{
	StringVector shortcuts;
	collectShortcuts(url.string(),shortcuts);
    this->m_ShortcutFilterRules.prepareStartFind();
    this->m_ShortcutWhiteRules.prepareStartFind();
	for(StringVector::iterator it=shortcuts.begin();
			it!=shortcuts.end();++it) {
        if(m_ShortcutWhiteRules.doFilter(mainURL,*it,url,t))
			return false;
		for(FilterRuleVector::iterator fit=this->m_UnshortcutWhiteRules.begin();
				fit!=this->m_UnshortcutWhiteRules.end();++fit) {
            if((*fit)->shouldFilter(mainURL,url,t))
				return false;
		}
	}
	for(StringVector::iterator it=shortcuts.begin();
			it!=shortcuts.end();++it) {
        if(this->m_ShortcutFilterRules.doFilter(mainURL,*it,url,t))
			return true;
		for(FilterRuleVector::iterator fit=this->m_UnshortcutFilterRules.begin();
				fit!=this->m_UnshortcutFilterRules.end();++fit) {
            if((*fit)->shouldFilter(mainURL,url,t))
				return true;
		}
	}
	return false;
}
String FilterManager::cssrules(const String & host)
{
	String res;
	StringVector dotsplits;
	host.split('.',dotsplits);
	if(dotsplits.size()<=1) //domain error? no dot? at least g.cn
		return res;
	int ignore=2;
	String l2domain=dotsplits[dotsplits.size()-2];
	if(l2domain =="com" ||
			l2domain=="net" ||
			l2domain=="org" ||
			l2domain=="edu")
		ignore=3;
	for(int i=dotsplits.size()-ignore;i>=0;i--) {
		String domain=dotsplits[i];
		for(int j=i+1;j<dotsplits.size();j++) {
			domain=domain+"."+dotsplits[j];
		}
		HideRuleMap::iterator it=this->m_hiderules.find(domain);
		//qDebug()<<"handle domain:"<<domain;
		if(it!=m_hiderules.end()) {
			res=res.isEmpty()?it->second:res+","+it->second;
		}
	}
	if(!res.isEmpty()) {
		res+="{display:none !important;}";
	}
	return res;
}
static FilterManager * m=NULL;
FilterManager * FilterManager::getManager(const StringVector & rules)
{
	if(m)
		return m;
	m=new FilterManager(rules);
	return m;
}
FilterManager * FilterManager::getManager(const String & filename)
{
	if(m)
		return m;
	m=new FilterManager(filename);
#if 1
    qDebug()<<"filter rules:"<<m->m_ShortcutFilterRules.size();
    qDebug()<<"unshortcut filter rules:" << m->m_UnshortcutFilterRules.size();
    qDebug()<<"whitelist rules:"<<m->m_ShortcutWhiteRules.size();
    qDebug()<<"unshortcut whitelist rules:"<<m->m_UnshortcutWhiteRules.size();
    for(HideRuleMap::iterator it=m->m_hiderules.begin();it!= m->m_hiderules.end();++it) {
    	qDebug()<<it->first<<":"<<it->second;
    }
#endif
#if 0
	for(FilterRuleMap::iterator it=m->m_ShortcutFilterRules.begin();
			it!=m->m_ShortcutFilterRules.end();++it) {
        //qDebug()<<"key:"<<it->first;
		it->second->r->print();
		FilterRuleList * l=it->second->next;
		while(l) {
			l->r->print();
			l=l->next;
		}
	}
	for(FilterRuleVector::iterator it=m->m_UnshortcutFilterRules.begin();
			it!=m->m_UnshortcutFilterRules.end();++it) {
        //qDebug()<<"unshort:"<<(*it)->getRegularFilter();
	}
    //qDebug()<<"whiter list:"<<"!!!!!!!!";
	for (FilterRuleMap::iterator it = m->m_ShortcutWhiteRules.begin(); it
			!= m->m_ShortcutWhiteRules.end(); ++it) {
        //qDebug() << "key:" << it->first;
		it->second->r->print();
		FilterRuleList * l = it->second->next;
		while (l) {
			l->r->print();
			l=l->next;
		}
	}
#endif
	return m;
}
}
