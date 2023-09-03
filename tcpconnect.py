import http.client

connection = http.client.HTTPConnection("10.20.1.120")
connection.request("GET", "/")
response = connection.getresponse()
print("Status: {} and reason: {}".format(response.status, response.reason))

connection.close()