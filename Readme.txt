2010-7-12
ǰ��ʱ����Ϊ��ResourceHandler����start�н��й��˾Ϳ����ˣ�
����������Է��ֲ��У���򲻿���ҳ��������֤������û�з���������Ϣ����
������ResourceLoader::didReceiveResponse�����й��ˣ�
FilterManager * m=FilterManager::getManager(getenv("WEBKIT_ADB_FILTERFILE"));
if(m->shouldFilter(this->frameLoader()->frame()->document()->baseURL(),r.url()))
{
    didFail(frameLoader()->fileDoesNotExistError(r));
}
���ַ�����news.sina.com.cn,www.sina.com.cn������ҳ��֤��û�����⣬
���˲�֧�ֵ��������ˣ����о���subdocument�ȡ�
����һ���������Ч�����⣬��Ҫ�ص����

2010-6-27
Ŀǰֻ֧��script��image��stylesheet��third-party��domain����
����script��image��stylesheet����ͨ��url�ַ������е��ļ���չ����ƥ�䣬
third-party��domain�����KURL����н�����
�������ع������о�webkit�Ժ���ʵ�֡�

������һ�£������ٶȻ����ԣ��Ͷ��ĵ�chinalist�����˴󲿷�url���ǳ��飬��������ʱ�䡣
