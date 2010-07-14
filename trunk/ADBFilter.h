/*
 Copyright (C) 2010 Steven Bai<baizhenxuan@gmail.com>

 This file is part of the WebKitadblockplus project

 */

#ifndef FILTER_H
#define FILTER_H

#include "PlatformString.h"
#include "FilterManager.h"
#include <wtf/Vector.h>
#include <wtf/HashMap.h>
#include "KURL.h"
namespace WebCore {
	class FilterRule {
	public:
		/*
		 首先manager已经判断过是过滤而不是隐藏规则了
		 以@@开始，则是白名单，manager会优先考虑
		 以||开始则是不匹配协议名的过滤，并去掉||
		 以|开始，则去掉|，否则在开始处添加*
		 含有$类型指定规则，去掉这些字符串，并处理类型
		 以|结尾，去掉|，否则在结尾处添加*
		 */
		FilterRule( const String & rule);
		/*
		 是否应该过滤，如果是白名单，匹配则应该不过滤，否则过滤
		 类型用于只过滤adlbock plus规则中指定的类型。
		 */
        bool shouldFilter(const KURL & mainURL,const KURL & url, FilterType t);
		//是否是白名单
        bool isWhiteFilter() { return m_isException;}
		//是否是通过类型来进行过滤，比如只过滤脚本等。这个可能需要很多信息，暂时不予考虑，比如domain类型过滤，
        bool isNeedMimeType() { return m_type!=0;}
        const String & getRegularFilter(){ return m_reFilter;}
        const String & getWholeRule() { return m_rule;}
		//inline const StringVector &  constantsForFastSearch() {return constants;}
		void print();
	private:
        bool m_isException; // start with @@ //白名单
        bool m_isMatchProtocol;

		/*
		 adblock rule describe in regular expression
		 */
        String m_reFilter;
		//StringVector constants;
        String m_rule;
		/*
		 Type options: determine which types of elements a filter can block (or whitelist in case of an exception rule). Multiple type options can be specified to indicate that the filter should be applied to several types of elements. Possible types are:
		 */
        FilterType m_type;
        /*
Restriction to third-party/first-party requests: If the third-party option is
specified, the filter is only applied to requests from a different origin
than the currently viewed page. Similarly, ~third-party restricts the filter
to requests from the same origin as the currently viewed page.
         */
        bool m_filterThirdParty;
        bool m_matchFirstParty;

        /*
Domain restrictions: The option domain=example.com means that the filter
should only be applied on pages from “example.com” domain. Multiple domains
can be specified using “|” as separator: with the option
domain=example.com|example.net the filter will only be applied on pages from
“example.com” or “example.net” domains. If a domain name is preceded with
“~”, the filter should not be applied on pages from this domain. For example,
domain=~example.com means that the filter should be applied on pages from any
domain but “example.com” and domain=example.com|~foo.example.com restricts
the filter to the “example.com” domain with the exception of
“foo.example.com” subdomain.
         */
        Vector<String> m_domains;
        Vector<String> m_inverseDomains;

    private:
        bool isMatchType(const KURL & url,FilterType t);
        bool isMatchThirdParty(const KURL & host,const KURL & other);
        bool isMatchDomains( const KURL & url);
        bool processDomains(String & ds);

	};
	//隐藏规则，含有##的规则
	class HideRule {
	public:
		/*
		 将##之前的字符串解析为一组域名，后面的原封不动，作为css选择器来处理。
		 */
		HideRule(const String & r);
		//隐藏规则适用的domain。如果为空，则适用于所有，否则只适用于指明的domain
		const StringVector & domains() { return m_domains;}
		//example.com,~foo.example.com##*.sponsor
		//*.sponsor就是selector
		const String & selector() { return m_sel;}
		void print();
	private:
		String m_sel;
		StringVector m_domains;
	};
}
#endif // FILTER_H
