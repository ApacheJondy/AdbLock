/*
 Copyright (C) 2010 Steven Bai<baizhenxuan@gmail.com>

 This file is part of the WebKitadblockplus project

 */

#ifndef FILTERMANAGER_H
#define FILTERMANAGER_H
#include "PlatformString.h"
#include <wtf/Vector.h>
#include "StringHash.h"
#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include "KURL.h"


//#define ADB_NO_QT_DEBUG

namespace WebCore {

	/*
     ƥ�����ͣ�Ŀǰ��ʱֻ֧�֣�script��image��stylesheet���Լ�third_party,
	 */
	#define FILTER_TYPE_SCRIPT 0x0001
	#define FILTER_TYPE_IMAGE 0X0002
	#define FILTER_TYPE_BACKGROUND 0x0004
	#define FILTER_TYPE_STYLESHEET 0X0008
	#define FILTER_TYPE_OBJECT 0X0010

	#define FILTER_TYPE_XBL 0X0020 //����֧��
	#define FILTER_TYPE_PING 0X0040

	#define FILTER_TYPE_XMLHTTPREQUEST 0x0080
	#define FILTER_TYPE_OBJECT_SUBREQUEST 0X0100
	#define FILTER_TYPE_DTD 0X0200
	#define FILTER_TYPE_SUBDOCUMENT 0X0400
	#define FILTER_TYPE_DOCUMENT 0X0800
	#define FILTER_TYPE_ELEMHIDE 0X1000
	#define FILTER_TYPE_THIRD_PARTY 0x2000
//	#define FILTER_TYPE_DOMAIN 0X4000
//	#define FILTER_TYPE_MATCH_CASE 0X8000
//	#define FILTER_TYPE_COLLAPSE 0x10000

	typedef unsigned int FilterType;
	typedef Vector<String> StringVector;
	class FilterRule;
	class HideRule;
	class FilterRuleList;
	class HideRuleList;
	//ֻӦ����һ��ʵ��,
	/*
	 ������Ҫ���ǵ��Ǳ�֤�����Ƕ��̰߳�ȫ�ģ�������ѯ���Ա�֤
	 ֻ�Ƕ�̬ɾ���Լ����ʱ��α�֤���̰߳�ȫ,�ڲ�����map��������ֹ���
	 ����hash������
	 */
	class FilterManager {
		//typedef HashMap<String,FilterRuleList* , CaseFoldingHash > FilterRuleMap;
		typedef HashMap<String,HideRuleList* ,CaseFoldingHash> HideRuleMap;

		typedef Vector<FilterRule *> FilterRuleVector;


		class FilterRuleMap: public HashMap<String,FilterRuleList* , CaseFoldingHash > {
            HashSet<unsigned int > unMatchRules;
		public:
			~FilterRuleMap();
             //prepare to start find
            inline void prepareStartFind() { this->unMatchRules.clear();}
            // release resource
            //inline void endFind() {}
            bool doFilter(const KURL & mainURL,const String & key,const KURL & url,FilterType t);
		};



	private:
		HideRuleMap hiderules;
		FilterRuleMap m_ShortcutWhiteRules; //white list, can use shortcut
		FilterRuleVector m_UnshortcutWhiteRules;
		FilterRuleMap m_ShortcutFilterRules;
		FilterRuleVector m_UnshortcutFilterRules;
		FilterRuleVector m_AllFilterRules;
		Vector<HideRule * > m_AllHideRules;
	private:
		/*
		 ���ļ���ȡ����,stringҪ����qt����������ͺ��ˣ�webkitʹ�õ�string
		 ����������������ֱ�Ӵ�ֵ
		 */
		FilterManager(const String & filename);
		//���򼯺�
		FilterManager(const StringVector & rules);
	public:
		static FilterManager* getManager(const String & filename);
		static FilterManager * getManager(const StringVector & rules);
		~FilterManager();
		bool addRule(String rule);
		//�ĸ���������ʱ�������أ�ֻ��ɾ��
		bool hideRule(int id);

		/*
		 �Ƿ�Ӧ�ù���,
		 Ŀǰ�ݲ���������ƥ�䣬��Ϊ������Ϣ�޷���ȡ
		 ��Ϊ�ܶ�����޷���ȷ֪��������background����������css������Ŀǰ�޷�ȷ֪
		 */
		/*
		 * Besides of translating filters into regular expressions Adblock Plus also
tries to extract text information from them. What it needs is a unique
string of eight characters (a ��shortcut��) that must be present in every
address matched by the filter (the length is arbitrary, eight just seems
reasonable here). For example, if you have a filter |http://ad.* then
Adblock Plus has the choice between ��http://a��, ��ttp://ad�� and ��tp://ad.��,
any of these strings will always be present in whatever this filter will
match. Unfortunately finding a shortcut for filters that simply don��t have
eight characters unbroken by wildcards or for filters that have been
specified as regular expressions is impossible.

All shortcuts are put into a lookup table, Adblock Plus can find the filter
by its shortcut very efficiently. Then, when a specific address has to be
tested Adblock Plus will first look for known shortcuts there (this can be
done very fast, the time needed is almost independent from the number of
shortcuts). Only when a shortcut is found the string will be tested against
the regular expression of the corresponding filter. However, filters
without a shortcut still have to be tested one after another which is slow.

To sum up: which filters should be used to make a filter list fast? You
should use as few regular expressions as possible, those are always slow.
You also should make sure that simple filters have at least eight
characters of unbroken text (meaning that these don��t contain any
characters with a special meaning like *), otherwise they will be just as
slow as regular expressions. But with filters that qualify it doesn��t
matter how many filters you have, the processing time is always the same.
That means that if you need 20 simple filters to replace one regular
expression then it is still worth it. Speaking of which �� the deregifier is
very recommendable.
		 */
        bool shouldFilter(const KURL & mainURL,const KURL & url, FilterType t=0);
		//ʹ��webkit�ڲ���ָ�����취��������ֵ��
		//����������ȷ�����õ�css���������֧�ֵ�css������ʱ����.
		String cssrules(const String & domain);
	private:
		void addRule(FilterRule * r);
		void addRule(HideRule * r);
	};
}
#endif // FILTERMANAGER_H
