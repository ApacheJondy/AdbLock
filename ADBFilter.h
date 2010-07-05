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
		 ����manager�Ѿ��жϹ��ǹ��˶��������ع�����
		 ��@@��ʼ�����ǰ�������manager�����ȿ���
		 ��||��ʼ���ǲ�ƥ��Э�����Ĺ��ˣ���ȥ��||
		 ��|��ʼ����ȥ��|�������ڿ�ʼ�����*
		 ����$����ָ������ȥ����Щ�ַ���������������
		 ��|��β��ȥ��|�������ڽ�β�����*
		 */
		FilterRule( const String & rule);
		/*
		 �Ƿ�Ӧ�ù��ˣ�����ǰ�������ƥ����Ӧ�ò����ˣ��������
		 ��������ֻ����adlbock plus������ָ�������͡�
		 */
        bool shouldFilter(const KURL & mainURL,const KURL & url, FilterType t);
		//�Ƿ��ǰ�����
        bool isWhiteFilter() { return m_isException;}
		//�Ƿ���ͨ�����������й��ˣ�����ֻ���˽ű��ȡ����������Ҫ�ܶ���Ϣ����ʱ���迼�ǣ�����domain���͹��ˣ�
        bool isNeedMimeType() { return m_type!=0;}
        const String & getRegularFilter(){ return m_reFilter;}
        const String & getWholeRule() { return m_rule;}
		//inline const StringVector &  constantsForFastSearch() {return constants;}
		void print();
	private:
        bool m_isException; // start with @@ //������
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
should only be applied on pages from ��example.com�� domain. Multiple domains
can be specified using ��|�� as separator: with the option
domain=example.com|example.net the filter will only be applied on pages from
��example.com�� or ��example.net�� domains. If a domain name is preceded with
��~��, the filter should not be applied on pages from this domain. For example,
domain=~example.com means that the filter should be applied on pages from any
domain but ��example.com�� and domain=example.com|~foo.example.com restricts
the filter to the ��example.com�� domain with the exception of
��foo.example.com�� subdomain.
         */
        Vector<String> m_domains;
        Vector<String> m_inverseDomains;

    private:
        bool isMatchType(const KURL & url,FilterType t);
        bool isMatchThirdParty(const KURL & host,const KURL & other);
        bool isMatchDomains( const KURL & url);
        bool processDomains(String & ds);

	};
	//���ع��򣬺���##�Ĺ���
	class HideRule {
	public:
		/*
		 ��##֮ǰ���ַ�������Ϊһ�������������ԭ�ⲻ������Ϊcssѡ����������
		 */
		HideRule(const String & r);
		//���ع������õ�domain�����Ϊ�գ������������У�����ֻ������ָ����domain
		const StringVector & domains();
		//example.com,~foo.example.com##*.sponsor
		//*.sponsor����selector
		const String & selector();
		void print();
	private:
		String m_sel;
		StringVector m_domains;
	};
}
#endif // FILTER_H
