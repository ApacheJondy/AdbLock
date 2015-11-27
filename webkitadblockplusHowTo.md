wekitadblockplus没有采用脚本等实现方法，因为那样有诸多限制以及效率问题，
这里采用直接集成到webkit代码中，但是与webkit又相对独立，
如何实现过滤：

在ResourceLoader::didReceiveResponse来进行过滤：

FilterManager **m=FilterManager::getManager(getenv("WEBKIT\_ADB\_FILTERFILE"));**

if(m->shouldFilter(this->frameLoader()->frame()->document()，&r))

{

> didFail(frameLoader()->fileDoesNotExistError(r));

}

如何实现元素到隐藏：
CSSStyleSelector是每一个页面都必然创建到对象，用于管理页面中到css脚本。
m\_filterCSS 是新添加到CSSStyleSelector的成员变量，用于保持刚创建的sheet变量。
在CSSStyleSelector::CSSStyleSelector构造函数中加入如下代码：

在

> // add stylesheets from document

> m\_authorStyle = new CSSRuleSet();

后面加入：

if (doc) {

> String res =FilterManager::getManager( getenv(

> "WEBKIT\_ADB\_FILTERFILE"))->cssrules(

> doc->url().host());

> //取得了css脚本，如果是空，则是没有找到过滤规则

> if (!res.isEmpty()) {

> RefPtr

&lt;CSSStyleSheet&gt;

 sheet = CSSStyleSheet::create(doc);
> sheet->parseString(res);

> m\_filterCSS = sheet;

> fprintf(stderr,"css for %s :%s\n",doc->url().host().utf8().data(),res.utf8().data());

> m\_authorStyle->addRulesFromSheet(sheet.get(), **m\_medium, this);**

> }

> }