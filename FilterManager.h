/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*

 This file is part of the WebKitadblockplus project
 author: Steven Bai<baizhenxuan#gmail.com>

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
     匹配类型，目前暂时只支持，script，image，stylesheet，以及third_party,object(swf only), subdocument,
	 */
	#define FILTER_TYPE_SCRIPT 0x0001
	#define FILTER_TYPE_IMAGE 0X0002
	#define FILTER_TYPE_BACKGROUND 0x0004
	#define FILTER_TYPE_STYLESHEET 0X0008
	#define FILTER_TYPE_OBJECT 0X0010

	#define FILTER_TYPE_XBL 0X0020 //不会支持
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
	class Document;
	class ResourceResponseBase;
	//只应该有一个实例,
	/*
	 这里需要考虑的是保证该类是多线程安全的，正常查询可以保证
	 只是动态删除以及添加时如何保证多线程安全,内部适用map来管理各种规则
	 或者hash来管理。
	 */
	class FilterManager {
		//typedef HashMap<String,FilterRuleList* , CaseFoldingHash > FilterRuleMap;
		typedef HashMap<String /*domain*/,String /*selector*/,CaseFoldingHash> HideRuleMap;

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
		HideRuleMap m_hiderules;
		FilterRuleMap m_ShortcutWhiteRules; //white list, can use shortcut
		FilterRuleVector m_UnshortcutWhiteRules;
		FilterRuleMap m_ShortcutFilterRules;
		FilterRuleVector m_UnshortcutFilterRules;
		FilterRuleVector m_AllFilterRules;
	private:
		/*
		 从文件读取规则,string要是有qt的隐含共享就好了，webkit使用的string
		 就是隐含共享，可以直接传值
		 */
		FilterManager(const String & filename);
		//规则集合
		FilterManager(const StringVector & rules);
	public:
		static FilterManager* getManager(const String & filename);
		static FilterManager * getManager(const StringVector & rules);
		~FilterManager();
		bool addRule(String rule);
		//哪个规则，运行时不能隐藏，只能删除
		bool hideRule(int id);

		/*
		 是否应该过滤,
		 目前暂不考虑类型匹配，因为类型信息无法获取
		 因为很多规则无法明确知道，比如background，必须来自css的请求，目前无法确知
		 */
		/*
		 * Besides of translating filters into regular expressions Adblock Plus also
tries to extract text information from them. What it needs is a unique
string of eight characters (a “shortcut”) that must be present in every
address matched by the filter (the length is arbitrary, eight just seems
reasonable here). For example, if you have a filter |http://ad.* then
Adblock Plus has the choice between “http://a”, “ttp://ad” and “tp://ad.”,
any of these strings will always be present in whatever this filter will
match. Unfortunately finding a shortcut for filters that simply don’t have
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
characters of unbroken text (meaning that these don’t contain any
characters with a special meaning like *), otherwise they will be just as
slow as regular expressions. But with filters that qualify it doesn’t
matter how many filters you have, the processing time is always the same.
That means that if you need 20 simple filters to replace one regular
expression then it is still worth it. Speaking of which — the deregifier is
very recommendable.
		 */
        bool shouldFilter(const Document *doc,const ResourceResponseBase * response);
		//使用webkit内部的指针管理办法来管理返回值？
		//根据域名来确定适用的css规则，如果不支持的css规则，暂时忽略.
		String cssrules(const String & host);
	private:
		void addRule(FilterRule * r);
		void addRule(HideRule * r);
		bool shouldFilter(const KURL & mainURL,const KURL & url,FilterType t);
	};
}
#endif // FILTERMANAGER_H
