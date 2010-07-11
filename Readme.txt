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
