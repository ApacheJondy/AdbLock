2010-7-14
更改FilterManager过滤接口，原接口定义为private，新接口形如：
bool shouldFilter(const Document *doc,const ResourceResponseBase * response);
更改以后可以获取到更多的信息，可以进行更准确的过滤。
增加对subdocument类型过滤支持
下一步要考虑内容：
目前过滤以后只是不下载，但是在页面中仍会为被过滤对象留出空间，显得非常难看
下一步计划找出发起资源下载的tag，然后将其隐藏。
2010-7-14
实现隐藏规则，实现方法将选择器后面追加{display :none !important;}
然后在系统加载缺省css时加载进去，
使用方法：
在CSSStyleSelector::CSSStyleSelector构造函数中加入如下代码：
在 
  // add stylesheets from document
    m_authorStyle = new CSSRuleSet();
后面加入：
if (doc) {
        RefPtr<CSSStyleSheet> sheet = CSSStyleSheet::create(doc);
        String res =FilterManager::getManager( getenv(
                                "WEBKIT_ADB_FILTERFILE"))->cssrules(
                                doc->url().host());
        //取得了css脚本，如果是空，则是没有找到过滤规则
        if (!res.isEmpty()) {
            sheet->parseString(res);
            m_filterCSS = sheet;
            fprintf(stderr,"css for %s :%s\n",doc->url().host().utf8().data(),res.utf8().data());
            m_authorStyle->addRulesFromSheet(sheet.get(), *m_medium, this);
        }
    }
2010-7-12
前段时间以为在ResourceHandler：：start中进行过滤就可以了，
经过认真测试发现不行，会打不开网页，经过验证可能是没有反馈错误信息导致
，改在ResourceLoader::didReceiveResponse来进行过滤：
FilterManager * m=FilterManager::getManager(getenv("WEBKIT_ADB_FILTERFILE"));
if(m->shouldFilter(this->frameLoader()->frame()->document()->baseURL(),r.url()))
{
    didFail(frameLoader()->fileDoesNotExistError(r));
}
此种方法经news.sina.com.cn,www.sina.com.cn两个网页验证，没有问题，
除了不支持到像对象过滤，还有就是subdocument等。
还有一个问题就是效率问题，需要重点测试

2010-6-27
目前只支持script，image，stylesheet，third-party，domain规则
其中script，image，stylesheet规则通过url字符串的中的文件扩展名来匹配，
third-party，domain则借助KURL类进行解析，
对于隐藏规则，再研究webkit以后来实现。

测试了一下，过滤速度还可以，就订阅的chinalist，过滤大部分url都非常块，几乎不需时间。
