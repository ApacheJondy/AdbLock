2010-6-27
目前只支持script，image，stylesheet，third-party，domain规则
其中script，image，stylesheet规则通过url字符串的中的文件扩展名来匹配，
third-party，domain则借助KURL类进行解析，
对于隐藏规则，再研究webkit以后来实现。

测试了一下，过滤速度还可以，就订阅的chinalist，过滤大部分url都非常块，几乎不需时间。
