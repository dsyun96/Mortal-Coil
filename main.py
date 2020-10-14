import requests
import os

s = requests.session()
userid = input('id: ')
userpw = input('pw: ')
payload = {
    'username': userid,
    'password': userpw,
    'redirect': '',
    'login': 'Log+in'
}

url = 'http://www.hacker.org/forum/login.php'
ret = s.post(url, data=payload)

now, fin = map(int, input('level: ').split())

for level in range(now, fin):
    if level == now:
        req = s.get(f'http://www.hacker.org/coil/index.php?gotolevel={level}&go=Go+To+Level')
    pre = '<param name="FlashVars" value="'
    value = req.text[req.text.find(pre) + len(pre):]
    value = value[:value.find('"')]
    arr = value.split('&')

    with open('board_info.txt', 'wt') as f:
        f.write(f'{arr[0][2:]} {arr[1][2:]} {arr[2][6:]}')

    print(f'********** Now Level: {level} **********')
    os.system(f'solver.exe')

    with open('outurl.txt', 'rt') as f:
        qpathurl = f.read(1 << 15)
        with open(f'solution/Lv{level}.txt', 'wt') as g:
            g.write(qpathurl)

    req = s.get(qpathurl)
