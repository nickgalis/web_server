#!/usr/bin/python3
#Do not forget to make cgi script exectuable
print("Content-Type: text/html; charset=UTF-8\r\n"); 
print() #For the blank line 
print("""
<html>
<head> 
    <title>CS410 Webserver Histogram</title>
    <style>
        body { background-color: white; text-align: center}
        h1 { color:red; font-size: 16pt }
        img { margin-top: 20px; }
    </style>
</head>
<body>
    <h1>CS410 Webserver</h1>
    <img src="./histogram.jpeg" alt="Histogram">
</body>
</html> 
""")



