from urllib import parse,request
import requests
#输出内容:user=admin&password=admin
# header_dict = {'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko'


for _ in range(10):
	header_dict = {'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko',"Content-Type": "application/json",
		"Connection" : "close"}
	url = 'http://9.134.237.210:8000'
	d = {'key1': 'value1', 'key2': 'value2', 'message': 'ddfdsa'}
	res = requests.post(url, data=d, headers=header_dict)
	#print(res.text)



textmod={'user':'admin','password':'admin'}
textmod = parse.urlencode(textmod)

url='http://9.134.237.210:8000/mainsugar/loginGET/'
req = request.Request(url='%s%s%s' % (url,'?',textmod))
res = request.urlopen(req)
print(res.read())